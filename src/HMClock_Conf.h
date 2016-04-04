#define HMdebug         //시리얼 포트 출력
//#define BTSerialPort
//#define HWSerialPort
#define led6X6


#define NeoPixelPIN 6

#define releaseAlarmDisplayTime 10000     // 알람시간 디스플레이 해제 시간

#ifdef led4X4
  #define NeoPixelNum 25
#endif

#ifdef led6X6
  #define NeoPixelNum 36
#endif

