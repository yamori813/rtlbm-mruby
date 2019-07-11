#
# rtlbm-mruby mruby script
#
# Machinist metrix update script
#

APIKEY = "naisyo"
INTERVAL = 10

def delay(yabm, val) 
  start = yabm.count() 
  while yabm.count() < start + val do
  end
end

begin

yabm = YABM.new

# net setup

yabm.netstartdhcp

yabm.print yabm.getaddress + "\n"

# BearSSL need correct clock
ntpaddr = rtl.lookup("ntp.nict.jp")
yabm.sntp(ntpaddr)

# JTAG(GPIOA2,4,5,6) and LED_PORT3(GPIOB5) is GPIO
yabm.gpiosetsel(0x06, 0x1ffffff, 0x600, 0x3fff)

# WR8165N, WR8166N
# GPIOA6 GREEN LED 0x0040
# GPIOB5 RED LED   0x2000
# GPIOA2 Slide SW  0x0004
# GPIOA5 Push SW   0x0020
# GPIOA4 ?         0x0010

GLED = 0x0040
RLED = 0x2000
SSW = 0x0004
PSW = 0x0020

yabm.gpiosetctl(~(GLED | RLED | SSW | PSW))
yabm.gpiosetdir(GLED | RLED)
reg = yabm.gpiogetdat()
yabm.gpiosetdat(reg & ~(RLED | GLED))

count = 0

accesstoken = APIKEY

yabm.watchdogstart(256)

while 1 do
  yabm.print "."
  count = count + 1
  yabm.print " " + count.to_s
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg | RLED)

  body='{ "agent": "mruby de IoT", "metrics": [ { "namespace": "mruby_yabm_kani", "name":      "hartbeet", "data_point": { "value": ' + count.to_s + ' } } ] }'

  header = Hash.new
  header.store('User-Agent', "test-agent")
  header.store('Authorization', "Bearer " + accesstoken)
  header.store('Content-Type', "application/json")
  header.store('Body', body)
  res = SimpleHttp.new("https", "gw.machinist.iij.jp", 443).post("/endpoint", header)
  reg = yabm.gpiogetdat()
  if res 
    yabm.print " " + res.status.to_s
  end
  yabm.gpiosetdat(reg & ~RLED)
  yabm.watchdogreset
  yabm.print "\n"
  delay(yabm, INTERVAL * 1000)
end

rescue => e
  yabm.print e.to_s
end
