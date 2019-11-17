#include "DS3231.h"

const int daysArray [] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const int dowArray[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };


DS3231::DS3231(I2C& bus): i2c(bus)
    {
    } 
    
    
// BCD to decimal conversion    
int DS3231::bcd2dec(int k)
    {return(((k&0xF0)>>4)*10+(k&0x0F));}
    
    
// decimal to BCD conversion    
int DS3231::dec2bcd(int k)
    {return((k/10)*16+(k%10));}      
    
void DS3231::setI2Cfrequency(int frequency)
    {i2c.frequency(frequency);
    }
    
// read temperature from DS3231    
float DS3231::readTemperature()
    {char buf[2];
     buf[0]=DS3231_MSB_Temp;
     int w=i2c.write(DS3231_Address,buf,1);
     int r=i2c.read(DS3231_Address,buf,2);
     error=((w!=0)||(r!=0));   
     return(buf[0]+buf[1]/256.0);}  

// force temperatue conversion     
void DS3231::convertTemperature()
    {int Control=readRegister(DS3231_Control)|DS3231_bit_CONV;
     writeRegister(DS3231_Control,Control);
    }

// decode time registers     
void DS3231::decodeTime(int regHours, int regMinutes, int regSeconds,int *Hours, int *Minutes, int *Seconds)
    {*Hours=bcd2dec(regHours);
     *Minutes=bcd2dec(regMinutes);
     *Seconds=bcd2dec(regSeconds);
    }      
     
     
// read time registers and decode them to decimal     
void DS3231::readTime(int *hours, int *minutes, int *seconds)
    {char buf[3];
     buf[0]=DS3231_Seconds;
     int w=i2c.write(DS3231_Address,buf,1);
     int r=i2c.read(DS3231_Address,buf,3);
     decodeTime(buf[2],buf[1],buf[0],hours,minutes,seconds);    
     error=((w!=0)||(r!=0));
    }  

// set time register    
void DS3231::setTime(int hours, int minutes, int seconds)
    {writeRegister(DS3231_Seconds,dec2bcd(seconds));
     writeRegister(DS3231_Minutes,dec2bcd(minutes));
     writeRegister(DS3231_Hours,dec2bcd(hours));
    }    

// read time register and decode hours
int DS3231::readHours()
    {int regHours = readRegister(DS3231_Hours);
     return bcd2dec(regHours);
    }  

// set hours register    
void DS3231::setHours(int hours)
    {
     writeRegister(DS3231_Hours,dec2bcd(hours));
    }

// read time register and decode minutes
int DS3231::readMinutes()
    {int regMinutes = readRegister(DS3231_Minutes);
     return bcd2dec(regMinutes);
    }  

// set minutes register    
void DS3231::setMinutes(int minutes)
    {
     writeRegister(DS3231_Minutes,dec2bcd(minutes));
    }    


// read time register and decode seconds
int DS3231::readSeconds()
    {int regSeconds = readRegister(DS3231_Seconds);
     return bcd2dec(regSeconds);
    }  

// set seconds register    
void DS3231::setSeconds(int seconds)
    {
     writeRegister(DS3231_Seconds,dec2bcd(seconds));
    }    

// decode date registers
void DS3231::decodeDate(int regDay,int regMonth, int regYear, int *day, int *month, int *year)
    {*day=bcd2dec(regDay);
     int century=regMonth&128;
     *month=bcd2dec(regMonth&127);
     *year=bcd2dec(regYear);
     if (century==0)         // only XXth and XXIst centuries allowed
        {*year=*year+1900;}
     else
        {*year=*year+2000;}
    }         
    
// read date registers    
void DS3231::readDate(int *day, int *month, int *year)
    {char buf[3];
     buf[0]=DS3231_Day;
     int w=i2c.write(DS3231_Address,buf,1);
     int r=i2c.read(DS3231_Address,buf,3);
     decodeDate(buf[0],buf[1],buf[2],day,month,year);            
     error=((w!=0)||(r!=0));         
    }     

// set the date registers
void DS3231::setDate(int dayOfWeek, int day, int month, int year)
    {writeRegister(DS3231_Day,dec2bcd(day));
     writeRegister(DS3231_DayOfWeek,dayOfWeek);
     int century=0; // only XXth and XXIst centuries allowed
     if (year<1900){year=1900;}
     if (year>2099){year=2099;}
     if (year<2000)
        {year=year-1900;}
     else
        {year=year-2000;century=1;}   
     writeRegister(DS3231_Month_Century,dec2bcd(month)+128*century);   
     writeRegister(DS3231_Year,dec2bcd(year));
    }     
    
