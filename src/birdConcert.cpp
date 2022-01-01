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

/**
 * Simulate the chirp of a bird. Start with fStart and reach fStop in n steps.
 * Each individual frequency is composed of n periods 
 * 
 * fStart    Chirp starts with this frequency
 * fStop     Chirp ends   with this frequency
 * nSteps    The frequency interval is divided into n steps 
 * nPeriods  Every frequency step contains n periods of the base period
 * nChirps   n chirps are played
 * msPause   ms Pause between chirps
 */
void chirp(uint32_t fStart, uint32_t fStop, int nSteps, int nPeriods, int nChirps, uint32_t msPause = 50)
{
    double pStart = 1000000.0 / (double)fStart;
    double pStop  = 1000000.0 / (double)fStop;
    // We calculate the multiplicator k to get fStop in nSteps
    // fStop = fStart * k ^ nSteps ---> 1/pStop = 1/pStart * k ^ nSteps ---> pStart/pStop = k ^ nSteps
    // But we use the periods, because that fits better to toggle the buzzer 
    double k = log(pStart / pStop) / (double)nSteps;  
    k = exp(-k);  // here we get actually 1/k, because we will multiply in the buzzer loop (see beloW) ❗

    // lambda expression to toggle buzzer once with duty cycle 50%  |¨¨|__|
    //                                                               T0 T0
    auto buz = [](uint32_t usT0){
        digitalWrite(PIN_BUZZER, HIGH);
        delayMicroseconds(usT0);
        digitalWrite(PIN_BUZZER, LOW);
        delayMicroseconds(usT0);};

    for (int n = 0; n < nChirps; n++) // output nChirps
    { 
        uint32_t p = (uint32_t)round((double)pStart);

        for (int s = 0; s <= nSteps; s++)
        {
            for (int n = 0; n < nPeriods; n++) buz(p/2); // output nPulses with same pitch
            p *= k; // calculate next period, here we should divide instead of multiply ❗
        }
        delay(msPause);
    }  
}


typedef void (*bird)();  // bird is a pointer to a function taking no parameters and returning void

// Define some birds with different chirps
void bird0()
{
    chirp(random(1200, 1900), random(4300, 4500), random(10, 42), random(1,5), 5, random(59, 199));
    chirp(random(2000, 2050), random(3200, 3400), random(5, 30),  random(2,15), random(4, 10));
    chirp(1500, 4500, random(50, 150), random(1, 13), random(1, 5), 100);
}

void bird1()
{
    chirp(random(4200, 4400), random(2800, 2500), 100,  random(1,3), random(3, 9), random(25, 75));
}

void bird2()
{
    chirp(random(3500,3900), random(5600,5900), random(2,5), random(2,6), 1, random(50, 100));
    chirp(random(5600,5900), random(3500,3900), random(6,15), random(3,7), 1, random(50, 100));
};

void bird3()
{
    chirp(random(1280,1300), random(1310,1620), 10, random(4,8), random(2,9), random(100, 200));
}

void bird4()
{
    chirp(4000, 4800, 10, 4, random(10, 15));
    chirp(3500, 4300, 15, 10, 1);
    chirp(3500, 3000, 25, 10, 1, random(75, 150));
};

 void bird5()
 {
   chirp(random(4404, 4484), random(4380,4420), 10, random(1,4), random(1,7));
 }

 void bird6()
 {
   chirp(random(1000, 1050), random(900, 1200), 20, random(1, 5), random(10, 15), random(150, 250));
 }

 void bird7()
 {
   chirp(2600, 4400, 10, 1, random(5,9));
 }

void cuckoo()
{
  const float minorThird = 1.335;
  const float cuc = 739.989;          // F#5
  const float koo = cuc / minorThird; // C#5

  chirp(cuc, cuc, 1, 46, 1, 200);
  chirp(koo, koo, 1, 52, 1, 830);
}

// Store the birds in an array
bird birds[] = { bird0, bird1, bird2, bird3, bird4, bird5, bird6, bird7, cuckoo };
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
}

void loop()
{
    birdConcert(random(1000, 5000));
    printf("\n");
}
