
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>

#include "statusPanel.h"

// constructor of communication panel, calls group constructor to create group with given size
StatusPanel::StatusPanel(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
    powerstageLabel = new Fl_Box(30,330,120,40, "Powerstage:");
    powerstageStatus = new Fl_Box(180,330,40,40, "OFF");
    controlModeLabel = new Fl_Box(250,330,120,40, "ControlMode:");
    controlModeStatus = new Fl_Box(400,330,70,40, "SPEED");

    torqueLabel = new Fl_Box(30,430,80,40, "Torque:");
    torqueStatus = new Fl_Box(140,430,100,40, "0");
    speedLabel = new Fl_Box(270,430,80,40, "Speed:");
    speedStatus = new Fl_Box(380,430,100,40, "0");

    currentLabel = new Fl_Box(30,530,80,40, "Current:");
    currentStatus = new Fl_Box(140,530,100,40, "0");
    dutyLabel = new Fl_Box(270,530,80,40, "Duty:");
    dutyStatus = new Fl_Box(380,530,100,40, "0");

    // call end, but why?
    end();
}

void StatusPanel::updateStatusPanel(uint8_t *packet, uint8_t len) {
    // powerstage status
    uint8_t pwrStageStatus = (packet[0] & 0b1);
    (pwrStageStatus) ? pwrStageLabel = "ON" : pwrStageLabel = "OFF";
    powerstageStatus->label(pwrStageLabel.c_str());
    powerstageStatus->redraw();

    // control mode status
    uint8_t ctrlModeStatus = (packet[0] & (0b1 << 7));
    (ctrlModeStatus) ? ctrlModeLabel = "TORQUE" : ctrlModeLabel = "SPEED";
    controlModeStatus->label(ctrlModeLabel.c_str());
    controlModeStatus->redraw();

    // update torque
    int16_t torqueRaw = (packet[3] << 8) | packet[4];
    float torqueVal = ((float) (torqueRaw)) / (1 << 12);
    torqueStringLabel = std::to_string(torqueVal);
    torqueStatus->label(torqueStringLabel.c_str());
    torqueStatus->redraw();

    // update speed
    int16_t speedVal = (packet[5] << 8) | packet[6];
    speedStringLabel = std::to_string(speedVal);
    speedStatus->label(speedStringLabel.c_str());
    speedStatus->redraw();

    // update current
    int16_t currentRaw = (packet[1] << 8) | packet[2];
    float currentVal = ((float) (currentRaw)) / (1 << 12);
    currentStringLabel = std::to_string(currentVal);
    currentStatus->label(currentStringLabel.c_str());
    currentStatus->redraw();

    // update duty
    int8_t duty = packet[7];
    dutyStringLabel = std::to_string(duty);
    dutyStatus->label(dutyStringLabel.c_str());
    dutyStatus->redraw();
}
