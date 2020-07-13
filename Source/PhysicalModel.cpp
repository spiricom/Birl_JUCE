#include "PhysicalModel.h"

PhysicalModel::PhysicalModel()
{
    numToneHoles_ = NUM_OF_KEYS;
    numTubes_ = NUM_OF_KEYS+1;
    tubeIndex_ = 0;

    
    dcBlocker_ = initDCFilter(0.995);
    dcBlocker2_ = initDCFilter(0.995);
    biquad_ = initBiquad();
    biquadSetCoeffs(biquad_, 0.169301, 0.338601, 0.169301, -0.482013, 0.186622);
    pf_ = initSVF(2000.0, 0.5);
    lp_ = initSVF(5000.0, 0.5);
    pf2_ = initSVF(1000.0, 1.0);
    lp2_ = initSVF(5000.0, 0.5);
    noiseBP_ = initSVF(16000.0, 1.0);
        
    outputGain_ = 1.0;
    noiseGain_ = 0.2;
    
    breathPressure_ = 0.0;
    prevBreathPressure_ = 0.0;
    count = 0;
    min = 0.0;
    max = 0.0;
    m_drive_ = 0.0;
    shaperMix_ = 0.0;
    srand((unsigned)time(NULL));

}
PhysicalModel::~PhysicalModel() { }

void PhysicalModel::setToneHoleRadius(int index, double radius) {
    if (radius < MIN_TONEHOLE_RADIUS || radius > MAX_TONEHOLE_RADIUS) {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
            "Radius out of range",
            "Error: Radius is out of range",
            "OK");
        return;
    }
    rth_[index] = radius;
    scatter_[index] = -pow(radius,2) / ( pow(radius,2) + 2*pow(rb_,2) );

    // Calculate toneHole coefficients.
    double te = radius;
    thCoeff_[index] = (te*2*(SRATE*OVERSAMPLE) - C_m) / (te*2*(SRATE*OVERSAMPLE) + C_m);
}

void PhysicalModel::setToneHole(int index, double newValue) {
    double new_coeff;
    if (newValue < 0.0)
        new_coeff = 0.9995;
    else if (newValue >= 1.0)
        new_coeff = thCoeff_[index];
    else
        new_coeff = newValue * (thCoeff_[index] - 0.9995) + 0.9995;
//    toneHoles_[index] = initPoleZero();
    setA1PoleZero(toneHoles_[index], new_coeff);
    setB0PoleZero(toneHoles_[index], new_coeff);
}

void PhysicalModel::setLength(int index, int length) {
//    if (index < 0 || index >= numTubes_) {
//        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
//            "Range Error",
//            "Error: Index is out of range",
//            "OK");
//        return;
//    }
//    if (length < 0 || length >= MAX_TUBE_LENGTH) {
//        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
//            "Range Error",
//            "Error: Length is out of range",
//            "OK");
//        return;
//    }
//    tubeLengths_[index] = length;
//    tubes_[index] = initTube(length);
}

void PhysicalModel::setBreathPressure(double input) {
    breathPressure_ = input;
}



void PhysicalModel::calcTHCoeffs() {
    scatter_[0] = -pow(rth_[0],2) / ( pow(rth_[0],2) + 2*pow(rb_,2) );
    
    thCoeff_[0] = (rth_[0]*2*(SRATE*OVERSAMPLE) - C_m) / (rth_[0]*2*(SRATE*OVERSAMPLE) + C_m);

    // Initialize tone holes.
    toneHoles_[0] = initPoleZero();
    setA1PoleZero(toneHoles_[0], -thCoeff_[0]);
    setB0PoleZero(toneHoles_[0], thCoeff_[0]);
    setB1PoleZero(toneHoles_[0], -1.0);
}

#if 0
void PhysicalModel::tune(double Fc) {
//    double LS = calcLS(Fc);
//    int LC = calcLC(LS);
//    double d1 = calcd1(LC, LS);
//    printf("LC: %d\n", LC);
//    printf("d1: %f\n", d1);
//
//    int prevlL = 0;
//
//    for (int i = 0; i < numTubes_; i++) {
////        printf("tubelength %d: %d\n", i, calclL(d1, i, LS) - prevlL);
////        tubeLengths_[i] = calclL(d1, i, LS) - prevlL;
//        tubeLengths_[i] = calclL(d1, i, LS);
//
//        if (tubeLengths_[i] == 0) {
//            printf("ERROR: Integer delay line lengths clash!!!!! Use a different tuning or try oversampling.\n");
//            return;
//        }
//        // if (tubes_[i] != NULL) {
//        //     printf("WANNA BE FREEEEEEE\n");
//        //     freeTube(tubes_[i]);
//        // }
//
//        if (i == 0) {
//            tubes_[i] = initTube(tubeLengths_[i]);
//            // prevlL += tubeLengths_[i] + correction;
//            prevlL += tubeLengths_[i];
//        } else {
//            tubes_[i] = initTube(tubeLengths_[i]);
//            prevlL += tubeLengths_[i];
//        }
//        printf("th %d: lL = %d\n", i, tubeLengths_[i]);
//    }
//    originalrb_ = d1 / 2.0;    // main bore radius, in cm.
//    rb_ = originalrb_;
//    int lL = tubeLengths_[0];
//    for (int i = 0; i < numToneHoles_; i++) {
//        originalRth_[i] = calcdH(i, d1, LS, lL)/2.0;
//        rth_[i] = originalRth_[i];
//        lL += tubeLengths_[i+1];
//    }
//    calcTHCoeffs();
}
#endif

