void initNeoColor(){  
  NeoColorHH_index = readNeoColor(addrNeoColorHH);
  NeoColorMM_index = readNeoColor(addrNeoColorMM);
  NeoColorMM10_index = readNeoColor(addrNeoColorMM10);
  HMClock.changeColorHH(NeoColor[NeoColorHH_index][0],NeoColor[NeoColorHH_index][1],NeoColor[NeoColorHH_index][2]);
  HMClock.changeColorMM(NeoColor[NeoColorMM_index][0],NeoColor[NeoColorMM_index][1],NeoColor[NeoColorMM_index][2]);
  HMClock.changeColorMM10(NeoColor[NeoColorMM10_index][0],NeoColor[NeoColorMM10_index][1],NeoColor[NeoColorMM10_index][2]);
}

#ifdef HMdebug
  void dbgSerial(String  prtStr){
    Serial.println(prtStr);
  }
  
  void dbgSerial(String  prtStr, int prtNum){
    Serial.print(prtStr);
    Serial.println(prtNum);
  }
  
  void dbgSerial(String  prtStr1, int prtNum1, String prtStr2, int prtNum2){
    Serial.print(prtStr1);
    Serial.print(prtNum1);
    Serial.print(prtStr2);
    Serial.println(prtNum2);
  }
#else
  void dbgSerial(String  prtStr1, int prtNum1, String prtStr2, int prtNum2){}
  void dbgSerial(String  prtStr, int prtNum){}
  void dbgSerial(String  prtStr){}
#endif

void errStateLED(boolean ledState){
  if(!ledState){
    Timer1.initialize(500000);
    Timer1.attachInterrupt(errLed );
  } else {
    Timer1.detachInterrupt();
    digitalWrite(rtcErrLED_Pin,LOW);
  }
}


/// --------------------------
/// Custom ISR Timer : error indicate led
/// --------------------------
void errLed()
{
    // RTC Error LED
    //if(!errReadRTC(errStateHMC,errRTCBit)) 
    digitalWrite(rtcErrLED_Pin, digitalRead( rtcErrLED_Pin ) ^ 1 );
}





