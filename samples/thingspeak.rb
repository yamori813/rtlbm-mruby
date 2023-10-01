#
# yabmbm-mruby mruby script
#
# thingspeak channel update by HomeSpotCube
#

APIKEY = "naisyo"

def ledon yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg & ~TOP_LED1)
end

def ledoff yabm
  reg = yabm.gpiogetdat()
  yabm.gpiosetdat(reg | TOP_LED1)
end

begin

yabm = YABM.new

yabm.netstartdhcp

yabm.gpiosetsel(0x003c3000, 0x003c3000, 0x00001800, 0x00001800)

STATUS_LED1 = (1 << 0)
STATUS_LED2 = (1 << 6)
STATUS_LED3 = (1 << 17)
TOP_LED1 = (1 << 16)
TOP_LED2 = (1 << 4)
TOP_LED3 = (1 << 1)
TOP_BUTTON = (1 << 3)

ALLLED = STATUS_LED1 | STATUS_LED2 | STATUS_LED3 | TOP_LED1 | TOP_LED2 | TOP_LED3

yabm.gpiosetdir(ALLLED)
reg = yabm.gpiogetdat()
yabm.gpiosetdat(reg | ALLLED)

# sync date by ntp use https X.509
ntpaddr = yabm.lookup("ntp.nict.jp")
yabm.sntp(ntpaddr)

#yabm.msleep 3_000 
#ntpaddr6 = yabm.lookup6("ntp.nict.jp")
#yabm.print ntpaddr6 + "\r\n"
#yabm.sntp(ntpaddr6)

count = 0
interval = 30

yabm.watchdogstart(256)

loop do
  count = count + 1
  ledon yabm
  yabm.print count.to_s
  res = SimpleHttp.new("https", "api.thingspeak.com", 443).request("GET", "/update?api_key=" + APIKEY + "&field1=" + count.to_s, {'User-Agent' => "test-agent"})
  ledoff yabm
  yabm.print "." + "\r\n"
  yabm.msleep interval * 1000
  yabm.watchdogreset
end

rescue => e
  yabm.print e.to_s
end
