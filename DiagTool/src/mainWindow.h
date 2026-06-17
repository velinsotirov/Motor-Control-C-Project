#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "commModule.h"
#include "commPanel.h"
#include "controlPanel.h"

class MainWindow: public Fl_Window {
    public: 
        MainWindow();

        void attachCommPanelCallbacks();
        void attachControlPanelCallbacks();

    private:
        CommPanel *commPanel;
        ControlPanel *controlPanel;
        //StatusPanel *statusPanel;

        CommModule commModule;
};

#endif // MAINWINDOW_H