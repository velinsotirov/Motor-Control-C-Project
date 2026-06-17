
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>

#include "commPanel.h"

// constructor of communication panel, calls group constructor to create group with given size
CommPanel::CommPanel(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
    // create both buttons
    activateBtn = new Fl_Button(30,30,100,40, "Activate COM");
    deactivateBtn = new Fl_Button(160,30,120,40, "Deactivate COM");
    comSelectionDdn = new Fl_Choice(350,30,100,40, "Port:");
    statusBox = new Fl_Box(500,30,120,40, "DISCONNECTED");
    setupCommCallbacks();

    // call end, but why?
    end();
}

void CommPanel::onCommBtnClick(Fl_Widget *w, void *userdata) {
    // recast to a proper pointer
    CommPanel *commPanel = static_cast<CommPanel*> (userdata);

    // if callback comes from activate button
    if (w == commPanel->activateBtn) {
        if (commPanel->activateFcn) { // ensure function pointer has been set before calling
            commPanel->activateFcn();
        }
    }
    // if callback comes from deactivate button
    else if(w == commPanel->deactivateBtn) {
        if (commPanel->deactivateFcn) { // ensure function pointer has been set before calling
            commPanel->deactivateFcn();
        }
    }
}

void CommPanel::setupCommCallbacks() {
    // one callback, but the method differentiates based on FLTK inputs
    activateBtn->callback(onCommBtnClick, this);
    deactivateBtn->callback(onCommBtnClick, this);
}

void CommPanel::setAvailablePorts(std::vector<std::string> &comPorts) {

    // clear selection entries and add all available ports
    comSelectionDdn->clear();
    for (const std::string &port : comPorts) {
        comSelectionDdn->add(port.c_str());
    }

    // select the first one, if any are present
    if (!comPorts.empty()) {
        comSelectionDdn->value(0);
    }
}

int CommPanel::getSelectedPort() {
    return comSelectionDdn->value();
}

void CommPanel::setConnectLabel(std::string newlabel) {
    statusLabel = newlabel;
    statusBox->label(statusLabel.c_str());
    statusBox->redraw();
}
