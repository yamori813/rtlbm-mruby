#
# rtlbm-mruby mruby script
#

begin

rtl = RTL8196C.new(RTL8196C::MODULE_BBR4HGV2)

addr = 10 << 24 | 0 << 16 | 1 << 8 | 222
mask = 255 << 24 | 255 << 16 | 255 << 8 | 0
gw = 10 << 24 | 0 << 16 | 1 << 8 | 1
dns = 10 << 24 | 0 << 16 | 1 << 8 | 1

apikey = "naisyo"

rtl.netstart(addr, mask, gw, dns)

hcount = 0
count = 0
interval = 20

while 1 do
  rtl.print "."
  start = rtl.count() 
  while rtl.count() < start + 100 do
  end
  count = count + 1
  if count % interval == 0 then
    hcount = hcount + 1
    rtl.print " " + hcount.to_s + "\r\n"
    res = SimpleHttp.new("https", "api.thingspeak.com", 443).request("GET", "/update?api_key=" + apikey + "&field1=1", {'User-Agent' => "test-agent"})
  end
end

rescue => e
  rtl.print e.to_s
end
