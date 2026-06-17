#ifndef COMMPANEL_H
#define COMMPANEL_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <functional>
#include <vector>

class CommPanel: public Fl_Group {
    public:
        CommPanel(int, int, int, int);

        static void onCommBtnClick(Fl_Widget*, void*);

        void setupCommCallbacks();

        void setAvailablePorts(std::vector<std::string>&);

        int getSelectedPort();

        void setConnectLabel(std::string);

        std::function<void()> activateFcn;
        std::function<void()> deactivateFcn;

    private:
        Fl_Button *activateBtn;
        Fl_Button *deactivateBtn;
        Fl_Choice *comSelectionDdn;
        Fl_Box *statusBox;

        std::string statusLabel;
};

#endif // COMMPANEL_H