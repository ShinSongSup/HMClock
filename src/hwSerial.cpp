/*
 * 시리얼 포트을 이용한 시간 설정
 */

#ifdef HWSerialPort
void setTimeWithSerial(){
  static time_t tLast;
  time_t t;
  tmElements_t tm;
  
  //check for input to set the RTC, minimum length is 12, i.e. yy,m,d,h,m,s
  if (Serial.available() >= 12) {
      //note that the tmElements_t Year member is an offset from 1970,
      //but the RTC wants the last two digits of the calendar year.
      //use the convenience macros from Time.h to do the conversions.
      int y = Serial.parseInt();
      if (y >= 100 && y < 1000)
          Serial << F("Error: Year must be two digits or four digits!") << endl;
      else {
          if (y >= 1000)
              tm.Year = CalendarYrToTm(y);
          else    //(y < 100)
              tm.Year = y2kYearToTm(y);
          tm.Month = Serial.parseInt();
          tm.Day = Serial.parseInt();
          tm.Hour = Serial.parseInt();
          tm.Minute = Serial.parseInt();
          tm.Second = Serial.parseInt();
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
          while (Serial.available() > 0) Serial.read();
      }
  }
}
#endif
