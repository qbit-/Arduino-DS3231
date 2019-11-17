#include "mbed.h"

#ifndef MBED_DS3231_H
#define MBED_DS3231_H

//DS3231 8 bit adress
#define DS3231_Address          0xD0

//DS3231 registers
#define DS3231_Seconds          0x00
#define DS3231_Minutes          0x01
#define DS3231_Hours            0x02
// DS3231 Hours bits
#define DS3231_bit_AM_PM        0x20
#define DS3231_bit_12_24        0x40

#define DS3231_DayOfWeek        0x03
#define DS3231_Day              0x04
#define DS3231_Month_Century    0x05
#define DS3231_Year             0x06
#define DS3231_Alarm1_Seconds   0x07
#define DS3231_Alarm1_Minutes   0x08
#define DS3231_Alarm1_Hours     0x09
#define DS3231_Alarm1_Day_Date  0x0A
#define DS3231_Alarm2_Minutes   0x0B
#define DS3231_Alarm2_Hours     0x0C
#define DS3231_Alarm_2_Day_Date 0x0D

#define DS3231_Control          0x0E
// DS3231 Control bits
#define DS3231_bit_A1IE        1
#define DS3231_bit_A2IE        2
#define DS3231_bit_INTCN       4
#define DS3231_bit_SQW_1Hz     0
#define DS3231_bit_SQW_1024Hz  8
#define DS3231_bit_SQW_4096Hz 16
#define DS3231_bit_SQW_8192Hz 24
#define DS3231_bit_CONV       32
#define DS3231_bit_BBSQW      64
#define DS3231_bit_EOSCb     128


#define DS3231_Control_Status   0x0F
// DS3231 Control/Status bits
#define DS3231_bit_BSY     0x04
#define DS3231_bit_EN32kHz 0x08
#define DS3231_bit_OSF     0x80

#define DS3231_Aging_Offset     0x10
#define DS3231_MSB_Temp         0x11
#define DS3231_LSB_Temp         0x12

// Structures for datetime manipulation
#ifndef RTCDATETIME_STRUCT_H
#define RTCDATETIME_STRUCT_H
struct RTCDateTime
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int dayOfWeek;
    uint32_t unixtime;
};

struct RTCAlarmTime
{
    int8_t day;
    int8_t hour;
    int8_t minute;
    int8_t second;
};
#endif

/* Interface to MAXIM DS3231 RTC */
class DS3231{
public :
     /** Create an instance of the DS3231 connected to specfied I2C pins
     *
     * @param sda The I2C data pin
     * @param scl The I2C clock pin
     */
     DS3231(I2C& bus);
     
     /** set I2C bus speed
     * @param frequency : I2C clocl frequenct (Hz)
     */
     void setI2Cfrequency(int frequency);
     
     /** Read the temperature
     *
     * @return The temperature
     */
     float readTemperature();
     
     /** Read the time registers
     * @param hours
     * @param minutes
     * @param seconds
     */
     void readTime(int *hours, int *minutes, int *seconds);
     
     /** Set the time registers
     * @param hours
     * @param minutes
     * @param seconds
     */
     void setTime(int hours, int minutes, int seconds);

     /** Read hour register
      *
     */
     int readHours();

     /** Set hour register
     * @param hours
     */
     void setHours(int hours);

     /** Read minutes register
      *
     */
     int readMinutes();

     /** Set minutes register
     * @param minutes
     */
     void setMinutes(int minutes);

     /** Read seconds register
      *
     */
     int readSeconds();

     /** Read seconds register
     * @param seconds
     */
     void setSeconds(int seconds);

     /** force temperature conversion
     * 
     */
     void convertTemperature();

     /** Read the date registers
     * @param date
     * @param month
     * @param year
     */
     void readDate(int *date, int *month, int *year);
     
     /** Set the date registers
     * @param dayOfWeek : day of week
     * @param date
     * @param month
     * @param year
     */
     void setDate(int dayOfWeek, int day, int month, int year);
     
     /** Read the date and time registers
     * @param dayOfWeek : day of week
     * @param day
     * @param month
     * @param year
     * @param hours
     * @param minutes
     * @param seconds
     */
     void readDateTime(int *dayOfWeek, int *day, int *month, int *year, int *hours, int *minutes, int *seconds);

