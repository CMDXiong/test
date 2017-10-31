/* ========================================================================== */
/* THREAD.H -- interface for the CThread class.
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

#ifndef _THREAD_H_LCH
#define _THREAD_H_LCH

#ifdef  WIN32
#include <process.h>
#else
#include <pthread.h>
#endif
//#include "Common.h"

class CThread  
{
private:
#ifdef  WIN32
    unsigned long  m_hThread;    //在win32的环境下
    static unsigned int __stdcall CThread::ThreadFunc(void *pArg);
#else
    pthread_t  m_hThread; //在linux环境下
    static void*   ThreadFunc(void *);     //linux下的线程函数
#endif
    void        (*m_fpUser)(void *);
    bool        m_bCreate;
    void*       m_pArg;
public:
	static void ExitThread();
    //等待线程结束，参数dwWait为等待时间，linux下忽略，如果为0，表示永久等待
	bool WaitforThreadExit(unsigned long dwWait=0);
	int TerminateThread();
    //创建线程,fpThread参数为线程函数指针,如果为0,则默认为ThreadRunning()函数
    //参数lpParam为用户自定义参数
	bool NewThread(void (*fpThread)(void *), void *lpParam);
    //得到用于自定义参数
    void* GetThreadParam(){return m_pArg;}
    //线程运行函数，如果用户创建线程时没有没有指定线程函数，则系统自动运行该函数，否则系统不会调用此函数
    //参数pParam为用户自定义参数
    virtual void ThreadRunning(void *pParam){};
	CThread();
	virtual ~CThread();

};

#endif 

