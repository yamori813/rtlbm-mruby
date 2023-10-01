#
# rtlbm-mruby mruby script
#

begin

addr = "10.10.10.2"
mask = "255.255.255.0"
gw = "10.10.10.1"
dns = "10.10.10.1"

rtl = YABM.new

rtl.netstart(addr, mask, gw, dns)                                              

ports = Array[0,1,2,3,4]

i = 0

while true
  rtl.print "."

  rtl.msleep 200

  ports.each{|port|
    m = rtl.getmib(port, YABM::MIB_IN, YABM::MIB_IFINOCTETS)
    rtl.print m.to_s + " "
    m = rtl.getmib(port, YABM::MIB_OUT, YABM::MIB_IFOUTOCTETS)
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