     /** Read the day of week register
     *
     */
     int readDayOfWeek();
     
     /** Set the day of week register
     * @param dow
     */
     void setDayOfWeek(int dow);

     /** Read the day register
     *
     */
     int readDay();
     
     /** Set the day register
     * @param day
     */
     void setDay(int day);
     
     /** Read the month_century register
     *   returns the month only
     */
     int readMonth();
     
     /** Set the month_century register
     * @param month
     * @param century, default 1
     */
      void setMonth(int month, int century=1);

     /** Read the year and month_century registers
     *   returns the year only
     */
     int readYear();
     
     /** Set the year and month_century register
     * @param year
     */
      void setYear(int year);

     /** Read a register
     * @param reg : register address
     * @return The register content
     */     
     int readRegister(char reg);
     
     /** Write to a register
     * @param reg : register address
     * @param The register content
     */       
     void writeRegister(int reg, char byte);
     
     /** set OSF (Oscillator Stop Flag) bit to 0 in Control Status register
     * should be done just after power up DS3231
     * OSF bit is automaticaly set to 1 when on power up or when the DS3231 oscillator stops
     */
     void eraseOSF();
     
     /** Return OSF bit. If true the oscillator stopped or the DS3231 just powered up
     * @return The OSF bit
     */
     bool OSF();
     
     bool error;

      /***************************************************
      * Time utility functions. Can be deleted if unneded
      ****************************************************/

     /** Calculates unixtime from current contents of RTCDateTime
      *  structure
      * @param day
      * @param month
      * @param year
      * @param hours
      * @param minutes
      * @param seconds
      */
      uint32_t unixtime(int day, int month, int year, int hours, int minutes, int seconds);

     /** Read the date and time registers and return a struct
     *
     */
     RTCDateTime readDateTime();

     /** Read the date and time registers and return a struct
     *
     */
     uint32_t readUnixtime();

     /** Sets the time using unixtime
     * @param t 
     */
     void setDateTime(uint32_t t);

      /** Prints time according to the format string 
       * @param dateFormat
       * @param dt
      */
      char const * dateFormat(char* formattedDate, const char* dateFormat, RTCDateTime dt);

      /** Prints time according to the format string 
       * @param dateFormat
       * @param dt
      */
     char const * dateFormat(char* formattedDate, const char* dateFormat, RTCAlarmTime dt);

private :
     I2C& i2c;
      
     int bcd2dec(int k); // bcd to decimal conversion
     int dec2bcd(int k); // decimal to bcd conversion
     void decodeTime(int regHours, int regMinutes, int regSeconds,int *Hours, int *Minutes, int *Seconds);
     void decodeDate(int regDate,int regMonth, int regYear, int *date, int *month, int *year);
   
  /**********************************************************
   * Additional utility functions for datetime manipulation
   * Can be deleted if unneded
   ***********************************************************/
     /** Calculates the number of seconds 
      * @param days
      * @param hours
      * @param minutes
      * @param seconds
      */
     long time2long(uint16_t days, int hours, int minutes, int seconds);

     /** Calculates the number of days since date
      * @param year
      * @param month
      * @param day
      */
     uint16_t date2days(int year, int month, int day);

     /** Checks if year is leap year
      */
     bool isLeapYear(int year);

     /** Calculate the day of week
      * @param year
      * @param month
      * @param day
      */
     int dow(uint16_t year, int month, int day);

      /** Prints the name of the day of week
       * @param dayOfWeek
      */
     char const *strDayOfWeek(int dayOfWeek);

      /** Prints the name of the month
       * @param month
      */
     char const *strMonth(int month);

      /** Prints the AM/PM for given hour
       * @param month
      */     
     char const *strAmPm(int hour, bool uppercase);

      /** Prints the suffix of the day
       * @param month
      */     
     char const *strDaySufix(int day);

      /** Returns 12 hours part of 24 hours
       * @param hour24
       */
     int hour12(int hour24);

      /** Returns the number of days in month
       * @param year
       * @param month
       */
     int daysInMonth(uint16_t year, int month);

      /** Returns the absulute number of the day in a year
       * @param year
       * @param month
       * @param day
      */
     uint16_t dayInYear(uint16_t year, int month, int day);
};

#endif
