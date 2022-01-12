# Bird Call Simulator
My attempts to imitate different bird calls ended up in a highly parameterizable function that can also serve as a sweep generator for other experiments. I also discovered that a lambda expression is useful to make the code more concise.

The basic function for sound generation is to turn a buzzer on and off. For simplicity, the on time and the off time should be the same. If we pack this functionality into a lambda expression and call it buz, we get:
```
    // lambda expression to toggle a buzzer once 
    // with a duty cycle of 50% |¨¨¨|___|
    //                           T0  T0
    auto buz = [](uint32_t usT0){ 
        digitalWrite(PIN_BUZZER, HIGH);
        delayMicroseconds(usT0); 
        digitalWrite(PIN_BUZZER, LOW);
        delayMicroseconds(usT0);};
```
If we call this function n times, we get a tone consisting of n square waves of the period 2*T0 microseconds.
A bird call, however, is not a sound of a single frequency, but a periodic chirping with rising or falling frequencies.

So the chirp function I want to design surely needs the parameters "start frequency" and "end frequency". Additionally I want to be able to define in how many steps the end frequency should be reached. And as already mentioned, I can specify how many periods the tone of a single frequency should consist of.

I could do this by adding a small frequency difference at each step. However, it is also easy to divide the frequency range into equal intervals, as in the chromatic scale, and multiply the frequency of successive notes by a constant factor. 

A chirp function that sweeps a certain frequency range once in n steps could therefore look like this:
```
    chirp(fStart, fEnd, nSteps, nPeriods)
```
But wait, a bird chirps several times with small interruptions between each peep. So the function gets 2 more parameters, namely nChirps and msPause, which specify how many chirps should sound and in which time interval.

The whole conversion of frequencies into periods and the determination of the frequency multiplier are hidden in the function body and the "birdcall designer" can fully concentrate on the selection of the function parameters to imitate a specific bird.

After all these considerations I decided to make the duty cycle variable as well (1 .. 99%) and finally the chirp function looked like this:

## Exponential Chirp
Because successive frequencies are multiplied by a constant factor at each step, the frequencies increase exponentially.
```
void chirp(uint32_t fStart, uint32_t fStop, int nSteps, int nPeriods, int nChirps, int duty, uint32_t msPause)
{
    double pStart = 1000000.0 / (double)fStart;
    double pStop  = 1000000.0 / (double)fStop;
    // We calculate the multiplicator k to get fStop in nSteps
    // fStop = fStart * k ^ nSteps ---> 1/pStop = 1/pStart * k ^ nSteps ---> pStart/pStop = k ^ nSteps
    // But we use the periods, because that fits better to toggle the buzzer 
    double k = log(pStart / pStop) / (double)nSteps;  
    k = exp(-k);  // here we get actually 1/k, because we will multiply in the buzzer loop (see beloW) ❗

    // lambda expression to toggle a buzzer once 
    // with a duty cycle of duty% |¨¨¨|______|
    //                             tOn  tOff
    auto buz = [](uint32_t usTon, uint32_t usToff){  
        digitalWrite(PIN_BUZZER, HIGH);  
        delayMicroseconds(usTon);       
        digitalWrite(PIN_BUZZER, LOW); 
        delayMicroseconds(usToff);};

    for (int n = 0; n < nChirps; n++) // output nChirps
    { 
        uint32_t p = (uint32_t)round((double)pStart);

        for (int s = 0; s <= nSteps; s++)
        {
            uint32_t tOn  = period * duty / 100;
            uint32_t tOff = period - tOn;

            // output nPulses with same pitch
            for (int n = 0; n < nPeriods; n++) buz(tOn, tOff); 
            period *= k; // calculate next period ❗
        }
        delay(msPause);
    } 
}
```
The diagram below shows the generated frequencies with a chirp over 3 octaves from 440 Hz to 3520 Hz in 12 steps per octave.

![Chirp_0](images/chirps_exp.jpg)

## Let's take a look at some function calls:
```
    chirp(1, 1, 1, 1, 1, 50, 2000)
```

Start and end frequency are each 1 Hz. The end frequency is reached in one step and the two frequencies each consist of one period and the chirp is executed once. The duty cycle is 50%. At the end of this chirp there is a 2 sec. wait. But because I call the function repeatedly in the main loop of the program, the pulse train also appears repeatedly in the screenshot from my scope (see below).

![Chirp_1](images/chirp111112000.png)
## ---

```
    chirp(1, 2, 1, 1, 1, 50, 2000)
```
Same as above, but the final frequency is 2 Hz.

![Chirp_2](images/chirp121112000.png)
## ---

```
    chirp(1, 4, 4, 1, 1, 50, 2000)
```
The start frequency is 1 HZ, the end frequency is 4 Hz. This is reached in 4 steps and each intermediate frequency consists of 1 period and 1 chirp is executed. At the end also 2000 ms are waited.

