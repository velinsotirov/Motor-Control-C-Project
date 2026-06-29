APPCC = g++

APPINCLUDES = -I . -I seriallib
APPSRC = src/main.cpp seriallib/serialib.cpp \
		src/mainWindow.cpp src/commPanel.cpp \
		src/commModule.cpp src/controlPanel.cpp src/statusPanel.cpp

APPEXE = DiagTool.exe
APPLIBS = -L . -lfltk -lgdi32 -lgdiplus -lole32 -luuid \
			-lcomctl32 -lcomdlg32 -lws2_32 -lwinspool -mwindows

app: $(APPEXE)

$(APPEXE): $(APPSRC)
	$(APPCC) $(APPSRC) $(APPLIBS) \
	$(APPINCLUDES) -o $(APPEXE) \
	-std=c++11
