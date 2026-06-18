
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "mainWindow.h"

#include "commModule.h"
#include "commPanel.h"
#include "controlPanel.h"
#include "statusPanel.h"

// constructor, calls Fl_Window constructor before it runs the custom constructor code 
MainWindow::MainWindow() : Fl_Window(800, 600, "MotorDiagTool") {
    commPanel = new CommPanel(0,0,800,100);
    controlPanel = new ControlPanel(0,100,800,200);
    statusPanel = new StatusPanel(0,300,800,300);

    // assign callbacks
    attachCommPanelCallbacks();
    attachControlPanelCallbacks();

    // get COM ports and set in GUI
    commPanel->setAvailablePorts(commModule.getAvailablePorts());

    // add 500ms task
    Fl::add_timeout(STATUS_UPDATE_TIME_S, task_500ms, this);

    // close the window so everything defined till now is a child, and show the window
    end();
    show();
}

void MainWindow::task_500ms(void *userdata) {
    auto *win = static_cast<MainWindow*> (userdata);

    // fetch data from commModule and pass to statusPanel
    win->commModule.getLastReceivedPacket(win->exchangePacket, 8u);
    win->statusPanel->updateStatusPanel(win->exchangePacket, 8u);

    // reschedule task
    Fl::repeat_timeout(win->STATUS_UPDATE_TIME_S, task_500ms, userdata);
}

void MainWindow::attachCommPanelCallbacks() {
    commPanel->activateFcn = [this]() {
        bool openedSuccessfully = this->commModule.openPort(commPanel->getSelectedPort());
        if (openedSuccessfully) {
            this->commPanel->setConnectLabel("CONNECTED");
        }
        else {
            this->commPanel->setConnectLabel("CONNECT FAIL");
        }

    };
    commPanel->deactivateFcn = [this]() {
        this->commModule.closePort();
        this->commPanel->setConnectLabel("DISCONNECTED");
    };
}

void MainWindow::attachControlPanelCallbacks() {
    controlPanel->powerstageEnableFcn = [this]() {
        uint8_t pwrstage_enable[this->commModule.TX_PACKET_LEN] = {0u};
        pwrstage_enable[0] = 1u;
        this->commModule.sendPacket(pwrstage_enable, this->commModule.TX_PACKET_LEN);
    };
    controlPanel->powerstageDisableFcn = [this]() {
        uint8_t pwrstage_disable[this->commModule.TX_PACKET_LEN] = {0u};
        pwrstage_disable[0] = 2u;
        this->commModule.sendPacket(pwrstage_disable, this->commModule.TX_PACKET_LEN);
    };
    controlPanel->speedControlEnableFcn = [this]() {
        uint8_t speedctrl_enable[this->commModule.TX_PACKET_LEN] = {0u};
        speedctrl_enable[0] = 3u;
        this->commModule.sendPacket(speedctrl_enable, this->commModule.TX_PACKET_LEN);
    };
    controlPanel->torqueControlEnableFcn = [this]() {
        uint8_t torquectrl_enable[this->commModule.TX_PACKET_LEN] = {0u};
        torquectrl_enable[0] = 4u;
        this->commModule.sendPacket(torquectrl_enable, this->commModule.TX_PACKET_LEN);
    };
    controlPanel->speedRequestFcn = [this]() {
        uint8_t speed_request[this->commModule.TX_PACKET_LEN] = {0u};
        speed_request[0] = 5u;
        this->controlPanel->getDesiredSpeed_int16(&speed_request[1]);
        this->commModule.sendPacket(speed_request, this->commModule.TX_PACKET_LEN);
    };
    controlPanel->torqueRequestFcn = [this]() {
        uint8_t torque_request[this->commModule.TX_PACKET_LEN] = {0u};
        torque_request[0] = 6u;
        this->controlPanel->getDesiredTorque_q4_12(&torque_request[1]);
        this->commModule.sendPacket(torque_request, this->commModule.TX_PACKET_LEN);
    };
}
