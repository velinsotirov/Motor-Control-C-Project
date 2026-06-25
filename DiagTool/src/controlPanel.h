#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <functional>
#include <vector>

class ControlPanel: public Fl_Group {
    public:
        ControlPanel(int, int, int, int);

        static void onControlBtnClick(Fl_Widget*, void*);

        void setupControlCallbacks();

        void getDesiredTorque_q4_12(uint8_t*);

        void getDesiredSpeed_int16(uint8_t*);

        void getDesiredDuty_q8_8(uint8_t*);

        std::function<void()> powerstageEnableFcn;
        std::function<void()> powerstageDisableFcn;
        std::function<void()> dutyControlEnableFcn;
        std::function<void()> torqueControlEnableFcn;
        std::function<void()> speedControlEnableFcn;
        std::function<void()> dutyRequestFcn;
        std::function<void()> speedRequestFcn;
        std::function<void()> torqueRequestFcn;

    private:
        Fl_Button *powerstageEnableBtn;
        Fl_Button *powerstageDisableBtn;
        Fl_Button *torqueControlEnableBtn;
        Fl_Button *speedControlEnableBtn;
        Fl_Button *dutyControlEnableBtn;
        Fl_Input *desiredDutyIpt;
        Fl_Input *desiredTorqueIpt;
        Fl_Input *desiredSpeedIpt;
        Fl_Button *dutyReqBtn;
        Fl_Button *torqueReqBtn;
        Fl_Button *speedReqBtn;
};

#endif // CONTROLPANEL_H