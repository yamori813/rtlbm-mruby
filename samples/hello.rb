#
# rtlbm-mruby mruby script
#

begin

rtl = YABM.new

rtl.print "Hello Bear Metal mruby on YABM " + rtl.getarch.to_s

while 1 do
   rtl.print "."
   start = rtl.count() 
   while rtl.count() < start + 500 do
   end
end

rescue => e
  rtl.print e.to_s
end
