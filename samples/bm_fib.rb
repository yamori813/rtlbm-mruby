#
# rtlbm-mruby mruby script
#

def fib n
  return n if n < 2
  fib(n-2) + fib(n-1)
end

begin

rtl = YABM.new

while true
   rtl.print "."
   start = rtl.count() 
   fib(32)
   time = (rtl.count() - start) / 1000
   rtl.print "fib(32): " + time.to_s + "sec\n"
end

rescue => e
  rtl.print e.to_s
end
