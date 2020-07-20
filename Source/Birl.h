#ifndef BIRL_H
#define BIRL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int NUM_OF_KEYS = 9;
const int NUM_OF_BUTTONS = 9;
const double CLIP_RATIO = 0.7;
const int BIRL_PORT = 1234;

const int FRONT_TUBES = 1;
const int MAX_TONEHOLES = NUM_OF_KEYS;
const int MAX_TUBES = NUM_OF_KEYS + FRONT_TUBES;
const int SAMPLE_INDEX = 0;
const int MAX_TUBE_LENGTH = 100;
const double MIN_TONEHOLE_RADIUS = 0.0001;
const double MAX_TONEHOLE_RADIUS = 0.004;
const double RB_TWEAK_FACTOR = 0.0001;
const double RTH_TWEAK_FACTOR = 0.005;

static double originalRth_[MAX_TONEHOLES];
static double rth_[MAX_TONEHOLES];
static double tubeLengths_[MAX_TUBES];


const int OVERSAMPLE = 1;

const double C_cm = 34500.0;
const double C_m = 345.0;
const double SRATE = 44100.0;
const double reedTableOffset = 0.7;
const double reedTableSlope = -0.3;
/* const double MIN_D1 = 0.05; */

static float keys_[NUM_OF_KEYS];
static bool buttons_[NUM_OF_BUTTONS];
static int maxKeyArg[NUM_OF_KEYS];
static int minKeyArg[NUM_OF_KEYS];
static double keysGradient [NUM_OF_KEYS];



#endif

