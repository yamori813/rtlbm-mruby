#
# rtlbm-mruby mruby script
#
# thingspeak channel update 
#

APIKEY = "naisyo"

begin

rtl = YABM.new

rtl.netstartdhcp

# sync date by ntp use https X.509
ntpaddr = rtl.lookup("ntp.nict.jp")
rtl.sntp(ntpaddr)

hcount = 0
count = 0
interval = 20

yabm.watchdogstart(256)

while 1 do
  rtl.print "."
  start = rtl.count() 
  while rtl.count() < start + 60 * 1000 do
  end
  count = count + 1
  if count % interval == 0 then
    hcount = hcount + 1
    rtl.print " " + hcount.to_s + "\r\n"
    res = SimpleHttp.new("https", "api.thingspeak.com", 443).request("GET", "/update?api_key=" + APIKEY + "&field1=1", {'User-Agent' => "test-agent"})
  end
  yabm.watchdogreset
end

rescue => e
  rtl.print e.to_s
end