// read date and time registers    
void DS3231::readDateTime(int *dayOfWeek, int *day, int *month, int *year, int *hours, int *minutes, int *seconds)
    {char buf[7];
     buf[0]=DS3231_Seconds;
     int w=i2c.write(DS3231_Address,buf,1);
     int r=i2c.read(DS3231_Address,buf,7);
     decodeTime(buf[2],buf[1],buf[0],hours,minutes,seconds);
     *dayOfWeek=buf[3];
     decodeDate(buf[4],buf[5],buf[6],day,month,year);
     error=((w!=0)||(r!=0));     
    }    

// Read date and time registers and return a structure
RTCDateTime DS3231::readDateTime()
{
  RTCDateTime dt;
  readDateTime(&dt.dayOfWeek, &dt.day, &dt.month, &dt.year, &dt.hour, &dt.minute, &dt.second);
  dt.unixtime = unixtime(dt.day, dt.month, dt.year, dt.hour, dt.minute, dt.second);
  return dt;
}

// read day register    
int DS3231::readDayOfWeek()
    {int regDayOfWeek = readRegister(DS3231_DayOfWeek);
     return regDayOfWeek;
    }     

// set day register    
void DS3231::setDayOfWeek(int dow)
    {writeRegister(DS3231_DayOfWeek, (char) dow);
    }     

// read day register    
int DS3231::readDay()
    {int regDay = readRegister(DS3231_Day);
     return bcd2dec(regDay);
    }     

// set day register    
void DS3231::setDay(int day)
    {writeRegister(DS3231_Day,dec2bcd(day));
    }     

// read month register    
int DS3231::readMonth()
    {int regMonth = readRegister(DS3231_Month_Century);
     return bcd2dec(regMonth&127);
    }     

// set month register    
void DS3231::setMonth(int month, int century)
    {
      if (century < 0) {century = 0;} // Only XX (0) and XXI (1)
      if (century > 1) {century = 1;} // centuries supported
      writeRegister(DS3231_Month_Century,dec2bcd(month)+128*century);
    }     

int DS3231::readYear()
    {int regMonth = readRegister(DS3231_Month_Century);
     int regYear = readRegister(DS3231_Year);
     int century = regMonth&128;

     int year=bcd2dec(regYear);
     if (century==0)         // only XXth and XXIst centuries allowed
        {year=year+1900;}
     else
        {year=year+2000;}
     return year;
    }         

// set the date registers
void DS3231::setYear(int year)
    {
     int century = 0;
     if (year<1900){year=1900;}
     if (year>2099){year=2099;}
     if (year<2000)
        {year=year-1900;}
     else
        {year=year-2000;century=1;}
     int regMonth = readRegister(DS3231_Month_Century);
     regMonth &= (century << 7);
     writeRegister(DS3231_Month_Century,(char) regMonth);   
     writeRegister(DS3231_Year,dec2bcd(year));
    }     

// read a register    
int DS3231::readRegister(char reg)
    {char buf[1];
     buf[0]=reg;
     int w=i2c.write(DS3231_Address,buf,1);
     int r=i2c.read(DS3231_Address,buf,1);
     error=((w!=0)||(r!=0));  
     return(buf[0]);
    }   

// write a register    
void DS3231::writeRegister(int reg,char byte)
    {char buf[2];
     buf[0]=reg;
     buf[1]=byte;
     int w=i2c.write(DS3231_Address,buf,2); 
     error=(w!=0);   
    }      
    
void DS3231::eraseOSF()
    {int reg=readRegister(DS3231_Control_Status);
     reg=reg&0x7F;
     writeRegister(DS3231_Control_Status,reg);
    }     
    
bool DS3231::OSF()
    {int reg=readRegister(DS3231_Control_Status);
    return(reg&DS3231_bit_OSF);
    }


/**
 * Various time / date manipulation functions
 */
// Converts time to a unix timestamp
uint32_t DS3231::unixtime(int day, int month, int year, int hours, int minutes, int seconds)
{
    uint32_t u;

    u = time2long(date2days(year, month, day), hours, minutes, seconds);
    return u;
}

// Converts time to the number of seconds
long DS3231::time2long(uint16_t days, int hours, int minutes, int seconds)
{
    return ((days * 24L + hours) * 60 + minutes) * 60 + seconds;
}

// Calculates the number of days since 01/01/1970
uint16_t DS3231::date2days(int year, int month, int day)
{
    year = year - 1970;
    uint16_t days16 = 0;
    for (uint8_t i = 1; i < month; ++i)
      {
        days16 += daysArray[i - 1];
      }

    if ((month == 2) && isLeapYear(year))
      {
        ++days16;
      }
    return days16 + 365 * year + (year + 3) / 4 + day - 1;
}

