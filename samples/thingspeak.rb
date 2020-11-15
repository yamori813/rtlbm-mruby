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

count = 0
interval = 60

rtl.watchdogstart(256)

while 1 do
  count = count + 1
  rtl.print count.to_s + "\r\n"
  res = SimpleHttp.new("https", "api.thingspeak.com", 443).request("GET", "/update?api_key=" + APIKEY + "&field1=" + count.to_s, {'User-Agent' => "test-agent"})
  start = rtl.count() 
  while rtl.count() < start + interval * 1000 do
  end
  rtl.watchdogreset
end

rescue => e
  rtl.print e.to_s
end
