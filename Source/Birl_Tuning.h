#ifndef BIRL_TUNING_H
#define BIRL_TUNING_H

#include "Birl.h"

typedef enum {EQUAL_TEMPERED, JUST_INTONATION, MEANTONE, HIGHLAND_BAGPIPE, CUSTOM} tuningSystem;
enum {NUM_NOTES = 11};
static double justIntonation[] = {10.0/4.0, 18.0/8.0, 2.0/1.0, 15.0/8.0, 5.0/3.0, 3.0/2.0, 4.0/3.0, 5.0/4.0, 9.0/8.0, 1.0, 15.0/16.0};
/* double justIntonation[] = {10.0/4.0, 18.0/8.0, 2.0/1.0, 15.0/8.0, 5.0/3.0, 3.0/2.0, 4.0/3.0, 5.0/4.0, 9.0/8.0, 1.0, 15.0/16.0, 5.0/6.0, 3.0/4.0, 4.0/6.0, 5.0/8.0}; */
static double equalTempered[] = {2.519840, 2.244920, 2.000000, 1.887750, 1.681790, 1.498310, 1.334830, 1.259920, 1.122460, 1.000000, 0.9439};
static double meanTone[] = {2.5, 2.236, 2.0000, 1.8692, 1.6719, 1.4953, 1.3375, 1.2500, 1.1180, 1.0000, 0.9346};
static double highlandBagpipe[] = {2.25, 2., 1.8, 1.66666, 1.5, 1.35, 1.25, 1.125, 1, 0.9, 0.83333};
static double customTuning[] = {2.519840, 2.244920, 2.000000, 1.887750, 1.681790, 1.498310, 1.334830, 1.259920, 1.122460, 1.000000, 0.9439};

const double MIN_D1 = 1.0; // = 0.002 * 2 * 200
const double DH_FIRST_GUESS = 1.0;

// Set a pointer called tuning, which will ultimately point to one of the arrays containing ratios.
static inline double *tuning;

// Convert from centimeters to samples per second.
static inline double convertToSamples(double cm) {
    return (cm * ((SRATE*OVERSAMPLE) / C_cm));
}

// Convert from samples per second to centimeters.
static inline double convertTocm(double samps) {
    return (samps * (C_cm / (SRATE*OVERSAMPLE)));
}

//static inline double calcLS(double Fc) {
//    return (SRATE*OVERSAMPLE)/(4.0 * Fc);
//}
static inline double calcLS(double Fc) {
    return C_cm / (4.0 * Fc);
}

static inline double calcg(int thNum) {
    double temp = tuning[thNum];

    if (thNum < 0 || thNum > NUM_NOTES) {
        printf("calcg: thNum out of bounds: %d\n", thNum);
        return 0.0;
    }
    if (thNum == 0) {
        return temp;
    }
    else
        return (temp / tuning[thNum-1]);
}

static inline double calcd1(int LC, double LS) {
//    return (LS - (double) LC)/(0.3 * convertToSamples(2.54));
     return (LS - (double) LC)/0.3;
}

static inline int calcLC(double LS) {
    double LC = (int) LS;
    double d1 = calcd1(LC, LS);
    while (d1 < MIN_D1) {
        printf("calcLC: d1 = %f for this value of LC so we're shortening LC!!!!!!\n", d1);
        LC -= 1.0;
        d1 = calcd1(LC, LS);
    }
    return LC;
}

static inline double calcLBh(int thNum, double LSh, int lLint) {
    double g = calcg(thNum);
    double lL = (double) lLint;

    double gLSh = g * LSh;
    double nmrtr1 = (LSh + 0.5*gLSh - lL) * (LSh + 0.5*gLSh - lL);
    nmrtr1 /= gLSh;
    return nmrtr1 - (gLSh / 4.0);
}


/** Calculates and returns Lh, the "per tonehole cut length."
 */
static inline int calclL(double d1, int thNum, double LS) {
    double dH = 1.0 * OVERSAMPLE;
    double g = calcg(thNum);
    double LSh = (1.0/tuning[thNum]) * LS;
    double LBh = dH * ((d1*d1)/(dH*dH)) - 0.45*d1;
//    double z = 0.5 * g * sqrt(1 + 4*(LBh/(g*LSh))) - 0.5*g;
    double z = 0.5 * g * sqrt(1 + 4*(LBh/(g*LS))) - 0.5*g;
//    return (int) (LSh - (z*LSh));
    return (int) (z*LSh);

}
//
static inline double calcdH(int thNum, double d1, double LS, int lL) {
    double LSh = (1.0/tuning[thNum]) * LS;
    double LBh = calcLBh(thNum, LSh, lL);
    return (d1*d1) / (LBh + 0.45 * d1);
}
//static inline double calcdH(int thNum, double d1, double LS, int lL) {
//    double LSh = (1.0/tuning[thNum]) * LS;
//    double LBh = calcLBh(thNum, LSh, lL);
//    double chunk = (LBh + 0.45*d1) / (d1*d1);
//    double numerator = 1.0 + sqrt(1.0 + 4*lL*chunk);
//    double denominator = 2.0 * chunk;
//    return numerator / denominator;
//}

static inline void populateCustomTuning(double freqs[NUM_NOTES]) {
    double Fc = freqs[NUM_NOTES-1];
    /* printf("fundamental: %f\n", Fc); */
    for (int i = 0; i < NUM_NOTES; i++) {
        customTuning[i] = freqs[i] / Fc;
    }
}

static inline void setTuning(tuningSystem t) {
    switch (t) {
        case JUST_INTONATION: {
            tuning = justIntonation;
            break;
        }
        case EQUAL_TEMPERED: {
            tuning = equalTempered;
            break;
        }
        case MEANTONE: {
            tuning = meanTone;
            break;
        }
        case HIGHLAND_BAGPIPE: {
            tuning = highlandBagpipe;
            break;
        }
        case CUSTOM: {
            tuning = customTuning;
            break;
        }
        default: {
            break;
        }
    }
}

static inline double checkTuning(double d1, double dH, double LSh, double lL, double g) {
    double LBh = dH * ((d1*d1)/(dH*dH)) - 0.45*d1;
    double z = 0.5 * g * sqrt(1 + 4*(LBh/(g*LSh))) - 0.5*g;
    return (SRATE*OVERSAMPLE)/(4 * (lL + (z*LSh)));

}

#endif


