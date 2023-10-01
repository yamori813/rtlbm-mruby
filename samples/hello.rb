#
# rtlbm-mruby mruby script
#

begin

rtl = YABM.new

rtl.print "Hello Bear Metal mruby on YABM " + rtl.getarch.to_s

loop do
   rtl.print "."
   rtl.msleep 500
end

rescue => e
  rtl.print e.to_s
end
