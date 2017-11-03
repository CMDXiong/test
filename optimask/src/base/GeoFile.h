/* ========================================================================== */
/* GEOFILE.H -- 基本图形文件定义头文件
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

#ifndef _GEOFILELAYER_H
#define _GEOFILELAYER_H

#include "Geo.h"

//文件格式类型定义
#define     FILE_TYPE_GDS       "GDS"
#define     FILE_TYPE_DXF       "DXF"


//文件基类
class CGeoFileBase
{
protected:
    std::string m_strFileName;

public:
    //打开文件读取数据，如果文件格式不对或者数据有问题，则返回NULL
    virtual CGeoScene* ReadFile(const std::string& strFile) = 0;
    //写入场景数据到文件，如果写入出现问，则返回false，成功返回true
    virtual bool WriteFile(const std::string& /*strFile*/, const CGeoScene* /*lpData*/) { return false; }
    virtual ~CGeoFileBase(){}
    //获取文件的时间
    bool GetTimeOfFile(const char* lpFile, time_t* pTimeCreate, time_t* pTimeModify=0, time_t* pTimeAccess=0);
};

//以下两个函数以后要扩充为针对ANSII和UNICODE的两个版本
//从文件读取场景
//如果nType为FILE_TYPE_AUTO，则表示文件格式根据文件扩展名来确定
//如果返回为NULL，则表示读取失败
//如果pError有效，则返回值为NULL时，*pError 表示错误代码，-1表示传入参数错误，-2表示文件不存在，-3表示指定格式处理器不存在
CGeoScene* ReadDataFromFile_A(const std::string& strFile, const std::string& strType, int* pError=0);
CGeoScene* ReadDataFromFile_W(const std::wstring& strFile, const std::wstring& strType, int* pError = 0);


//写入场景数据到文件中
//如果nType为FILE_TYPE_AUTO，则表示文件格式根据文件扩展名来确定
//返回值为读取错误时的错误代码，0表示成功，负数表示失败。-1表示传入参数错误，-2表示文件没法打开，-3表示指定格式处理器不存在
int WriteDataToFile_A(const CGeoLayer* pLayer,const std::string& strFile, const std::string& strType);
int WriteDataToFile_W(const CGeoLayer* pLayer, const std::wstring& strFile, const std::wstring& strType);

#ifdef UNICODE
#define WriteDataToFile WriteDataToFile_W
#define ReadDataFromFile ReadDataFromFile_W
#else
#define WriteDataToFile WriteDataToFile_A
#define ReadDataFromFile ReadDataFromFile_A
#endif

typedef CGeoFileBase* (*FuncCreateFileObj)(const std::string& strExt);
struct FileFormatToClass
{
    static struct FileFormatToClass* g_pFirst;
    struct FileFormatToClass* m_pNext;
    FuncCreateFileObj m_pFunCreate;
    FileFormatToClass(FuncCreateFileObj fpTemp)
    {
        m_pNext = g_pFirst;
        g_pFirst = this;
        m_pFunCreate = fpTemp;
    }
};

#define DECLARE_FILEFORMAT(class_name) \
public: \
    static CGeoFileBase* class_name##CreateObject(const std::string& strExt); \
    static FileFormatToClass class_name##Ext;

#define  BEGIN_REGISTRATION(class_name) \
    FileFormatToClass class_name::class_name##Ext(class_name::class_name##CreateObject); \
    CGeoFileBase* class_name::class_name##CreateObject(const std::string& strExt) \
    { 
//记住类型名字必须全部用大些
#define REGISTRATION_EXT(class_name,ext_name) \
    if(strExt == ext_name)  \
        return new class_name();

#define END_REGISTRATION(class_name) \
    return 0; \
}

#endif
