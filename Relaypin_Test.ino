/*
  =======================데이터시트=============================
  코드 및 회로 제작자  : 이상민
  제작자 이메일 연락처 : smgm030@gmail.com
  제작년도           : 2021-05-28(마지막 수정)

  MCU : 아두이노 나노 33 IoT

  릴레이 모듈  : [S : D12], [VCC : 5V], [GND : GND], : 12Vin, 12V OUT
  모터쉴드     : [ENA : D PWM 9],[DN1 : D 8], [DN2 : D 7]; [AOUT1 : 모터GND
  자동 버튼    : [GND] [D4]
  자동 버튼    : [GND] [D5]
  네오픽셀     : [GND] [5V~12V] [D6] 


  * 12V 입력 커넥터를 사용하여 제작된 미세먼지 측정 공기청정기 입니다.
  * 자동버튼을 누를시 미세먼지를 측정하여 미세먼지의 수치에 따라 네오픽셀 LED색이 변경됩니다.
  * 자동버튼을 두 번누를시 네오픽셀이 꺼지며 미세먼지의 수치에 따라 쿨팬의 속도가 달라집니다.
  * 수동버튼은 1단계부터 3단계로 구성되어 있습니다.
  
  
  */
//===========================네오픽셀 선언부 시작=======================
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN 6          // 네오픽셀 pin 6
#define NUMPIXELS 32
#define BRIGHTNESS 100// 밝기 설정 0(어둡게) ~ 255(밝게) 까지 임의로 설정 가능

#define DELAYVAL 10
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
//===========================네오픽셀 선언부 끝=========================
//====================수동 버튼 쿨링팬 선언=============================
#define ENA 9         //모터쉴드 PWM 9번핀
#define EN1 8         //모터쉴드 방향 8번핀
#define EN2 7         //모터쉴드 방향 7번핀
//#define Button_Pin 5  // 수동 버튼 디지털 5번핀
#define Fine_Dust_Button_Pin 5//자동 미세먼지 디지털 4핀 
//#define NeoPixel_Button_Pin 3  // 네오픽셀 전원 디지털 3핀
int Relaypin = 12;    // 릴레이모듈 12번핀
int Button_Count = 0; // 수동 버튼카운트 변수
int Fine_Dust_Button_Count = 0; // 자동 미세먼지 버튼카운트 변수
int NeoPixel_Count = 0; // 네오픽셀 전원 카운트 변수
//====================수동 버튼 쿨링팬 선언=============================

//===========================미세먼지=================================
//빨강 : 5V, 검정,노랑 : GND, 흰색 :SDA(A4), 녹색 : SCL(A5)
#include <pm2008_i2c.h>
PM2008_I2C pm2008_i2c;
//===========================미세먼지=================================



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(11, INPUT);
  //====================수동 and 자동 버튼 쿨링팬 선언=============================
  pinMode(Relaypin, OUTPUT);           // 릴레이핀 OUTPUT
  pinMode(ENA, OUTPUT);                // 모터모듈 PWM제어핀 OUTPUT
  pinMode(EN2, OUTPUT);                // Motor A 방향설정1
  //pinMode(Button_Pin, INPUT_PULLUP);   // 수동 스위치버튼 INPUT_PULLUP모드 LOW일때 1
  pinMode(Fine_Dust_Button_Pin, INPUT_PULLUP);   // 자동 미세먼지 스위치버튼 INPUT_PULLUP모드 LOW일때 1
  //pinMode(NeoPixel_Button_Pin, INPUT_PULLUP);
  //====================수동 버튼 쿨링팬 선언=============================


  //==========================미세먼지 선언==============================
  pm2008_i2c.begin();
  pm2008_i2c.command();
  delay(1000);
  //==========================미세먼지 선언==============================

  //===========================네오픽셀 setup 시작=======================
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  //===========================네오픽셀 setup 시작=========================
}

unsigned long pulse = 0;

float ugm3 = 0;

