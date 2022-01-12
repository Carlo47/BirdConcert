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
 *                  4  o-----------------|    || piezo
 *                     |                 |-¦¦-|| buzzer
 *                 GND o-----------------|____|| 
 *                     |                      ||
 *
 * Remarks  
 */
#include <Arduino.h>

const uint8_t PIN_BUZZER = GPIO_NUM_4;
typedef double (*FreqGen)(int stepNbr, double fStart, double fStop,int nSteps);

/**
 * Simulate the chirp of a bird. Start with fStart and reach fStop in n steps.
 * Each individual frequency is composed of n periods. freq(i) = k * freq(i-1)
 * The duty cycle of the square wave determines the timbre of the generated tone.  
 * 
 * fStart    Chirp starts with this frequency
 * fStop     Chirp ends   with this frequency
 * nSteps    The frequency interval is divided into n steps 
 * nPeriods  Every frequency contains n periods
 * nChirps   n chirps are played
 * fgen      frequency generator to step through the frequency range
 * duty      duty cycle (1..99 %) of a period 
 * msPause   ms Pause between chirps
 */
void chirp(double fStart, double fStop, int nSteps, int nPeriods, int nChirps, FreqGen fgen, int duty, uint32_t msPause)
{
    auto buz = [](uint32_t usTon, uint32_t usToff){
      digitalWrite(PIN_BUZZER, HIGH);
      delayMicroseconds(usTon);
      digitalWrite(PIN_BUZZER, LOW);
      delayMicroseconds(usToff);};

  for (int n = 0; n < nChirps; n++) // output nChirps
  {
    for (int s = 0; s <= nSteps; s++)
    {
      double fNext = fgen(s, fStart, fStop, nSteps);
      double p = 1000000.0 / fNext;
      uint32_t tOn  = p * duty / 100.0;
      uint32_t tOff = p - tOn;
      //log_i("%2d: f = %5.2f, ton = %d, toff = %d", s, fNext, tOn, tOff);
      for (int n = 0; n < nPeriods; n++) buz(tOn, tOff);
    }
    delay(msPause);
  }
}

/**
 * Generates n periods of a square wave of frequency freq and varies its 
 * duty cycle from dutyStart to dutyEnd in steps of 1 %
 * 
 * freq         Frequency of square wave
 * nPeriods     n periods per for each duty cycle
 * dutyStart    minimum duty cycle
 * dutyEnd      maximum duty cycle
 * nChirps      n chirps are generated
 * msPause      ms to wait after each chirp
 */
void phaser(uint32_t freq, int nPeriods, int dutyStart, int dutyEnd, int nChirps, uint32_t msPause)
{
  uint32_t p = 1000000/freq;

  auto buz = [](uint32_t usTon, uint32_t usToff){
      digitalWrite(PIN_BUZZER, HIGH);
      delayMicroseconds(usTon);
      digitalWrite(PIN_BUZZER, LOW);
      delayMicroseconds(usToff);};

  for (int n = 0; n < nChirps; n++) // output nChirps
  {
    for (int d = dutyStart; d <= dutyEnd; d++)
    {
        uint32_t tOn  = p * d / 100;
        uint32_t tOff = p - tOn;
        for (int n = 0; n < nPeriods; n++) buz(tOn, tOff);
    } 
    delay(msPause);
  }    
}

double linearScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double df = (fStop - fStart) / nSteps;
  double fNext = fStart + stepNbr * df;
  return fNext;
}

double chromaticScale(int stepNbr, double fStart, double fStop,int nSteps)
{
    // We calculate the multiplicator k to get fStop in nSteps
    // fStop = fStart * k ^ nSteps
    double k = log(fStop / fStart) / (double)nSteps;  
    double fNext = fStart * exp(k * stepNbr);
    return fNext;
}

double sinePiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double fa = (fStop - fStart);  // max. frequency swing
  const double k = PI / nSteps;
  double fNext = fStart + fa * sin(k * stepNbr); // get next frequency
  return fNext;
}

double sine2PiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double fm = (fStart + fStop) / 2.0;  // arithmetic mean
  double fa = (fStop - fStart) / 2.0;  // max. frequency swing around fm
  const double k = TWO_PI / nSteps;
  double fNext = fm + fa * sin(k * stepNbr); // get next frequency
  return fNext;
}

double cosinePiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double fm = (fStart + fStop) / 2.0;  // arithmetic mean
  double fa = (fStop - fStart) / 2.0;  // max. frequency swing around fm
  const double k = PI / nSteps;
  double fNext = fm - fa * cos(k * stepNbr); // get next frequency
  return fNext;
}

double cosine2PiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double fm = (fStart + fStop) / 2.0;  // arithmetic mean
  double fa = (fStop - fStart) / 2.0;  // max. frequency swing around fm
  const double k = TWO_PI / nSteps;
  double fNext = fm - fa * cos(k * stepNbr); // get next frequency
  return fNext;
}
double atanPiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double k = (fStop - fStart)/atan(PI);
  double fNext = fStart + k * atan(PI/nSteps * stepNbr);
  return fNext;
}

double atan2PiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double k = (fStop - fStart)/atan(TWO_PI);
  double fNext = fStart + k * atan(TWO_PI/nSteps * stepNbr);
  return fNext;
}


