#
# rtlbm-mruby mruby script
#

begin

rtl = RTL8196C.new(RTL8196C::MODULE_HOMESPOTCUBE)

ports = Array[0, 4]

i = 0

while 1 do
  rtl.print "."

  start = rtl.count() 
  while rtl.count() < start + 200 do
  end

  ports.each{|port|
    m = rtl.getmib(port, RTL8196C::MIB_IN, RTL8196C::MIB_IFINOCTETS)
    rtl.print m.to_s + " "
    m = rtl.getmib(port, RTL8196C::MIB_OUT, RTL8196C::MIB_IFOUTOCTETS)
    rtl.print m.to_s + " "
  }
  rtl.print "\r\n"

  for reg in 0..4 do
    ports.each{|port|
      dat = rtl.readmdio(port, reg);
      rtl.print dat.to_s + " "
    }
    rtl.print "\r\n"
  end
  i = i + 1
end

rescue => e
  rtl.print e.to_s
end