void loop() {
  pulse = pulseIn(11, LOW, 20000);
  ugm3 = pulse2ugm3(pulse);
  //Serial.println(ugm3);
  //Serial.println("\tug/m3");

  //=====================네오픽셀 실행 Start========================
  /*if (digitalRead(NeoPixel_Button_Pin) == LOW) {            // 버튼을 누르면
    NeoPixel_Count = NeoPixel_Count + 1;                        // 1씩 카운트
    delay(500);                                             // 1초 대기
    Serial.print("네오픽셀버튼번호 : ");
    Serial.println(NeoPixel_Count);                           // 카운트확인
    } else if (NeoPixel_Count == 5) {                           // 카운트가 5가 되면
    NeoPixel_Count = 0;                                       // 카운트 0으로 초기화
    }*/
  //=====================수동 버튼 쿨링팬 실행 END========================


  //==============미세먼지 츨정 and 미세먼지별 자동 동작버튼====================
  if (digitalRead(Fine_Dust_Button_Pin) == LOW) {           // 버튼을 누르면
    Fine_Dust_Button_Count = Fine_Dust_Button_Count + 1;    // 1씩 카운트
    delay(500);                        // 1초 대기
    Serial.print("자동버튼번호 : ");
    Serial.println(Fine_Dust_Button_Count);                 // 카운트확인
  } else if (Fine_Dust_Button_Count == 6) {                 // 카운트가 4가 되면
    Fine_Dust_Button_Count = 0;                             // 카운트 0으로 초기화
  }
  //=========================================

  /*=====================수동 버튼 쿨링팬 실행 Start========================
  if (digitalRead(Button_Pin) == LOW) {                     // 버튼을 누르면
    Button_Count = Button_Count + 1;                        // 1씩 카운트
    delay(500);                                             // 1초 대기
    Serial.print("수동버튼번호 : ");
    Serial.println(Button_Count);                           // 카운트확인
  } else if (Button_Count == 4) {                           // 카운트가 4가 되면
    Button_Count = 0;                                       // 카운트 0으로 초기화
  }
  =====================수동 버튼 쿨링팬 실행 END========================*/



  //==============미세먼지 츨정 and 미세먼지별 자동 동작버튼====================

  /*if (Button_Count >= 1 && Button_Count <= 4) {
    SWH_Button(Button_Count);                                // 수동 버튼 스위치 쿨팬 구동
  }*/

  if (Fine_Dust_Button_Count >= 1 && Fine_Dust_Button_Count <= 6) {
    Fine_Dust(Fine_Dust_Button_Count, ugm3);                       // 자동 버튼 스위치 쿨팬 구동
    if (Fine_Dust_Button_Count == 2) {                       //자동버튼이 2면 네오픽셀 구동가능
      NeoPixel_Control(NeoPixel_Count);                      //네오픽셀
    } else if (Fine_Dust_Button_Count == 3) {                // 자동버튼이 3이면
      NeoPixel_Count = 0;                                   //네오픽셀 꺼짐
    }
  }

}

float pulse2ugm3(unsigned long pulse) {
  float value = (pulse - 1610) / 10.2;
  if (value > 300) {
    value = 0;
  }
  return value;
}


/*=====================수동 버튼 쿨링팬 구동 함수 Start====================
void SWH_Button(int Button_Num) {
  switch (Button_Num) {               // 버튼 카운트 입력
    case 0:                             // 카운트가 0일때
      digitalWrite(Relaypin, LOW);      // 릴레이모듈 LOW
      digitalWrite(7, LOW);             // Motor A 방향설정1
      analogWrite(ENA, 0);              // Motor A 속도조절 (0~255)
      break;
    case 1:                             // 카운트가 1일때
      digitalWrite(Relaypin, HIGH);     // 릴레이 HIGH
      digitalWrite(7, HIGH);            // Motor A 방향설정1
      analogWrite(ENA, 150);            // Motor A 속도조절 (1단)
      break;
    case 2:                              // 카운트가 2일때
      digitalWrite(Relaypin, HIGH);      // 릴레이 HIGH
      digitalWrite(7, HIGH);
      analogWrite(ENA, 190);             // Motor A 속도조절 (2단)
      break;
    case 3:                              // 카운트가 3일때
      digitalWrite(Relaypin, HIGH);      // 릴레이 HIGH
      digitalWrite(7, HIGH);
      analogWrite(ENA, 255);             // Motor A 속도조절 (3단)
      break;
    case 4:                              // 카운트가 4일때
      digitalWrite(Relaypin, LOW);       // 릴레이 HIGH
      delay(100);
      break;
  }
}
=====================수동 버튼 쿨링팬 구동 함수  END====================*/




