/** mbed library for driving the MAXIM DS3231 Real Time Clock
datasheet link : http://datasheets.maximintegrated.com/en/ds/DS3231.pdf
WARNING : sda and sdl should be pulled up with 2.2k resistor
based on the codes of 
2012 Remi Cormier
2014 Korneliusz Jarzebski 
www.jarzebski.pl

By Qbit-, 11.2019
www.github.com/qbit-

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mbed.h"
#include "DS3231.h"

// Serial monitor
Serial pc(USBTX, USBRX);

// I2C bus
#define I2C_1_SDA p9
#define I2C_1_SCL p10

I2C i2c(I2C_1_SDA, I2C_1_SCL);
DS3231 RTC(p28,p27);

int hour, minute, second;
int dayOfWeek, date, month, year;  

int main(){
     printf("\r\n\nDS3231 Library test program\r\nQbit- 2019\r\n\n");
     RTC.setI2Cfrequency(400000);
    
     //RTC.writeRegister(DS3231_Aging_Offset,0); // uncomment to set Aging Offset 1LSB = approx. 0.1 ppm according from datasheet = 0.05 ppm @ 21 °C from measurments
     
     RTC.convertTemperature();
      
     int reg=RTC.readRegister(DS3231_Aging_Offset);
     if (reg>127)
        {reg=reg-256;}
     pc.printf("Aging offset : %i\r\n",reg);
         
     pc.printf("OSF flag : %i",RTC.OSF());
     pc.printf("\r\n");
     
     RTC.readDate(&date,&month,&year);
     pc.printf("date : %02i-%02i-%02i",date,month,year);
     pc.printf("\r\n");
     
     //RTC.setTime(19,48,45); // uncomment to set time
     
     RTC.readTime(&hour,&minute,&second);
     pc.printf("time : %02i:%02i:%02i",hour,minute,second);
     pc.printf("\r\n");
     
     //RTC.setDate(6,22,12,2012); // uncomment to set date
     
     RTC.readDateTime(&dayOfWeek,&date,&month,&year,&hour,&minute,&second);
     pc.printf("date time : %i / %02i-%02i-%02i %02i:%02i:%02i",dayOfWeek,date,month,year,hour,minute,second);
     pc.printf("\r\n");
     
     pc.printf("temperature :%6.2f",RTC.readTemp());
     pc.printf("\r\n");

     // RTC.setDateTime(1573979220);  // uncomment to set datetime from a linux timestamp
     pc.printf("Unix timestamp :%lu", RTC.readUnixtime(););
     pc.printf("\r\n");

     // Read time into a structure and print using given format
     RTCDateTime dt = RTC.readDateTime();  
     char* datetime = new char[20];
     datetime[19] = '\0';
     pc.printf("Formatted datetime : %s", RTC.dateFormat(datetime, "d F Y H:i:s", dt));
     pc.printf("\r\n");
}  
