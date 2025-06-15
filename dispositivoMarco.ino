/************************************************* *************************************************
* OPEN-SMART Rich Shield Lesson 1:  Run four LED
* You can learn how to turn on and turn off the LED.
*
* The following functions are available:
*
* led.on(uint8_t num);//num can be 1, 2, 3, 4 that is consistent with the LED number
* led.off(uint8_t num);
*
************************************************** *******************************************************/

#include <Wire.h>
#include "RichShieldLED.h"
#include "RichShieldKEY.h"
#include "RichShieldTM1637.h"
#include "RichShieldKnob.h"

#define KNOB_PIN A0//SIG pin of Rotary Angle Sensor module connect to A0 of IO Shield, that is pin A0 of OPEN-SMART UNO R3
Knob knob(KNOB_PIN);

#define CLK 10//CLK of the TM1637 IC connect to D10 of OPEN-SMART UNO R3
#define DIO 11//DIO of the TM1637 IC connect to D11 of OPEN-SMART UNO R3
TM1637 disp(CLK,DIO);

#include "RichShieldPassiveBuzzer.h"

#define PassiveBuzzerPin 3//the SIG pin of the module is connected with D3 of OPEN-SMART UNO R3
                     //There is no white waterproof sticker on the passive buzzer
PassiveBuzzer buz(PassiveBuzzerPin);

//------------------------------------
//This is a macro definition of bass, midrange, and treble frequencies
#define NOTE_L1  262
#define NOTE_L2  294
#define NOTE_L3  330
#define NOTE_L4  349
#define NOTE_L5  392
#define NOTE_L6  440
#define NOTE_L7  494
#define NOTE_M1  523
#define NOTE_M2  587
#define NOTE_M3  659
#define NOTE_M4  698
#define NOTE_M5  784
#define NOTE_M6  880
#define NOTE_M7  988
#define NOTE_H1  1046
#define NOTE_H2  1175
#define NOTE_H3  1318
#define NOTE_H4  1397
#define NOTE_H5  1568
#define NOTE_H6  1760
#define NOTE_H7  1976
//-------------------------------------------
//musical notation of Two Tigers 
const static int doubleTiger[] = { 
NOTE_M1,NOTE_M2,NOTE_M3,NOTE_M1, 
NOTE_M1,NOTE_M2,NOTE_M3,NOTE_M1/*, 
NOTE_M3,NOTE_M4,NOTE_M5, 
NOTE_M3,NOTE_M4,NOTE_M5, 
NOTE_M5,NOTE_M6,NOTE_M5,NOTE_M4,
NOTE_M3,NOTE_M1, 
NOTE_M5,NOTE_M6,NOTE_M5,NOTE_M4,
NOTE_M3,NOTE_M1, 
NOTE_M2,NOTE_L5,
NOTE_M1,0,
NOTE_M2,NOTE_L5,
NOTE_M1,0
*/
};
//-------------------------------------------
////note duration of Two Tigers, 2 is half a beat, 1 is a beat.
const static int tempo[] = { 
2,2,2,2, 
2,2,2,2/*, 
2,2,1, 
2,2,1,  
2,2,2,2,
1,1,
2,2,2,2,
1,1,
1,1,
1,1,
1,1,
1,1
*/
};


#define ledGiallo 7
#define ledBlu 6
#define ledVerde 5

#define LED1 7
#define LED2 6
#define LED3 5
#define LED4 4
LED led(LED1,LED2,LED3,LED4);

#define KEY1_PIN 9//
#define KEY2_PIN 8//
Key key(KEY1_PIN,KEY2_PIN);

boolean statoLedG;
int statoLed=16;
int timer1;
int max;
int ritardoLed=375; // più ritardo Tone = 500 ms
int ritardoFor=1000;
unsigned long timerLed;
unsigned long tempoFor;


void setup() {
  pinMode(ledGiallo,OUTPUT);
  pinMode(ledBlu,OUTPUT);
  pinMode(ledVerde,OUTPUT);
  pinMode(A0,INPUT);
  disp.init();//The initialization of the display
  timerLed=millis();
}

void loop() {

  //led.on(2);//turn on LED i
  int angle;
  //angle = knob.getAngle();
  angle=analogRead(A0);
  max=map(angle,0,1023,10,900);
  //max=map(angle,0,280,10,600);
  disp.display(max);
  timer1=max;

  if(millis()>timerLed+ritardoLed) {
    analogWrite(ledBlu,statoLed);
    statoLed=16-statoLed;
    //led.off(2);//turn off it.
    timerLed=millis();
  }

  int keynum;
  keynum = key.get();
  if(keynum == 1) //if you press K1
  {          
    led.off(2);
    led.off(1);
    led.off(4);
    analogWrite(ledVerde,8);
    //led.on(3);//turn on LED i
    for (int i=1;i<=max;i++) {
      //if(millis()>tempoFor+ritardoFor) {
        timer1=timer1-1;
        disp.display(timer1);
        delay(1000);

        keynum = key.get();      
        if(keynum == 1) //if you press K1 
        {
          break;
        }

        if (timer1<=max/4) {
          led.off(2);//turn off it.
          led.off(3);//turn off it.        
          //led.on(1);//turn on LED i
          if(millis()>timerLed+ritardoLed) {
            digitalWrite(ledGiallo,statoLedG);
            buz.playTone(NOTE_L1, 125);
            //tone(3, 165);
            //delay(125);
            //noTone(3);
            statoLedG=!statoLedG;
            //led.off(2);//turn off it.
            timerLed=millis();
          }
        }
        //tempoFor=millis();
      //}
      
    }
 
    if (timer1==0) {      
      led.off(1);//turn off it.
      led.off(2);//turn off it.
      led.off(3);//turn off it.
      led.on(4);//turn on LED i
      delay(125);
      sing();
    }
  }

  led.off(3);//turn off it.
  
/*
  for(uint8_t i=1;i < 5; i++)
  {
    led.on(i);//turn on LED i
    delay(500);
    led.off(i);//turn off it.
  }
*/
  delay(250);
}

void sing() {
  // iterate over the notes of the melody:
    int size = sizeof(doubleTiger) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 500 / tempo[thisNote];//a beat for 0.5 second

      buz.playTone(doubleTiger[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 0.30;
      delay(pauseBetweenNotes);
    }
}
