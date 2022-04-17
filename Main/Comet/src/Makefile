# Comet Build System
# (C) 2010-2022 Pr. Sidi HAMADY
# http://www.hamady.org
# sidi@hamady.org

BITS := $(shell getconf LONG_BIT)
ifeq "$(BITS)" "64"
    DEVC_OUTDIR = ../../../output/Linux64
else
    DEVC_OUTDIR = ../../../output/Linux32
endif
DEVC_BINDIR = $(DEVC_OUTDIR)/bin
DEVC_LIBDIR = $(DEVC_OUTDIR)/lib
DEVC_MAINDIR = ../../../Main

WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -Wall -fexceptions
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = 

ARCH := $(shell getconf LONG_BIT)
WXBUILDDIR := build/linux32
ifeq ($(ARCH),64)
	WXBUILDDIR := build/linux64
endif

INC_RELEASE = -I../include $(INC)
CFLAGS_RELEASE = $(CFLAGS) -fPIC -O2 -std=c++0x `pkg-config --cflags gtk+-2.0` `pkg-config --cflags glib-2.0` -I$(DEVC_MAINDIR)/wxWidgets/$(WXBUILDDIR)/lib/wx/include/gtk2-unicode-release-2.8 -I $(DEVC_MAINDIR)/wxWidgets/include -I $(DEVC_MAINDIR)/wxWidgets/contrib/include -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE =  -s -L$(DEVC_OUTDIR)/bin `pkg-config --libs gtk+-2.0` `pkg-config --libs glib-2.0` -lluacore -lfile -lluaext -lwxbase -lwxcore -lwxadv -lwxaui -lwxstc $(LDFLAGS)
OBJDIR_RELEASE = $(DEVC_OUTDIR)/obj/Comet
DEP_RELEASE = 
OUT_RELEASE = $(DEVC_OUTDIR)/bin/comet

OBJ_RELEASE = $(OBJDIR_RELEASE)/ScriptSamples.o $(OBJDIR_RELEASE)/interact/print.o $(OBJDIR_RELEASE)/interact/hook.o $(OBJDIR_RELEASE)/ScriptThread.o $(OBJDIR_RELEASE)/ConsoleThread.o $(OBJDIR_RELEASE)/CometFrame.o $(OBJDIR_RELEASE)/CometFrameAnalyzer.o $(OBJDIR_RELEASE)/CometFrameBookmark.o $(OBJDIR_RELEASE)/CometFrameFile.o $(OBJDIR_RELEASE)/CometFrameFind.o $(OBJDIR_RELEASE)/CometFrameInit.o $(OBJDIR_RELEASE)/CometFrameUpdate.o $(OBJDIR_RELEASE)/CometFileExplorer.o $(OBJDIR_RELEASE)/CometApp.o $(OBJDIR_RELEASE)/ColorButton.o $(OBJDIR_RELEASE)/ScriptPrint.o $(OBJDIR_RELEASE)/ScriptStats.o $(OBJDIR_RELEASE)/ScriptEdit.o $(OBJDIR_RELEASE)/ScriptEditEncoding.o $(OBJDIR_RELEASE)/ScriptEditFile.o $(OBJDIR_RELEASE)/ScriptEditFind.o $(OBJDIR_RELEASE)/ScriptEditMarker.o $(OBJDIR_RELEASE)/ScriptEditProcess.o $(OBJDIR_RELEASE)/OutputEdit.o $(OBJDIR_RELEASE)/EditorConfig.o $(OBJDIR_RELEASE)/ConsoleEdit.o $(OBJDIR_RELEASE)/ColorsDlg.o $(OBJDIR_RELEASE)/TabDlg.o $(OBJDIR_RELEASE)/CometConfig.o $(OBJDIR_RELEASE)/CodeEdit.o $(OBJDIR_RELEASE)/CodeEditSyntax.o $(OBJDIR_RELEASE)/FindFileDlg.o $(OBJDIR_RELEASE)/FindDirDlg.o $(OBJDIR_RELEASE)/FindThread.o $(OBJDIR_RELEASE)/BookmarkList.o $(OBJDIR_RELEASE)/ToolsDlg.o $(OBJDIR_RELEASE)/CometProcess.o $(OBJDIR_RELEASE)/CodeAnalyzer.o $(OBJDIR_RELEASE)/CometComboBox.o $(OBJDIR_RELEASE)/LexerConfig.o $(OBJDIR_RELEASE)/LexerDlg.o

