
//DS1302:  CE pin    -> Arduino Digital 2
//          I/O pin   -> Arduino Digital 3
//          SCLK pin  -> Arduino Digital 4

// Neopixel pin -> Arduino Digital 6

#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "HMClock_Conf.h"
#include "displayClock.h"
#include <EEPROM.h>
#include <Streaming.h>        //http://arduiniana.org/libraries/streaming/
#include <Time.h>             //http://playground.arduino/Code/Time
#include <DS1302RTC.h>
#include <Button.h>
#include <ButtonEventCallback.h>
#include <PushButton.h>
#include <TimerOne.h>
#include <SoftwareSerial.h>

// DS1302 핀 할당
#define RTC_CE 2
#define RTC_IO 3
#define RTC_CLK 4

// HMC 모드
#define ClockMode 0               // 시계 모드
#define TimeHHSetMode 1           // 시간 설정 모드
#define TimeMMSetMode 2           // 분 설정 모드
#define AlarmHHSetMode 3          // 알람 시간 설정 모드
#define AlarmMMSetMode 4          // 알람 분 설정 모드
#define ColorHHSetMode 5          // 시간 컬러 설정 모드
#define ColorMM10SetMode 6
#define ColorMMSetMode 7          // 분 컬러 설정 모드

//#define errLed 13

// NeoPixel 핀 할당
//#define NeoPixelPIN 6             // Neo Pixel 핀 번호
//#define NeoPixelNum 25            // Neo Pixel 수

// 버튼 핀 할당
#define btnMODE_Pin 8             // Mode 핀
#define btnSET_Pin 9             // Set 핀

// 부저 핀 할당
#define BZ_Pin 10                  // 부저 핀

// 소프트웨어 시리얼 포트 설정
#define swSerial_Rx_Pin 11
#define swSerial_Tx_Pin 12

// 에러관련 설정
#define errRTCBit  0                // RTC 에러 비트
#define rtcErrLED_Pin 13            // RTC 오류 LED 핀

// 핀 객체 생성
PushButton btnMODE = PushButton(btnMODE_Pin, ENABLE_INTERNAL_PULLUP);
PushButton btnSET = PushButton(btnSET_Pin, ENABLE_INTERNAL_PULLUP);

// NeoPixel 객체 생성
displayClock HMClock=displayClock(NeoPixelNum, NeoPixelPIN, NEO_GRB + NEO_KHZ800);

// Software Serial 생성
SoftwareSerial btSerial(swSerial_Rx_Pin, swSerial_Tx_Pin);

// 시간공용체 생성
tmElements_t tm,lasttm,temptm;
time_t t;

// DS1302 RTC 객체 생성 
// Set pins:  CE( Digital 2), IO( Digital 3),CLK( Digital 4)
DS1302RTC RTC(RTC_CE,RTC_IO, RTC_CLK);

int alarm_note = 770;         // 알람 주파수
int alarm_duration = 100;     // 알람 반복 시간
long EscapeTime=0;
boolean state_tone=false;
boolean onNeo=false;          // 처음 켜지는 것을 확인하기 위한 레지스터(NEO 깜빡이는 것을 개선하기 위해)

//EEPROM address
uint8_t addrAlarmHH=0;          // 알람 : 시간
uint8_t addrAlarmMM=1;          // 알람 : 분
uint8_t addrSetAlarm = 10;      // 알람설정 EEPROM 주소
uint8_t addrNeoColorHH = 20;    // 네오컬러 : 시간
uint8_t addrNeoColorMM = 23;    // 네오컬러 : 분
uint8_t addrNeoColorMM10 = 26;  // 네오컬러 : 10분 단위

uint8_t alarmHH;
uint8_t alarmMM; 
boolean onAlarm =false;       //
boolean runAlarmON=false;     // 알람 도작상태
boolean setAlarm;             // 알람 설정상태
boolean displayAlarmTimeMode=false;
boolean stateAlarmLED = false;
//uint8_t stateAlarm;

// Error state
// 8 bit :     , 7 bit :    , 6 bit:     , 5 bit:   , 4 bit :     , 3 bit:    , 2bit:     , 1 bit: RTC error
uint8_t errStateHMC = 0x00;      //

unsigned long releaseAlarmTime = 120000;      // 알람지속시간, 120초 동안만 알람이 울림
unsigned long releaseSetModeTime = 60000;     // 셋팅모드 탈출시간(60초내에 버튼 입력이 없을 경우 시계모드로 변경됨
unsigned long tempTime;                       // 알람지속시간, 120초 동안만 알람이 울림
unsigned long releaseTempTime;                // 셋팅모드 탈출시간을 확인하기 위한 변수
unsigned long onDelayTime;                    // 알람해제 시간
uint8_t HMCMode = ClockMode;
uint8_t NeoColorHH_index;
uint8_t NeoColorMM_index;
uint8_t NeoColorMM10_index;

// 컬러 값
uint8_t NeoColor[][3]={  {255,0,0},             // R,G,B
                         {255,128,0},     
                         {255,255,0},     
                         {128,255,0},
                         {0,255,0},
                         {0,255,128},
                         {0,128,255},
                         {0,128,255},
                         {127,0,255},
                         {255,0,255},
                         {255,0,127}
                       };

