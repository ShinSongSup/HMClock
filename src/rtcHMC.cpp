/*
 * RTC에 관련된 화일모음
 */

/*
 * RTC 시간 확인
 */
void checkRTC(){
  
  Serial.println("DS1302RTC RTC");
  Serial.println("-------------------");
  delay(500);
  
  if (RTC.haltRTC()) {
    Serial.println("The DS1302 is stopped.  Please run the SetTime");
    Serial.println("example to initialize the time and begin running.");
    Serial.println();
  }

  if (!RTC.writeEN()) {
    Serial.println("The DS1302 is write protected. This normal.");
    Serial.println();
  }

/*
  //setSyncProvider() causes the Time library to synchronize with the
  //external RTC by calling RTC.get() every five minutes by default.
  setSyncProvider(RTC.get);

  Serial.print("RTC Sync");
  if (timeStatus() == timeSet){
    dbgSerial(" Ok!");
    errStateLED(true);
  } else {
    dbgSerial(" FAIL!");
    errStateLED(false);
  }
  */
}

/*
//set/clear RTC error bit
void errWriteRTC(boolean err)
{
    if(err) bitWrite(errStateHMC, errRTCBit, 1);
    else bitWrite(errStateHMC, errRTCBit, 0);
}


//Read RTC error bit
boolean errReadRTC()
{
    return bitRead(errStateHMC, errRTCBit);
}
*/

//print date and time to Serial
void printDateTime(time_t t)
{
    printDate(t);
    Serial << ' ';
    printTime(t);
}

//print time to Serial
void printTime(time_t t)
{
    printI00(hour(t), ':');
    printI00(minute(t), ':');
    printI00(second(t), ' ');
}

//print date to Serial
void printDate(time_t t)
{
    printI00(day(t), 0);
    Serial << monthShortStr(month(t)) << _DEC(year(t));
}

//Print an integer in "00" format (with leading zero),
//followed by a delimiter character to Serial.
//Input value assumed to be between 0 and 99.
void printI00(int val, char delim)
{
    if (val < 10) Serial << '0';
    Serial << _DEC(val);
    if (delim > 0) Serial << delim;
    return;
}

/*
//Tickle Cahrger
void RTC_Tickle_chagEN(){
  RTC.writeEN(true);
  RTC.writeRTC(DS1302_TRICKLE,DS1302_TCR_1D8K);
  RTC.writeEN(false);
}
*/

