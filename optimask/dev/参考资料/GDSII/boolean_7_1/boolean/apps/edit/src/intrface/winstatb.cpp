#ifdef __GNUG__
#pragma implementation
#endif


#include "misc.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "b_statusb.h"
#include "mainwin.h"
#include "chandbas.h"

#include "winstatb.h"
#include "wxwin.h"
#include "gdsapp.h"

// there are 5 entries in the statusbar.
//	0 = process info            -1
// 1 = recording info          -1
// 2 = filename                15 chars
// 3 = zoomfactor               8 chars  precies goed
// 4 = XY coordinates          14 chars



//our constructor for the inherited statusbar.
WinStatusBar::WinStatusBar(GDSMainWindow* _GDSMainWindow)
{
	frame = _GDSMainWindow;

	frame->CreateStatusBar(5);
	int widths[5];

	widths[0] = (-1);
	widths[1] = (100);
	widths[2] = (120);
	widths[3] = (80);
	widths[4] = (140);

	frame->SetStatusWidths(5,widths);

	timer=0;
	strcpy(_Status_process,"Ready"); frame->SetStatusText(_Status_process,0);
	strcpy(_Status_time,"");
	strcpy(_Status_recording,"");	frame->SetStatusText(_Status_recording,1);
	strcpy(_Status_filename,""); frame->SetStatusText(_Status_filename,2);
	strcpy(_Status_zoomfactor,"1:1"); frame->SetStatusText(_Status_zoomfactor,3);
	strcpy(_Status_XY,"0, 0"); frame->SetStatusText(_Status_XY,4);
}

WinStatusBar::~WinStatusBar()
{
	timer=0;

}

// set the X and Y coordinate on the WinStatusBar
void WinStatusBar::SetXY(double x, double y)
{
	int exp=0;
	double scaling=GLOB->PHYSUNITS()/GLOB->ENTRYUNITS();
   x=x*scaling;
   y=y*scaling;
	sprintf(_Status_XY, "%.*f, %.*f", 3, x, 3, y );
   strcat(_Status_XY,GLOB->CMDH->GetDefaultUnit().GetMultiplierString());
	frame->SetStatusText(_Status_XY,4);
}

// set the coordinates in the WinStatusBar on zero
void WinStatusBar::ResetCoord()
{
	strcpy(_Status_XY,"0, 0");
	frame->SetStatusText(_Status_XY,4);
}

// the given text will be displayed in the process applet
// if process = 0 -> the applet will be cleared
void WinStatusBar::SetProcess(char* process)
{
	if (GetDTimerOn()!=0)
		SetTime(GetDTimer());
	if (process == 0)
	{
		strcpy(_Status_process,"");
	}
	else
		strcpy(_Status_process,process);
	frame->SetStatusText(_Status_process,0);

//	while (wxGetApp().Pending())
//		wxGetApp().Dispatch();
    //very important (else it will not be seen directly)
//   ::wxYield();
}


void WinStatusBar::ResetTime()
{
#ifdef _LOSTFUNCTIONS
	_messagehandler->info("lost function in winstatb.h::ResetTime()","geen ruimte voor time");
		_Status_time->SetText(""); nog geen ruimte voor.
#endif
}

void WinStatusBar::SetTime(time_t seconds)
{
	if (seconds>3599)
		sprintf(_Status_time, "%0d:%02d:%02d", int(seconds/3600), int((seconds/60)%60), int(seconds%60));
	else
		sprintf(_Status_time, "%0d:%02d", int(seconds/60)%60, int(seconds%60));

#ifdef _LOSTFUNCTIONS
	_messagehandler->info("lost function in winstatb.h::SetTime(rime_t seconds)","geen ruimte voor time");
//	_Status_time->SetText(buf); nog geen ruimte voor
//	UpdateWindow(); hoeft dus ook niet.
#endif
}


// when processrecording is on this function is called before to indicate this
void WinStatusBar::SetRecording(int status)
{
	if (status)
		strcpy(_Status_recording,"Recording");
	else
		strcpy(_Status_recording,"");
	frame->SetStatusText(_Status_recording,1);
#ifdef _LOSTFUNCTIONS
	_messagehandler->info("lost function in winstatb.h::SetProcess(char* process)","geen UpdateWindow()");
//	UpdateWindow();
#endif
}

// when processrecording is on this function is called before to indicate this
void WinStatusBar::SetInsertLayer(const char* insertlayer)
{
	frame->SetStatusText(insertlayer,1);
#ifdef _LOSTFUNCTIONS
	_messagehandler->info("lost function in winstatb.h::SetProcess(char* process)","geen UpdateWindow()");
//	UpdateWindow();
#endif
}

// the zoomfactor is displayed here
void WinStatusBar::SetZoom(float zoomfactor)
{
	if (zoomfactor >= 1 && zoomfactor < 1000000000l) {
		sprintf(_Status_zoomfactor, "%.f:1", zoomfactor);
	} else {
		if (zoomfactor > 0.000000000l && zoomfactor < 1 ) {
			sprintf(_Status_zoomfactor, "1:%.f", 1/zoomfactor);
		} else strcpy (_Status_zoomfactor,"out of range");
	}
	frame->SetStatusText(_Status_zoomfactor,3);
#ifdef _LOSTFUNCTIONS
	_messagehandler->info("lost function in winstatb.h::SetProcess(char* process)","geen UpdateWindow()");
//	UpdateWindow();
#endif
}

void 	WinStatusBar::SetFile(const wxString& filename)
{
	if (filename.Len())
   {
      wxString help=wxFileNameFromPath(filename);
		strcpy(_Status_filename,help);
	}
	else strcpy(_Status_filename,"");
	frame->SetStatusText(_Status_filename,2);
}

// displays the filename currently loaded
void WinStatusBar::SetFile(char* filename)
{
	if (filename!=0)
   {
		int l;
		if ( (l=strlen(filename))>0 )
      {
			int t = l-1;
			while ( (filename[t]!='\\' && filename[t]!='/') && t >= 0 )
				t--;
			strcpy(_Status_filename,filename+t+1);
		}
      else strcpy(_Status_filename,"");
	}
	else strcpy(_Status_filename,"");
	frame->SetStatusText(_Status_filename,2);
}

// reset the WinStatusBar
void WinStatusBar::Reset()
{
	SetProcess();
#ifdef _LOSTFUNCTIONS
	_messagehandler->info("lost function in winstatb.h::reset","geen ruimte voor time");
//	ResetTime(); NIET NODIG, OMDAT ER GEEN RUIMTE VOOR IS.
#endif
	SetFile();
	SetRecording();
	SetZoom(1);
	SetXY();
#ifdef _LOSTFUNCTIONS
	_messagehandler->info("lost function in winstatb.cpp::reset","geen Invalidate");
	//	Invalidate(); nog geen invalidate voor wxWindows.
#endif
}



