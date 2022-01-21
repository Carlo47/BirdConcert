#include <Arduino.h>

// typedef double (*FreqGen)(int stepNbr, double fStart, double fStop,int nSteps); // is equivalent to "using FreqGen = ... "
using FreqGen = double (&)(int stepNbr, double fStart, double fStop, int nSteps);
using FreqGenSinc = double (&)(int stepNbr, double fStart, double fStop, int nSteps, int nPi);

double linearScale(int stepNbr, double fStart, double fStop,int nSteps);
double chromaticScale(int stepNbr, double fStart, double fStop,int nSteps);
double sinePiScale(int stepNbr, double fStart, double fStop,int nSteps);
double sine2PiScale(int stepNbr, double fStart, double fStop,int nSteps);
double cosinePiScale(int stepNbr, double fStart, double fStop,int nSteps);
double cosine2PiScale(int stepNbr, double fStart, double fStop,int nSteps);
double atanPiScale(int stepNbr, double fStart, double fStop,int nSteps);
double atan2PiScale(int stepNbr, double fStart, double fStop,int nSteps);

double sincScaleNpi_Npi(int stepNbr, double fStart, double fStop,int nSteps, int nPi);
double sincScale0_Npi(int stepNbr, double fStart, double fStop,int nSteps, int nPi);
double sincScaleNpi_0Scale(int stepNbr, double fStart, double fStop,int nSteps, int nPi);

class Chirpmaker
{
    public:
        using Bird = void (Chirpmaker::*)();

        Chirpmaker(uint8_t pinBuzzer) : _pinBuzzer(pinBuzzer)
        {
            pinMode(_pinBuzzer, OUTPUT);

            _birds[0] = &Chirpmaker::_bird0; // Store the birds in an array
            _birds[1] = &Chirpmaker::_bird1;
            _birds[2] = &Chirpmaker::_bird2;
            _birds[3] = &Chirpmaker::_bird3;
            _birds[4] = &Chirpmaker::_bird4;
            _birds[5] = &Chirpmaker::_bird5;
            _birds[6] = &Chirpmaker::_bird6;
            _birds[7] = &Chirpmaker::_bird7;
            _birds[8] = &Chirpmaker::_bird8;
            _birds[9] = &Chirpmaker::_bird9;
            _birds[10] = &Chirpmaker::_bird10;
            _birds[11] = &Chirpmaker::_cuckoo;
            _birds[12] = &Chirpmaker::_raven;
            _birds[13] = &Chirpmaker::_chaffinch;
            _birds[14] = &Chirpmaker::_blackbird;
        }

        void chirp(double fStart, double fStop, int nSteps, int nPeriods, int nChirps, FreqGen fgen, int duty, uint32_t msPause);
        void chirp(double fStart, double fStop, int nSteps, int nPeriods, int nPi, FreqGenSinc fgen, int duty, uint32_t msPause);
        void phaser(uint32_t freq, int nPeriods, int dutyStart, int dutyEnd, int nChirps, uint32_t msPause);
        void birdVoice(uint8_t birdNbr, uint32_t msPause);
        void birdConcert(uint32_t msPause);
        void signet();
        void phoneCall(uint8_t nTimes);
        void cuckoo();
        void raven();
        void chaffinch();
        void blackbird();

    private:
        uint8_t _pinBuzzer;

        void _bird0();
        void _bird1();
        void _bird2();
        void _bird3();
        void _bird4();
        void _bird5();
        void _bird6();
        void _bird7();
        void _bird8();
        void _bird9();
        void _bird10();
        void _cuckoo();
        void _raven();
        void _chaffinch();
        void _blackbird();
        
        Bird _birds[15];
        int _nbrBirds = sizeof(_birds) / sizeof(_birds[0]);
};