#
# rtlbm-mruby mruby script
#

def delay(yabm, val)
  start = yabm.count()
  while yabm.count() < start + val do
  end
end
    
class YABMTIME
  def mkstr epoch

    wday_names = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
    mon_names = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    tm = mktm epoch
    hstr = tm[2] < 10 ? "0" + tm[2].to_s : tm[2].to_s
    mstr = tm[1] < 10 ? "0" + tm[1].to_s : tm[1].to_s
    sstr = tm[0] < 10 ? "0" + tm[0].to_s : tm[0].to_s
    return wday_names[tm[6]] + " " + mon_names[tm[4]] + " " + tm[3].to_s + " " + hstr + ":" + mstr + ":" + sstr + " " + tm[5].to_s
  end
    
  def mktm epoch
    seconds = epoch
    minutes  = seconds / 60
    seconds -= minutes * 60
    hours    = minutes / 60
    minutes -= hours * 60
    days     = hours / 24
    hours   -= days * 24
    
    year      = 1970
    dayOfWeek = 4
    
    while 1 do
      leapYear   = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
      daysInYear = leapYear ? 366 : 365
      if days >= daysInYear then
        dayOfWeek += leapYear ? 2 : 1
        days      -= daysInYear
        if dayOfWeek >= 7 then
          dayOfWeek -= 7
        end
        year += 1
      else
        ydays = days
        dayOfWeek  += days
        dayOfWeek  %= 7
        daysInMonth = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
        for month in 0..11 do
          dim = daysInMonth[month]
          if month == 1 && leapYear
            dim += 1
          end
          if days >= dim then
            days -= dim
          else
            break
          end
        end
        break
      end
    end
    
    return [seconds, minutes, hours, (days + 1), month, year, dayOfWeek, ydays]
    
  end
end
    
begin

yabm = YABM.new

yabm.netstartdhcp

yabm.print yabm.getaddress + "\n"
  
ntpaddr = yabm.lookup("ntp.nict.jp")
yabm.sntp(ntpaddr)
delay yabm, 3000

d = YABMTIME.new
date = d.mkstr yabm.now + 9 * 60 * 60

yabm.print "Hello Bear Metal mruby on YABM "
yabm.print date
    
while 1 do
   yabm.print "."
   delay yabm, 500
end
    
rescue => e
  yabm.print e.to_s
end
