/* ========================================================================== */
/* THREAD.CPP -- 线程
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

//#include "stdafx.h"
#ifdef  WIN32
#include <windows.h>
#endif
#include "Thread.h"


CThread::CThread()
{
    m_hThread = 0;
    m_bCreate = false;
    m_fpUser = 0;
    m_pArg = 0;
}

CThread::~CThread()
{

    if(m_bCreate)
    {
#ifdef  WIN32
        TerminateThread();         
#endif   
    }
}


/***********************************************************************
* 函数名	  : CThread::NewThread
* 功能描述	  : 
* 返回类型	  : bool 
* 描述        : 

* 参数1: LPTHREAD_FUN lpStartFun
* 描述 :

* 参数2: void *pArg
* 描述 :

* 作者        : 刘朝洪
* 函数生成日期: 2004-5-25 15:31:09
* 函数修改者  :
* 函数修改日期:
*************************************************************************/
bool CThread::NewThread(void (*fpThread)(void *), void *lpParam)
{
    if(m_bCreate)
        return false;
    
    m_fpUser  = fpThread;
    m_pArg    = lpParam;
#ifdef  WIN32
    m_hThread = _beginthreadex(0,0,&CThread::ThreadFunc,this,0,0);
    if(0 != m_hThread)
        m_bCreate = true;
#else
    if(0 == pthread_create(&m_hThread,0,&CThread::ThreadFunc,this))
        m_bCreate = true;
#endif
    return m_bCreate;
}


/***********************************************************************
* 函数名	  : CThread::TerminateThread
* 功能描述	  : 
* 返回类型	  : int 
* 描述        : 

* 作者        : 刘朝洪
* 函数生成日期: 2004-5-25 15:31:05
* 函数修改者  :
* 函数修改日期:
*************************************************************************/
int CThread::TerminateThread()
{
    if(!m_bCreate)
        return -2;

#ifdef  WIN32
    if(::TerminateThread((HANDLE)m_hThread, 2))
    {
        ::CloseHandle((HANDLE)m_hThread); 
        return 1;
    }
#else
        return 0;
#endif
    m_bCreate = false;
    return -1;
}


/***********************************************************************
* 函数名	  : CThread::WaitThreadEnd
* 功能描述	  : 
* 返回类型	  : void 
* 描述        : 

* 作者        : 刘朝洪
* 函数生成日期: 2004-5-25 15:30:59
* 函数修改者  :
* 函数修改日期:
*************************************************************************/
bool CThread::WaitforThreadExit(unsigned long dwWait)
{
    if(!m_bCreate)
        return false;

#ifdef  WIN32
    if(0 == dwWait)
        dwWait = INFINITE;
    if(::WaitForSingleObject((HANDLE)m_hThread,dwWait) == WAIT_TIMEOUT)
        ::TerminateThread((HANDLE)m_hThread, 2); 
    ::CloseHandle((HANDLE)m_hThread);
#else
    pthread_join(m_hThread,0);
#endif  
    m_bCreate = false;
    return true;
}

void CThread::ExitThread()
{
#ifdef  WIN32
    _endthreadex(0); 
#else
    pthread_exit(0);
#endif 
}

//linux环境下的线程入口函数
#ifdef  WIN32
unsigned int __stdcall CThread::ThreadFunc(void *pArg)
{
    CThread *pThis = (CThread*)pArg;
    if(0 == pThis->m_fpUser)
        pThis->ThreadRunning(pThis->m_pArg);
    else
        pThis->m_fpUser(pThis->m_pArg);
    pThis->m_bCreate = false;
    return 0;
}
#else
void*   CThread::ThreadFunc(void *pArg)
{
    CThread *pThis = (CThread*)pArg;
    if(0 == pThis->m_fpUser)
        pThis->ThreadRunning(pThis->m_pArg);
    else
        pThis->m_fpUser(pThis->m_pArg);
    pThis->m_bCreate = false;
    return 0;
}
#endif
