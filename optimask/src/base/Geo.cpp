/* ========================================================================== */
/* GEO.CPP -- 基本图形定义 
 *
 * REFERENCE:
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

#include "Geo.h"

#define SET_ROW(row, v1, v2, v3 )    \
    m_mat[(row)][0] = (v1); \
    m_mat[(row)][1] = (v2); \
    m_mat[(row)][2] = (v3);

#define INNER_PRODUCT(a,b,r,c) \
    ((a).m_mat[r][0] * (b).m_mat[0][c]) \
    +((a).m_mat[r][1] * (b).m_mat[1][c]) \
    +((a).m_mat[r][2] * (b).m_mat[2][c])

//生成对象

bool GetLineProjectPoint(const CGeoPt& first, const CGeoPt& second, const CGeoPt& pt, CGeoPt* ptDist, CGeoPt* projPt)
{
    FLOAT_T dbDirX = pt.dx - first.dx;
    FLOAT_T dbDirY = pt.dy - first.dy;

    FLOAT_T dbDist = sqrt((second.dx - first.dx)*(second.dx - first.dx) + (second.dy - first.dy)*(second.dy - first.dy));
    if (dbDist < MIN_PRECISION)
        return false;       //not line
    FLOAT_T dbBaseX = (second.dx - first.dx) / dbDist;
    FLOAT_T dbBaseY = (second.dy - first.dy) / dbDist;

    //计算映射距离
    FLOAT_T dbLenAix = dbDirX*dbBaseX + dbDirY*dbBaseY;
    if (dbLenAix < 0 || dbLenAix > dbDist)   //不在映射范围
        return false;

    //计算距离
    if (ptDist != 0)
    {
        ptDist->dy = fabs(dbBaseX*dbDirY - dbBaseY*dbDirX);
        ptDist->dx = dbLenAix;
    }

    //计算实际点
    if (projPt != 0)
    {
        projPt->dx = first.dx + dbLenAix*dbBaseX;
        projPt->dy = first.dy + dbLenAix*dbBaseY;
    }
    return true;
}

//计算点到圆上的映射点
//参数pt表示实际的点，projPt参数表示映射点
//dbDist表示该店到圆心的距离
//如果点没法映射到地物上或者该函数没法计算，则返回值为false
//如果参数projPt为0，则表示不需要知道映射点，pt的x坐标不用，因为没法求弧长，y表示到圆弧上的距离
bool GetCircleProjectPoint(const CGeoPt& center, FLOAT_T radius, const CGeoPt& pt, FLOAT_T& dbDist, CGeoPt* projPt)
{
    FLOAT_T dbDirX = pt.dx - center.dx;
    FLOAT_T dbDirY = pt.dy - center.dy;
    dbDist = sqrt(dbDirX*dbDirX + dbDirY*dbDirY);

    if (dbDist < MIN_PRECISION)
        return false; //it is center point
    //计算距离
    dbDist = dbDist > radius ? dbDist - radius : radius - dbDist;

    if (projPt != 0)
    {
        radius = radius / dbDist;
        projPt->dy = dbDirY * radius + center.dx;
        projPt->dx = dbDirX * radius + center.dy;
    }
    return true;
}
//点必须在圆弧的扇形辐射方向内
bool GetArcProjectPoint(const CGeoArc& arc,const CGeoPt& pt, CGeoPt* ptDist, CGeoPt* projPt)
{
    const CGeoPt& center = arc.GetCenterPt();
    FLOAT_T radius = arc.GetRadius();

    FLOAT_T dbDirX = pt.dx - center.dx;
    FLOAT_T dbDirY = pt.dy - center.dy;
    FLOAT_T dbLength = sqrt(dbDirX*dbDirX + dbDirY*dbDirY);

    dbDirX /= dbLength;
    dbDirY /= dbLength;

    FLOAT_T dbAngle = 0;
    if (dbLength >= MIN_PRECISION)
    {
        FLOAT_T dbAngle = arc.GetAngleS() * M_PI / 180;
        FLOAT_T dbBaseX = cos(dbAngle);
        FLOAT_T dbBaseY = sin(dbAngle);

        FLOAT_T dbCos = dbDirX*dbBaseX + dbDirY*dbBaseY;
        FLOAT_T dbSin = dbBaseX * dbDirY - dbBaseY * dbDirX;
        dbAngle = atan2(dbSin, dbCos);
        if (dbAngle < 0) //小于0
            dbAngle += 2 * M_PI;

        double dbBaseAngle = arc.GetAngleE() - arc.GetAngleS();
        if (dbBaseAngle < 0)
            dbBaseAngle += 360;

        if (dbAngle * 180 / M_PI > dbBaseAngle)
            return false;  //beyond the end angele
    }
    else
        return false;   //it is center point
    //计算距离
    if (ptDist != 0)
    {
        ptDist->dy = dbLength > radius ? dbLength - radius : radius- dbLength;
        ptDist->dx = radius * dbAngle;
    }

    //计算位置点
    if (projPt != 0)
    {
        projPt->dy = dbDirY * radius + center.dx;
        projPt->dx = dbDirX * radius + center.dy;
    }

    return true;
}

void CGeoGDSStruct::InitData()
{
    m_tCreateTime = m_tLastModifyTime = time(NULL);
}

CGeoPt CGeoArc::GetCentrePoint() const
{
    FLOAT_T dbBaseAngle = m_dbAngleE - m_dbAngleS;
    if (dbBaseAngle < 0)
        dbBaseAngle += 360;
    dbBaseAngle = dbBaseAngle / 2 + m_dbAngleS;
    if (dbBaseAngle > 360)
        dbBaseAngle -= 360;

    FLOAT_T dbBaseX = m_dbRad*cos(dbBaseAngle* M_PI / 180) + m_dbX;
    FLOAT_T dbBaseY = m_dbRad*sin(dbBaseAngle* M_PI / 180) + m_dbY;

    return CGeoPt(dbBaseX, dbBaseY);
}

CGeoRect CGeoArc::GetBoundBox() const                 // Add by xiong 17/09/07
{
    std::vector<CGeoPt> allPoint;

    CGeoPt startPoint, endPoint, centerPoint;
    GetFirstPt(startPoint);
    GetSecondPt(endPoint);
    centerPoint = GetCentrePoint();
    allPoint.push_back(startPoint);
    allPoint.push_back(endPoint);
    allPoint.push_back(centerPoint);

    if (m_dbAngleS>270 && m_dbAngleE <90) { allPoint.push_back(CGeoPt(m_dbX + m_dbRad, m_dbY)); }
    if (m_dbAngleS>0 && m_dbAngleE <180) { allPoint.push_back(CGeoPt(m_dbX, m_dbY + m_dbRad)); }
    if (m_dbAngleS>90 && m_dbAngleE <270) { allPoint.push_back(CGeoPt(m_dbX - m_dbRad, m_dbY)); }
    if (m_dbAngleS>180 && m_dbAngleE <360) { allPoint.push_back(CGeoPt(m_dbX, m_dbY - m_dbRad)); }

    std::vector<FLOAT_T> xVector;
    std::vector<FLOAT_T> yVector;
    for (std::vector<CGeoPt>::const_iterator itr = allPoint.begin(); itr != allPoint.end(); ++itr)
    {
        xVector.push_back((*itr).dx);
        yVector.push_back((*itr).dy);
    }
    FLOAT_T dbMinX = *(std::min_element(xVector.begin(), xVector.end()));
    FLOAT_T dbMaxX = *(std::max_element(xVector.begin(), xVector.end()));
    FLOAT_T dbMinY = *(std::min_element(yVector.begin(), yVector.end()));
    FLOAT_T dbMaxY = *(std::max_element(yVector.begin(), yVector.end()));
    return CGeoRect(dbMinX, dbMinY, dbMaxX, dbMaxY);
}

CGeoRect CGeoLWPolyline::GetBoundBox() const       // Add by xiong 17/09/07
{
    std::vector<FLOAT_T> xVector;
    std::vector<FLOAT_T> yVector;
    for (std::vector<CGeoPt>::const_iterator itr = m_allPoint.begin(); itr != m_allPoint.end(); ++itr)
    {
        xVector.push_back((*itr).dx);
        yVector.push_back((*itr).dy);
    }
    FLOAT_T dbMinX = *(std::min_element(xVector.begin(), xVector.end()));
    FLOAT_T dbMaxX = *(std::max_element(xVector.begin(), xVector.end()));
    FLOAT_T dbMinY = *(std::min_element(yVector.begin(), yVector.end()));
    FLOAT_T dbMaxY = *(std::max_element(yVector.begin(), yVector.end()));
    return CGeoRect(dbMinX, dbMinY, dbMaxX, dbMaxY);
}

void CMatrix::MakeIdentity()
{
    SET_ROW(0, 1, 0, 0)
    SET_ROW(1, 0, 1, 0)
    SET_ROW(2, 0, 0, 1)
}

void CMatrix::MakeScale(FLOAT_T x, FLOAT_T y)
{
    SET_ROW(0, x, 0, 0)
    SET_ROW(1, 0, y, 0)
    SET_ROW(2, 0, 0, 1)
}

void CMatrix::MakeTranslate(FLOAT_T x, FLOAT_T y)
{
    SET_ROW(0, 1, 0, 0)
    SET_ROW(1, 0, 1, 0)
    SET_ROW(2, x, y, 1)
}

void CMatrix::MakeRotate(FLOAT_T angle)
{
    SET_ROW(0, cos(angle), sin(angle), 0)
    SET_ROW(1, -sin(angle), cos(angle), 0)
    SET_ROW(2, 0, 0, 1)
}

//根据角度的余弦和正弦设置旋转
void CMatrix::MakeRotate(FLOAT_T cos, FLOAT_T sin)
{
    SET_ROW(0, cos, sin, 0)
    SET_ROW(1, -sin, cos, 0)
    SET_ROW(2, 0, 0, 1)
}

void CMatrix::Mult(const CMatrix& lhs, const CMatrix& rhs)
{
    if (&lhs == this)
    {
        PostMult(rhs);
        return;
    }
    if (&rhs == this)
    {
        PreMult(lhs);
        return;
    }

    // PRECONDITION: We assume neither &lhs nor &rhs == this
    // if it did, use preMult or postMult instead
    m_mat[0][0] = INNER_PRODUCT(lhs, rhs, 0, 0);
    m_mat[0][1] = INNER_PRODUCT(lhs, rhs, 0, 1);
    m_mat[0][2] = INNER_PRODUCT(lhs, rhs, 0, 2);
    m_mat[1][0] = INNER_PRODUCT(lhs, rhs, 1, 0);
    m_mat[1][1] = INNER_PRODUCT(lhs, rhs, 1, 1);
    m_mat[1][2] = INNER_PRODUCT(lhs, rhs, 1, 2);
    m_mat[2][0] = INNER_PRODUCT(lhs, rhs, 2, 0);
    m_mat[2][1] = INNER_PRODUCT(lhs, rhs, 2, 1);
    m_mat[2][2] = INNER_PRODUCT(lhs, rhs, 2, 2);
    m_mat[3][0] = INNER_PRODUCT(lhs, rhs, 3, 0);
    m_mat[3][1] = INNER_PRODUCT(lhs, rhs, 3, 1);
    m_mat[3][2] = INNER_PRODUCT(lhs, rhs, 3, 2);
}

void CMatrix::PreMult(const CMatrix& other)
{
    FLOAT_T t[3];
    for (int col = 0; col<3; ++col) {
        t[0] = INNER_PRODUCT(other, *this, 0, col);
        t[1] = INNER_PRODUCT(other, *this, 1, col);
        t[2] = INNER_PRODUCT(other, *this, 2, col);
        m_mat[0][col] = t[0];
        m_mat[1][col] = t[1];
        m_mat[2][col] = t[2];
    }
}

void CMatrix::PostMult(const CMatrix& other)
{
    FLOAT_T t[3];
    for (int row = 0; row<3; ++row)
    {
        t[0] = INNER_PRODUCT(*this, other, row, 0);
        t[1] = INNER_PRODUCT(*this, other, row, 1);
        t[2] = INNER_PRODUCT(*this, other, row, 2);
        SET_ROW(row, t[0], t[1], t[2])
    }
}
FLOAT_T CMatrix::GetScale() const
{ 
    FLOAT_T dbScal = m_mat[0][0] * m_mat[1][1] - m_mat[0][1] * m_mat[1][0];
    if (dbScal < 0)
        return -sqrt(-dbScal);
    else
        return sqrt(dbScal);
}

void CMatrix::GetScale(FLOAT_T& x, FLOAT_T& y) const
{
    x = GetScale();
    y = fabs(x);

    //以下是针对两个不同的缩放比例进行计算
    //FLOAT_T len = m_mat[0][0] * m_mat[1][1] - m_mat[0][1] * m_mat[1][0];    //两个缩放比例的乘积
}

void CMatrix::GetTranslate(FLOAT_T& x, FLOAT_T& y) const
{ 
    FLOAT_T dbScal = m_mat[0][0] * m_mat[1][1] - m_mat[0][1] * m_mat[1][0];
    x = m_mat[2][0]/ dbScal;
    y = m_mat[2][1]/ dbScal;
}
//假设缩放比例相同
FLOAT_T CMatrix::GetAngle()const
{
    FLOAT_T a, b;
    GetScale(a, b);

    FLOAT_T dbCos = m_mat[0][0] / a;
    FLOAT_T dbSin = m_mat[0][1] / b;

    a= atan2(dbSin, dbCos);
    if (a < 0)
        a += 2 * M_PI;

    return a;
}
////得到旋转与平移矩阵
void CMatrix::GetRotateAndTranslate(CMatrix& mat) const
{
    mat.MakeIdentity();
    double x = fabs(GetScale());
    for (int i = 0;i < 3;i++)
        for (int j = 0;j < 2;j++)
            mat.m_mat[i][j] = m_mat[i][j]/x;
}

//得到矩阵的逆
void CMatrix::GetInvMatrix(CMatrix& mat) const
{
    mat.MakeIdentity();

    FLOAT_T dbScal = m_mat[0][0] * m_mat[1][1] - m_mat[0][1] * m_mat[1][0];
    mat.m_mat[0][0] = m_mat[1][1] / dbScal;
    mat.m_mat[0][1] = -m_mat[0][1] / dbScal;
    mat.m_mat[1][0] = -m_mat[1][0] / dbScal;
    mat.m_mat[1][1] = m_mat[0][0] / dbScal;
    mat.m_mat[2][0] = (m_mat[1][0]*m_mat[2][1]-m_mat[1][1]*m_mat[2][0]) / dbScal;
    mat.m_mat[2][1] = (m_mat[0][1]*m_mat[2][0] - m_mat[0][0]*m_mat[2][1]) / dbScal;
}

//以下为矩形操作函数----------------------
void CGeoRectangle::SetRectPt(FLOAT_T dbLeft, FLOAT_T dbRight, FLOAT_T dbTop, FLOAT_T dbBottom)
{
    if (dbLeft < dbRight)
    {
        m_dbLeft = dbLeft;
        m_dbRight = dbRight;
    }
    else
    {
        m_dbLeft = dbRight;
        m_dbRight = dbLeft;
    }

    if (dbTop > dbBottom)
    {
        m_dbBottom = dbBottom;
        m_dbTop = dbTop;
    }
    else
    {
        m_dbBottom = dbTop;
        m_dbTop = dbBottom;
    }
}

void CGeoRectangle::SetRectPt(const std::vector<CGeoPt>& lstPt)
{
    FLOAT_T dbLeft, dbRight, dbTop, dbBottom;
    if (lstPt.size() < 2)
        return;
    else
    {
        const CGeoPt& pt = lstPt.at(0);
        dbLeft = dbRight = pt.dx;
        dbTop = dbBottom = pt.dy;
    }

    for (unsigned i=1; i<lstPt.size(); ++i)
    {
        const CGeoPt& pt = lstPt.at(i);
        if (pt.dx < dbLeft)
            dbLeft = pt.dx;
        else if (pt.dx > dbRight)
            dbRight = pt.dx;

        if (pt.dy < dbBottom)
            dbBottom = pt.dy;
        else if (pt.dy > dbTop)
            dbTop = pt.dy;
    }
    SetRectPt(dbLeft, dbRight, dbTop, dbBottom);
}
void CGeoRectangle::SetRectPt(const CGeoLWPolyline& poly)
{
    const std::vector<CGeoPt>& lstPt = poly.GetPtList();
    SetRectPt(lstPt);
}

//---------------------------------------

CGeoScene::CGeoScene()
{
    m_dwLastNo = 0;
}

bool CGeoLayer::DelObj(unsigned nIndex)
{
    if(nIndex >= m_lstChildList.size())
        return false;
    m_lstChildList.erase(m_lstChildList.begin()+nIndex);
    return true;
}

//添加子节点，成功返回true，失败返回false
CGeoRect CGeoMulripler::GetBoundBox() const
{

    std::vector<FLOAT_T> allPointX; //所有点的X坐标;
    std::vector<FLOAT_T> allPointY; //所有点的Y坐标;
    CMatrix mat;
    for(unsigned int i = 0; i<m_vecChildren.size(); ++i)
    {
        CGeoBase *geoChild = m_vecChildren[i].get();
        CGeoTransform transform;
        transform.SetTransformMatrix(mat);
        transform.SetChild(geoChild);
        CGeoRect rect = transform.GetBoundBox();
        allPointX.push_back(rect.m_dLeft);
        allPointX.push_back(rect.m_dRight);
        allPointY.push_back(rect.m_dTop);
        allPointY.push_back(rect.m_dBottom);
    }
    FLOAT_T dbMinX = *(std::min_element(allPointX.begin(), allPointX.end()));
    FLOAT_T dbMaxX = *(std::max_element(allPointX.begin(), allPointX.end()));
    FLOAT_T dbMinY = *(std::min_element(allPointY.begin(), allPointY.end()));
    FLOAT_T dbMaxY = *(std::max_element(allPointY.begin(), allPointY.end()));
    return CGeoRect(dbMinX, dbMinY, dbMaxX, dbMaxY);

    //        std::vector<FLOAT_T> xVector;
    //        std::vector<FLOAT_T> yVector;

    //        for(unsigned int i = 0; i<m_vecChildren.size(); ++i)
    //        {
    //            CGeoBase* pData = m_vecChildren[i].get();
    //            if(pData->GetBoundBox().m_dLeft > pData->GetBoundBox().m_dRight)
    //                xVector.push_back(pData->GetBoundBox().m_dLeft);
    //            else
    //                xVector.push_back(pData->GetBoundBox().m_dRight);

    //            if(pData->GetBoundBox().m_dTop > pData->GetBoundBox().m_dBottom)
    //                yVector.push_back(pData->GetBoundBox().m_dTop);
    //            else
    //                yVector.push_back(pData->GetBoundBox().m_dBottom);
    //        }
    //        FLOAT_T dbMinX = *(std::min_element(xVector.begin(), xVector.end()));
    //        FLOAT_T dbMaxX = *(std::max_element(xVector.begin(), xVector.end()));
    //        FLOAT_T dbMinY = *(std::min_element(yVector.begin(), yVector.end()));
    //        FLOAT_T dbMaxY = *(std::max_element(yVector.begin(), yVector.end()));

    //        return CGeoRect(dbMinX, dbMinY, dbMaxX, dbMaxY);
}

bool CGeoMulripler::AddChild(CGeoBase* pElement)
{
    if (pElement != 0)
    {
        m_vecChildren.push_back(pElement);
        return true;
    }
    else
        return false;
}
//设置子节点，成功返回true，失败返回false
bool CGeoMulripler::SetChild(unsigned nIndex, CGeoBase* pElement)
{
    if (nIndex >= m_vecChildren.size() || 0 == pElement)
        return false;
    m_vecChildren[nIndex] = pElement;
    return true;
}

#include <QDebug>

CGeoRect CGeoTransform::GetBoundBox() const
{
    CMatrix mat = *m_mat;

    CGeoTransform* pTran = dynamic_cast<CGeoTransform*>(m_geoChild.get()); // 判断子节点是否是CGeoTransform类或其派生类
    if (pTran != 0)
    {

        CGeoTransform trans = *pTran;
        mat *= pTran->GetTransformMatrix();//更新矩阵
        trans.SetTransformMatrix(mat);
        return trans.GetBoundBox();
    }

    CGeoMulripler* pMulti = dynamic_cast<CGeoMulripler*>(m_geoChild.get());
    if (pMulti != 0)
    {
        std::vector<FLOAT_T> allPointX; //所有点的X坐标;
        std::vector<FLOAT_T> allPointY; //所有点的Y坐标;
        if (pMulti->GetObjType() == GEO_TYPE_MULRIPLER || pMulti->GetObjType() == GEO_TYPE_STRUCT) {
            for (unsigned i = 0; i < pMulti->GetChildCount(); ++i)
            {
                CGeoBase *geoChild = pMulti->GetChild(i);
                CGeoTransform transform;
                transform.SetTransformMatrix(mat);
                transform.SetChild(geoChild);
                CGeoRect rect = transform.GetBoundBox();
                allPointX.push_back(rect.m_dLeft);
                allPointX.push_back(rect.m_dRight);
                allPointY.push_back(rect.m_dTop);
                allPointY.push_back(rect.m_dBottom);
            }
            if ((!allPointX.empty()) || (!allPointY.empty())) {
                FLOAT_T dbMinX = *(std::min_element(allPointX.begin(), allPointX.end()));
                FLOAT_T dbMaxX = *(std::max_element(allPointX.begin(), allPointX.end()));
                FLOAT_T dbMinY = *(std::min_element(allPointY.begin(), allPointY.end()));
                FLOAT_T dbMaxY = *(std::max_element(allPointY.begin(), allPointY.end()));
                return CGeoRect(dbMinX, dbMinY, dbMaxX, dbMaxY);
            }
            else {
                return CGeoRect();  //这里并没有写准确
            }

        }
        if (pMulti->GetObjType() == GEO_TYPE_ARRAY) {
            CGeoArray *pArray = static_cast<CGeoArray*>(m_geoChild.get());

            int     rowNum, colNum;
            FLOAT_T interRow, interCol;
            pArray->GetRowAndCol(rowNum, colNum);
            pArray->GetRowAndColInterDist(interRow, interCol);
            for (int i = 0; i<rowNum; ++i)
                for (int j = 0; j<colNum; ++j)
                    for (unsigned k = 0; k < pArray->GetChildCount(); ++k)
                    {
//                        CGeoTransform transform;
//                        CGeoPt pt(j*interCol, i*interRow);
//                        CMatrix   invMat;
//                        mat.GetInvMatrix(invMat);
//                        CGeoPt newPt = invMat * pt;

//                        transform.SetTransformMatrix(mat);
//                        transform.Translate(newPt.dx,newPt.dy);
//                        transform.SetChild(pArray->GetChild(k));
//                        CGeoRect rect = transform.GetBoundBox();
//                        allPointX.push_back(rect.m_dLeft);
//                        allPointX.push_back(rect.m_dRight);
//                        allPointY.push_back(rect.m_dTop);
//                        allPointY.push_back(rect.m_dBottom);

                        CGeoTransform transform;
                        FLOAT_T angle = mat.GetAngle();
                        CGeoPt pt(j*interCol, i*interRow);

                        CMatrix rotateMat = CMatrix::Rotate(angle);
                        CGeoPt newPt = rotateMat * pt;
                        CMatrix translateMat = CMatrix::Translate(newPt.dx, newPt.dy);
                        translateMat *= mat;
                        transform.SetTransformMatrix(translateMat);
                        transform.SetChild(pArray->GetChild(k));

                        CGeoRect rect = transform.GetBoundBox();
                        allPointX.push_back(rect.m_dLeft);
                        allPointX.push_back(rect.m_dRight);
                        allPointY.push_back(rect.m_dTop);
                        allPointY.push_back(rect.m_dBottom);
                    }
            FLOAT_T dbMinX = *(std::min_element(allPointX.begin(), allPointX.end()));
            FLOAT_T dbMaxX = *(std::max_element(allPointX.begin(), allPointX.end()));
            FLOAT_T dbMinY = *(std::min_element(allPointY.begin(), allPointY.end()));
            FLOAT_T dbMaxY = *(std::max_element(allPointY.begin(), allPointY.end()));
            return CGeoRect(dbMinX, dbMinY, dbMaxX, dbMaxY);
        }
    }

    switch (m_geoChild->GetObjType()) {
    case GEO_TYPE_POINT:
    {
        CGeoPoint* point = static_cast<CGeoPoint* >(m_geoChild.get());
        CGeoPt newPt = mat * point->GetPt();
        CGeoPoint newPoint;
        newPoint.SetPt(newPt.dx, newPt.dy);
        return newPoint.GetBoundBox();
    }

    case GEO_TYPE_LINE:
    {
        CGeoLine* line = static_cast<CGeoLine* >(m_geoChild.get());
        CGeoPt newFirstPt = mat * line->GetFirstPt();
        CGeoPt newSecondPt = mat * line->GetSecondPt();
        CGeoLine newLine(newFirstPt, newSecondPt);
        return newLine.GetBoundBox();
    }
    case GEO_TYPE_CIRCLE:
    {
        CGeoCircle* circle = static_cast<CGeoCircle* >(m_geoChild.get());
        CGeoCircle newCircle;
        CMatrix rotateTranslateMaT;                                         // 旋转平移矩阵
        mat.GetRotateAndTranslate(rotateTranslateMaT);
        newCircle.SetRadius(mat.GetScale() * circle->GetRadius());          // 半径只能放缩
        newCircle.SetCenterPt(rotateTranslateMaT * circle->GetCenterPt());  // 圆心旋转平移
        return newCircle.GetBoundBox();
    }
    case GEO_TYPE_ARC:
    {
        CGeoArc* arc = static_cast<CGeoArc* >(m_geoChild.get());
        CGeoArc newArc;
        CMatrix rotateTranslateMaT;
        mat.GetRotateAndTranslate(rotateTranslateMaT);

        newArc.SetRadius(mat.GetScale() * arc->GetRadius());                 // 设置新的半径
        newArc.SetCenterPt(rotateTranslateMaT * arc->GetCenterPt());         // 设置新的中心点
        newArc.SetAngleS(mat.GetAngle() + arc->GetAngleS());                 // 设置新的起始角度
        newArc.SetAngleE(mat.GetAngle() + arc->GetAngleE());                 // 设置新的终止角度
        return newArc.GetBoundBox();
    }
    case GEO_TYPE_LWPOLYLINE:
    {
        const CGeoLWPolyline* pPloy = static_cast<const CGeoLWPolyline*>(m_geoChild.get());
        const std::vector<CGeoPt>& ptList = pPloy->GetPtList();
        CGeoLWPolyline newLwPolyline;
        newLwPolyline.SetClose(pPloy->IsClosed());
        newLwPolyline.SetFill(pPloy->IsFill());
        for (unsigned i = 0;i < ptList.size();i++)
        {
            const CGeoPt& pt = ptList.at(i);
            CGeoPt ptNew = mat*pt;
            newLwPolyline.AddPt(ptNew);
        }
        return newLwPolyline.GetBoundBox();
    }
    case GEO_TYPE_ELLIPSE:
    {
        CGeoEllipse* pEllipse = static_cast<CGeoEllipse* >(m_geoChild.get());
        FLOAT_T width  = mat.GetScale() * pEllipse->GetWidth();
        FLOAT_T height = mat.GetScale() * pEllipse->GetHeight();
        CMatrix rotateTranslateMaT;
        mat.GetRotateAndTranslate(rotateTranslateMaT);
        CGeoPt center = rotateTranslateMaT * pEllipse->GetCenterPt(); // 圆心
        FLOAT_T angle = mat.GetAngle();  // 弧度

        CGeoRect   FrameRect ;
        FLOAT_T   a = width;                         // 椭圆长轴一半
        FLOAT_T   b = height;                        // 椭圆短轴一半
        FLOAT_T angleXT = atan((-b * tan(angle))/a);
        FLOAT_T xFrame1 = center.dx + a * cos(angleXT) * cos(angle) - b * sin(angleXT) *sin(angle);
        angleXT += M_PI;
        FLOAT_T xFrame2 = center.dx + a * cos(angleXT) * cos(angle) - b * sin(angleXT) *sin(angle);
        if(xFrame1 > xFrame2){
            FrameRect.m_dLeft  = xFrame2;
            FrameRect.m_dRight = xFrame1;
        }else{
            FrameRect.m_dLeft  = xFrame1;
            FrameRect.m_dRight = xFrame2;
        }

        FLOAT_T angleYT = atan(b/(a * tan(angle)));
        FLOAT_T yFrame1 = center.dy + b * sin(angleYT) * cos(angle) + a * cos(angleYT) *sin(angle);
        angleYT += M_PI;
        FLOAT_T yFrame2 = center.dy + b * sin(angleYT) * cos(angle) + a * cos(angleYT) *sin(angle);
        if(yFrame1 > yFrame2){
            FrameRect.m_dBottom  = yFrame1;
            FrameRect.m_dTop     = yFrame2;
        }else{
            FrameRect.m_dBottom  = yFrame2;
            FrameRect.m_dTop     = yFrame1;
        }
        return FrameRect;
    }
    case GEO_TYPE_TEXT:
    {
        CGeoRect   FrameRect = m_geoChild->GetBoundBox();
        CGeoPt leftTop(FrameRect.m_dLeft, FrameRect.m_dTop);
        CGeoPt rightBottom(FrameRect.m_dRight, FrameRect.m_dBottom);
        CGeoPt rightTop(FrameRect.m_dRight, FrameRect.m_dTop);
        CGeoPt leftBottom(FrameRect.m_dLeft, FrameRect.m_dBottom);

        leftTop = (*m_mat) * leftTop;
        rightBottom = (*m_mat) * rightBottom;
        rightTop = (*m_mat) * rightTop;
        leftBottom = (*m_mat) * leftBottom;

        FrameRect.m_dLeft = std::min(std::min(leftTop.dx, rightBottom.dx), std::min(rightTop.dx, leftBottom.dx));
        FrameRect.m_dTop = std::min(std::min(leftTop.dy, rightBottom.dy), std::min(rightTop.dy, leftBottom.dy));
        FrameRect.m_dRight = std::max(std::max(leftTop.dx, rightBottom.dx), std::max(rightTop.dx, leftBottom.dx));
        FrameRect.m_dBottom = std::max(std::max(leftTop.dy, rightBottom.dy), std::max(rightTop.dy, leftBottom.dy));
        return FrameRect;
    }
    case GEO_TYPE_RECT:
    {
        CGeoRect   FrameRect = m_geoChild->GetBoundBox();
        CGeoPt leftTop(FrameRect.m_dLeft, FrameRect.m_dTop);
        CGeoPt rightBottom(FrameRect.m_dRight, FrameRect.m_dBottom);
        CGeoPt rightTop(FrameRect.m_dRight, FrameRect.m_dTop);
        CGeoPt leftBottom(FrameRect.m_dLeft, FrameRect.m_dBottom);

        leftTop = (*m_mat) * leftTop;
        rightBottom = (*m_mat) * rightBottom;
        rightTop = (*m_mat) * rightTop;
        leftBottom = (*m_mat) * leftBottom;

        FrameRect.m_dLeft = std::min(std::min(leftTop.dx, rightBottom.dx), std::min(rightTop.dx, leftBottom.dx));
        FrameRect.m_dTop = std::min(std::min(leftTop.dy, rightBottom.dy), std::min(rightTop.dy, leftBottom.dy));
        FrameRect.m_dRight = std::max(std::max(leftTop.dx, rightBottom.dx), std::max(rightTop.dx, leftBottom.dx));
        FrameRect.m_dBottom = std::max(std::max(leftTop.dy, rightBottom.dy), std::max(rightTop.dy, leftBottom.dy));
        return FrameRect;
    }
    default://到这里应该是不知道的类型
        break;
    }
}

//构元添加子节点(图元)，该函数限制添加的子节点必须是从CGeoGDSElement类派生的
bool CGeoGDSStruct::AddChild(CGeoBase* pElement)
{
    if (0 == pElement)
        return false;

    unsigned short wType = pElement->GetObjType();
    if (wType < GEO_TYPE_ELEMENT || wType >= GEO_TYPE_ELEM_END)
        return false;

    CGeoGDSElement* pChildElement = static_cast<CGeoGDSElement*>(pElement);
    pChildElement->SetParent(this);
    m_vecChildren.push_back(pElement);
    return true;
}
//构元设置子节点(图元)，该函数限制子节点必须是从CGeoGDSElement类派生的
bool CGeoGDSStruct::SetChild(unsigned nIndex, CGeoBase* pElement)
{
    if (0 == pElement || nIndex >= m_vecChildren.size())
        return false;

    unsigned short wType = pElement->GetObjType();
    if (wType < GEO_TYPE_ELEMENT || wType >= GEO_TYPE_ELEM_END)
        return false;
    static_cast<CGeoGDSElement*>(m_vecChildren[nIndex].get())->SetParent(0);
    m_vecChildren[nIndex] = pElement;
    static_cast<CGeoGDSElement*>(pElement)->SetParent(this);
    return true;
}

//添加应用该构元的图元
//引用该构元的图元数目等于GetRefCount()函数获取的数目
bool CGeoGDSStruct::AddRefElement(CGeoGDSElement* pElement)
{
    if (0 == pElement)
        return false;

    unsigned short wType = pElement->GetObjType();
    if (wType == GEO_TYPE_SREF || wType == GEO_TYPE_AREF)
    {
        m_vecRefElement.push_back(pElement);
        return true;
    }
    else
        return false;
}

//删除引用该构元的图元
bool CGeoGDSStruct::DelRefElement(CGeoGDSElement* pElement)
{
    if (0 == pElement)
        return false;

    std::vector<ref_ptr<CGeoGDSElement> >::iterator pos = m_vecRefElement.begin();
    while (pos != m_vecRefElement.end())
    {
        if (pos->get() == pElement)
        {
            m_vecRefElement.erase(pos);
            return true;
        }
    }
    return false;
}

//得到该构元里面各种类型的子节点的数据
//返回值得到子节点的类型数目
//pType表示要求各类型数目的数组，每一个元素的高32为表示类型值如GEO_TYPE_PATH，低32为表示具体的数目
//如果该参数为零或者nSize参数为零，则返回具体的类型数目，忽略其他参数
//nSize参数表示pType数组大小，如果该大小小于实际的类型数据，则按照类型值得大小返回前面nSize个数据
int CGeoGDSStruct::GetAllChildTypeCount(ULLong* pType, int nSize) const
{
    int nCount = 0;
    unsigned tmpType[10] = { 0 };

    if (m_vecChildren.empty())
        return 0;

    GeoObjList::const_iterator pos = m_vecChildren.begin();
    while (pos != m_vecChildren.end())
    {
        unsigned short wType = pos->get()->GetObjType();
        if (wType >= GEO_TYPE_BOUNDARY && wType < GEO_TYPE_REF_REV)
        {
            tmpType[wType - GEO_TYPE_BOUNDARY]++;
        }
        pos++;
    }

    for (int i = 0; i < 10;i++)
    {
        if (tmpType[i] != 0)
        {
            if (pType != 0 && nCount < nSize)
            {
                pType[nCount] = i + GEO_TYPE_BOUNDARY;
                pType[nCount] <<= 32;
                pType[nCount] |= tmpType[i];
            }
            nCount++;
        }
    }
    return nCount;
}

//根据索引获取引用该构元的图元
CGeoGDSElement* CGeoGDSStruct::GetRefElement(unsigned nIndex)
{
    if (nIndex >= m_vecRefElement.size())
        return 0;
    return m_vecRefElement.at(nIndex).get();
}
//根据索引获取引用该构元的图元所属的构元
CGeoGDSStruct* CGeoGDSStruct::GetRefCell(unsigned nIndex)
{
    CGeoGDSElement* pTemp = GetRefElement(nIndex);
    if (pTemp != 0)
    {
        return static_cast<CGeoGDSStruct*>(pTemp->GetParent());
    }
    else
        return 0;
}

//设置子节点，各个派生类可以根据具体的情况对节点类型做一些判断
bool CGeoTransform::SetChild(CGeoBase* child)
{ 
    m_geoChild = child;
    return true;
}

//设置子节点
//REF(SREF,AREF)节点只能设置为cell，其他的只能设置为基本图元不能设置为cell图元
bool CGeoGDSElement::SetElementChild(CGeoBase* child)
{
    if (0 == child)
        return false;

    unsigned short dwChildType = child->GetObjType();
    if (GEO_TYPE_SREF == m_wType || GEO_TYPE_AREF == m_wType)
    {
        if (dwChildType != GEO_TYPE_STRUCT)
            return false;

        if (GEO_TYPE_SREF == m_wType) //SREF节点
        {
            //如果原先当前的节点存在构元，则删除
            if (m_geoChild.valid())
            {
                static_cast<CGeoGDSStruct*>(m_geoChild.get())->DelRefElement(this);
            }
            m_geoChild = child;
        }
        else //AREF节点
        {
            CGeoArray* pMid = static_cast<CGeoArray*>(m_geoChild.get());
            CGeoBase* pChild = pMid->FrontChild();
            if (pChild != 0 && pChild->GetObjType() == GEO_TYPE_STRUCT)
            {
                static_cast<CGeoGDSStruct*>(pChild)->DelRefElement(this);
                pMid->SetChild(0, child);
            }
            else
            {
                if (pChild != 0)
                    pMid->SetChild(0, child);
                else
                    pMid->AddChild(child);
            }
        }
        static_cast<CGeoGDSStruct*>(child)->AddRefElement(this);
        return true;
    }
    else
    {
        if (dwChildType == GEO_TYPE_STRUCT)
            return false;
    }

    m_geoChild = child;
    return true;
}

CGeoBase* CGeoGDSElement::GetElementChild()
{ 
    if (GEO_TYPE_AREF == m_wType)
    {
        CGeoArray* pMid = static_cast<CGeoArray*>(m_geoChild.get());
        return pMid->GetChild(0);
    }
    else
        return GetChild();
}

CGeoRect CGeoGDSBoundary::GetBoundBox() const
{
    CGeoLWPolyline *pLWPolyling = static_cast<CGeoLWPolyline* >(m_geoChild.get());
    FLOAT_T dbMinX = std::numeric_limits<double>::max();
    FLOAT_T dbMinY = std::numeric_limits<double>::max();
    FLOAT_T dbMaxX = std::numeric_limits<double>::min();
    FLOAT_T dbMaxY = std::numeric_limits<double>::min();

    for (unsigned int i = 0; i<pLWPolyling->GetPtList().size(); ++i)
    {
        CGeoPt pt = (*m_mat) * pLWPolyling->GetPtList().at(i);
        if (pt.dx < dbMinX) dbMinX = pt.dx;
        if (pt.dx > dbMaxX) dbMaxX = pt.dx;
        if (pt.dy < dbMinY) dbMinY = pt.dy;
        if (pt.dy > dbMaxY) dbMaxY = pt.dy;
    }

    return CGeoRect(dbMinX, dbMinY, dbMaxX, dbMaxY);

    //        另一种方式
    //        std::vector<FLOAT_T> allPointX; //所有点的X坐标;
    //        std::vector<FLOAT_T> allPointY; //所有点的X坐标;
    //        for(unsigned int i = 0; i<pLWPolyling->GetPtList().size(); ++i)
    //        {
    //            CGeoPt pt = m_mat * pLWPolyling->GetPtList().at(i);
    //            allPointX.push_back(pt.dx);
    //            allPointY.push_back(pt.dy);
    //        }

    //        FLOAT_T dbMinX = *(std::min_element(allPointX.begin(), allPointX.end()));
    //        FLOAT_T dbMaxX = *(std::max_element(allPointX.begin(), allPointX.end()));
    //        FLOAT_T dbMinY = *(std::min_element(allPointY.begin(), allPointY.end()));
    //        FLOAT_T dbMaxY = *(std::max_element(allPointY.begin(), allPointY.end()));

    //        return CGeoRect(dbMinX, dbMinY, dbMaxX, dbMaxY);
}

CGeoRect CGeoGDSSRef::GetBoundBox() const //对于子节点中包含椭圆的图元来说，这个边框计算的不够准确
{
    CGeoRect   FrameRect = m_geoChild->GetBoundBox();
    CGeoPt leftTop(FrameRect.m_dLeft, FrameRect.m_dTop);
    CGeoPt rightBottom(FrameRect.m_dRight, FrameRect.m_dBottom);
    CGeoPt rightTop(FrameRect.m_dRight, FrameRect.m_dTop);
    CGeoPt leftBottom(FrameRect.m_dLeft, FrameRect.m_dBottom);

    leftTop = (*m_mat) * leftTop;
    rightBottom = (*m_mat) * rightBottom;
    rightTop = (*m_mat) * rightTop;
    leftBottom = (*m_mat) * leftBottom;

    FrameRect.m_dLeft = std::min(std::min(leftTop.dx, rightBottom.dx), std::min(rightTop.dx, leftBottom.dx));
    FrameRect.m_dTop = std::min(std::min(leftTop.dy, rightBottom.dy), std::min(rightTop.dy, leftBottom.dy));
    FrameRect.m_dRight = std::max(std::max(leftTop.dx, rightBottom.dx), std::max(rightTop.dx, leftBottom.dx));
    FrameRect.m_dBottom = std::max(std::max(leftTop.dy, rightBottom.dy), std::max(rightTop.dy, leftBottom.dy));
    return FrameRect;
}

CGeoRect CGeoGDSARef::GetBoundBox() const//对于子节点中包含椭圆的图元来说，这个边框计算的不够准确
{
    CGeoRect   FrameRect = m_geoChild->GetBoundBox();
    CGeoPt leftTop(FrameRect.m_dLeft, FrameRect.m_dTop);
    CGeoPt rightBottom(FrameRect.m_dRight, FrameRect.m_dBottom);
    leftTop = (*m_mat) * leftTop;
    rightBottom = (*m_mat) * rightBottom;

    FrameRect.m_dLeft = m_dbRefX;
    FrameRect.m_dTop = m_dbRefY;
    FrameRect.m_dRight = m_dbRefX + (rightBottom.dx - leftTop.dx);
    FrameRect.m_dBottom = m_dbRefY + (rightBottom.dy - leftTop.dy);
    return FrameRect;
}

CGeoRect CGeoGDSText::GetBoundBox() const
{
    CGeoRect   FrameRect = m_geoChild->GetBoundBox();
    CGeoPt leftTop(FrameRect.m_dLeft, FrameRect.m_dTop);
    CGeoPt rightBottom(FrameRect.m_dRight, FrameRect.m_dBottom);
    leftTop = (*m_mat) * leftTop;
    rightBottom = (*m_mat) * rightBottom;

    FrameRect.m_dLeft = leftTop.dx;
    FrameRect.m_dTop = leftTop.dy;
    FrameRect.m_dRight = rightBottom.dx;
    FrameRect.m_dBottom = rightBottom.dy;
    return FrameRect;

    //       const CGeoText* pConstTemp = static_cast<const CGeoText*>(GetChild());
    //       CGeoText* pTemp = const_cast<CGeoText* >(pConstTemp);
    //       CGeoPt pt = m_mat * pTemp->GetStartPt();

    //       FLOAT_T right  = pt.dx + pTemp->GetHeight() * pTemp->GetText().size();
    //       FLOAT_T bottom = pt.dy + pTemp->GetHeight();
    //       return CGeoRect(pt.dx , pt.dy, right,bottom);
}

//添加构元，成功返回true，失败返回false。
//如果添加的构元与保存的有同名的，则会失败
bool CGeoScene::AddCell(CGeoGDSStruct* pCell)
{
    if (0 == pCell)
        return false;

    GeoStructList::const_iterator pos =  m_vecGdsStructList.begin();
    while (pos != m_vecGdsStructList.end())
    {
        if (pos->get()->GetName() == pCell->GetName())
            return false;
        pos++;
    }

    m_vecGdsStructList.push_back(pCell);
    return true;
}

//设置构元，成功返回true，失败返回false。
//如果bAdd为false，则如果找不到同名的构元，则返回失败。true则会添加。
bool CGeoScene::SetCell(CGeoGDSStruct* pCell, bool bAdd)
{
    if (0 == pCell)
        return false;

    for(unsigned i=0; i<m_vecGdsStructList.size(); i++)
    {
        if (pCell->GetName() == m_vecGdsStructList[i]->GetName())
        {
            m_vecGdsStructList[i] = pCell;
            return true;
        }
    }
    
    if (bAdd)
    {
        m_vecGdsStructList.push_back(pCell);
        return true;
    }
    else
        return false;
}
