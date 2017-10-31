/*
Program	STATUSB.H
Purpose	Controls the statusbar of the application (header)
		This statusbar is a typical Windows statusbar
		For porting to another platform there must be a StatusBar class
		but you have to make one on your own.
		User interface element (See documentation for more details
		about the functions needed in this class)
Programmers	R. Spekreijse & N. Noorlander
Last Update	22-12-1995
*/

#ifndef WINSTATB_H
#define WINSTATB_H

#ifdef __GNUG__
#pragma interface
#endif

class GDSMainWindow;

// because Windows can generate it's own statusbar inherite from it
class WinStatusBar: public StatusBar
{

	public:
		// constructor & destructor
		WinStatusBar(GDSMainWindow*);
		~WinStatusBar();

		// public memberfunctions (these are always needed in every platform!)
		void	SetXY(double = 0.0, double = 0.0);
		void	ResetCoord();
		void 	SetFile(char* = 0);
		void 	SetFile(const wxString&);
		void	SetProcess(char* = 0);
		void	SetTime(time_t seconds = 0);
		void	ResetTime();
		void	SetRecording(int status = 0);
		void  SetInsertLayer(const char* insertlayer);
		void	SetZoom(float factor = 1);
		void	Reset();

	protected:
		GDSMainWindow*	frame;
		char				_Status_process[80];
		char				_Status_time[80];
		char				_Status_recording[80];
		char				_Status_filename[80];
		char				_Status_zoomfactor[80];
		char				_Status_XY[80];
};

#endif

