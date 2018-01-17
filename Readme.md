This projct use these libraries.

newlib-2.5.0.20171222  
lwip-2.0.3  
mruby-1.4.0  

Source code 

start.S -- start up assembra  
main.c -- main  
syscalls.c -- dummy and sbrk function for newlib  

intr.c -- inturrupt code  
inthandler.S -- excemtion assembra  
traps.c -- cache control code  

rtl_timer.c -- rtl8196 timer code  
net.c -- netwrok code  
rtl_ether.c -- rt8196 ethernet nic support code  
rtl_switch.c -- rtl8196 switch vlan setup code  
swCore.c -- realtek switch control code  

hoge.rb -- mruby code

Build tools is this.  

rsdk-1.5.5-5281-EB-2.6.30-0.9.30.3-110714  

I build on FreeBSD/amd64 10.4 used by linux emuration.  
