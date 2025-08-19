/*
 * Copyright (c) 2025 Hiroki Mori. All rights reserved.
 */

#include <sys/cdefs.h>

#include "lwip/init.h"

#include "lwip/debug.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"

#include "lwip/stats.h"

#include "lwip/ip.h"
#include "lwip/ip4_frag.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

#include "lwip/dns.h"
#include "lwip/dhcp.h"

#if defined(RTL8196) || defined(RTL8196E)
#include "system.h"
#endif

//#define	NETDEBUG

static const char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
static const char http_index_html[] = "<html><head><title>Congrats!</title></head><body><h1>Welcome to our lwIP HTTP server!</h1><p>This is a small test page, served by httpserver raw.</body></html>";

static struct tcp_pcb *httpsvr_pcb;

char reqbuff[1024];
char resbuff[1024*32];
int reslen;

enum httpsvr_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

struct httpsvr_state
{
  u8_t state;
  u8_t retries;
  struct tcp_pcb *pcb;
  /* pbuf (chain) to recycle */
  struct pbuf *p;
};

static void
httpsvr_free(struct httpsvr_state *es)
{
  if (es != NULL) {
    if (es->p) {
      /* free the buffer chain if present */
      pbuf_free(es->p);
    }

    mem_free(es);
  }  
}

static void
httpsvr_close(struct tcp_pcb *tpcb, struct httpsvr_state *es)
{
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);

  httpsvr_free(es);

  tcp_close(tpcb);
}

static void
httpsvr_send(struct tcp_pcb *tpcb, struct httpsvr_state *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) &&
         (es->p != NULL) && 
         (es->p->len <= tcp_sndbuf(tpcb))) {
    ptr = es->p;

    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
    if (wr_err == ERR_OK) {
      u16_t plen;

      plen = ptr->len;
      /* continue with next pbuf in chain (if any) */
      es->p = ptr->next;
      if(es->p != NULL) {
        /* new reference! */
        pbuf_ref(es->p);
      }
      /* chop first pbuf from chain */
      pbuf_free(ptr);
      /* we can read more data now */
      tcp_recved(tpcb, plen);
    } else if(wr_err == ERR_MEM) {
      /* we are low on memory, try later / harder, defer to poll */
      es->p = ptr;
    } else {
      /* other problem ?? */
    }
  }
}

static void
httpsvr_error(void *arg, err_t err)
{
  struct httpsvr_state *es;

  LWIP_UNUSED_ARG(err);

  es = (struct httpsvr_state *)arg;

  httpsvr_free(es);
}

static err_t
httpsvr_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct httpsvr_state *es;

  es = (struct httpsvr_state *)arg;
  if (es != NULL) {
    if (es->p != NULL) {
      /* there is a remaining pbuf (chain)  */
      httpsvr_send(tpcb, es);
    } else {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING) {
        httpsvr_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  } else {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

static err_t
httpsvr_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct httpsvr_state *es;

  LWIP_UNUSED_ARG(len);

  es = (struct httpsvr_state *)arg;
  es->retries = 0;
  
  if(es->p != NULL) {
    /* still got pbufs to send */
    tcp_sent(tpcb, httpsvr_sent);
    httpsvr_send(tpcb, es);
  } else {
    /* no more pbufs to send */
    if(es->state == ES_CLOSING) {
      httpsvr_close(tpcb, es);
    }
  }
  return ERR_OK;
}

static err_t
httpsvr_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct httpsvr_state *es;
  err_t ret_err;

  LWIP_ASSERT("arg != NULL",arg != NULL);
  es = (struct httpsvr_state *)arg;
  if (p == NULL) {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p == NULL) {
      /* we're done sending, close it */
      httpsvr_close(tpcb, es);
    } else {
      /* we're not done yet */
      httpsvr_send(tpcb, es);
    }
    ret_err = ERR_OK;
  } else if(err != ERR_OK) {
    /* cleanup, for unknown reason */
    if (p != NULL) {
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED) {
    /* first data chunk in p->payload */
    es->state = ES_RECEIVED;
    /* store reference to incoming pbuf (chain) */
//    es->p = p;
//    httpsvr_send(tpcb, es);
    pbuf_copy_partial(p, reqbuff, p->tot_len, 0);
    pbuf_free(p);
    if ((reqbuff[p->tot_len - 2] == '\n' && reqbuff[p->tot_len - 1] == '\n') ||
      (reqbuff[p->tot_len - 4] == '\r' && reqbuff[p->tot_len - 3] == '\n' &&
      reqbuff[p->tot_len - 2] == '\r' && reqbuff[p->tot_len - 1] == '\n')) {

      struct pbuf* b = pbuf_alloc(PBUF_TRANSPORT, reslen, PBUF_POOL);
      pbuf_take(b, (char*)resbuff, reslen);
      es->p = b;
      httpsvr_send(tpcb, es);
    }
    ret_err = ERR_OK;
  } else if (es->state == ES_RECEIVED) {
    /* read some more data */
    if(es->p == NULL) {
//      es->p = p;
//      httpsvr_send(tpcb, es);
        pbuf_free(p);
    } else {
      struct pbuf *ptr;

      /* chain pbufs to the end of what we recv'ed previously  */
      ptr = es->p;
      pbuf_cat(ptr,p);
    }
    ret_err = ERR_OK;
  } else {
    /* unkown es->state, trash data  */
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

static err_t
httpsvr_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct httpsvr_state *es;

  LWIP_UNUSED_ARG(arg);
  if ((err != ERR_OK) || (newpcb == NULL)) {
    return ERR_VAL;
  }

  /* Unless this pcb should have NORMAL priority, set its priority now.
     When running out of pcbs, low priority pcbs can be aborted to create
     new pcbs of higher priority. */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  es = (struct httpsvr_state *)mem_malloc(sizeof(struct httpsvr_state));
  if (es != NULL) {
    es->state = ES_ACCEPTED;
    es->pcb = newpcb;
    es->retries = 0;
    es->p = NULL;
    /* pass newly allocated es to our callbacks */
    tcp_arg(newpcb, es);
    tcp_recv(newpcb, httpsvr_recv);
    tcp_err(newpcb, httpsvr_error);
    tcp_poll(newpcb, httpsvr_poll, 0);
    tcp_sent(newpcb, httpsvr_sent);
    ret_err = ERR_OK;
  } else {
    ret_err = ERR_MEM;
  }
  return ret_err;
}

void
httpsvr_init()
{
  httpsvr_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);

  reslen = sizeof(http_html_hdr) + sizeof(http_index_html) - 2;
  memcpy(resbuff, http_html_hdr, sizeof(http_html_hdr) - 1);
  memcpy(resbuff + sizeof(http_html_hdr) - 1, http_index_html, sizeof(http_index_html) - 1);
}

void
httpsvr_bind(int port)
{
  if (httpsvr_pcb != NULL) {
    err_t err;

    err = tcp_bind(httpsvr_pcb, IP_ANY_TYPE, port);
    if (err == ERR_OK) {
      httpsvr_pcb = tcp_listen(httpsvr_pcb);
      tcp_accept(httpsvr_pcb, httpsvr_accept);
    } else {
      /* abort? output diagnostic? */
    }
  } else {
    /* abort? output diagnostic? */
  }
}

void
httpsvr_setres(char *buff, int len)
{
  cli();
  if (len < sizeof(resbuff)) {
    memcpy(resbuff, buff, len);
    reslen = len;
  }
  sti();
}
