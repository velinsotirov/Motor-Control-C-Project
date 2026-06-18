
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>

#include <cstdlib>

#include "controlPanel.h"

// constructor of communication panel, calls group constructor to create group with given size
ControlPanel::ControlPanel(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
    // create buttons
    powerstageEnableBtn = new Fl_Button(30,130,150,40, "Powerstage ON");
    powerstageDisableBtn = new Fl_Button(210,130,150,40, "Powerstage OFF");
    torqueControlEnableBtn = new Fl_Button(390,130,150,40, "Torque CONTROL");
    speedControlEnableBtn = new Fl_Button(570,130,150,40, "Speed CONTROL");
    desiredTorqueIpt = new Fl_Input(90,230,100,40, "Torque:");
    torqueReqBtn = new Fl_Button(220,230,120,40, "Torque REQ");
    desiredSpeedIpt = new Fl_Input(420,230,100,40, "Speed:");
    speedReqBtn = new Fl_Button(550,230,120,40, "Speed REQ");
    setupControlCallbacks();

    // call end, but why?
    end();
}

void ControlPanel::onControlBtnClick(Fl_Widget *w, void *userdata) {
    // recast to a proper pointer
    ControlPanel *controlPanel = static_cast<ControlPanel*> (userdata);

    // if callback comes from activate button
    if (w == controlPanel->powerstageEnableBtn) {
        if (controlPanel->powerstageEnableFcn) { // ensure function pointer has been set before calling
            controlPanel->powerstageEnableFcn();
        }
    }
    // if callback comes from deactivate button
    else if(w == controlPanel->powerstageDisableBtn) {
        if (controlPanel->powerstageDisableFcn) { // ensure function pointer has been set before calling
            controlPanel->powerstageDisableFcn();
        }
    }
    else if(w == controlPanel->torqueControlEnableBtn) {
        if (controlPanel->torqueControlEnableFcn) { // ensure function pointer has been set before calling
            controlPanel->torqueControlEnableFcn();
        }
    }
    else if(w == controlPanel->speedControlEnableBtn) {
        if (controlPanel->speedControlEnableFcn) { // ensure function pointer has been set before calling
            controlPanel->speedControlEnableFcn();
        }
    }
    else if(w == controlPanel->torqueReqBtn) {
        if (controlPanel->torqueRequestFcn) { // ensure function pointer has been set before calling
            controlPanel->torqueRequestFcn();
        }
    }
    else if(w == controlPanel->speedReqBtn) {
        if (controlPanel->speedRequestFcn) { // ensure function pointer has been set before calling
            controlPanel->speedRequestFcn();
        }
    }
}

void ControlPanel::setupControlCallbacks() {
    // one callback, but the method differentiates based on FLTK inputs
    powerstageEnableBtn->callback(onControlBtnClick, this);
    powerstageDisableBtn->callback(onControlBtnClick, this);
    torqueControlEnableBtn->callback(onControlBtnClick, this);
    speedControlEnableBtn->callback(onControlBtnClick, this);
    torqueReqBtn->callback(onControlBtnClick, this);
    speedReqBtn->callback(onControlBtnClick, this);
}

void ControlPanel::getDesiredTorque_q4_12(uint8_t* bytes) {
    float torqueVal = std::atof(desiredTorqueIpt->value());
    int16_t torqueVal_Q4_12 = (int16_t) (torqueVal * (1 << 12));
    bytes[0] = (uint8_t) (torqueVal_Q4_12 >> 8);
    bytes[1] = (uint8_t) (torqueVal_Q4_12 & 255u);
}

void ControlPanel::getDesiredSpeed_int16(uint8_t* bytes) {
    float speedVal = std::atof(desiredSpeedIpt->value());
    int16_t speedVal_int16 = (int16_t) speedVal;
    bytes[0] = (uint8_t) (speedVal_int16 >> 8);
    bytes[1] = (uint8_t) (speedVal_int16 & 255u);
}