// Checks if the year is leap year
bool DS3231::isLeapYear(int year)
{
    return (year % 4 == 0);
}

// Calculates the day of week
int DS3231::dow(uint16_t year, int month, int day)
{
    int dow;
    year -= month < 3;
    dow = ((year + year/4 - year/100 + year/400 + dowArray[month-1] + day) % 7);
    if (dow == 0)
      {return 7;}
    return dow;
}

// Sets the datetime from unix stamp
void DS3231::setDateTime(uint32_t t)
{
  uint16_t year;
  int month, day;
  int hour, minute, second;

  second = t % 60;
  t /= 60;
  minute = t % 60;
  t /= 60;
  hour = t % 24;
  uint16_t days = t / 24;
  int leap;

  for(year=1970; ;++year)
    {
      leap = year % 4 == 0;
      if (days < 365 + leap)
        {break;}
      days -= 365 + leap;
    }

  for (month=1; ; ++month)
    {
      int daysPerMonth = daysArray[month - 1];
      if (leap && month == 2)
        {++daysPerMonth;}
      
      if (days < daysPerMonth)
        {break;}
      days -= daysPerMonth;
    }
  day = days + 1;

  setDate(dow(year, month, day), day, month, year);
  setTime(hour, minute, second);
}

// Reads the datetime as unix stamp
uint32_t DS3231::readUnixtime()
{
  int year, month, day, dayOfWeek;
  int hour, minute, second;
  readDateTime(&dayOfWeek, &day, &month, &year, &hour, &minute, &second);
  return unixtime(day, month, year, hour, minute, second);
}

// Returns the name of the day of week
char const *DS3231::strDayOfWeek(int dayOfWeek)
{
    switch (dayOfWeek) {
        case 1:
            return "Monday"; break;
        case 2:
            return "Tuesday"; break;
        case 3:
            return "Wednesday"; break;
        case 4:
            return "Thursday"; break;
        case 5:
            return "Friday"; break;
        case 6:
            return "Saturday"; break;
        case 7:
            return "Sunday"; break;
        default:
            return "Unknown";
    }
}

// Returns the name of the month
char const *DS3231::strMonth(int month)
{
    switch (month) {
        case 1:
            return "January"; break;
        case 2:
            return "February"; break;
        case 3:
            return "March"; break;
        case 4:
            return "April"; break;
        case 5:
            return "May"; break;
        case 6:
            return "June"; break;
        case 7:
            return "July"; break;
        case 8:
            return "August"; break;
        case 9:
            return "September"; break;
        case 10:
            return "October"; break;
        case 11:
            return "November"; break;
        case 12:
            return "December"; break;
        default:
            return "Unknown";
    }
}

// Returns the AM/PM string
char const *DS3231::strAmPm(int hour, bool uppercase)
{
    if (hour < 12)
    {
        if (uppercase)
        {return "AM";}
	else
        {return "am";}
    } else
    {
        if (uppercase)
        {return "PM";}
	else
        {return "pm";}
    }
}

// Returns the suffix of the date
char const *DS3231::strDaySufix(int day)
{
    if (day % 10 == 1)
      {return "st";}
    else
      if (day % 10 == 2)
	{return "nd";}
    if (day % 10 == 3)
      {return "rd";}
    return "th";
}

// Returns the 12 hours out of 24 hours
int DS3231::hour12(int hour24){
    if (hour24 == 0)
      {return 12;}
    if (hour24 > 12)
      {return (hour24 - 12);}
    return hour24;
}

// Returns the number of days in a month
int DS3231::daysInMonth(uint16_t year, int month)
{
    int days;
    days = daysArray[month - 1];

    if ((month == 2) && isLeapYear(year))
      {++days;}
    return days;
}

// Returns the number of days in a year
uint16_t DS3231::dayInYear(uint16_t year, int month, int day)
{
    uint16_t fromDate;
    uint16_t toDate;
    fromDate = date2days(year, 1, 1);
    toDate = date2days(year, month, day);
    return (toDate - fromDate);
}