![Chirp_3](images/chirp144112000.png)
## ---

```
    chirp(880, 440, 12, 10, 1, 50, 2000);
```
Generates the 12 semitones of the chromatic scale starting at 880 Hz and ending at the concert pitch 440 Hz. Each tone consists of 10 periods. To hear the individual tones better, increase the number of periods from 10 to 1000. 

![Chirp_3](images/chirp880440121011000_full.png) 

Chromatic chirp over one octave from 880 Hz to 440 Hz
## ---

![Chirp_3](images/chirp880440121011000_start.png)

Zoomed in at start of chromatic chirp, beginning with 880 Hz
## --- 

![Chirp_3](images/chirp880440121011000_end.png)

Zoomed in at end of chromatic chirp, ending with 440 Hz
## --- 

Let's combine several chirps to imitate different birds, e.g. the ***cuckoo***.
```
    void cuckoo()
    {
    const float minorThird = 1.335;
    const float cuc = 739.989;          // F#5
    const float koo = cuc / minorThird; // C#5

    chirp(cuc, cuc, 1, 46, 1, 50, 200);
    chirp(koo, koo, 1, 52, 1, 50, 830);
    }
```
![Cuckoo](images/cuckoo.jpg)

Start and end frequency of "cuc" as well as of "koo" are the same. Therefore, the two calls sound 2 * 46 * 1/740 = 124 ms and 2 * 52 * 1/554.3 = 188 ms long with a pause of 200 ms in between.

This does not sound very natural, because the real cuckoo does not call with square waves, but with almost pure sine tones.

The cawing of a ***raven*** comes quite close to the real call if we reduce the duty cycle to 20% and call the function like this:
```
void raven()
{
  chirp(75,65,8,4,random(2, 6), 20, 350);
}
```

## Sinusoidal Chirp
Why would a bird change the frequencies of its beeps only exponentially? Interesting effects can be created if the frequencies of a chirp are changed sinusoidally. 

We map the step interval (0 .. nSteps) to the range (0 .. 2Pi), so we run through this range in nSteps steps. For each step we now calculate the corresponding frequency in the following way: 
```
    f = fm + fa * sin(k * s + phase)
    with
    fm = (fStart + fStop) / 2    The "middle" frequency
    fa = (fStop - fStart) / 2    The "frequency swing (amplitude)" around fm 
    k  = TWO_PI / nSteps;
    s  = step number, 0 .. nSteps
    phase = 0 or Pi/2    Phase shift to allow falling or rising chirps
```
The function looks like this:
```
void chirp_sinus(uint32_t fStart, uint32_t fStop, int nSteps, int nPeriods, bool phaseShifted, int nChirps, int duty, uint32_t msPause)
{
  double fm = (double)(fStart + fStop) / 2.0;          // arithmetic mean  
  double fa = ((double)fStop - (double)fStart) / 2.0;  // max. frequency swing around fm
  double k = TWO_PI / nSteps;
  double phase = phaseShifted ? HALF_PI : 0.0;

    auto buz = [](uint32_t usTon, uint32_t usToff){
        digitalWrite(PIN_BUZZER, HIGH);
        delayMicroseconds(usTon);
        digitalWrite(PIN_BUZZER, LOW);
        delayMicroseconds(usToff);};

  for (int n = 0; n < nChirps; n++) // output nChirps
  {
    for (int s = 0; s <= nSteps; s++)
    {
      double f = fm + fa * sin(k * s + phase); // get next frequency
      double p = 1000000.0 / f;
      uint32_t tOn  = p * duty / 100.0;
      uint32_t tOff = p - tOn;

      for (int n = 0; n < nPeriods; n++) buz(tOn, tOff);
    }
    delay(msPause);
  }
}
```

---

If we choose only a few steps to go through the frequency range, we will hear the individual tones well. For example:
```
chirp_sinus(440, 1320, 6, 500, false, 1, 50, 5000);  // without phase shift
```
runs through the frequencies 880, 1261,1261, 880, 499, 499, 880 

or

```
chirp_sinus(440, 1320, 6, 500, true, 1, 50, 5000);  // with phase shift
```
runs through the frequencies 1320, 1100, 660, 440, 660, 1100, 1320

![Chirp_Sinus_1](images/chirp_sinus_1.jpg)

---

If we swap the start and stop frequencies, we get the following sound sequences:
```
chirp_sinus(1320, 440, 6, 500, false, 1, 50, 5000);  // without phase shift
```
runs through the frequencies 880, 499,499, 880, 1261, 1261, 880 

or

```
chirp_sinus(1320, 440, 6, 500, true, 1, 50, 5000);  // with phase shift
```
runs through the frequencies 440, 660, 1100, 1320, 1100, 660, 440

![Chirp_Sinus_2](images/chirp_sinus_2.jpg)

