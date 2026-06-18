#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "commModule.h"

#include "commPanel.h"
#include "controlPanel.h"
#include "statusPanel.h"

class MainWindow: public Fl_Window {
    public: 
        MainWindow();

        void attachCommPanelCallbacks();
        void attachControlPanelCallbacks();

    private:
        CommPanel *commPanel;
        ControlPanel *controlPanel;
        StatusPanel *statusPanel;

        CommModule commModule;

        static void task_500ms(void *);

        uint8_t exchangePacket[8];
        static constexpr double STATUS_UPDATE_TIME_S {0.5f};
};

#endif // MAINWINDOW_H