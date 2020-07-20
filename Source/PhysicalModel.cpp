#include "PhysicalModel.h"

PhysicalModel::PhysicalModel()
{
    numToneHoles_ = NUM_OF_KEYS;
    numTubes_ = NUM_OF_KEYS+1;
    tubeIndex_ = 0;
//    ftubes_[0] = initFracTube(25.5);
    tubes_[0] = initTube(25);

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
    if (newValue <= 0.0)
        new_coeff = 0.9995;
    else if (newValue >= 1.0)
        new_coeff = thCoeff_[index];
    else
        new_coeff = newValue * (thCoeff_[index] - 0.9995) + 0.9995;
    setA1PoleZero(toneHoles_[index], -new_coeff);
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
    // Calculate the initial tonehole three-port scattering coefficient.
    scatter_[0] = -pow(rth_[0],2) / ( pow(rth_[0],2) + 2*pow(rb_,2) );
    
    // Calculate tonehole coefficients and set for initially open.
    float te = 1.4 * rth_[0]; // effective length of the open hole
    thCoeff_[0] = (te*2*(SRATE*OVERSAMPLE) - C_m) / (te*2*(SRATE*OVERSAMPLE) + C_m);
//    thCoeff_[0] = (rth_[0]*2*(SRATE*OVERSAMPLE) - C_m) / (rth_[0]*2*(SRATE*OVERSAMPLE) + C_m);

    // Initialize tone holes.
    setA1PoleZero(toneHoles_[0], -thCoeff_[0]);
    setB0PoleZero(toneHoles_[0], thCoeff_[0]);
    setB1PoleZero(toneHoles_[0], -1.0);
}

void PhysicalModel::tune(double Fc) {
    double effectiveLength = calcLS(Fc);
    int cutLength = calcLC(effectiveLength);
    double boreDiameter = calcd1(cutLength, effectiveLength);
    
    tubeLengths_[0] = calclH(0, boreDiameter, calcLSh(0, Fc));
//    C.O. : FINAL TUBE USES FRACTIONAL DELAY LENGTH.
//    tubeLengths_[1] = calclH(1, boreDiameter, calcLSh(1, Fc));
    tubeLengths_[1] = calcLSh(1, Fc);

    
    if (tubeLengths_[0] == 0 || tubeLengths_[1] == 0) {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
            "Int Cast Error",
            "Error: Cut length cast down to 0. Use a different tuning or try oversampling.",
            "OK");
        return;
    }
    // Initialize tonehole.
//    toneHoles_[0] = initPoleZero();

    // Initialize tube A.
    tubes_[0] = initTube(tubeLengths_[0]);
    
    // Initialize tube B.
//    tLinearDelay_init (&endTube_[0], tubeLengths_[1], (int)(tubeLengths_[1]+1));
//    tLinearDelay_init (&endTube_[1], tubeLengths_[1], (int)(tubeLengths_[1]+1));
    
    // Main bore radius, in centimeters.
    rb_ = boreDiameter / 2.0;
    
    // Calculate the toneHole radii.
//    rth_[0] = 0.5 * calcdH(0, boreDiameter, tubeLengths_[0], Fc);
    
    // Calculate the tonehole coefficients.
//    calcTHCoeffs();
}

void PhysicalModel::retune(double f) {
    double effectiveLength = calcLS(f);
    int cutLength = calcLC(effectiveLength);
    double boreDiameter = calcd1(cutLength, effectiveLength);
    
    double old_upper = *(tubes_[0]->upper->data);
    double old_lower = *(tubes_[0]->lower->data);
    

    tubeLengths_[0] = calclH(0, boreDiameter, calcLSh(0, f));
//    tubeLengths_[0] = calcLSh(0, f);

    freeTube(tubes_[0]);
//    freeFracTube(ftubes_[0]);

    tubes_[0] = initTube(tubeLengths_[0]);
//    ftubes_[0] = initFracTube(tubeLengths_[0]);
    
    
    inputDelayLine(tubes_[0]->upper, old_upper);
    inputDelayLine(tubes_[0]->lower, old_lower);
//    tLinearDelay_tickIn(&(ftubes_[0]->upper), old_upper);
//    tLinearDelay_tickIn(&(ftubes_[0]->lower), old_lower);
    rb_ = boreDiameter / 2.0;

}

