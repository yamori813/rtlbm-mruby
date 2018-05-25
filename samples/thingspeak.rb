#
# rtlbm-mruby mruby script
#
# thingspeak channel update 
#

APIKEY = "naisyo"

begin

# ip address setting

addr = 10 << 24 | 0 << 16 | 1 << 8 | 222
mask = 255 << 24 | 255 << 16 | 255 << 8 | 0
gw = 10 << 24 | 0 << 16 | 1 << 8 | 1
dns = 10 << 24 | 0 << 16 | 1 << 8 | 1


rtl = RTL8196C.new(RTL8196C::MODULE_BBR4HGV2)

rtl.netstart(addr, mask, gw, dns)

# sync date by ntp use https X.509
ntpaddr = rtl.lookup("ntp.nict.jp")
rtl.sntp(ntpaddr)

hcount = 0
count = 0
interval = 20

while 1 do
  rtl.print "."
  start = rtl.count() 
  while rtl.count() < start + 1000 do
  end
  count = count + 1
  if count % interval == 0 then
    hcount = hcount + 1
    rtl.print " " + hcount.to_s + "\r\n"
    res = SimpleHttp.new("https", "api.thingspeak.com", 443).request("GET", "/update?api_key=" + APIKEY + "&field1=1", {'User-Agent' => "test-agent"})
  end
end

rescue => e
  rtl.print e.to_s
end
