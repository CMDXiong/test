/* ========================================================================== */
/* SCENERENDER.CPP -- 场景处理
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

#include "SceneRender.h"

CSceneRender::CSceneRender()
{
    m_nCurLayer = 0;
}


CSceneRender::~CSceneRender()
{
}

void CSceneRender::DrawScene()
{
//    if (!m_CurScene.valid() && m_CurScene->GetLayerNum() == 0)
//        return;
//    //遍历所有图层中的所有图元，然后分别画出图元
//    std::vector<ref_ptr<CGeoLayer> > layerList = m_CurScene->GetLayerList();
//    for(std::vector<ref_ptr<CGeoLayer> >::iterator iterLayer = layerList.begin(); iterLayer != layerList.end(); ++iterLayer){
//        ref_ptr<CGeoLayer> layer = *iterLayer;
//        GeoObjList objList = layer->GetObjList();
//        for(std::vector<ref_ptr<CGeoBase> >::iterator iterObj = objList.begin(); iterObj != objList.end(); ++iterObj){
//            CGeoBase* pData = (*iterObj).get();
//            switch(pData->GetObjType())
//            {
////            case GEO_TYPE_POINT:
////                DrawPrimitive(dynamic_cast<CGeoPoint*>(pData));
////                break;
//            case GEO_TYPE_LINE:
//                DrawPrimitive(dynamic_cast<CGeoLine*>(pData));
//                break;
//            case GEO_TYPE_CIRCLE:
//                DrawPrimitive(dynamic_cast<CGeoCircle*>(pData));
//                break;
//            case GEO_TYPE_ARC:
//                DrawPrimitive(dynamic_cast<CGeoArc*>(pData));
//                break;
//            case GEO_TYPE_LWPOLYLINE:
//                DrawPrimitive(dynamic_cast<CGeoLWPolyline*>(pData));
//                break;
//            case GEO_TYPE_ELLIPSE:
//                DrawPrimitive(dynamic_cast<CGeoEllipse*>(pData));
//                break;
//            case GEO_TYPE_TEXT:
//                DrawPrimitive(dynamic_cast<CGeoText*>(pData));
//                break;
//            case GEO_TYPE_MULRIPLER:
//                DrawPrimitive(dynamic_cast<CGeoMulripler*>(pData));
//                break;
//            case GEO_TYPE_NONE:
//                break;
//            default:
//                break;
//            }
//        }
//    }
}

void CSceneRender::DrawPrimitive(CGeoLine* pData)
{
    if (0 == pData)
        return;
    //begin to draw
}

CGeoLine* CSceneRender::AddLine(const CGeoPt& first, const CGeoPt& second)
{
    if (m_CurScene.valid() && m_CurScene->GetLayerNum() > 0)
    {
        CGeoLine* pData = new CGeoLine;
        pData->SetFirstPt(first);
        pData->SetSecondPt(second);
        m_CurScene->GetLayerByIndex(m_nCurLayer)->AddObj(pData);

        //DrawLine(pData);  //不要调用，这样可以解决只添加不绘制的情况
        return pData;
    }
    else
        return 0;
}

void CSceneRender::AddObjToScene(CGeoBase *pObj)
{
    if(m_CurScene->GetLayerByIndex(m_nCurLayer) == NULL){//还没有考虑层号
        CGeoLayer * newLayer = new CGeoLayer();
        newLayer->AddObj(pObj);
        m_CurScene->AddLayer(newLayer);
    }else{
        m_CurScene->GetLayerByIndex(m_nCurLayer)->AddObj(pObj);
    }

}