all: release

clean: clean_release

before_release: 
	test -d . || mkdir -p .
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)
	test -d $(OBJDIR_RELEASE)/interact || mkdir -p $(OBJDIR_RELEASE)/interact

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/ScriptSamples.o: ScriptSamples.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ScriptSamples.cpp -o $(OBJDIR_RELEASE)/ScriptSamples.o

$(OBJDIR_RELEASE)/interact/print.o: interact/print.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c interact/print.cpp -o $(OBJDIR_RELEASE)/interact/print.o

$(OBJDIR_RELEASE)/interact/hook.o: interact/hook.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c interact/hook.cpp -o $(OBJDIR_RELEASE)/interact/hook.o

$(OBJDIR_RELEASE)/ScriptThread.o: ScriptThread.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ScriptThread.cpp -o $(OBJDIR_RELEASE)/ScriptThread.o

$(OBJDIR_RELEASE)/ConsoleThread.o: ConsoleThread.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ConsoleThread.cpp -o $(OBJDIR_RELEASE)/ConsoleThread.o

$(OBJDIR_RELEASE)/CometFrame.o: CometFrame.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometFrame.cpp -o $(OBJDIR_RELEASE)/CometFrame.o

$(OBJDIR_RELEASE)/CometFrameAnalyzer.o: CometFrameAnalyzer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometFrameAnalyzer.cpp -o $(OBJDIR_RELEASE)/CometFrameAnalyzer.o

$(OBJDIR_RELEASE)/CometFrameBookmark.o: CometFrameBookmark.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometFrameBookmark.cpp -o $(OBJDIR_RELEASE)/CometFrameBookmark.o

$(OBJDIR_RELEASE)/CometFrameFile.o: CometFrameFile.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometFrameFile.cpp -o $(OBJDIR_RELEASE)/CometFrameFile.o

$(OBJDIR_RELEASE)/CometFrameFind.o: CometFrameFind.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometFrameFind.cpp -o $(OBJDIR_RELEASE)/CometFrameFind.o

$(OBJDIR_RELEASE)/CometFrameInit.o: CometFrameInit.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometFrameInit.cpp -o $(OBJDIR_RELEASE)/CometFrameInit.o

$(OBJDIR_RELEASE)/CometFrameUpdate.o: CometFrameUpdate.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometFrameUpdate.cpp -o $(OBJDIR_RELEASE)/CometFrameUpdate.o

$(OBJDIR_RELEASE)/CometFileExplorer.o: CometFileExplorer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometFileExplorer.cpp -o $(OBJDIR_RELEASE)/CometFileExplorer.o

$(OBJDIR_RELEASE)/CometApp.o: CometApp.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometApp.cpp -o $(OBJDIR_RELEASE)/CometApp.o

$(OBJDIR_RELEASE)/ColorButton.o: ColorButton.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ColorButton.cpp -o $(OBJDIR_RELEASE)/ColorButton.o

$(OBJDIR_RELEASE)/ScriptPrint.o: ScriptPrint.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ScriptPrint.cpp -o $(OBJDIR_RELEASE)/ScriptPrint.o

$(OBJDIR_RELEASE)/ScriptStats.o: ScriptStats.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ScriptStats.cpp -o $(OBJDIR_RELEASE)/ScriptStats.o

$(OBJDIR_RELEASE)/ScriptEdit.o: ScriptEdit.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ScriptEdit.cpp -o $(OBJDIR_RELEASE)/ScriptEdit.o

$(OBJDIR_RELEASE)/ScriptEditEncoding.o: ScriptEditEncoding.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ScriptEditEncoding.cpp -o $(OBJDIR_RELEASE)/ScriptEditEncoding.o

