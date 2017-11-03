/* ========================================================================== */
/* GEOFILE.CPP -- 基本图形文件定义
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

#include "GeoFile.h"
#include <algorithm>
#include <cctype>

FileFormatToClass* FileFormatToClass::g_pFirst = 0;


static void  WideCharToAnsi(const std::wstring& strData,std::string& strAnsii)
{
    char* pszBuf = new char[strData.size()*2 + 2];

    int nLen = WideCharToMultiByte(CP_ACP, 0, strData.c_str(), -1, pszBuf, strData.size()*2 + 1, 0, 0);
    pszBuf[nLen] = 0;
    strAnsii = pszBuf;
    delete[] pszBuf;
}

CGeoScene* ReadDataFromFile_W(const std::wstring& strFile, const std::wstring& strType, int* pError)
{
    std::string strName ,strFileType ;
    WideCharToAnsi(strFile, strName);
    WideCharToAnsi(strType, strFileType);
    return ReadDataFromFile_A(strName, strFileType, pError);
}

CGeoScene* ReadDataFromFile_A(const std::string& strFile, const std::string& strType, int* /*pError*/)
{
    //根据不同的文件调用不同文件处理子类
    if(0 == FileFormatToClass::g_pFirst)
        return 0;
    FileFormatToClass* pTemp = FileFormatToClass::g_pFirst;
    CGeoFileBase* pFile = 0;

    std::string strFileType = strType;
    if (strType.empty())    //if no type
    {
        unsigned nIndex = strFile.find_last_of('.');
        if (std::string::npos == nIndex)
            return 0;
        strFileType = strFile.substr(nIndex+1);
    }
    std::transform(strFileType.begin(), strFileType.end(), strFileType.begin(), toupper);
    //std::transform(strFileType.begin(), strFileType.end(), strFileType.begin(), std::toupper);

    while (pTemp != 0)
    {
        pFile = pTemp->m_pFunCreate(strFileType);
        if (pFile != 0)
            break;
        pTemp = pTemp->m_pNext;
    }

    CGeoScene* pScene = 0;
    if (pFile != 0)
    {
        pScene = pFile->ReadFile(strFile);
        return pScene;
    }
    else
    {
        return 0;
    }
}

int WriteDataToFile_A(const CGeoLayer* /*pLayer*/, const std::string& /*strFile*/, const std::string& /*strType*/)
{
    return 0;
}

int WriteDataToFile_W(const CGeoLayer* /*pLayer*/, const std::wstring& /*strFile*/, const std::wstring& /*strType*/)
{
    return 0;
}

bool CGeoFileBase::GetTimeOfFile(const char* lpFile, time_t* pTimeCreate, time_t* pTimeModify, time_t* pTimeAccess)
{
#ifdef WIN32
    HANDLE hFile;

    hFile = CreateFileA(lpFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    FILETIME ftCreate,ftAccess,ftWrite;
    SYSTEMTIME stUTC;

    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
    {
        CloseHandle(hFile);
        return false;
    }
    CloseHandle(hFile);

    struct tm       tmTempTime;
    if (pTimeCreate != 0)
    {
        FileTimeToSystemTime(&ftCreate, &stUTC);        
        tmTempTime.tm_year = stUTC.wYear - 1900;
        tmTempTime.tm_mon = stUTC.wMonth - 1;
        tmTempTime.tm_mday = stUTC.wDay;

        tmTempTime.tm_hour = stUTC.wHour;
        tmTempTime.tm_min = stUTC.wMinute;
        tmTempTime.tm_sec = stUTC.wSecond;

        tmTempTime.tm_isdst = 0;
        tmTempTime.tm_wday = 0;
        tmTempTime.tm_yday = 0;
        *pTimeCreate = mktime(&tmTempTime);
    }

    if (pTimeModify != 0)
    {
        FileTimeToSystemTime(&ftWrite, &stUTC);
        tmTempTime.tm_year = stUTC.wYear - 1900;
        tmTempTime.tm_mon = stUTC.wMonth - 1;
        tmTempTime.tm_mday = stUTC.wDay;

        tmTempTime.tm_hour = stUTC.wHour;
        tmTempTime.tm_min = stUTC.wMinute;
        tmTempTime.tm_sec = stUTC.wSecond;

        tmTempTime.tm_isdst = 0;
        tmTempTime.tm_wday = 0;
        tmTempTime.tm_yday = 0;
        *pTimeModify = mktime(&tmTempTime);
    }

    if (pTimeAccess != 0)
    {
        FileTimeToSystemTime(&ftAccess, &stUTC);
        tmTempTime.tm_year = stUTC.wYear - 1900;
        tmTempTime.tm_mon = stUTC.wMonth - 1;
        tmTempTime.tm_mday = stUTC.wDay;

        tmTempTime.tm_hour = stUTC.wHour;
        tmTempTime.tm_min = stUTC.wMinute;
        tmTempTime.tm_sec = stUTC.wSecond;

        tmTempTime.tm_isdst = 0;
        tmTempTime.tm_wday = 0;
        tmTempTime.tm_yday = 0;
        *pTimeAccess = mktime(&tmTempTime);
    }
    
#else

#endif
    return true;
}