void setup()
{
  // 시리얼 포트 설정
  Serial.begin(9600);
    
  // initialize Arduino pin assignment
  initArduinoPin();
  
  //NeoPixel initialize
  HMClock.begin();
  HMClock.show(); // Initialize all pixels to 'off'
  tempTime=millis();
  
  //SW 시리얼 설정
  //#ifdef BTSerialPort
    btSerial.begin(9600);
  //#endif

/*
  // RTC Tickle Charger Mode
  Serial.println("RTC Tickle Charger Mode");
  Serial.println(RTC.readRTC(0x91));
  RTC_Tickle_chagEN();
*/
  Serial.println(RTC.readRTC(0x91));
  //RTC.writeEN(true);
  //RTC.writeRTC(DS1302_TRICKLE,DS1302_TCR_2D8K);
  //RTC.writeEN(false);

  //Serial.println(RTC.readRTC(0x91));


  // RTC 확인 코드
  checkRTC();
   
  initNeoColor();
  
  initButton();

  if (! RTC.read(tm)) displayClock2Serial();  // display Clock to Serial Port

  // 저장되어 있는 알람 시간을 읽어옴. FF 값이 아니면 셋팅함
  if(!initAlarmHMC())setAlarm=false;
  else setAlarm = readSetAlarm;
  
  dbgSerial("Alarm Time = ",alarmHH,":",alarmMM);
  dbgSerial("Alarm set =",setAlarm=EEPROM.read(addrSetAlarm));

}

void loop(){

  // 시리얼 포트를 통한 시간설정
#ifdef HWSerialPort  
  setTimeWithSerial();
#endif

#ifdef BTSerialPort
  setTimeWithBTSerial();
#endif

  // 모드 선택
  if(HMCMode == ClockMode){
    if(!displayAlarmTimeMode) displayClock();
    else{
      displayAlarmTime();
      blinkAlarmLED();      
    }
  }

  // 버튼 인식
  updateButton();
  
  // onAlarm : 알람상태 변수, 
  //    true : 알람 ON, false : 알람 OFF
  if(onAlarm) playAlarm();
  else stopAlarm();

  // 설정모드에서 정해진 시간(
  releaseSetMode();
}

void print2digits(int number) {
  if (number >= 0 && number < 10)
    Serial.write('0');
  Serial.print(number);
}

void displayClock(){
    
  if (!RTC.read(tm)){
       
    errStateLED(true);      // RTC Erorr LED 

    HMClock.displayNeoHH(tm.Hour);  
    HMClock.displayNeoMM(tm.Minute);
    writeAlarmLED();
    
    if((millis()- tempTime) > 60000){
      tempTime = millis();
      displayClock2Serial();                // display Clock to Serial Port
      displayAlarm2Serial();                // display Alarm time to Serial Port
    }

    // setAlarm :  알람설정상태확인
    // runAlarmON : 알람울리고 있는 상태를 확인, 버튼을 이용하여 Off 하고 난후에 다시 false로 셋팅해야 1분내에 셋팅했을 경우 다시 알람이 울리는 것을 방지할 수 있음
    // onAlarm : 알람 부저 ON  
   
    if(setAlarm && !runAlarmON && !onAlarm){             
      if(alarmHH == tm.Hour && alarmMM == tm.Minute){
        onAlarm = true; 
        runAlarmON = true;
        Serial.println("Alarm ON");
        onDelayTime = millis();                       // 1분 이내에 해제되어도 다시 알람이 설정되지 않도록 하기 위한 시간
      }
    } else if(setAlarm && runAlarmON && onAlarm){     // 1분 이내에 알람이 해제되어도 다시 설정되지 않도록 해 주기 위함
      if((millis() - onDelayTime) > 61000){
        runAlarmON = false;
        Serial.println("DelayTime off");
      }
    } else if(setAlarm && !runAlarmON && onAlarm){   // 알람이 지속되는 시간, 120000 : 2분 동안 알람이 지속되고 2분 경과후 자동으로 알람해제
      if((millis() - onDelayTime) > releaseAlarmTime){
        onAlarm = false;
        Serial.println("Alarm off");
      }
    }
  } else {
    dbgSerial("DS1302 read error!  Please check the circuitry.");
    displayClock2Serial();                // display Clock to Serial Port
    errStateLED(false);
  }
}

/*
/*
 * displayed Clock to Serial Port
 
void displayClock2Serial(){
  Serial.print("Time = ");
  print2digits(tm.Hour);
  Serial.write(':');
  print2digits(tm.Minute);
  Serial.println();
}
*/

/*
 * displayed Clock to Serial Port
 */
void displayClock2Serial(){
  Serial.print("Y:M:D = ");
  print2digits(tm.Year);
  Serial.print(":");

  print2digits(tm.Month);
  Serial.print(":");

  print2digits(tm.Day);
  Serial.println();
  
  Serial.print("Time = ");
  print2digits(tm.Hour);
  Serial.write(':');
  print2digits(tm.Minute);
  Serial.println();
}


/*
 * displayed Alarm Time to Serial Port
 */
void displayAlarm2Serial(){
  Serial.print("Alarm Time = ");
  print2digits(alarmHH);
  Serial.write(':');
  print2digits(alarmMM);
  Serial.println();
}

/* 
 * 화면 초기 표시
 */
void initDisplay(){
  for(int i=0 ; i <= NeoPixelNum; i+=2){
    HMClock.displayNeoPixel(i,0,255,0);
    delay(200);
  }

  for(int i=1 ; i <= NeoPixelNum; i+=2){
    HMClock.displayNeoPixel(i,0,0,255);
    delay(200);
  }
  HMClock.clear();
}

void initArduinoPin(){
  pinMode(BZ_Pin,OUTPUT);
  pinMode(rtcErrLED_Pin,OUTPUT);
  
  digitalWrite(BZ_Pin, LOW);
  digitalWrite(rtcErrLED_Pin, LOW);
}


