/* ========================================================================== */
/* SCENERENDER.H -- 场景处理头文件
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

#ifndef CSCENERENDER_H
#define CSCENERENDER_H

#include "Geo.h"

//该类提供实现具体画图的接口
class CSceneRender
{
protected:
    ref_ptr<CGeoScene>  m_CurScene;
    int                 m_nCurLayer;

public:
    CSceneRender();
    virtual ~CSceneRender();

    void SetSceneData(CGeoScene* lpData) {
        m_CurScene = lpData;
    }
    void SetCurrentLayer(int nIndex)
    {
        if (nIndex > m_CurScene->GetLayerNum() || nIndex < 0)
            return;
        m_nCurLayer = nIndex;
    }
    int GetCurrentLayer() const { return m_nCurLayer; }
    CGeoLine* AddLine(const CGeoPt& first, const CGeoPt& second); //返回NULL表示添加失败
    //inline CGeoBase* AddObjToScene(CGeoBase* pObj);   //必须实现
    void AddObjToScene(CGeoBase* pObj);
    virtual void DrawScene(CGeoScene* lpData)
    {
        m_CurScene = lpData;
        DrawScene();
    }
    virtual void DrawScene();
    virtual void DrawBasePrimitive(CGeoBase* pData) =0;
    virtual void DrawPrimitive(CGeoLine* pData)=0;
    virtual void DrawPrimitive(CGeoCircle* pData)=0;
    virtual void DrawPrimitive(CGeoRectangle* pData)=0;
    virtual void DrawPrimitive(CGeoArc* pData)=0;
    virtual void DrawPrimitive(CGeoLWPolyline* pData)=0;
    virtual void DrawPrimitive(CGeoEllipse* pData)=0;
    virtual void DrawPrimitive(CGeoText* pData)=0;
    virtual void DrawPrimitive(CGeoMulripler* pData)=0;
    virtual void DrawPrimitive(CGeoTransform* pData)=0;
};

#endif
