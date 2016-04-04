void updateButton(){
  btnMODE.update();
  btnSET.update();
}
void initButton(){
  
  // Configure the button as you'd like - not necessary if you're happy with the defaults
  btnMODE.configureButton(configurePushButton);
  btnSET.configureButton(configurePushButton);

  // When the button is first pressed, call the function onButtonPressed (further down the page)
  btnMODE.onPress(onButtonPressed);
  btnSET.onPress(onButtonPressed);

  // Once the button has been held for 1 second (1000ms) call onButtonHeld. Call it again every 0.5s (500ms) until it is let go
  btnMODE.onHoldRepeat(1000, 500, onButtonHeld);
  btnSET.onHoldRepeat(1000, 500, onButtonHeld);
  
  // When the button is released, call onButtonReleased
  btnMODE.onRelease(onButtonReleased);
  btnSET.onRelease(onButtonReleased);
}

void configurePushButton(Bounce& bouncedButton){

  // Set the debounce interval to 15ms - default is 10ms
  bouncedButton.interval(15);
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressed(Button& btn){

  Serial.print("HMC Mode = ");
  Serial.println(HMCMode);

  uint8_t tempNeoColor_index;

  if(btn.is(btnMODE)){
    Serial.println("Button Mode pressed");
    
    EscapeTime=millis();
    
    switch (HMCMode){
      case(ClockMode):
        Serial.println("Clock Mode");
        //onAlarm =0;                                       

        if(onAlarm){ 
          onAlarm = false;                    // 알람해제 버튼
        } else {
          displayAlarmTimeMode=true;
        }
        releaseTempTime=millis();        
        break;
      case(TimeHHSetMode):
        Serial.println("change to Time Minute Set Mode");
        
        // 변경된 시간을 DS1302에 저장
        tm.Hour = temptm.Hour;
        t = makeTime(tm);
        if(RTC.set(t) ==0){
          setTime(t);
          Serial << F("RTC set to: ");
          printDateTime(t);
          Serial << endl;
        }
                        
        HMClock.clearNeoHH();
        HMClock.clearLastTime();
        HMClock.displayNeoMM(temptm.Minute);
        HMCMode = TimeMMSetMode;
        releaseTempTime=millis();        // releaseAlarmTime만큼만 셋팅모드가 동작. releaseAlarmTime시간만큼 키입력이 없으면 Clock Mode로 자동변경
        break;
      case(TimeMMSetMode):
        Serial.println("Time Minute Set Mode");

        // 변경된 분을 DS1302에 저장
        tm.Minute = temptm.Minute;
        t = makeTime(tm);
        if(RTC.set(t) ==0){
          setTime(t);
          Serial << F("RTC set to: ");
          printDateTime(t);
          Serial << endl;
        }
        
        alarmHH = readAlarmHH();
        HMClock.clearNeoMM();
        HMClock.displayNeoHH(alarmHH);
        HMCMode = AlarmHHSetMode;
        releaseTempTime=millis();
        break;
      case(AlarmHHSetMode):
        Serial.println("Alarm Hour Set Mode -> Mode Btn");
        HMCMode = AlarmMMSetMode;
        alarmMM = readAlarmMM();
        Serial.print("Mode alarmMM =");
        Serial.println(alarmMM);
        HMClock.clearNeoHH();
        HMClock.clearLastTime();
        HMClock.displayNeoMM(alarmMM);
        releaseTempTime=millis();
        break;
     case(AlarmMMSetMode):
        Serial.println("Alarm Munite Set Mode -> Mode Btn");
        HMCMode = ColorHHSetMode;
        
        HMClock.clearNeoMM();
        HMClock.displayAllNeoHH();
               
        releaseTempTime=millis();
        break;
      case(ColorHHSetMode):
        Serial.println("Color Hour Set  Mode -> Mode Btn");
        HMCMode = ColorMM10SetMode;

        HMClock.clearNeoHH();
        HMClock.clearNeoMM();
        HMClock.displayAllNeoMM10();
        
        releaseTempTime=millis();
        break;
      case(ColorMM10SetMode):                 // 2016.1.22일 수정 MM10 컬러 수정하기
        Serial.println("Color Minute 10 Set Mode -> Mode Btn");
        HMCMode = ColorMMSetMode;

        HMClock.clearNeoHH();
        HMClock.clearNeoMM10();
        HMClock.displayAllNeoMM();
        
        releaseTempTime=millis();
        break;
      case(ColorMMSetMode):
        Serial.println("Color Minutee Set  Mode -> Mode Btn");
        HMCMode = ClockMode;
        HMClock.clearLastTime();
        HMClock.clearNeoHH();
        HMClock.clearNeoMM();
        displayClock();
        releaseTempTime=millis();
        break;
      default:
        break;
    }
  }

  if(btn.is(btnSET)){
    Serial.println("Button Set pressed");
    EscapeTime=millis();

    switch (HMCMode){
      case(ClockMode):
        Serial.println("Clock Mode -> Set Btn");
        onAlarm =0;                                       // 알람해제 버튼
        releaseTempTime=millis();
        break;
      case(TimeHHSetMode):
        Serial.println("Time Set Mode -> Set Btn");
        ++temptm.Hour;
        
        if(temptm.Hour > 24 ){
          temptm.Hour = 0;
        } else if(temptm.Hour == 24){
          temptm.Hour = 0;
          HMClock.displayNeoHH(temptm.Hour);
          dbgSerial(" temptm =", temptm.Hour);
        } else {
          HMClock.displayNeoHH(temptm.Hour);
        }

        dbgSerial(" temptm =", temptm.Hour);
        releaseTempTime=millis();
        break;
      case(TimeMMSetMode):
        Serial.println("Time Set Mode -> Set Btn");

#ifdef led4X4
        temptm.Minute +=5;                 // 오분씩 증가
#endif

#ifdef led6X6
        ++temptm.Minute;                   // 일분씩 증가
#endif

        if(temptm.Minute >= 60 ){
          temptm.Minute = 0;
          HMClock.displayNeoMM(temptm.Minute);
        } else {
          HMClock.displayNeoMM(temptm.Minute);
        }
        
        dbgSerial("temptm.Minute =", temptm.Minute);
        releaseTempTime=millis();
        break;
      case(AlarmHHSetMode):
        Serial.println("Alarm Set Mode -> Set Btn");
                
        ++alarmHH;
        
        if(alarmHH > 23 ){
          alarmHH = 0;
          //HMClock.displayNeoHH(temptm.Hour);
        }
        else if(alarmHH == 24){
          alarmHH = 12;
        }
        
        HMClock.displayNeoHH(alarmHH);
        setAlarmHH(alarmHH);
        
        dbgSerial("Alarm Hour = ", alarmHH,":",alarmMM );
        releaseTempTime=millis();
        break;
     case(AlarmMMSetMode):
        Serial.println("Alarm Set Mode -> Set Btn");

        if(alarmMM >= 60) alarmMM = 0;
        
#ifdef led4X4
        alarmMM+=5                 // 오분씩 증가
#endif

#ifdef led6X6
        ++alarmMM;                   // 일분씩 증가
#endif
                
        HMClock.displayNeoMM(alarmMM);
        setAlarmMM(alarmMM);
        
        dbgSerial("Alarm Hour = ", alarmHH,":",alarmMM );

        releaseTempTime=millis();
        break;
      case(ColorHHSetMode):
        Serial.println("Color Set  Mode -> Set Btn");
        Serial.print("Color Index = ");
        Serial.println(NeoColorHH_index);
                
        tempNeoColor_index=readNeoColor(addrNeoColorHH);
        
        if(tempNeoColor_index >= 11) tempNeoColor_index = 0; 
        
        ++tempNeoColor_index;
        
        HMClock.changeColorHH(NeoColor[tempNeoColor_index][0],NeoColor[tempNeoColor_index][1],NeoColor[tempNeoColor_index][2]);
        EEPROM.write(addrNeoColorHH,tempNeoColor_index);

        HMClock.clearNeoMM();
        HMClock.clearNeoMM10();
        HMClock.displayAllNeoHH();
                
        releaseTempTime=millis();
        break;
      case(ColorMM10SetMode):                                       // 2016.1.22 추가
        Serial.println("Color Set 10 Minute Set Mode -> Set Btn");

        tempNeoColor_index=readNeoColor(addrNeoColorMM10);
        if(tempNeoColor_index >= 11) tempNeoColor_index = 0;        // 11 : Neo Color Array first index size
        ++tempNeoColor_index;
                
        HMClock.changeColorMM10(NeoColor[tempNeoColor_index][0],NeoColor[tempNeoColor_index][1],NeoColor[tempNeoColor_index][2]);
        EEPROM.write(addrNeoColorMM10,tempNeoColor_index);

        HMClock.clearNeoHH();
        HMClock.clearNeoMM();
        HMClock.displayAllNeoMM10();
        
        releaseTempTime=millis();
        break;
      case(ColorMMSetMode):
        Serial.println("Color Set  Mode -> Set Btn");

        tempNeoColor_index=readNeoColor(addrNeoColorMM);
        if(tempNeoColor_index >= 11) tempNeoColor_index = 0;        // 11 : Neo Color Array first index size
        ++tempNeoColor_index;
                
        HMClock.changeColorMM(NeoColor[tempNeoColor_index][0],NeoColor[tempNeoColor_index][1],NeoColor[tempNeoColor_index][2]);
        EEPROM.write(addrNeoColorMM,tempNeoColor_index);

        HMClock.clearNeoHH();
        HMClock.clearNeoMM10();
        HMClock.displayAllNeoMM();
        
        releaseTempTime=millis();
        break;
      default:
        break;
    }
  }
}

// duration reports back how long it has been since the button was originally pressed.
// repeatCount tells us how many times this function has been called by this button.
void onButtonHeld(Button& btn, uint16_t duration, uint16_t repeatCount){

  Serial.print("button has been held for ");
  Serial.print(duration);
  Serial.print(" ms; this event has been fired ");
  Serial.print(repeatCount);
  Serial.println(" times");
  
  if(btnSET.isPressed()){
    Serial.println("Button OK is also pressed");
    if(repeatCount == 5){                        // Clock 모드에서 길게눌렸을 경우 알람 셋팅의 변경
      Serial.println("AlarmMode is changed");
      setAlarm = !setAlarm;
      EEPROM.write(addrSetAlarm, setAlarm);     // 알람모드의 현재 설정을 EEPROM에 저장, Address(addrSetAlarm)는 10
      dbgSerial("AlarmMode is ",setAlarm);
    }
  }

  if(btnMODE.isPressed()){
    Serial.println("Button SET is also pressed");
    if(repeatCount > 5){
      Serial.println("Time Set Mode");
      HMCMode = TimeHHSetMode;
      temptm = tm;
      HMClock.clearNeoMM();
      
      releaseTempTime = millis();
    }
  }
}

void releaseSetMode(){
  if(HMCMode != ClockMode){
    if((millis() - releaseTempTime) > releaseSetModeTime){     //releaseSetModeTime 초 이상 입력이 없을 경우 Clock Mode로 전환
      HMCMode = ClockMode;      
      releaseTempTime = millis();
      Serial.println("Clockmode");
      HMClock.clearLastTime();
      displayClock();
    }
  } else {
    if((millis() - releaseTempTime) > releaseAlarmDisplayTime){     //releaseAlarmDisplayTime 시간동안만 알람 시간을 표시함
      displayAlarmTimeMode =false;
    }
  }
}

// duration reports back the total time that the button was held down
void onButtonReleased(Button& btn, uint16_t duration){

  Serial.print("button released after ");
  Serial.print(duration);
  Serial.println(" ms");

  if (btn.is(btnMODE)){
    dbgSerial("Button UP is released");
  }

  if (btn.is(btnSET)){
    dbgSerial("Button Set is released");
  }

}

