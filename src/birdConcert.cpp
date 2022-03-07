/**
 * Program      birdConcert.cpp
 * Author       2021-12-10 Charles Geiser ((https://www.dodeka.ch))
 * 
 * Purpose      Allows the simulation of a wide variety of bird calls with a 
 *              single function that can be parameterized in a variety of ways.
 *              The switching on and off of the piezo buzzer is realized with 
 *              a local lambda expression.
 *              The function can also serve as an experimental sweep generator
 * 
 * Board        DoIt ESP32 DevKit V1
 * 
 * Wiring       
 *             --------.
 *               ESP32 |
 *                     |                  ____||
 *          GPIO_NUM_4 o-----------------|    || piezo
 *                     |                 |-¦¦-|| buzzer
 *                 GND o-----------------|____|| 
 *                     |                      ||
 *
 * Remarks  
 */
#include "Chirpmaker.h"

const uint8_t PIN_BUZZER = GPIO_NUM_4;
Chirpmaker cm(PIN_BUZZER);

void setup() 
{
  Serial.begin(115200);
  cm.signet();
}

void loop() 
{
  printf("Phone call\n");
  cm.phoneCall(7);
  delay(1000);

  printf("Birdconcert\n");
  cm.birdConcert(3000);

  printf("Chirp\n");
  cm.chirp(1800, 2400, 50, 15, 7, sincScale0_Npi, 50, 5000);

  printf("Cuckoo\n");
  cm.cuckoo();

  printf("Raven\n");
  cm.raven();

  printf("Chaffinch\n");
  cm.chaffinch();

  printf("Blackbird\n");
  cm.blackbird();

  printf("Phaser\n");
  cm.phaser(1500, 30, 5, 95, 3, 200);
  delay(2000);
}