/*=====================미세먼지 구동 함수 Start====================*/

void Fine_Dust(int Fine_Dust_Num, int FD_Num) {
  //int FD_Num = ugm3;
  switch (Fine_Dust_Num) {               // 버튼 카운트 입력
    case 0:                             // 카운트가 0일때
      digitalWrite(Relaypin, LOW);      // 릴레이모듈 LOW
      digitalWrite(7, LOW);             // Motor A 방향설정1
      analogWrite(ENA, 0);              // Motor A 속도조절 (0~255)
      break;
    case 1:                             // 카운트가 1일때
      if (FD_Num < 50 ) {                 // === 초미세먼지 50 이하 일때 1단
        digitalWrite(Relaypin, HIGH);     // 릴레이 HIGH
        digitalWrite(7, HIGH);            // Motor A 방향설정1
        analogWrite(ENA, 150);            // Motor A 속도조절 (1단)
        // 반복문으로 사용하셔도 됩니다.
        pixels.setPixelColor(0, pixels.Color(0, 0, 255));
        pixels.setPixelColor(1, pixels.Color(0, 0, 255));
        pixels.setPixelColor(2, pixels.Color(0, 0, 255));
        pixels.setPixelColor(3, pixels.Color(0, 0, 255));
        pixels.setPixelColor(4, pixels.Color(0, 0, 255));
        pixels.setPixelColor(5, pixels.Color(0, 0, 255));
        pixels.setPixelColor(6, pixels.Color(0, 0, 255));
        pixels.setPixelColor(7, pixels.Color(0, 0, 255));
        pixels.setPixelColor(8, pixels.Color(0, 0, 255));
        pixels.setPixelColor(9, pixels.Color(0, 0, 255));
        pixels.setPixelColor(0, pixels.Color(0, 0, 255));
        pixels.setPixelColor(11, pixels.Color(0, 0, 255));
        pixels.setPixelColor(12, pixels.Color(0, 0, 255));
        pixels.setPixelColor(13, pixels.Color(0, 0, 255));
        pixels.setPixelColor(14, pixels.Color(0, 0, 255));
        pixels.setPixelColor(15, pixels.Color(0, 0, 255));
        pixels.setPixelColor(16, pixels.Color(0, 0, 255));
        pixels.setPixelColor(17, pixels.Color(0, 0, 255));
        pixels.setPixelColor(18, pixels.Color(0, 0, 255));
        pixels.setPixelColor(19, pixels.Color(0, 0, 255));
        pixels.setPixelColor(20, pixels.Color(0, 0, 255));
        pixels.setPixelColor(21, pixels.Color(0, 0, 255));
        pixels.setPixelColor(22, pixels.Color(0, 0, 255));
        pixels.setPixelColor(23, pixels.Color(0, 0, 255));
        pixels.setPixelColor(24, pixels.Color(0, 0, 255));
        pixels.setPixelColor(25, pixels.Color(0, 0, 255));
        pixels.setPixelColor(26, pixels.Color(0, 0, 255));
        pixels.setPixelColor(27, pixels.Color(0, 0, 255));
        pixels.setPixelColor(28, pixels.Color(0, 0, 255));
        pixels.setPixelColor(29, pixels.Color(0, 0, 255));
        pixels.setPixelColor(30, pixels.Color(0, 0, 255));
        pixels.setPixelColor(31, pixels.Color(0, 0, 255));
        pixels.setPixelColor(32, pixels.Color(0, 0, 255));
        pixels.setPixelColor(33, pixels.Color(0, 0, 255));
        pixels.setPixelColor(34, pixels.Color(0, 0, 255));

        pixels.show();   // Send the updated pixel colors to the hardware.


      } else if (FD_Num > 51 && FD_Num < 140) { // === 초미세먼지 51이상 and 120이하 일때 2단
        digitalWrite(Relaypin, HIGH);      // 릴레이 HIGH
        digitalWrite(7, HIGH);
        analogWrite(ENA, 190);             // Motor A 속도조절 (2단)

        pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        pixels.setPixelColor(1, pixels.Color(0, 255, 0));
        pixels.setPixelColor(2, pixels.Color(0, 255, 0));
        pixels.setPixelColor(3, pixels.Color(0, 255, 0));
        pixels.setPixelColor(4, pixels.Color(0, 255, 0));
        pixels.setPixelColor(5, pixels.Color(0, 255, 0));
        pixels.setPixelColor(6, pixels.Color(0, 255, 0));
        pixels.setPixelColor(7, pixels.Color(0, 255, 0));
        pixels.setPixelColor(8, pixels.Color(0, 255, 0));
        pixels.setPixelColor(9, pixels.Color(0, 255, 0));
        pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        pixels.setPixelColor(11, pixels.Color(0, 255, 0));
        pixels.setPixelColor(12, pixels.Color(0, 255, 0));
        pixels.setPixelColor(13, pixels.Color(0, 255, 0));
        pixels.setPixelColor(14, pixels.Color(0, 255, 0));
        pixels.setPixelColor(15, pixels.Color(0, 255, 0));
        pixels.setPixelColor(16, pixels.Color(0, 255, 0));
        pixels.setPixelColor(17, pixels.Color(0, 255, 0));
        pixels.setPixelColor(18, pixels.Color(0, 255, 0));
        pixels.setPixelColor(19, pixels.Color(0, 255, 0));
        pixels.setPixelColor(20, pixels.Color(0, 255, 0));
        pixels.setPixelColor(21, pixels.Color(0, 255, 0));
        pixels.setPixelColor(22, pixels.Color(0, 255, 0));
        pixels.setPixelColor(23, pixels.Color(0, 255, 0));
        pixels.setPixelColor(24, pixels.Color(0, 255, 0));
        pixels.setPixelColor(25, pixels.Color(0, 255, 0));
        pixels.setPixelColor(26, pixels.Color(0, 255, 0));
        pixels.setPixelColor(27, pixels.Color(0, 255, 0));
        pixels.setPixelColor(28, pixels.Color(0, 255, 0));
        pixels.setPixelColor(29, pixels.Color(0, 255, 0));
        pixels.setPixelColor(30, pixels.Color(0, 255, 0));
        pixels.setPixelColor(31, pixels.Color(0, 255, 0));
        pixels.setPixelColor(32, pixels.Color(0, 255, 0));
        pixels.setPixelColor(33, pixels.Color(0, 255, 0));
        pixels.setPixelColor(34, pixels.Color(0, 255, 0));
        pixels.show();   // Send the updated pixel colors to the hardware.


      } else if (FD_Num > 140) {           // === 초미세먼지 120이상 일때 2단
        digitalWrite(Relaypin, HIGH);      // 릴레이 HIGH
        digitalWrite(7, HIGH);
        analogWrite(ENA, 255);             // Motor A 속도조절 (3단)

        pixels.setPixelColor(0, pixels.Color(255, 0, 0));
        pixels.setPixelColor(1, pixels.Color(255, 0, 0));
        pixels.setPixelColor(2, pixels.Color(255, 0, 0));
        pixels.setPixelColor(3, pixels.Color(255, 0, 0));
        pixels.setPixelColor(4, pixels.Color(255, 0, 0));
        pixels.setPixelColor(5, pixels.Color(255, 0, 0));
        pixels.setPixelColor(6, pixels.Color(255, 0, 0));
        pixels.setPixelColor(7, pixels.Color(255, 0, 0));
        pixels.setPixelColor(8, pixels.Color(255, 0, 0));
        pixels.setPixelColor(9, pixels.Color(255, 0, 0));;
        pixels.setPixelColor(0, pixels.Color(255, 0, 0));
        pixels.setPixelColor(11, pixels.Color(255, 0, 0));
        pixels.setPixelColor(12, pixels.Color(255, 0, 0));
        pixels.setPixelColor(13, pixels.Color(255, 0, 0));
        pixels.setPixelColor(14, pixels.Color(255, 0, 0));
        pixels.setPixelColor(15, pixels.Color(255, 0, 0));
        pixels.setPixelColor(16, pixels.Color(255, 0, 0));;
        pixels.setPixelColor(17, pixels.Color(255, 0, 0));
        pixels.setPixelColor(18, pixels.Color(255, 0, 0));
        pixels.setPixelColor(19, pixels.Color(255, 0, 0));
        pixels.setPixelColor(20, pixels.Color(255, 0, 0));
        pixels.setPixelColor(21, pixels.Color(255, 0, 0));
        pixels.setPixelColor(22, pixels.Color(255, 0, 0));
        pixels.setPixelColor(23, pixels.Color(255, 0, 0));
        pixels.setPixelColor(24, pixels.Color(255, 0, 0));
        pixels.setPixelColor(25, pixels.Color(255, 0, 0));
        pixels.setPixelColor(26, pixels.Color(255, 0, 0));;
        pixels.setPixelColor(27, pixels.Color(255, 0, 0));
        pixels.setPixelColor(28, pixels.Color(255, 0, 0));
        pixels.setPixelColor(29, pixels.Color(255, 0, 0));
        pixels.setPixelColor(30, pixels.Color(255, 0, 0));
        pixels.setPixelColor(31, pixels.Color(255, 0, 0));
        pixels.setPixelColor(32, pixels.Color(255, 0, 0));
        pixels.setPixelColor(33, pixels.Color(255, 0, 0));
        pixels.setPixelColor(34, pixels.Color(255, 0, 0));
        pixels.show();   // Send the updated pixel colors to the hardware.

      }
      break;
    case 2:                             // 카운트가 1일때
      if (FD_Num < 50 ) {                 // === 초미세먼지 50 이하 일때 1단
        digitalWrite(Relaypin, HIGH);     // 릴레이 HIGH
        digitalWrite(7, HIGH);            // Motor A 방향설정1
        analogWrite(ENA, 150);            // Motor A 속도조절 (1단)

      } else if (FD_Num > 51 && FD_Num < 140) { // === 초미세먼지 51이상 and 120이하 일때 2단
        digitalWrite(Relaypin, HIGH);      // 릴레이 HIGH
        digitalWrite(7, HIGH);
        analogWrite(ENA, 190);             // Motor A 속도조절 (2단)

      } else if (FD_Num > 140) {           // === 초미세먼지 120이상 일때 2단
        digitalWrite(Relaypin, HIGH);      // 릴레이 HIGH
        digitalWrite(7, HIGH);
        analogWrite(ENA, 255);             // Motor A 속도조절 (3단)

      }
      break;
    case 3:                             // 카운트가 1일때
      digitalWrite(Relaypin, HIGH);     // 릴레이 HIGH
      digitalWrite(7, HIGH);            // Motor A 방향설정1
      analogWrite(ENA, 150);            // Motor A 속도조절 (1단)
      break;
    case 4:                              // 카운트가 2일때
      digitalWrite(Relaypin, HIGH);      // 릴레이 HIGH
      digitalWrite(7, HIGH);
      analogWrite(ENA, 190);             // Motor A 속도조절 (2단)
      break;
    case 5:                              // 카운트가 3일때
      digitalWrite(Relaypin, HIGH);      // 릴레이 HIGH
      digitalWrite(7, HIGH);
      analogWrite(ENA, 255);             // Motor A 속도조절 (3단)
      break;
    case 6:                              // 카운트가 4일때
      digitalWrite(Relaypin, LOW);       // 릴레이 HIGH
      delay(100);
      break;
  }
}