void PhysicalModel::tune(double Fc) {
    double effectiveLength = calcLS(Fc);
    int cutLength = calcLC(effectiveLength);
    double boreDiameter = calcd1(cutLength, effectiveLength);
    
    tubeLengths_[0] = calclH(0, boreDiameter, calcLSh(0, Fc));
    tubeLengths_[1] = calclH(1, boreDiameter, calcLSh(1, Fc));
    
//    tubeLengths_[0] = calclL(boreDiameter, 0, effectiveLength);
//    tubeLengths_[1] = calclL(boreDiameter, 1, effectiveLength);
    
    if (tubeLengths_[0] == 0 || tubeLengths_[1] == 0) {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
            "Int Cast Error",
            "Error: Cut length cast down to 0. Use a different tuning or try oversampling.",
            "OK");
        return;
    }
    
    tubes_[0] = initTube(tubeLengths_[0]);
    tubes_[1] = initTube(tubeLengths_[1]);
    
    // Main bore radius, in centimeters.
    rb_ = boreDiameter / 2.0;
    
    // Calculate the toneHole radii.
    rth_[0] = 0.5 * calcdH(0, boreDiameter, tubeLengths_[0], calcLSh(0, Fc));
    
    calcTHCoeffs();
}


double PhysicalModel::interpolateLinear(double a, double b, double alpha) {
    return (alpha * a) + ((1.0-alpha) * b);
}
 
#if 0
double PhysicalModel::birlTick()
{
    double breathInterp[OVERSAMPLE];
    for (int i = 0; i < OVERSAMPLE; i++) {
        breathInterp[i] = interpolateLinear(breathPressure_, prevBreathPressure_, (double) (i+1) / (double) OVERSAMPLE);
    }
    prevBreathPressure_ = breathPressure_;

    double pap;
    double pbm;
    double pthm;
    double outsamp = 0.0;
    double scatter;
    double bellReflected;

    for (int t = 0; t < OVERSAMPLE; t++) {
        double breath = breathInterp[t];
        double noise = ((double)rand()/(double)RAND_MAX);
        noise = noiseGain_ * (inputSVFBand(noiseBP_, noise));
        // double noise = noiseGain_ * rand();
        breath += breath * noise;

        // Calculate the differential pressure = reflected - mouthpiece pressures
        double pressureDiff = accessDelayLine(tubes_[0]->lower) - breath;
        double reedLookup = pressureDiff * reedTable( pressureDiff );
//        breath = tanhClip(breath + reedLookup);
        breath = LEAF_clip(-1.0, breath + reedLookup, 1.0);
        if (breath > 1.0 || breath < -1.0) {
            printf("breath going out of bounds of -1 to 1: %f\n", breath);
        }

        for (int i = 0; i < numToneHoles_; i++) {
            // Index in tubes_[] of tube positioned before toneHoles[i].
            int a = i + FRONT_TUBES - 1;
            // Index in tubes_[] of tube positioned after toneHoles[i].
            int b = i + FRONT_TUBES;

            // Three-port junction scattering.
            pap = accessDelayLine(tubes_[a]->upper);
            pbm = accessDelayLine(tubes_[b]->lower);
            pthm = z1PoleZero(toneHoles_[i]);

            scatter = scatter_[i] * (pap + pbm - (2 * pthm));
            pbp_[i] = pap + scatter;
            pam_[i] = pbm + scatter;
            pthp_[i] = pap + scatter + pbm - pthm;

            // Sample output at tubes_[SAMPLE_INDEX].
            if (a == SAMPLE_INDEX) {
                outsamp += pap + pam_[i];
            }

            // Bell reflection at last tube.
            if (i == numToneHoles_ - 1) {
                double bell = accessDelayLine(tubes_[b]->upper);
                // double bell2 = shaper(bell1, m_drive_);
                // double bell4 = inputSVFPeak(pf2_, bell1);
                // bell = inputSVFLP(lp2_, bell);
                // // Reflection = Inversion + gain reduction + lowpass filtering.
                // bell = inputSVFLP(lp2_, bell);
                // bell = inputDCFilter(dcBlocker2_, bell);
                bellReflected = tSVF_tick(&inLoopLP, (bell * -0.995));
                // bellReflected = filter_.tick(bell * -0.995);
            }
        }

        // Perform all inputs at the end so that we're not altering
        // state prior to calculations.
        for (int i = 0; i < numToneHoles_; i++) {
            // Index in tubes_[] of tube positioned before toneHoles[i].
            int a = i + FRONT_TUBES - 1;
            // Index in tubes_[] of tube positioned after toneHoles[i].
            int b = i + FRONT_TUBES;

            inputPoleZero(toneHoles_[i], pthp_[i]);
            // toneHoles_[i]->tick(pthp_[i]);
            inputDelayLine(tubes_[a]->lower, pam_[i]);
            inputDelayLine(tubes_[b]->upper, pbp_[i]);
        }

        inputDelayLine(tubes_[0]->upper, breath);
        inputDelayLine(tubes_[numTubes_-1]->lower, bellReflected);

    }

    count++;
    // Clipping, account for oversampling, and gain.
    outsamp /= (double) OVERSAMPLE;
    outsamp = tanhClip(outsamp);
    outsamp *= outputGain_;
    return outsamp;
}
#endif

