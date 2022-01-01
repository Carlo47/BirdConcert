/**
 * Program      birdSong.cpp
 * Author       2021-12-10 Charles Geiser ((https://www.dodeka.ch))
 * 
 * Purpose 
 * Board
 * Wiring
 * Remarks  
 */
#include <Arduino.h>

#define CLR_LINE "\r%*c\r", 80, ' '   // possible for printf() on ESP32
const uint8_t PIN_BUZZER = GPIO_NUM_4;

typedef struct { const char key; const char *txt; void (&action)(); } MenuItem;

uint32_t fStart  = 1000;
uint32_t fStop   = 1020;
int nSteps       = 5;
int nPeriods     = 2;
int nChirps      = 2;
uint32_t msPause = 50;


void showMenu();

void enterFbegin()
{
  int32_t value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseInt();
  }
  fStart = value;
  printf("chirp(%d, %d, %d, %d, %d, %d)\n", fStart, fStop, nSteps, nPeriods, nChirps, msPause);
}

void enterFend()
{
  int32_t value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseInt();
  }
  fStop = value;
  printf("chirp(%d, %d, %d, %d, %d, %d)\n", fStart, fStop, nSteps, nPeriods, nChirps, msPause);
}

void enterNsteps()
{
  int value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseInt();
  }
  nSteps = value;
  printf("chirp(%d, %d, %d, %d, %d, %d)\n", fStart, fStop, nSteps, nPeriods, nChirps, msPause);
}

void enterNpulses()
{
  int value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseInt();
  }
  nPeriods = value;
  printf("chirp(%d, %d, %d, %d, %d, %d)\n", fStart, fStop, nSteps, nPeriods, nChirps, msPause);
}

void enterNchirps()
{
  int value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseInt();
  }
  nChirps = value;
  printf("chirp(%d, %d, %d, %d, %d, %d)\n", fStart, fStop, nSteps, nPeriods, nChirps, msPause);
}

void enterMsPause()
{
  uint32_t value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseInt();
  }
  msPause = value;
  printf("chirp(%d, %d, %d, %d, %d, %d)\n", fStart, fStop, nSteps, nPeriods, nChirps, msPause);
}

// Menu definition
// Each menuitem is composed of a key, a text and an action
MenuItem menu[] = 
{
  { 'b', "[b] Enter fBegin",  enterFbegin },
  { 'e', "[e] Enter fEnd",    enterFend },
  { 's', "[s] Enter nSteps",  enterNsteps },
  { 'n', "[n] Enter nPulses", enterNpulses },
  { 'c', "[c] Enter nChirps", enterNchirps },
  { 'p', "[p] Enter msPause", enterMsPause },
  { 'S', "[S] Show menu",     showMenu },
};
constexpr uint8_t nbrMenuItems = sizeof(menu) / sizeof(menu[0]);

/**
 * @brief 
 * 
 * @param fStart    Chirp starts with this frequency
 * @param fStop     Chirp ends   with this frequency
 * @param nSteps    The frequency interval is divided into n steps 
 * @param nPulses   Every frequency step contains n pulses of the base pulse
 * @param nChirps   n chirps are played
 * @param msPause   ms Pause between chirps
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
    auto buz = [](uint32_t usPeriod){
        digitalWrite(PIN_BUZZER, HIGH);
        delayMicroseconds(usPeriod);
        digitalWrite(PIN_BUZZER, LOW);
        delayMicroseconds(usPeriod);};

    for (int n = 0; n < nChirps; n++) // output nChirps
    { 
        uint32_t p = (uint32_t)round((double)pStart);

        for (int s = 0; s <= nSteps; s++)
        {
            for (int n = 0; n < nPeriods; n++) buz(p/2); // output nPulses with same pitch
            p *= k; // calculate next period, here we should divide instead of multily ❗
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
    chirp(random(4200, 4400), random(2800, 2500), 100,  random(1,3), random(3, 9), random(5,25));
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
    chirp(3500, 3000, 25, 10, 1);
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
   for (int i = 0; i < nbrBirds-3; i++)
   {
       int bird = random(nbrBirds);
       printf("Bird %d is singing\n", bird);
       birds[bird]();
   }
    delay(msPause);
}

/**
 * Display menu on monitor
 */
void showMenu()
{
  // title is packed into a raw string
  Serial.print(
  R"TITLE(
---------------
 CLI Menu Demo 
---------------
)TITLE");

  for (int i = 0; i < nbrMenuItems; i++)
  {
    Serial.println(menu[i].txt);
  }
  Serial.print("\nPress a key: ");
}

/**
 * Execute the action assigned to the key
 */
void doMenu()
{
  char key = Serial.read();
  printf(CLR_LINE);
  for (int i = 0; i < nbrMenuItems; i++)
  {
    if (key == menu[i].key)
    {
      menu[i].action();
      break;
    }
  } 
}


void setup() 
{
    Serial.begin(115200);
    pinMode(PIN_BUZZER, OUTPUT);
    showMenu();
}

void loop()
{
  //if(Serial.available()) doMenu();
  //chirp(fStart, fStop, nSteps, nPeriods, nChirps, msPause);

  birdConcert(random(1000, 5000));
  printf("\n");

}
