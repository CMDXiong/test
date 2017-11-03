/*
 *  STATUSB.CPP
 *
 *  Dummy to make it work under GCC/G++
 */
#ifdef __GNUG__
#pragma implementation
#endif
#include "b_statusb.h"

StatusBar::~StatusBar()
{
}

void 	StatusBar::StartDTimer()
{
	timer=1;
	oldtime=time(0);
}

void 	StatusBar::EndDTimer()
{
	timer=0;
}

int StatusBar::GetDTimerOn()
{
	return timer;
}

time_t StatusBar::GetDTimer()
{
	return (time(0)-oldtime);
}


