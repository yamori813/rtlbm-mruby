This projct use these libraries.

newlib-2.5.0.20171222  
lwip-2.1.2  
mruby  
BearSSL  

Source code 

start.S -- start up assembra  
main.c -- main  
syscalls.c -- dummy and sbrk function for newlib  

intr.c -- inturrupt code  
inthandler.S -- excemtion assembra  
traps.c -- cache control code  

uart.c -- rtl8196 uart code
rtl_timer.c -- rtl8196 timer code  
rtl_ether.c -- rt8196 ethernet nic support code  
rtl_switch.c -- rtl8196 switch vlan setup code  
swCore.c -- realtek switch control code  
net.c -- netwrok code  
bear.c -- https code

Build tools is this.  

rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714  

I build on FreeBSD/amd64 11.2 used by linux emuration.  

Default build is RTL8196C. If you want build for RTL8196E do this.  

% make TARGET=RTL8196E  

```
+-- rsdk (synblic link to rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714)
|
+-- newlib-2.5.0.20171222  
|       |  
|       +--- mips  
|  
+-- mruby  
|       |  
|       +-- build/realtek/lib  
|  
+-- lwip-2.1.2  
|       |  
|       +-- realtek  
+-- BearSSL
        |
        +-- build
```
