This projct use these libraries.

newlib-3.0.0.20180831  
lwip-2.1.2  
bearssl-0.6  
mruby  

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

I build on FreeBSD/amd64 used by linux emuration.  

Default build is RTL8196C. If you want build for RTL8196E do this.  

% make TARGET=RTL8196E  

```
$ git clone --recursive https://github.com/yamori813/rtlbm-mruby.git
$ git clone https://github.com/yamori813/rtl819x-toolchain.git
$ cd rtlbm-mruby/build/
$ ./getfiles.sh
$ ./mknewlib.sh
$ ./mklwip.sh
$ ./mkbearssl.sh
$ ./mkmruby.sh
$ cd ..
$ ./mkallvm.sh
```

main.rtl --- RTL8196C  
main_e.rtl --- RTL8196E  
main_8198.rtl --- RTL8198  
main_sw.rtl --- RTL8197 with SWITCH  