typedef void (*bird)();  // bird is a pointer to a function taking no parameters and returning void

// Define some birds with different chirps
void bird0()
{
    chirp(random(1200, 1900), random(4300, 4500), random(10, 42), random(1,5), 5, chromaticScale, 50, random(59, 199));
    chirp(random(2000, 2050), random(3200, 3400), random(5, 30),  random(2,15), random(4, 10), atanPiScale, 50, 20 );
    chirp(1500, 4500, random(50, 100), random(1, 13), random(1, 5), sine2PiScale, 50, 100);
}

void bird1()
{
    chirp(random(4200, 4400), random(2800, 2500), 100,  random(1,3), random(3, 9), chromaticScale, 50, random(25, 75));
}

void bird2()
{
    chirp(random(3500,3900), random(5600,5900), random(3,7), random(5,10), 1, sine2PiScale, 50, random(50, 100));
    chirp(random(5600,5900), random(3500,3900), random(6,15), random(3,7), 1, cosine2PiScale, 50, random(50, 100));
};

void bird3()
{
    chirp(random(1280,1300), random(1310,1620), 10, random(4,8), random(2,9), linearScale, 50, random(100, 200));
}

void bird4()
{
    chirp(4000, 4800, 10, 4, random(10, 15), atan2PiScale, 50, 20);
    chirp(3500, 4300, 15, 10, 1, atanPiScale, 50, 20);
    chirp(3500, 3000, 25, 10, 1, sinePiScale, 50, random(75, 150));
};

 void bird5()
 {
   chirp(random(4404, 4484), random(4380,4420), 20, random(1,4), random(1,7), linearScale, 50, 20);
 }

 void bird6()
 {
   chirp(random(1000, 1050), random(900, 1200), 20, random(1, 5), random(10, 15), chromaticScale, 50, random(150, 250));
 }

 void bird7()
 {
   chirp(2600, 4400, 10, 1, random(5,9), chromaticScale, 50, 20);
 }

void bird8()
{
    chirp(1320, 3880, 5, 10, 5, sine2PiScale, 50, 100);
}

void bird9()
{
  phaser(random(3500,3540), random(6, 12), 5, 50, random(3,15), 0);
  phaser(random(1660,1800), random(3, 10), 5, 30, random(6,13), 0);
}

void bird10()
{
  chirp(1440, 1880, 20, 10, random(1,9), atanPiScale, 5, 10);
  chirp(1880, 1440, 20, 10, random(1,9), atanPiScale, 50, 30);
}

void cuckoo()
{
  const float third = 1.222;     // minorThird = 1.18 ... majorThird = 1.25
  const float cuc = 667;         //  E4
  const float koo = cuc / third; // ~C#4
  for (int i = 1; i < random(1,5); i++)
  {
    chirp(cuc, cuc, 1, 46, 1, linearScale, 50, 200);
    chirp(koo, koo, 1, 52, 1, linearScale, 50, 830);
  }
}

void raven()
{
  chirp(75,65,8,4,random(2, 6), atanPiScale, 20, 350);
}

void signet()
{
  chirp(440, 1320, 6, 300, 1, cosine2PiScale, 50, 1000);
  chirp(1320, 440, 6, 300, 1, cosine2PiScale, 50, 3000);
}

// Store the birds in an array
bird birds[] = { 
  bird0, bird1, 
  bird2, bird3, 
  bird4, bird5, 
  bird6, bird7, 
  bird8, bird9,
  bird10, 
  raven, cuckoo };
int nbrBirds = sizeof(birds) / sizeof(birds[0]);

/**
 * Make some birds sing in random order 
 * and then wait msPause milliseconds
 */
void birdConcert(int msPause)
{
   for (int i = 0; i < random(4, 13); i++)
   {
       int b = random(nbrBirds);
       printf("Bird %d is singing\n", b);
       birds[b]();
   }
    delay(msPause);
}

void setup() 
{
    Serial.begin(115200);
    pinMode(PIN_BUZZER, OUTPUT);
    signet();
}

double fStart = 1000;
double fStop  = 3000;
int nSteps = 20;
int nPeriods = 30;

void loop()
{
  birdConcert(random(1000, 5000));
  printf("\n");

  /*
  chirp(fStart, fStop, nSteps, nPeriods, 1, chromaticScale, 50, 1000);
  chirp(fStop, fStart, nSteps, nPeriods, 1, chromaticScale, 50, 1000);

  chirp(fStart, fStop, nSteps, nPeriods, 1, linearScale,    50, 1000);
  chirp(fStop, fStart, nSteps, nPeriods, 1, linearScale,    50, 1000);

  chirp(fStart, fStop, nSteps, nPeriods, 1, sinusScale,     50, 1000);
  chirp(fStop, fStart, nSteps, nPeriods, 1, sinusScale,     50, 1000);

  chirp(fStart, fStop, nSteps, nPeriods, 1, cosinusScale,   50, 1000);
  chirp(fStop, fStart, nSteps, nPeriods, 1, cosinusScale,   50, 1000);

  chirp(fStart, fStop, nSteps, nPeriods, 1, atanScale,      50, 1000);
  chirp(fStop, fStart, nSteps, nPeriods, 1, atanScale,      50, 1000);
  */  
}
