#ifndef PHYSICAL_MODEL_H
#define PHYSICAL_MODEL_H

#include <JuceHeader.h>
#include "Birl.h"
#include "Birl_Filters.h"
#include "Birl_Tube.h"
#include "Birl_Tuning.h"


class PhysicalModel
{
public:
    PhysicalModel();
    ~PhysicalModel();
    
    void setToneHoleRadius(int, double);
    void setToneHole(int, double);
    void setLength(int, int);
    
    void setBreathPressure(double);
    void setLPCutoff(double);
    void setLPQ(double);
    void setPFCutoff(double);
    void setPFQ(double);
    void setNoiseBPCutoff(double);
    void setNoiseGain(double);
    void setNoiseBPQ(double);
    void setShaperDrive(double);
    void setShaperMix(double);
    

    void calcTHCoeffs();
    void tune(double);
    void retune(double);
    double interpolateLinear(double a, double b, double alpha);
    tSVF inLoopLP; // in loop low pass
  
    double birlTick();


    
protected:
    int numTubes_;
    int tubeIndex_;
    int numToneHoles_;
    int toneHoleIndex_;
    
    double pam_[MAX_TONEHOLES];
    double pbp_[MAX_TONEHOLES];
    double pthp_[MAX_TONEHOLES];
    double scatter_[MAX_TONEHOLES];
    double thCoeff_[MAX_TONEHOLES];
    
    Tube *tubes_ [MAX_TUBES];
    PoleZero *toneHoles_[MAX_TONEHOLES];
//    FracTube *ftubes_[MAX_TUBES];
    
    
    DCFilter *dcBlocker_;
    DCFilter *dcBlocker2_;
    Biquad *biquad_;
    Biquad *biquad2_;
    SVF *pf_;
    SVF *lp_;
    SVF *pf2_;
    SVF *lp2_;
    SVF *noiseBP_;
        
    double rb_;
    double originalrb_;
    double rhGain_;
    double outputGain_;
    double noiseGain_;
    double m_drive_;
    double shaperMix_;
    
    double breathPressure_{0};
    double prevBreathPressure_{0};
    
    int count;
    double min, max;
};

#endif
