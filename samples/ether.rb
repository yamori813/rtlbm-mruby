#
# rtlbm-mruby mruby script
#

begin

addr = 10 << 24 | 10 << 16 | 10 << 8 | 2
mask = 255 << 24 | 255 << 16 | 255 << 8 | 0
gw = 10 << 24 | 10 << 16 | 10 << 8 | 1
dns = 10 << 24 | 10 << 16 | 10 << 8 | 1

rtl = YABM.new(YABM::MODULE_BBR4HGV2)

rtl.netstart(addr, mask, gw, dns)                                              

ports = Array[0,1,2,3,4]

i = 0

while 1 do
  rtl.print "."

  start = rtl.count() 
  while rtl.count() < start + 200 do
  end

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
