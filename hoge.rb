#
# rtlbm-mruby mruby script
#

begin

rtl = RTL8196C.new(RTL8196C::MODULE_HOMESPOTCUBE)

rtl.print "Hello Bear Metal mruby!!"

while 1 do
   rtl.print "."
   start = rtl.count() 
   while rtl.count() < start + 50 do
   end
end

rescue => e
  rtl.print e.to_s
end