double PhysicalModel::interpolateLinear(double a, double b, double alpha) {
    return (alpha * a) + ((1.0-alpha) * b);
}
 
double PhysicalModel::birlTick() {
    double outsamp = 0.0;
    double bellReflected;
//
//    double pap;
//    double pbm;
//    double pthm;
//    double scatter;
    
    double breath = breathPressure_;
    double noise = (double) rand() / (double) RAND_MAX;
    noise = noiseGain_ * (inputSVFBand(noiseBP_, noise));
    breath += breath * noise;
    
    // REED
    double pressureDiff = accessDelayLine(tubes_[0]->lower) - breath;
//    double pressureDiff = tLinearDelay_tickOut(&(ftubes_[0]->lower)) - breath;
    double reedLookup = pressureDiff * reedTable (pressureDiff);
    breath = LEAF_clip(-1.0, breath + reedLookup, 1.0);
//    breath = interpolateLinear(shaper(breath, m_drive_), breath, shaperMix_);
    breath = inputSVFPeak(pf_, breath);
    breath = inputSVFLP(lp_, breath);
    breath = inputDCFilter(dcBlocker_, breath);
    
    // SLIDE BIRL
    double bell = accessDelayLine(tubes_[0]->upper);
//    double bell = tLinearDelay_tickOut(&(ftubes_[0]->upper));
    // Reflection = Inversion + gain reduction + lowpass filtering.
    bell = inputSVFLP(lp2_, bell);
    bell = inputDCFilter(dcBlocker2_, bell);
    outsamp = bell;
    bellReflected = bell * -0.995;


    
    inputDelayLine(tubes_[0]->upper, breath);
    inputDelayLine(tubes_[0]->lower, bellReflected);
//    tLinearDelay_tickIn(&(ftubes_[0]->upper), breath);
//    tLinearDelay_tickIn(&(ftubes_[0]->lower), bellReflected);
    tanhClip(outsamp);
    return outsamp;
//
//
//    // ONE HOLE.
//    int a = 0;
////    int b = 1;
//
//    pap = accessDelayLine(tubes_[a]->upper);
//    pbm = tLinearDelay_tickOut(&endTube_[1]);
//    pthm = z1PoleZero(toneHoles_[0]);
//
//    scatter = scatter_[0] * (pap + pbm - (2 * pthm));
//    pbp_[0] = pap + scatter;
//    pam_[0] = pbm + scatter;
//    pthp_[0] = pap + scatter + pbm - pthm;
////    pthp_[0] = pap + scatter + pbm; // pthm is accounted for inside tonehole filter
//
//    outsamp += pap + pam_[0];
//
//    double bell = tLinearDelay_tickOut(&endTube_[0]);
//    outsamp += bell;
////    bellReflected = bell * -0.995;
//    bellReflected = tSVF_tick(&inLoopLP, (bell * -0.995));
//
//    // TONEHOLE.
//    inputPoleZero(toneHoles_[0], pthp_[0]);
//
//
//    inputDelayLine(tubes_[a]->upper, breath);
//    tLinearDelay_tickIn(&endTube_[0], pbp_[0]);
////    tLinearDelay_tickIn(&endTube_[0], pap);
//
//
//    inputDelayLine(tubes_[a]->lower, pam_[0]);
////    inputDelayLine(tubes_[a]->lower, pbm);
//    tLinearDelay_tickIn(&endTube_[1], bellReflected);
//
//
//    outsamp = tanhClip(outsamp);
//
//    return outsamp;
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

