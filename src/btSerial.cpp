/*
 * Bluetooth를 이용한 시간 설정
 */

#ifdef BTSerialPort
void setTimeWithBTSerial(){
  static time_t tLast;
  time_t t;
  tmElements_t tm;

  //check for input to set the RTC, minimum length is 12, i.e. yy,m,d,h,m,s
  if (btSerial.available() >= 12) {
      //note that the tmElements_t Year member is an offset from 1970,
      //but the RTC wants the last two digits of the calendar year.
      //use the convenience macros from Time.h to do the conversions.
      int y = btSerial.parseInt();
      if (y >= 100 && y < 1000)
          Serial << F("Error: Year must be two digits or four digits!") << endl;
      else {
          if (y >= 1000)
              tm.Year = CalendarYrToTm(y);
          else    //(y < 100)
              tm.Year = y2kYearToTm(y);
          tm.Month = btSerial.parseInt();
          tm.Day = btSerial.parseInt();
          tm.Hour = btSerial.parseInt();
          tm.Minute = btSerial.parseInt();
          tm.Second = btSerial.parseInt();
          t = makeTime(tm);
          //use the time_t value to ensure correct weekday is set
          if(RTC.set(t) == 0) { // Success
            setTime(t);
            Serial << F("RTC set to: ");
            printDateTime(t);
            Serial << endl;
          }
          else
            Serial << F("RTC set failed!") << endl;
          //dump any extraneous input
          while (btSerial.available() > 0) btSerial.read();
      }
  }
}

#endif