void NeoPixel_Control(int Fine_Dust_NPixel) {
  switch (Fine_Dust_NPixel) {               // 버튼 카운트 입력
    case 0:                             // 카운트가 0일때

      for (int a = 0; a < 32; a++) {
        pixels.setPixelColor(a, pixels.Color(0, 0, 0));
        pixels.show();   // Send the updated pixel colors to the hardware.
      }
      break;
    case 1:                             // 카운트가 1일때

      for (int a = 0; a < 32; a++) {
        pixels.setPixelColor(a, pixels.Color(255, 0, 221));
        pixels.show();   // Send the updated pixel colors to the hardware.
      }
      break;
    case 2:                              // 카운트가 2일때

      for (int a = 0; a < 32; a++) {
        pixels.setPixelColor(a, pixels.Color(255, 0, 0));
        pixels.show();   // Send the updated pixel colors to the hardware.
      }
      break;
    case 3:                              // 카운트가 2일때

      for (int a = 0; a < 32; a++) {
        pixels.setPixelColor(a, pixels.Color(0, 0, 255));
        pixels.show();   // Send the updated pixel colors to the hardware.
      }
      break;
    case 4:                              // 카운트가 2일때

      for (int a = 0; a < 32; a++) {
        pixels.setPixelColor(a, pixels.Color(255, 187, 0));
        pixels.show();   // Send the updated pixel colors to the hardware.
      }
      break;
  }
}