---
## Phaser
We can also keep the frequency constant and only change the duty cycle of the square wave. This changes the timbre of the tone. A corresponding function looks like this:
```
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
```
---
## Generalized Chirp
I was about to finish the  *birdsong* project when I took another look at the various chirp functions. Actually they all do the same, they only differ in how the chirp frequencies are generated. It must be possible to add another parameter "frequency generator" to the basic function and to program only different **frequency generators**. So let's define a type for the frequency generator function:
```
typedef double (*FreqGen)(int stepNbr, double fStart, double fStop, int nSteps);
```
The generalized chirp function now looks like this:
```
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
      double fNext = fgen(s, fStart, fStop, nSteps);  //🔴 calling the frequency generator

      double p = 1000000.0 / fNext;      // calculate the period
      uint32_t tOn  = p * duty / 100.0;  // calculate on and off time
      uint32_t tOff = p - tOn;
      //log_i("%2d: f = %5.2f, ton = %d, toff = %d", s, fNext, tOn, tOff);
      for (int n = 0; n < nPeriods; n++) buz(tOn, tOff);
    }
    delay(msPause);
  }
}
```
Now we only have to program the generators for exponential and sinusoidal frequency change. We remove the phase shift by Pi/2 introduced in the function chirp_sinus and code a cosine generator instead, which gives the same result.

### Generator for exponentially (chromatically) varying frequencies
```
double chromaticScale(int stepNbr, double fStart, double fStop,int nSteps)
{
    // We calculate the multiplicator k to get fStop in nSteps
    // fStop = fStart * k ^ nSteps
    double k = log(fStop / fStart) / (double)nSteps;  
    double fNext = fStart * exp(k * stepNbr);
    return fNext;
}
```
### 2 Generators for frequencies following a sine
The range of steps 0 .. nSteps is mapped to 0 .. Pi or to 0 .. 2Pi
```
double sinePiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double fa = (fStop - fStart);  // max. frequency swing
  const double k = PI / nSteps;
  double fNext = fStart + fa * sin(k * stepNbr); // get next frequency
  return fNext;
}
```
```
double sine2PiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double fm = (fStart + fStop) / 2.0;  // arithmetic mean
  double fa = (fStop - fStart) / 2.0;  // max. frequency swing around fm
  const double k = TWO_PI / nSteps;
  double fNext = fm + fa * sin(k * stepNbr); // get next frequency
  return fNext;
}
```

### 2 Generators for frequencies following a cosine
The range of steps 0 .. nSteps is mapped to 0 .. Pi or to 0 .. 2Pi
```
double cosinePiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double fm = (fStart + fStop) / 2.0;  // arithmetic mean
  double fa = (fStop - fStart) / 2.0;  // max. frequency swing around fm
  const double k = PI / nSteps;
  double fNext = fm - fa * cos(k * stepNbr); // get next frequency
  return fNext;
}
```
```
double cosin2PiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double fm = (fStart + fStop) / 2.0;  // arithmetic mean
  double fa = (fStop - fStart) / 2.0;  // max. frequency swing around fm
  const double k = TWO_PI / nSteps;
  double fNext = fm + fa * cos(k * stepNbr); // get next frequency
  return fNext;
}
```

And here is a linear generator
### Generator for linearly varying frequencies
```
double linearScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double k = (fStop - fStart) / nSteps;
  double fNext = fStart + k * stepNbr;
  return fNext;
}
```
The generated frequencies are decreasing because the start frequency is greater than the stop frequency.

![Chirp_Linear](images/chirp_linear.jpg)

---

### 2 Generator for frequencies following an arctan
The range of steps 0 .. nSteps is mapped to 0 .. Pi or to 0 .. 2Pi

```
double atanPiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double k = (fStop - fStart)/atan(PI);
  double fNext = fStart + k * atan(PI/nSteps * stepNbr);
  return fNext;
}
```
```
double atan2PiScale(int stepNbr, double fStart, double fStop,int nSteps)
{
  double k = (fStop - fStart)/atan(TWO_PI);
  double fNext = fStart + k * atan(TWO_PI/nSteps * stepNbr);
  return fNext;
}
```
![Chirp_Atan](images/chirp_atan.jpg)

---

## Summary
We have implemented a universal chirp function that can generate frequencies that follow a mathemathical rule.
For comparison, we call all generators and plot the generated frequencies all together in one diagram. The frequency range is swept in 5 steps.

The first diagram shows the frequencies when fStart is smaller than fStop so that all chirps start with increasing frequencies. 

![Chirp_All_Up](images/chirp1000-3000-5-80-1-xxxScale-50-200.jpg)

---

The second diagram shows the frequencies when fStart and fStop are swapped so that all chirps start with decreasing frequencies.

![Chirp_All_Down](images/chirp3000-1000-5-80-1-xxxScale-50-200.jpg)

---

## Program Code
There are several "birds" implemented in the program, which are called to sing in random order. Maybe a reader programs an alarm clock which greets him in the morning with a bird concert.
