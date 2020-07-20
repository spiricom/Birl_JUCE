#include "BirlOSCReceiver.h"
#include "SlideTuning.h"

BirlOSCReceiver::BirlOSCReceiver(BirlOneHoleAudioProcessor& p) : processor(p)
{
    rcvr.addListener(this);
    connectButton.onClick = [this] { connectButtonClicked(); };
    for (int i = 0; i < NUM_OF_KEYS; ++i) {
        maxKeyArg[i] = 1;
        minKeyArg[i] = 0;
    }
}

BirlOSCReceiver::~BirlOSCReceiver(){}

void BirlOSCReceiver::oscMessageReceived(const OSCMessage& message){
    if (message.getAddressPattern().toString()=="/birl/keys") {
        int keyIndex{0};
        for (auto* arg = message.begin(); arg != message.end(); ++arg) {
            if (keyIndex < NUM_OF_KEYS) {
                keys_[keyIndex] = getArgValue(*arg);

                if (getArgValue(*arg) > maxKeyArg[keyIndex]) {
                    maxKeyArg[keyIndex] = getArgValue(*arg);
                }
                float beforeclip = (float) getArgValue(*arg) / (maxKeyArg[keyIndex]);
                float afterclip = LEAF_clip(0.0, beforeclip, 1.0);
                keys_[keyIndex] = afterclip;
                keySliders[keyIndex].setValue(afterclip, dontSendNotification);
                midiAdjustmentLabel.setText((String)getMidiAdjustment(), dontSendNotification);
                frequencyLabel.setText((String)getToneholeFrequency(getMidiAdjustment(), OPENHOLE_FREQ, processor.octave), dontSendNotification);
                processor.frequency = getToneholeFrequency(getMidiAdjustment(), OPENHOLE_FREQ, processor.octave);
                ++keyIndex;
            }
        }
    }
    if (message.getAddressPattern().toString()=="/birl/buttons") {
        int buttonIndex{0};
        for (auto* arg = message.begin(); arg != message.end(); ++arg) {
            if (buttonIndex < NUM_OF_BUTTONS) {
                if (getArgValue(*arg) == 1) {
                    buttons_[buttonIndex] = true;
                    buttonButtons[buttonIndex].setButtonText("ON");
                }
                else {
                    buttons_[buttonIndex] = false;
                    buttonButtons[buttonIndex].setButtonText("OFF");
                }
                processor.octave = getOctaveAdjustment();
                ++buttonIndex;
            }
        }
    }
    if(message.getAddressPattern().toString()=="/birl/breathpos") {
        for (auto* arg = message.begin(); arg != message.end(); ++arg) {
            currentBreathPos_ = getArgValue(*arg);
            if (currentBreathPos_ > 0)
                isExhaling_ = true;
            if(currentBreathPos_ > maxBreathPos_)
                maxBreathPos_ = currentBreathPos_;
            if (isExhaling_) {
                processor.gain = LEAF_clip(0.0, currentBreathPos_ / maxBreathPos_, 1.0);
            }
        }
    }
    if(message.getAddressPattern().toString()=="/birl/breathneg") {
        for (auto* arg = message.begin(); arg != message.end(); ++arg) {
            currentBreathNeg_ = getArgValue(*arg);
            if (currentBreathNeg_ > 0)
                isExhaling_ = false;
            if(currentBreathNeg_ > maxBreathNeg_)
                maxBreathNeg_ = currentBreathNeg_;
            if (!isExhaling_)
                processor.gain = LEAF_clip(0.0, currentBreathNeg_ / maxBreathNeg_, 1.0);
        }
    }
    if(message.getAddressPattern().toString()=="/birl/abs") {
        for(auto* arg = message.begin(); arg != message.end(); ++arg)
            absBreath_ = getArgValue(*arg);
    }
    triggerAsyncUpdate();
}

void BirlOSCReceiver::oscBundleReceived (const OSCBundle& bundle){
    for (auto* element = bundle.begin(); element != bundle.end(); ++element)
      {
        if (element->isMessage())
            oscMessageReceived (element->getMessage());
        else if (element->isBundle())
            oscBundleReceived(element->getBundle());
    }
    triggerAsyncUpdate();
}

void BirlOSCReceiver::connectButtonClicked() {
    if (port != BIRL_PORT) {
        if (rcvr.connect(BIRL_PORT)) {
            port = BIRL_PORT;
            connectButton.setButtonText("Disconnect");
        }
    }
    else {
        if (rcvr.disconnect()) {
            port = -1;
            connectButton.setButtonText("Connect");
        }
    }
}

int BirlOSCReceiver::getArgValue(const OSCArgument& arg){
    if (arg.isInt32())
        return arg.getInt32();
    handleArgTypeError();
    return 0;
}
void BirlOSCReceiver::handleAsyncUpdate(){}

void BirlOSCReceiver::handleConnectError(){
    AlertWindow::showMessageBox(AlertWindow::WarningIcon,
        "OSC Connection Error",
        "Unable to find port " + (String)BIRL_PORT,
        "OK");
}
void BirlOSCReceiver::handleArgTypeError(){
    AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
        "Argument Type Error",
        "Error: Birl input not integer values",
        "OK");
}