$(OBJDIR_RELEASE)/ScriptEditFile.o: ScriptEditFile.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ScriptEditFile.cpp -o $(OBJDIR_RELEASE)/ScriptEditFile.o

$(OBJDIR_RELEASE)/ScriptEditFind.o: ScriptEditFind.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ScriptEditFind.cpp -o $(OBJDIR_RELEASE)/ScriptEditFind.o

$(OBJDIR_RELEASE)/ScriptEditMarker.o: ScriptEditMarker.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ScriptEditMarker.cpp -o $(OBJDIR_RELEASE)/ScriptEditMarker.o

$(OBJDIR_RELEASE)/ScriptEditProcess.o: ScriptEditProcess.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ScriptEditProcess.cpp -o $(OBJDIR_RELEASE)/ScriptEditProcess.o

$(OBJDIR_RELEASE)/OutputEdit.o: OutputEdit.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c OutputEdit.cpp -o $(OBJDIR_RELEASE)/OutputEdit.o

$(OBJDIR_RELEASE)/EditorConfig.o: EditorConfig.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c EditorConfig.cpp -o $(OBJDIR_RELEASE)/EditorConfig.o

$(OBJDIR_RELEASE)/ConsoleEdit.o: ConsoleEdit.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ConsoleEdit.cpp -o $(OBJDIR_RELEASE)/ConsoleEdit.o

$(OBJDIR_RELEASE)/ColorsDlg.o: ColorsDlg.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ColorsDlg.cpp -o $(OBJDIR_RELEASE)/ColorsDlg.o

$(OBJDIR_RELEASE)/TabDlg.o: TabDlg.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c TabDlg.cpp -o $(OBJDIR_RELEASE)/TabDlg.o

$(OBJDIR_RELEASE)/CometConfig.o: CometConfig.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometConfig.cpp -o $(OBJDIR_RELEASE)/CometConfig.o

$(OBJDIR_RELEASE)/CodeEdit.o: CodeEdit.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CodeEdit.cpp -o $(OBJDIR_RELEASE)/CodeEdit.o

$(OBJDIR_RELEASE)/CodeEditSyntax.o: CodeEditSyntax.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CodeEditSyntax.cpp -o $(OBJDIR_RELEASE)/CodeEditSyntax.o

$(OBJDIR_RELEASE)/FindFileDlg.o: FindFileDlg.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c FindFileDlg.cpp -o $(OBJDIR_RELEASE)/FindFileDlg.o

$(OBJDIR_RELEASE)/FindDirDlg.o: FindDirDlg.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c FindDirDlg.cpp -o $(OBJDIR_RELEASE)/FindDirDlg.o

$(OBJDIR_RELEASE)/FindThread.o: FindThread.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c FindThread.cpp -o $(OBJDIR_RELEASE)/FindThread.o

$(OBJDIR_RELEASE)/BookmarkList.o: BookmarkList.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c BookmarkList.cpp -o $(OBJDIR_RELEASE)/BookmarkList.o

$(OBJDIR_RELEASE)/ToolsDlg.o: ToolsDlg.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ToolsDlg.cpp -o $(OBJDIR_RELEASE)/ToolsDlg.o

$(OBJDIR_RELEASE)/CometProcess.o: CometProcess.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometProcess.cpp -o $(OBJDIR_RELEASE)/CometProcess.o

$(OBJDIR_RELEASE)/CodeAnalyzer.o: CodeAnalyzer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CodeAnalyzer.cpp -o $(OBJDIR_RELEASE)/CodeAnalyzer.o

$(OBJDIR_RELEASE)/CometComboBox.o: CometComboBox.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c CometComboBox.cpp -o $(OBJDIR_RELEASE)/CometComboBox.o
	
$(OBJDIR_RELEASE)/LexerConfig.o: LexerConfig.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c LexerConfig.cpp -o $(OBJDIR_RELEASE)/LexerConfig.o

$(OBJDIR_RELEASE)/LexerDlg.o: LexerDlg.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c LexerDlg.cpp -o $(OBJDIR_RELEASE)/LexerDlg.o
 
clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/interact

.PHONY: before_release after_release clean_release

