/*
Game of Thrones PIR sensor music sensor

Notes:
ATmega328PU will sleep until the pir sensor detects movement, when movement is detected a random wav file will play, after playing,
the device goes back to sleep waiting for the pir sensor to detect movement, which repeats the cycle.

Resources:
amp information
https://learn.adafruit.com/adafruit-tpa2016-2-8w-agc-stereo-audio-amplifier/software 

wav(s) on SD card must be 8bit,sampling=16000mhz,audio channels=mono,advanced options PCM format=unsigned 8bit
https://audio.online-convert.com/convert-to-wav


https://github.com/rocketscream/Low-Power
Rocketscream's Low Power library 

Excellent reading, which provided a ton of useful information for this project:
https://dronebotworkshop.com/arduino-uno-atmega328/
http://www.gammon.com.au/breadboard
https://makersportal.com/blog/2019/5/27/arduino-interrupts-with-pir-motion-detector
https://hwstartup.wordpress.com/2013/04/15/how-to-run-an-arduino-clone-on-aa-batteries-for-over-a-year-part-2/


Alan Thomas
alanscottthomas@gmail.com
https://www.linkedin.com/in/alanthomas84/
22JUN2019

*/

//libraries//
#include <LowPower.h>
#include <Wire.h>
#include "Adafruit_TPA2016.h"
#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"
//---------//

#define SD_ChipSelectPin 10                     //define pin for SD card (CS)

Adafruit_TPA2016 audioamp = Adafruit_TPA2016(); //create amp object
TMRpcm tmrpcm;                                  //create playback object
const int pirSensorPin = 2;                     // pin PIR Sensor is connected to digital 2 or 3 for interrupts
int motionDetected = LOW;                       // Start MotionDetected as low (No motion detected)
long random_number;                             //random number to be used when selecting what track to play from SD card
int amp_shutdown = 8;                           // pin for shutdown on amp, used to control noise when not playing music

//--- wakeup function ---//
void wakeUp(){}                                 //handler for interrupt
//-----------------------//


//--- setup function ---//
void setup() 
{
  pinMode(amp_shutdown, OUTPUT);                //setting pin mode for the amp shutdown pin
  digitalWrite(amp_shutdown, LOW);             //setting amp shutdown pin to high, keeping amp on for the moment
  randomSeed(analogRead(0));                    //generating seed to be used for random int
  audioamp.begin();                             //start the amp
  audioamp.setAGCCompression(TPA2016_AGC_2);    //1:2-1:8 compression, tells the amp to smooth changes in volume/tones
  audioamp.enableChannel(false, true);          //turn off right channel / turn on left channel
  //audioamp.enableChannel(false, false);       //turn off right channel / turn on left channel
  audioamp.setLimitLevelOn();                   //tells the amp to set a limit of additional voltage sent to the speaker
  audioamp.setLimitLevel(0);                    //limit additional voltage sent to speaker to 0v, seems to of resolved some popping issues
  audioamp.setGain(-28);                        //set gain of the amp; set to lowest in an attempt to control 'popping'
  pinMode(pirSensorPin, INPUT);                 //declare the pir sensor as input
  delay(5000);                                  //5 seceonds to allow time for the PIR Sensor to calibrate
  tmrpcm.speakerPin=9;                          //designate speaker pin
  //Serial.begin(9600);                         //Set serial out if we want debugging
  
  if(!SD.begin(SD_ChipSelectPin)){return;}      //if SD card cannot be read, stop. Could put serial print here for troubleshooting
}
//---------------------//


//--- play_music function ---//
void play_music()
{
    digitalWrite(amp_shutdown, HIGH);             //turn on amp
    random_number = random(1, 10);                //random number between 1 and 9
    tmrpcm.setVolume(5);                          //volume set to 5, range between 4-5.5 seems to work best
    tmrpcm.quality(1);                            //set playback quality (0-1)
    bool testing = false;                         //bool to be used when testing, this will play a short wav file instead of the longer wav files  

    if(testing == true){tmrpcm.play("test.wav");} //if testing true, play test wav, true to test, false to play normal music
    
    else                                          //if testing not true play normal music
    {
    //play wav file depending on random number result//
    if(random_number == 1){tmrpcm.play("1.wav");}
    else if(random_number == 2){tmrpcm.play("2.wav");}
    else if(random_number == 3){tmrpcm.play("3.wav");}
    else if(random_number == 4){tmrpcm.play("4.wav");}
    else if(random_number == 5){tmrpcm.play("5.wav");}
    else if(random_number == 6){tmrpcm.play("6.wav");}
    else if(random_number == 7){tmrpcm.play("7.wav");}
    else if(random_number == 8){tmrpcm.play("8.wav");}
    else if(random_number == 9){tmrpcm.play("9.wav");}
     }
    //-----------------------------------------------// 
    
    while (tmrpcm.isPlaying()){}                         //do nothing and let the music finish playing
    delay(2000);                                         //2 secs before turning off playback
    tmrpcm.disable();                                    //turn off playback
    digitalWrite(amp_shutdown, LOW);                     //shutdown the amp after playing music
    motionDetected = LOW;                                //set if motion detected to false
}
//---------------------//


//--- loop function ---//
void loop(){
  attachInterrupt(0, wakeUp, LOW);                      //set interrupt to pin 2 (0=pin 2,1=pin 3), call wakeup when not low
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);  //put atmega328pu in sleep, turn off analog controls and brownout detection
  detachInterrupt(0);                                   //turn off interrupt on pin 2
  motionDetected = digitalRead(pirSensorPin);           //read pir sensor state
  if(motionDetected == HIGH){play_music();}             //if motion detected call play music function
}
//-------------------//
