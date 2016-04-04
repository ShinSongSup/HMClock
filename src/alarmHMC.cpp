/*
 * Alarm에 관련된 함수 모음
 */

boolean initAlarmHMC(){
  alarmHH =readAlarmHH();
  alarmMM =readAlarmMM();
  if(alarmHH == 255  && alarmMM == 255) return false;
  else return true;
  // 시간과 분의 범위값이 아니면 리턴값을 false 로 보내주는 코드 추가해야 함
}
/*
 * Alarm LED를 알람 설정상태에 따라 켜고 끈다.
 */
void writeAlarmLED(){
  if(setAlarm) HMClock.displayAlarmLED(ledAlarm, 0,0,100);
  else HMClock.clearAlarmLED(ledAlarm);
}

void displayAlarmTime(){
  HMClock.displayNeoHH(alarmHH);
  HMClock.displayNeoMM(alarmMM);
}

/* 
 * 알람 LED 점멸하기 
 */
void blinkAlarmLED(){
  Timer1.initialize(200000);
  Timer1.attachInterrupt(ISRblinkAlarmLED);
}

void noBlinkAlarmLED(){
  Timer1.detachInterrupt();
  stateAlarmLED = readSetAlarm();
}

void ISRblinkAlarmLED()
{
  
  if(stateAlarmLED){
    HMClock.displayAlarmLED(ledAlarm, 0,0,100);
    stateAlarmLED=!stateAlarmLED;
  } else {
    HMClock.clearAlarmLED(ledAlarm);
    stateAlarmLED=!stateAlarmLED;
  }
}

void setAlarmHH(byte clockHH){
  EEPROM.write(addrAlarmHH, clockHH);
}

void setAlarmMM(byte clockMM){
  EEPROM.write(addrAlarmMM, clockMM);
}

void clearAlarmHMC(){
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0xFF);
  }
}

uint8_t readEEPROM(uint8_t addrEEPROM){
  return EEPROM.read(addrEEPROM);
}

//void writeEEPROM(uint8_t addrEEPROM, uint8_t WriteData){
//  EEPROM.write((addrEEPROM, WriteData);
//}


byte readAlarmHH(){
  return EEPROM.read(addrAlarmHH);
}

byte readAlarmMM(){
  return EEPROM.read(addrAlarmMM);
}

byte readSetAlarm(){
  return EEPROM.read(addrSetAlarm);
}

byte readNeoColor(byte addrEEPROM){
  return EEPROM.read(addrEEPROM);
}

void playAlarm(){
  Timer1.initialize(200000);
  Timer1.pwm(BZ_Pin,50);
}

void stopAlarm(){
  Timer1.stop();
}