// Prints the datetime with a given format
char const *DS3231::dateFormat(char* formattedDate, const char* dateFormat, RTCDateTime dt)
{
    
    formattedDate[0] = 0;
    char helper[11];

    while (*dateFormat != '\0')
    {
      switch (dateFormat[0])
        {
            // Day decoder
            case 'd':
                sprintf(helper, "%02d", dt.day); 
                strcat(formattedDate, (const char *)helper); 
                break;
            case 'j':
                sprintf(helper, "%d", dt.day);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'l':
                strcat(formattedDate, (const char *)strDayOfWeek(dt.dayOfWeek));
                break;
            case 'D':
                strncat(formattedDate, strDayOfWeek(dt.dayOfWeek), 3);
                break;
            case 'N':
                sprintf(helper, "%d", dt.dayOfWeek);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'w':
                sprintf(helper, "%d", (dt.dayOfWeek + 7) % 7);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'z':
                sprintf(helper, "%d", dayInYear(dt.year, dt.month, dt.day));
                strcat(formattedDate, (const char *)helper);
                break;
            case 'S':
                strcat(formattedDate, (const char *)strDaySufix(dt.day));
                break;

            // Month decoder
            case 'm':
                sprintf(helper, "%02d", dt.month);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'n':
                sprintf(helper, "%d", dt.month);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'F':
                strcat(formattedDate, (const char *)strMonth(dt.month));
                break;
            case 'M':
                strncat(formattedDate, (const char *)strMonth(dt.month), 3);
                break;
            case 't':
                sprintf(helper, "%d", daysInMonth(dt.year, dt.month));
                strcat(formattedDate, (const char *)helper);
                break;

            // Year decoder
            case 'Y':
                sprintf(helper, "%d", dt.year); 
                strcat(formattedDate, (const char *)helper); 
                break;
            case 'y': sprintf(helper, "%02d", dt.year-2000);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'L':
                sprintf(helper, "%d", isLeapYear(dt.year)); 
                strcat(formattedDate, (const char *)helper); 
                break;

            // Hour decoder
            case 'H':
                sprintf(helper, "%02d", dt.hour);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'G':
                sprintf(helper, "%d", dt.hour);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'h':
                sprintf(helper, "%02d", hour12(dt.hour));
                strcat(formattedDate, (const char *)helper);
                break;
            case 'g':
                sprintf(helper, "%d", hour12(dt.hour));
                strcat(formattedDate, (const char *)helper);
                break;
            case 'A':
                strcat(formattedDate, (const char *)strAmPm(dt.hour, true));
                break;
            case 'a':
                strcat(formattedDate, (const char *)strAmPm(dt.hour, false));
                break;

            // Minute decoder
            case 'i': 
                sprintf(helper, "%02d", dt.minute);
                strcat(formattedDate, (const char *)helper);
                break;

            // Second decoder
            case 's':
                sprintf(helper, "%02d", dt.second); 
                strcat(formattedDate, (const char *)helper); 
                break;

            // Misc decoder
            case 'U': 
                sprintf(helper, "%lu", dt.unixtime);
                strcat(formattedDate, (const char *)helper);
                break;

            default: 
                strncat(formattedDate, dateFormat, 1);
                break;
        }
        dateFormat++;
    }

    return formattedDate;
}

// Prints the datetime with a given format
char const * DS3231::dateFormat(char* formattedDate, const char* dateFormat, RTCAlarmTime dt)
{
    formattedDate[0] = 0;

    char helper[11];

    while (*dateFormat != '\0')
    {
        switch (dateFormat[0])
        {
            // Day decoder
            case 'd':
                sprintf(helper, "%02d", dt.day); 
                strcat(formattedDate, (const char *)helper); 
                break;
            case 'j':
                sprintf(helper, "%d", dt.day);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'l':
                strcat(formattedDate, (const char *)strDayOfWeek(dt.day));
                break;
            case 'D':
                strncat(formattedDate, strDayOfWeek(dt.day), 3);
                break;
            case 'N':
                sprintf(helper, "%d", dt.day);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'w':
                sprintf(helper, "%d", (dt.day + 7) % 7);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'S':
                strcat(formattedDate, (const char *)strDaySufix(dt.day));
                break;

            // Hour decoder
            case 'H':
                sprintf(helper, "%02d", dt.hour);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'G':
                sprintf(helper, "%d", dt.hour);
                strcat(formattedDate, (const char *)helper);
                break;
            case 'h':
                sprintf(helper, "%02d", hour12(dt.hour));
                strcat(formattedDate, (const char *)helper);
                break;
            case 'g':
                sprintf(helper, "%d", hour12(dt.hour));
                strcat(formattedDate, (const char *)helper);
                break;
            case 'A':
                strcat(formattedDate, (const char *)strAmPm(dt.hour, true));
                break;
            case 'a':
                strcat(formattedDate, (const char *)strAmPm(dt.hour, false));
                break;

            // Minute decoder
            case 'i': 
                sprintf(helper, "%02d", dt.minute);
                strcat(formattedDate, (const char *)helper);
                break;

            // Second decoder
            case 's':
                sprintf(helper, "%02d", dt.second); 
                strcat(formattedDate, (const char *)helper); 
                break;

            default: 
                strncat(formattedDate, dateFormat, 1);
                break;
        }
        dateFormat++;
    }
    return formattedDate;
}
