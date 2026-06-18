#ifndef STATUSPANEL_H
#define STATUSPANEL_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <functional>
#include <vector>
#include <string>

#include "serialib.h"

class StatusPanel : public Fl_Group {
    public:
        StatusPanel(int, int, int, int);

        void updateStatusPanel(uint8_t *, uint8_t);

        static constexpr float UPDATE_TIME_S {0.5f};

    private:
        Fl_Box *powerstageLabel;
        Fl_Box *powerstageStatus;
        Fl_Box *controlModeLabel;
        Fl_Box *controlModeStatus;

        Fl_Box *torqueLabel;
        Fl_Box *torqueStatus;
        Fl_Box *speedLabel;
        Fl_Box *speedStatus;

        Fl_Box *currentLabel;
        Fl_Box *currentStatus;
        Fl_Box *dutyLabel;
        Fl_Box *dutyStatus;

        std::string pwrStageLabel;
        std::string ctrlModeLabel;
        std::string torqueStringLabel;
        std::string speedStringLabel;
        std::string currentStringLabel;
        std::string dutyStringLabel;
};

#endif // STATUSPANEL_H