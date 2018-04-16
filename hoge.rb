#
# rtlbm-mruby mruby script
#
# echo -n "GREEN" | nc -w 0 -u 10.10.10.2 7000
#

begin

rtl = RTL8196C.new(RTL8196C::MODULE_HOMESPOTCUBE)

rtl.udpbind(7000)

rtl.print "Hello BearMetal mruby!!"

while 1 do
   rtl.print "."
   start = rtl.count() 
   while rtl.count() < start + 50 do
   end
end

rescue => e
  rtl.print e.to_s
end