double PhysicalModel::birlTick() {
    double outsamp = 0.0;
    double bellReflected;
    
    double pap;
    double pbm;
    double pthm;
    double scatter;
    
    double breath = breathPressure_;
    double noise = (double) rand() / (double) RAND_MAX;
    noise = noiseGain_ * (inputSVFBand(noiseBP_, noise));
    breath += breath * noise;
    
    // REED
    double pressureDiff = accessDelayLine(tubes_[0]->lower) - breath;
    double reedLookup = pressureDiff * reedTable (pressureDiff);
    breath = LEAF_clip(-1.0, breath + reedLookup, 1.0);
    
    // ONE HOLE.
    int a = 0;
    int b = 1;
    
    pap = accessDelayLine(tubes_[a]->upper);
    pbm = accessDelayLine(tubes_[b]->lower);
    pthm = z1PoleZero(toneHoles_[0]);
    
    scatter = scatter_[0] * (pap + pbm - (2 * pthm));
    pbp_[0] = pap + scatter;
    pam_[0] = pbm + scatter;
    pthp_[0] = pap + scatter + pbm - pthm;
    
    outsamp += pap + pam_[0];
    
    double bell = accessDelayLine(tubes_[b]->upper);
    outsamp += bell;
    bellReflected = bell * -0.995;

    // FIRST TUBE.
    inputDelayLine(tubes_[a]->upper, breath);
    inputDelayLine(tubes_[a]->lower, pam_[0]);

    // SECOND TUBE.
    inputDelayLine(tubes_[b]->upper, pbp_[0]);
    inputDelayLine(tubes_[b]->lower, bellReflected);
    
    // TONEHOLE.
    inputPoleZero(toneHoles_[0], pthp_[0]);


    outsamp = tanhClip(outsamp);
    
    return outsamp;
}


void PhysicalModel::setLPCutoff(double cut) {
    cut = clip(cut, 30.0, 16000.0);
    setCutoffSVF(lp_, cut);
    setCutoffSVF(lp2_, cut);
}
void PhysicalModel::setLPQ(double Q) {
    Q = clip(Q, 0.0, 1.0);
    setQSVF(lp_, Q);
    setQSVF(lp2_, Q);
}
void PhysicalModel::setPFCutoff(double cut) {
    cut = clip(cut, 30.0, 16000.0);
    setCutoffSVF(pf_, cut);
    setCutoffSVF(pf2_, cut);
}
void PhysicalModel::setPFQ(double Q) {
    Q = clip(Q, 0.0, 1.0);
    setQSVF(pf_, Q);
    setQSVF(pf2_, Q);
}
void PhysicalModel::setNoiseBPCutoff(double cut) {
    cut = clip(cut, 30.0, 16000.0);
    setCutoffSVF(noiseBP_, cut);
}
void PhysicalModel::setNoiseGain(double gain) {
    gain = clip(gain, 0.0, 1.0);
    noiseGain_ = gain;
}
void PhysicalModel::setNoiseBPQ(double Q) {
    Q = clip(Q, 0.0, 1.0);
    setQSVF(noiseBP_, Q);
}
void PhysicalModel::setShaperDrive(double d) {
    d = clip(d, 0.0, 1.0);
    m_drive_ = d;
}
void PhysicalModel::setShaperMix(double d) {
    d = clip(d, 0.0, 1.0);
    shaperMix_ = d;
}

