#include <float.h>   //用于DBL_MAX
#include <QFile>
#include <QDataStream>
#include <iostream>


#include "gds2reader.h"

const int GDS_HEADER_LEN=4;                 //.gds文件的前四个字节。c++中尽量用const代替#define.

/*************CGDSElement中函数*******************/
CGDSElement::CGDSElement():m_uintLayer(0),m_uintElementType(0)//后者设置为0，代表不是7种element中的一种。
{
    ELFLAGS_flag = false;        // 标记是否有ELFLAGS这个记录
    PLEX_flag    = false;        // 标记是否有PLEX这个记录
    StransFlag   = false;        // 标记是否有STRANS这个记录
    MagFlag      = false;        // 标记是否有MAG这个记录
    AngleFlag    = false;        // 标记是否有ANGLE这个记录
    PathTypeFlag = false;        // 标记是否有ANGLE这个记录
    WidthFlag    = false;        // 标记是否有ANGLE这个记录
    BgnextnFlag  = false;        // 标记是否有ANGLE这个记录
    EdextnFlag   = false;        // 标记是否有ANGLE这个记录
    PresentationFlag = false;    // 标记是否有PRESENTATION这个记录
}

CGDSElement::~CGDSElement()
{
}

inline
void CGDSElement::SetLayer(unsigned int layer)
{
    if(/*layer < 0 ||*/ layer >= GDS_MAX_LAYER)
        return;
    m_uintLayer = layer;
    return;
}

inline
void CGDSElement::SetPlex(int plex)
{
    m_intPlex = plex;
}

inline
void CGDSElement::SetElflags(int elflags)
{
    m_shortElflags = elflags;
}


/*************CGDSBoundary中函数*******************/
void CGDSBoundary::SetDataType(int type)
{
    if(type < 0 || type > GDS_MAX_DATA_TYPE)
        return;
    m_uintDataType = type;
}


/*************CGDSPath中函数***********************/
void CGDSPath::SetDataType(int type)
{
    if(type < 0 || type > GDS_MAX_DATA_TYPE)
        return;
    m_uintDataType = type;
    return;
}

void CGDSPath::LoadCoords(CCoord<int>* list, int num)
{
    CGDSXY::LoadCoords(list, num);
}

void CGDSPath::SetPathType(int type)
{
    switch(type){
    case 0:
    case 1:
    case 2:
    case 4:
        m_uintPathType = type;
        return;
        break;
    default:
        break;
    }
    return;
}

void CGDSPath::SetBgnExtn(int len)
{
    m_intBgnExtn = len;
}

void CGDSPath::SetEndExtn(int len)
{
    m_intEndExtn = len;
}

void CGDSPath::SetWidth(int width)
{
    m_intWidth = width;
}


/*************CGDSText中函数***********************/
void CGDSText::SetWidth(int width)
{
    m_intWidth = width;
}

void CGDSText::SetPresentation(int bits)
{
    m_uintPresebtation = bits;
    m_uintFont     = (bits & 0x0030) >> 4;
    m_uintVertJust = (bits & 0x000c) >> 2;
    m_uintHorzJust = (bits & 0x0003);
}

void CGDSText::SetPathType(int type)
{
    switch(type){
    case 0:
    case 1:
    case 2:
    case 4:
        m_uintPathType = type;
        return;
        break;
    default:
        break;
    }
    return;
}


/*************CGDSBox中函数***********************/
void CGDSBox::SetBoxType(int type)
{
    if(type >= 0 && type <= GDS_MAX_DATA_TYPE){
        m_uintBoxType = type;
        return;
    }
    return;
}


/*************CGDSNode中函数***********************/
void CGDSNode::SetNodeType(int type)
{
    if(type >= 0 && type <= GDS_MAX_DATA_TYPE)
        m_uintNodeType = type;
}


/*************CGDSElements中函数*******************/
void CGDSElements::AddElement(int type)
{
    CGDSElement* p;

    switch(type){
    case GDS_BOUNDARY:
        p = new CGDSBoundary;
        p->SetElementType(GDS_BOUNDARY);
        break;
    case GDS_PATH:
        p = new CGDSPath;
        p->SetElementType(GDS_PATH);
        break;
    case GDS_SREF:
        p = new CGDSSREF;
        p->SetElementType(GDS_SREF);
        break;
    case GDS_AREF:
        p = new CGDSAREF;
        p->SetElementType(GDS_AREF);
        break;
    case GDS_TEXT:
        p = new CGDSText;
        p->SetElementType(GDS_TEXT);
        break;
    case GDS_NODE:
        p = new CGDSNode;
        p->SetElementType(GDS_NODE);
        break;
    case GDS_BOX:
        p = new CGDSBox;
        p->SetElementType(GDS_BOX);
        break;
    default:
        break;
    }
    m_obaryElements.push_back(p);
}

void CGDSElements::DeleteElement(CGDSElement* p)
{
    if(!p)
        return;


    for(QList<CGDSElement*>::iterator pos = m_obaryElements.begin();pos!=m_obaryElements.end();pos++)
    {
        if(*pos == p)
        {
            delete p;
            m_obaryElements.erase(pos);
            break;
        }
    }
}

CGDSElements::~CGDSElements()
{
    m_obaryElements.clear();
}


/*************CGDSStructure中函数*******************/
CGDSStructure::~CGDSStructure()
{
    if(m_strStrName)
        delete [] m_strStrName;

//    for(int pos = 0;pos < m_obaryRefStructures.GetSize();pos++){
//        CGDSRefStructure* ptr = (CGDSRefStructure*)m_obaryRefStructures.GetAt(pos);
//        delete ptr;
//    }
//    m_obaryRefStructures.RemoveAll();
}

void CGDSStructure::AddRefStructure(CGDSStructure* str)
{
    CGDSRefStructure* refstructure;
    QList<CGDSRefStructure*>::iterator iter;
    for(iter=m_obaryRefStructures.begin();iter!=m_obaryRefStructures.end();iter++)//如果链表中有str
    {
        refstructure=(*iter);
        if(refstructure->GetStructure() == str)
            return ;
    }

    refstructure = new CGDSRefStructure;
    refstructure->SetStructure(str);
    refstructure->SetName(str->GetStructureName());

    m_obaryRefStructures.push_back(refstructure);
}

// 找到structure的SREF,AREF，TEXT element，根据其SName找到对应的构元。计算得到structure的最大，最小坐标。
void CGDSStructure::CalcDimension(CGDSStructures* strlist, bool* broken_link/*, CString* err*/)
{
    CCoord<double> min, max;
    bool flg = false;            // 标记取最大，最小坐标的初始值
    CGDSElement* element;
    CGDSStructure* ref;

    QList<CGDSElement*>::iterator iter;
    for(iter=m_obaryElements.begin();iter!=m_obaryElements.end();iter++)
    {
        element = *iter;
        switch(element->GetElementType())
        {
        case GDS_SREF:
        case GDS_AREF:
            ref = strlist->FindStructure(element->GetSname());// 依靠名字找到对应的structure
            if(ref){
                element->SetRefStructure(ref);                // 设置为被引用构元
                AddRefStructure(ref);                         // 添加到被引用构元的链表中
            }
            else{
                *broken_link = true;
            }
            break;
        case GDS_TEXT:
            break;
        default:                       // 不为SREF,AREF,TEXT，算出以下
            GetElementMinMax(element, min, max);
            if(!flg){                  // m_coordDimMin，m_coordDimMax的初次赋值
                m_coordDimMin = min;
                m_coordDimMax = max;
                flg = true;
            }
            if(m_coordDimMin.x > min.x)// 取min.x，max.x中的小者
                m_coordDimMin.x = min.x;
            if(m_coordDimMin.x > max.x)
                m_coordDimMin.x = max.x;

            if(m_coordDimMax.x < max.x)// 取max.x，min.x中的大者
                m_coordDimMax.x = max.x;
            if(m_coordDimMax.x < min.x)
                m_coordDimMax.x = min.x;

            if(m_coordDimMin.y > min.y)
                m_coordDimMin.y = min.y;
            if(m_coordDimMin.y > max.y)
                m_coordDimMin.y = max.y;

            if(m_coordDimMax.y < max.y)
                m_coordDimMax.y = max.y;
            if(m_coordDimMax.y < min.y)
                m_coordDimMax.y = min.y;
            break;
        }
    }
//    cout<<"m_coordDimmin: "<<m_coordDimMin.x<<" "<<m_coordDimMin.y<<endl;
//    cout<<"m_coordDimMax: "<<m_coordDimMax.x<<" "<<m_coordDimMax.y<<endl;
}

// 获取element的最小x,y，最大x,y。min(min.x,min.y),max(max.x,max.y)
void CGDSStructure::GetElementMinMax(CGDSElement *element, CCoord<double>& min, CCoord<double>& max, double angle)
{
    CCoord<double> coord;
    if(angle < 1e-6){

        for(int i = 0;i < element->GetNumCoords();++i){
            element->GetNthCoord(i, &coord);
            if(i == 0){
                min = coord;
                max = coord;
            }
            else{
                if(coord.x < min.x)
                    min.x = coord.x;
                else if(coord.x > max.x)
                    max.x = coord.x;
                if(coord.y < min.y)
                    min.y = coord.y;
                else if(coord.y > max.y)
                    max.y = coord.y;
            }
        }
    }else{   // 旋转后的element的最大，最小点，但是自己写的还没用上
        std::vector<double> xvalue,yvalue;
        for(int i = 0;i < element->GetNumCoords();++i){
            element->GetNthCoord(i, &coord);
            coord.RotatePoint(angle);
            xvalue.push_back(coord.x);
            yvalue.push_back(coord.y);
        }

        std::vector<double>::iterator iter1 = std::min_element(xvalue.begin(),xvalue.end());
        min.x = *iter1;
        std::vector<double>::iterator iter2 = std::max_element(xvalue.begin(),xvalue.end());
        max.x = *iter2;
        std::vector<double>::iterator iter3 = std::min_element(yvalue.begin(),yvalue.end());
        min.y = *iter3;
        std::vector<double>::iterator iter4 = std::max_element(yvalue.begin(),yvalue.end());
        max.y = *iter4;
    }
}

/*************CGDSRefStructure中函数*******************/
CGDSRefStructure* CGDSStructure::FindRefStructure(CGDSStructure* structure)
{
    if(!structure)
        return NULL;

    CGDSRefStructure* str;

    QList<CGDSRefStructure*>::iterator iter;
    for(iter=m_obaryRefStructures.begin();iter!=m_obaryRefStructures.end();iter++)
    {
        str=(*iter);
        if(str&&str->GetStructure() == structure)
            return str;
    }
    return NULL;
}

CGDSRefStructure* CGDSStructure::FindRefStructure(string name)
{
        CGDSRefStructure* str;
        QList<CGDSRefStructure*>::iterator iter;
        for(iter=m_obaryRefStructures.begin();iter!=m_obaryRefStructures.end();iter++)
        {
            str=(*iter);
            if(str->GetName() == name)
                return str;
        }
        return NULL;
}



/*************CGDSStructures中函数*******************/
void CGDSStructures::AddStructure()
{
    m_structActive = new CGDSStructure;
    m_obaryStructures.push_back(m_structActive);
}

CGDSStructure* CGDSStructures::FindStructure(string name)
{
    if(name.size()==0)
        return NULL;
    QList<CGDSStructure*>::iterator iter;
    for(iter=m_obaryStructures.begin();iter!=m_obaryStructures.end();iter++)//如果链表中有str
    {
        CGDSStructure* ptr=(*iter);
        if(ptr->GetStructureName()== name)
            return ptr;
    }
    return NULL;
}

CGDSStructures::~CGDSStructures()
{
    m_obaryStructures.clear();
    delete m_structActive;
}

CGDSStructure *CGDSStructures::FindTopStructure()
{
    CGDSStructure* str_found = NULL;

    double s_max, s_now;
    CCoord<double> min_now, max_now;
    CGDSStructure* structure;
    QList<CGDSStructure*>::iterator iter;
    for(iter = m_obaryStructures.begin();iter != m_obaryStructures.end();++iter) {
        structure = *iter;
        if(structure == NULL) continue;
        if(!(structure->IsRefered())) {
            structure->GetDimMin(&min_now);
            structure->GetDimMax(&max_now);
            s_now = fabs((max_now.x - min_now.x) * (max_now.y - min_now.y));
            if(!str_found || s_now > s_max){
                str_found = structure;
                s_max = s_now;
            }
        }
    }
    return str_found;
}

// 找到被引用的构元，并标记此构元为被引用构元
void CGDSStructures::MakeCrossRef()
{
    CGDSStructure* structure=NULL;
    CGDSElement* element=NULL;

    QList<CGDSStructure*>::iterator iter1;
    QList<CGDSElement*>::iterator iter2;
    for(iter1=m_obaryStructures.begin();iter1!=m_obaryStructures.end();++iter1){
        structure = *iter1;
        for(iter2=(structure->m_obaryElements).begin();iter2!=(structure->m_obaryElements).end();++iter2){
            CGDSStructure* structure1=NULL;           //与原文件的这里不同
            element = *iter2;
            if(!element)
            {
                continue;
            }
            switch(element->GetElementType())
            {
            case GDS_SREF:
            case GDS_AREF:
                if((structure1 = FindStructure(element->GetSname())) == NULL)
                    break;
                structure1->MarkRefered();    // 标记找到的structure为被引用构元
                break;
            default:
                break;
            }
         }
    }
}


/*************CGDSFormatType中函数*******************/
void CGDSFormatType::AddFormatType(string str)
{
    CMask* p = new CMask;
    p->SetMaskName(str);
    m_obaryMasks.push_back(p);
}

CGDSFormatType::~CGDSFormatType()
{
    CMask* ptr;
    QList<CMask*>::iterator iter;

    for(iter=m_obaryMasks.begin();iter!=m_obaryMasks.end();iter++)
    {
        ptr = *iter;
        delete ptr;
    }
    m_obaryMasks.clear();
}


/*************CGDShcy中函数*******************/
CGDShcy::CGDShcy()
{
    m_uintStreamVersion = m_uintLibDirSize = 0;
    m_uintGenerations = 3;
    m_dblUserUnit = 0.0;
    m_dblDBUnit = 0.0;
}

CGDShcy::~CGDShcy()
{
}

// 设置所有structure的最大，最小坐标
bool CGDShcy::CalculateDimension()
{
    bool broken_link = false;
    std::vector<double> maxxvaluelist, maxyvaluelist, minxvaluelist,minyvaluelist;

    QList<CGDSStructure*>::iterator iter;
    for(iter=m_obaryStructures.begin();iter!=m_obaryStructures.end();iter++)
    {
        m_structActive = *iter;
        if(!m_structActive)
            continue;
        if((m_structActive->GetStructureName()).size()==0){
            m_obaryStructures.erase(iter);
            continue;
        }
        m_structActive->CalcDimension(this, &broken_link);
    }
    for(iter=m_obaryStructures.begin();iter!=m_obaryStructures.end();iter++)
    {
        m_structActive = *iter;
        if(!m_structActive)
            continue;
        if((m_structActive->GetStructureName()).size()==0){
            m_obaryStructures.erase(iter);
            continue;
        }
        CalcDimensionRecurse(m_structActive, CCoord<double>(0, 0), false, 0.0, 1.0);

        CCoord<double> maxcoord,mincoord;
        m_structActive->GetDimMax(&maxcoord);
        m_structActive->GetDimMin(&mincoord);
        maxxvaluelist.push_back(maxcoord.x);
        maxyvaluelist.push_back(maxcoord.y);
        minxvaluelist.push_back(mincoord.x);
        minyvaluelist.push_back(mincoord.y);
//        cout<<"max: "<<aa.x<<" "<<aa.y<<endl;
//        cout<<"min: "<<bb.x<<" "<<bb.y<<endl;
    }

   // double maxxvalue, maxyvalue, minxvalue,minyvalue;
    std::vector<double>::iterator iter1 = std::max_element(maxxvaluelist.begin(),maxxvaluelist.end());
    std::vector<double>::iterator iter2 = std::max_element(maxyvaluelist.begin(),maxyvaluelist.end());

    std::vector<double>::iterator iter3 = std::min_element(minxvaluelist.begin(),minxvaluelist.end());
    std::vector<double>::iterator iter4 = std::min_element(minyvaluelist.begin(),minyvaluelist.end());
    maxxvalue = *iter1;
    maxyvalue = *iter2;
    minxvalue = *iter3;
    minyvalue = *iter4;
//    cout<<"maxx = "<<maxxvalue<<" "<<"maxy = "<<maxyvalue<<endl;
//    cout<<"minx = "<<minxvalue<<" "<<"miny = "<<minyvalue<<endl;
    return true;
}

// 设置一个structure中最大，最小坐标 这个函数很耗时，需要重新修改
void CGDShcy::CalcDimensionRecurse(CGDSStructure* structure, CCoord<double> pos, bool reflect, double angle, double mag)
{
    CGDSStructure* refStructure;
    CCoord<double> coord;
    double eangle, emag;
    bool   ereflect;
    CGDSElement* element;

    if(!structure)
        return;
    QList<CGDSElement*>::iterator iter;

    for(iter=(structure->m_obaryElements).begin();iter!=((structure->m_obaryElements).end());++iter)
    {
        element = *iter;
        if(!element)
            continue;
        switch(element->GetElementType()){
        case GDS_SREF:
            refStructure = element->GetRefStructure();
            if(!refStructure)
                break;
            element->GetPos(&coord);        // 得到SREF中的偏移坐标
            if(reflect)
                coord.y *= -1;
            RotatePoint(&coord, angle);     // 坐标旋转
            coord += pos;                   // 得到新的偏移坐标

            ((CGDSSREF*)element)->GetMag(&emag);// 获取Mag
            emag *= mag;                        // 得到新的Mag
            ereflect = reflect ^ (((CGDSStrans*)element)->IsReflect());//按位异或，得到新的reflect
            if(ereflect != ((CGDSStrans*)element)->IsReflect())
                eangle = angle - ((CGDSSREF*)element)->GetAngle();
            else
                eangle = angle + ((CGDSSREF*)element)->GetAngle();
            fmod360(eangle);//反角度画成-360到360间
            CalcDimensionRecurse(refStructure, coord, ereflect, eangle, emag);
            break;
        case GDS_AREF:
            refStructure = element->GetRefStructure();
            if(!refStructure)                           // 判断跳出的条件
                break;
        {
            int numRow, numCol;
            CCoord<double> coordOrg, coordCol, coordRow;
            element->GetNthCoord(0, &coordOrg);
            coordOrg *= mag;
            if(reflect)
                coordOrg.y *= -1.0;
            element->GetNthCoord(1, &coordCol);
            coordCol *= mag;
            if(reflect)
                coordCol.y *= -1.0;

            element->GetNthCoord(2, &coordRow);
            coordRow *= mag;
            if(reflect)
                coordRow.y *= -1.0;

            numCol = ((CGDSAREF*)element)->GetNumCol();
            numRow = ((CGDSAREF*)element)->GetNumRow();

            ((CGDSAREF*)element)->GetMag(&emag);
            emag *= mag;
            ereflect = reflect ^ ((CGDSStrans*)element)->IsReflect();
            if(ereflect != ((CGDSStrans*)element)->IsReflect())
                eangle = angle - ((CGDSSREF*)element)->GetAngle();
            else
                eangle = angle + ((CGDSSREF*)element)->GetAngle();
            fmod360(eangle);

            double xstep,ystep;                    // 计算得到步进值
            coordCol -= coordOrg;
            coordRow -= coordOrg;
            if(eangle<1e-6){                        // 等于0时不需要旋转
                xstep = coordCol.x/(double)numCol;
                ystep = coordRow.y/(double)numRow;
            }else{
                coordCol.RotatePoint(-eangle);
                coordRow.RotatePoint(-eangle);
                xstep = coordCol.x/(double)numCol;
                ystep = coordRow.y/(double)numRow;
            }
            for(int i = 0;i < numRow;i += numRow > 1 ? numRow - 1 : 1){
                for(int j = 0;j < numCol;j += numCol > 1 ? numCol - 1 : 1){
                    //coord.SetCoord(coordOrg.x + j * xstep, coordOrg.y + i * ystep);
                    coord.SetCoord(j * xstep,i * ystep);
                    //RotatePoint(&coord, angle);         // 旋转的为上一次引用的偏移坐标
                    coord.RotatePoint(eangle);
                    coord += coordOrg;                  // dx,dy要加上源坐标
                    coord += pos;                       // 偏移坐标更新 这些仅仅得到是偏移坐标，而不真正的element的各个坐标。
                    CalcDimensionRecurse(refStructure, coord, ereflect, eangle, emag);
                }
            }
        }
            break;
        default: // 获取一个structure中最大，最小坐标
        {
            CCoord<double> cmin, cmax;
            m_structActive->SetVisibleOnLayer(element->GetLayer());
            structure->SetVisibleOnLayer(element->GetLayer());

            //以下一块是计算structure的最大，最小坐标
            QList<CGDSElement*>::iterator iter0;
            CCoord<double> coord1;int j=0;


            for(iter0 = structure->m_obaryElements.begin();iter0 != structure->m_obaryElements.end();++iter0,++j){
                std::vector<double> xvalue,yvalue;
                CGDSElement *element1 = *iter0;
                for(int i = 0;i < element1->GetNumCoords();++i){
                    element1->GetNthCoord(i, &coord1);
                    coord1.RotatePoint(angle);
                    xvalue.push_back(coord1.x);
                    yvalue.push_back(coord1.y);
                }

                std::vector<double>::iterator iter1 = std::min_element(xvalue.begin(),xvalue.end());
                std::vector<double>::iterator iter2 = std::max_element(xvalue.begin(),xvalue.end());
                std::vector<double>::iterator iter3 = std::min_element(yvalue.begin(),yvalue.end());
                std::vector<double>::iterator iter4 = std::max_element(yvalue.begin(),yvalue.end());
                if(j==0){
                    cmin.x = *iter1;
                    cmax.x = *iter2;
                    cmin.y = *iter3;
                    cmax.y = *iter4;
                }else{
                    if(cmin.x > (*iter1))
                        cmin.x = (*iter1);
                    if(cmax.x < (*iter2))
                        cmax.x = *iter2;
                    if(cmin.y > *iter3)
                        cmin.y = *iter3;
                    if(cmax.y < *iter4)
                        cmax.y = *iter4;
                }
            }
            //          structure->GetDimMin(&cmin);       // 获取一个不是引用structure的最小坐标点
            //          structure->GetDimMax(&cmax);       // 获取一个不是引用structure的最大坐标点
            coord.SetCoord(pos.x, pos.y);      // 将得到的四个偏移坐标放入coord坐标中
            if(reflect){
                cmin.y *= -1.0;
                cmax.y *= -1.0;
            }
            //          RotatePoint(&cmin, angle);
            //          RotatePoint(&cmax, angle);
            //          cmin.RotatePoint(angle);            //应该是所有点旋转后的最大，最小值，此处有误
            //          cmax.RotatePoint(angle);
            cmin *= mag;
            cmax *= mag;
            cmin += coord;
            cmax += coord;

            CCoord<double>* min = m_structActive->GetDimMinP();
            CCoord<double>* max = m_structActive->GetDimMaxP();
            if(*min == *max){ // not initialized yet
                *min = cmin;
                *max = cmax;
            }

            if(cmin.x < min->x)
                min->x = cmin.x;
            if(cmax.x < min->x)
                min->x = cmax.x;

            if(cmin.x > max->x)
                max->x = cmin.x;
            if(cmax.x > max->x)
                max->x = cmax.x;

            if(cmin.y < min->y)
                min->y = cmin.y;
            if(cmax.y < min->y)
                min->y = cmax.y;

            if(cmin.y > max->y)
                max->y = cmin.y;
            if(cmax.y > max->y)
                max->y = cmax.y;
        }
            break;
        }
    }
}

inline
void CGDShcy::fmod360(double& angle)
{
    if(angle >= 360.0)
        angle -= 360.0;
    else if(angle <= -360.0)
        angle += 360.0;
}

void CGDShcy::RotatePoint(CCoord<double>* point, double& angle)
{
    if(angle == 0.0 || (point->x == 0.0 && point->y == 0.0))
        return;

    if(angle == 90.0 || angle == -270.0){
        double t = point->x;
        point->x = -point->y;
        point->y = t;
    }
    else if(angle == 180.0 || angle == -180.0){
        point->x = -point->x;
        point->y = -point->y;
    }
    else if(angle == 270.0 || angle == -90.0){
        double t = point->x;
        point->x = point->y;
        point->y = -t;
    }
    else{
        double x = point->x;
        double y = point->y;
        double len = sqrt(x * x + y * y);
        double newang = atan2(y, x) + angle/180.0*PAI;
        point->SetCoord(len*cos(newang), len*sin(newang));
    }
    return;
}


/*************CGDS中函数*******************/
CGDS::CGDS()
{
    for(unsigned int i = 0;i < sizeof(m_bLayerList)/sizeof(bool);i++)
        *(m_bLayerList + i) = false;
    m_intPropAttr = -1;
    m_strPropVal = "";
    m_ulNumStructure = 0;
    m_ulNumXY = 0;
    m_ulNumXYCoord = 0;//整个文件中的坐标点数
    font       = false;
    FontsFlag  = false;
}

bool CGDS::DoesLayerExist(int layer)
{
    if(layer < 0 || layer >= GDS_MAX_LAYER)
        return false;
    if(*(m_bLayerList + layer) == true)
        return true;
    else
        return false;
}

void CGDS::AddLayer(int layer)
{
    if(layer < 0 || layer >= GDS_MAX_LAYER)
        return;
    *(m_bLayerList + layer) = true;
}


bool CGDS::ReadGDSPlain(QString filename)
{
    bool anyFault = false;
    m_intCoordIdx = 0;//初始为0
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    else{
        QDataStream in(&file);
        
        while(1)
        {
            unsigned int n=0;
            for(int i=0;i<GDS_HEADER_LEN;i++)
            {
                in>>m_ucharBuf[i];
                n++;
            }
            if(n<4)
                break;
            else
                n=0;
            m_uintRecordLength  = (((unsigned int) *(m_ucharBuf + 0)) << 8);
            m_uintRecordLength += (unsigned int) *(m_ucharBuf + 1);//record 的长度
            if(m_uintRecordLength == 0)
                break;
            m_uintRecordLength -= GDS_HEADER_LEN;//真正的数据长度

            m_uintRecordType = *(m_ucharBuf + 2);//记录类型在第三个字节
            m_uintDataType = *(m_ucharBuf + 3);//数据类型在第四个字节

            for(unsigned int i=0;i < m_uintRecordLength;i++)
            {
                in>>m_ucharBuf[i];//数据存放在m_ucharBuf
                n++;
            }
            if(n<m_uintRecordLength)
                break;
            if(DecodeRecord() == false){
                anyFault = true;
                break;
            }
        }       
    }
    file.close();

    MakeCrossRef();

    if(!CalculateDimension())
    {
        return false;
    }
    FindTopStructure();
    if(m_ulNumStructure == 0) // empty file may not be detected to be invalid
        return false;
    if(anyFault)
        return false;
    else
        return true;
}

double CGDS::DecodeReal()
{
    unsigned len;

    switch(m_uintDataType){
    case GDS_Real8:
        len = 8;
        break;
    case GDS_Real4:
        len = 4;
        break;
    default:
        return DBL_MAX;
        break;
    }
    if(m_uintRecordLength - m_uintBufPtr < len)
        return DBL_MAX;

    __int64 mantissa    = 0;
    char* ptr = (char*)&mantissa;

    int exp16 = *(m_ucharBuf + m_uintBufPtr++);
    for(unsigned i = 1;i < len;i++)
        *(ptr + len - i - 1) = *(m_ucharBuf + m_uintBufPtr++);

    if(exp16 >> 7)
        mantissa = -mantissa;
    exp16 &= 0x7f; // MSB==sign;
    exp16 -= 64;   // Excess-64 bias;
    exp16 *= 4;    // 16^N to 2^N

    exp16 -= (len - 1) * 8; // mantissa length : first==1/2
    return pow(2.0, exp16) * mantissa;
}

unsigned CGDS::DecodeBitArray()               //占两个字节
{
    if(m_uintRecordLength - m_uintBufPtr < 2)
        return UINT_MAX;

    return (*(m_ucharBuf + m_uintBufPtr) << 8)
                + *(m_ucharBuf + m_uintBufPtr + 1);
}

int CGDS::DecodeInteger()
{
    unsigned len;

    switch(m_uintDataType){
    case GDS_Integer4:
        len = 4;
        break;
    case GDS_Integer2:
        len = 2;
        break;
    default:
        return 0;
        break;
    }
    if(m_uintRecordLength - m_uintBufPtr < len)
        return 0;

    __int32 ret_val = 0;
    qint8* ptr = (qint8*)&ret_val;

    for(unsigned i = 0;i < len;i++)
        *(ptr + len - i - 1) = *(m_ucharBuf + m_uintBufPtr++);


    return (int)ret_val;
}

bool CGDS::DecodeAscii(string* set_to, unsigned len)
{
    char *cptr = m_charBuf;

    if(len == 0)
        len = m_uintRecordLength;
    else if(m_uintRecordLength - m_uintBufPtr < len)
        return false;

    for(unsigned i = 0;i < len;i++){
        if(*(m_ucharBuf + m_uintBufPtr) == 0x00)
            break;
        *cptr++ = *(m_ucharBuf + m_uintBufPtr++);

    }
    if(font){
        for(unsigned i = 0;i < len;i++){
            font_name.push_back(m_charBuf[i]);
        }
    }


    *cptr = '\0';
    if(set_to)
        *set_to = m_charBuf;

    return true;
}

bool CGDS::DecodeDateTime(struct tm *tm_set)
{
    //const unsigned length = 12;

//    if(m_uintRecordLength - m_uintBufPtr < 0)
//        return false;

    tm_set->tm_year  = DecodeInteger();
    if(tm_set->tm_year > 1970) // if incorrectly encoded
        tm_set->tm_year -= 1900;
    if(tm_set->tm_year < 70)   // if incorrectly encoded
        tm_set->tm_year += 100;

    tm_set->tm_mon   = DecodeInteger() - 1;
    if(tm_set->tm_mon < 0)
        tm_set->tm_mon = 0;
    if(tm_set->tm_mon > 11)
        tm_set->tm_mon = 11;

    tm_set->tm_mday  = DecodeInteger();
    if(tm_set->tm_mday < 0)
        tm_set->tm_mday = 0;
    if(tm_set->tm_mday > 31)
        tm_set->tm_mday = 31;

    tm_set->tm_hour  = DecodeInteger();
    if(tm_set->tm_hour < 0)
        tm_set->tm_hour = 0;
    if(tm_set->tm_hour > 23)
        tm_set->tm_hour = 23;

    tm_set->tm_min   = DecodeInteger();
    if(tm_set->tm_min < 0)
        tm_set->tm_min = 0;
    if(tm_set->tm_min > 59)
        tm_set->tm_min = 59;

    tm_set->tm_sec   = DecodeInteger();
    if(tm_set->tm_sec < 0)
        tm_set->tm_sec = 0;
    if(tm_set->tm_sec > 59)
        tm_set->tm_sec = 59;

    tm_set->tm_isdst = 0;
    tm_set->tm_wday  = 0;
    tm_set->tm_yday  = 0;
    return true;
}

bool CGDS::ValidateXY(int elementtype, int num_xy)//确认XY的坐标有效
{
    switch(elementtype){
        case GDS_PATH://Path至少有两个点
        if(num_xy >= 2 && num_xy <= GDS_MAX_COORD)
            return true;
        break;
        case GDS_BOUNDARY://Boundary至少有四个点
        if(num_xy >= 4 && num_xy <= GDS_MAX_COORD){
            if(*(m_aryCoordBuf + 0) == *(m_aryCoordBuf + num_xy - 1))
                m_intCoordIdx--;// reduce memory usage
            return true;
        }
        break;
    case GDS_TEXT:
    case GDS_CONTACT:
        case GDS_SREF://Text,Contact,Sref为一个点
        if(num_xy == 1)
            return true;
        break;
        case GDS_NODE://Node的点数是1到50
        if(num_xy >= 1 && num_xy <= 50)
            return true;
        break;
        case GDS_BOX://Box为5个点
        if(num_xy == 5){
            if(*(m_aryCoordBuf + 0) == *(m_aryCoordBuf + num_xy - 1)){
                m_intCoordIdx--;// reduce memory usage 头一个坐标和尾一个坐标相等，为减少内存的使用，就不用存储最后一个坐标
                return true;
            }
        }
        break;
        case GDS_AREF://陈列引用Aref为3个
        if(num_xy == 3)
            return true;
        break;
    default:
        break;
    }

    return false;
}

bool CGDS::DecodeRecord()
{
    long int     int_val;
    double       double_val;
    CCoord<int>      coord;



    CGDSStructure* structure = GetActiveStructure(); //获得一个structure
    CGDSElement* element = NULL;
    if(structure)
        element = structure->GetLastElement();//当前的element

    m_uintBufPtr = 0;

    switch(m_uintRecordType){
    case GDS_XY :
        if(element == NULL)
            break;
        for(int_val = 0;(unsigned)int_val < m_uintRecordLength/8;int_val++){//x,y两个值共8个字节
            coord.x = DecodeInteger(); // 4byte integer
            coord.y = DecodeInteger();
            //从第三个坐标开始检查是否与前一个坐标相等，若相等，则删除
            if(m_intCoordIdx > 1 && (m_aryCoordBuf + m_intCoordIdx - 1)->Equals(coord))//m_intCoordIdx 初始为0在read中设置
                continue; // remove redundant coordinate 删除多余的坐标
            (m_aryCoordBuf + m_intCoordIdx++)->SetCoord(coord);//将坐标添加到m_aryCoordBuf中
        }
        if(m_intCoordIdx > 0 && ValidateXY(element->GetElementType(), m_intCoordIdx)){
            ((CGDSXY*)element)->LoadCoords(m_aryCoordBuf, m_intCoordIdx);
            m_ulNumXYCoord += m_intCoordIdx;//所有的坐标数目，而不是一个element的坐标数目
            //在结束元素的时候m_intCoordIdx重新设置为0
            m_ulNumXY++; //作用是什么？好像没什么用
        }       
        break;

    case GDS_DATATYPE :
        if(element == NULL)
            break;
        int_val = DecodeInteger();
        element->SetDataType(int_val);
        break;

    case GDS_LAYER :
        if(element == NULL)
            break;
        int_val = DecodeInteger();
        element->SetLayer(int_val);
        AddLayer(int_val);
        break;

    case GDS_PLEX :
        if(element == NULL)
            break;
        element->SetPLEX_flag(true);
        int_val = DecodeInteger();
        element->SetPlex(int_val);
        break;

    case GDS_ELFLAGS :
        if(element == NULL)
            break;
        element->SetELFLAGS_flag(true);
        int_val = DecodeBitArray();
        element->SetElflags(int_val);
        break;

    case GDS_WIDTH :
        if(element == NULL)
            break;
        element->SetWidthFlag(true);
        int_val = DecodeInteger();
        element->SetWidth(int_val);

        break;

    case GDS_SNAME :
        if(element == NULL)
            break;
        DecodeAscii(NULL);
        element->SetSname(m_charBuf);        
        break;

    case GDS_COLROW :
        if(element == NULL)
            break;
        coord.x = DecodeInteger();
        coord.y = DecodeInteger();
        element->SetColRow(coord.x, coord.y);
        break;

    case GDS_TEXTTYPE :
        if(element == NULL)
            break;
        int_val = DecodeInteger();
        element->SetTextType(int_val);
        break;

    case GDS_PRESENTATION :
        if(element == NULL)
            break;
        element->SetPresentationFlag(true);
        int_val = DecodeBitArray();
        element->SetPresentation(int_val);
        break;

    case GDS_STRING :
        if(element == NULL)
            break;
        DecodeAscii(NULL);
        element->SetString(m_charBuf);
        break;

    case GDS_STRANS :
        if(element == NULL)
            break;
        element->SetStransFlag(true);
        int_val = DecodeBitArray();
        ((CGDSStrans*)element)->SetStrans(int_val);
        break;

    case GDS_MAG :
        if(element == NULL)
            break;
        element->SetMagFlag(true);
        double_val = DecodeReal();
        element->SetMag(double_val);
        break;

    case GDS_ANGLE :
        if(element == NULL)
            break;
        element->SetAngleFlag(true);
        double_val = DecodeReal();
        element->SetAngle(double_val);
        break;

    case GDS_PATHTYPE :
        if(element == NULL)
            break;
        element->SetPathTypeFlag(true);
        int_val = DecodeInteger();
        element->SetPathType(int_val);
        break;

    case GDS_NODETYPE :
        if(element == NULL)
            break;
        int_val = DecodeInteger();
        element->SetNodeType(int_val);
        break;

    case GDS_BOXTYPE :
        if(element == NULL)
            break;
        int_val = DecodeInteger();
        element->SetBoxType(int_val);
        break;

    case GDS_BGNEXTN :
        if(element == NULL)
            break;
        element->SetBgnExtn(true);
        int_val = DecodeInteger();
        element->SetBgnExtn(int_val);
        break;

    case GDS_ENDEXTN :
        if(element == NULL)
            break;
        element->SetEdextnFlag(true);
        int_val = DecodeInteger();
        element->SetEndExtn(int_val);
        break;

    ////////// LIBRARY
    case GDS_HEADER :
        m_uintStreamVersion = DecodeInteger();
        break;

    case GDS_BGNLIB :
        DecodeDateTime(&m_tmCreationDate);
        DecodeDateTime(&m_tmLastAccessDate);
        break;

    case GDS_LIBDIRSIZE :
        break;

    case GDS_SRFNAME :
        DecodeAscii(&m_strSrfName);
        break;

    case GDS_LIBSECUR :
        break;

    case GDS_LIBNAME :
        DecodeAscii(&m_strLibName);
        break;

    case GDS_REFLIBS :
        break;

    case GDS_FONTS :
        font = true;
        SetFontsFlag(true);
        DecodeAscii(&m_strFonts);
        font = false;
        break;

    case GDS_ATTRTABLE :
        DecodeAscii(&m_strAttrTable);
        break;

    case GDS_GENERATIONS :
        int_val = DecodeInteger();
        m_uintGenerations = int_val;
        break;

    case GDS_UNITS :
        double_val = DecodeReal();
        m_dblUserUnit = double_val;
        double_val = DecodeReal();
        m_dblDBUnit = double_val;
        break;

    case GDS_ENDLIB : // End of library
        break;

    ////////// FORMAT TYPE
    case GDS_FORMAT :
        break;

    case GDS_MASK :
        break;

    case GDS_ENDMASKS :
        break;

    ////////// Structure
    case GDS_BGNSTR :
        AddStructure();
        m_ulNumStructure++;

        //DecodeDateTime(structure->GetCreateDate());
        //DecodeDateTime(structure->GetModifyDate());
        break;

    case GDS_STRNAME :
        DecodeAscii(NULL);
        if(structure != NULL)
            structure->SetStructureName(m_charBuf);
        break;

    case GDS_STRCLASS :
        // Cadence use only
        int_val = DecodeBitArray();
        break;

    case GDS_ENDSTR :
        ClearActiveStructure();
        break;


    ////////// ELEMENT
    case GDS_BOUNDARY :
        m_intPropAttr = -1;
        m_strPropVal = "";
        if(structure == NULL)
            break;
        structure->AddElement(GDS_BOUNDARY);
        element = structure->GetLastElement();
        break;

    case GDS_PATH :
        m_intPropAttr = -1;
        m_strPropVal = "";
        if(structure == NULL)
            break;
        structure->AddElement(GDS_PATH);
        element = structure->GetLastElement();
        break;

    case GDS_SREF :
        m_intPropAttr = -1;
        m_strPropVal = "";
        if(structure == NULL)
            break;
        structure->AddElement(GDS_SREF);
        element = structure->GetLastElement();
        break;

    case GDS_AREF :
        m_intPropAttr = -1;
        m_strPropVal = "";
        if(structure == NULL)
            break;
        structure->AddElement(GDS_AREF);
        element = structure->GetLastElement();
        break;

    case GDS_TEXT :
        m_intPropAttr = -1;
        m_strPropVal = "";
        if(structure == NULL)
            break;
        structure->AddElement(GDS_TEXT);
        element = structure->GetLastElement();
        break;

    case GDS_NODE :
        m_intPropAttr = -1;
        m_strPropVal = "";
        if(structure == NULL)
            break;
        structure->AddElement(GDS_NODE);
        element = structure->GetLastElement();
        break;

    case GDS_BOX :
        m_intPropAttr = -1;
        m_strPropVal = "";
        if(structure == NULL)
            break;
        structure->AddElement(GDS_BOX);
        element = structure->GetLastElement();
        break;

    case GDS_ENDEL :
        m_intCoordIdx = 0;
        m_intPropAttr = -1;
        m_strPropVal = "";
        if(structure == NULL)
            break;
        break;


    ////////// ELEMENT PROPERTY
    case GDS_PROPATTR :
        if(element == NULL)
            break;
        m_intPropAttr = DecodeInteger();
        break;

    case GDS_PROPVALUE :
        if(element == NULL)
            break;
        DecodeAscii(NULL);
        m_strPropVal = m_charBuf;
        if(m_intPropAttr >= 0){
           // element->AddProperty(m_intPropAttr, m_strPropVal);
            m_intPropAttr = -1;
            m_strPropVal = "";
        }
        break;

    ////////// UNUSED LEVEL 0
    case GDS_BORDER :
    case GDS_SOFTFENCE :
    case GDS_HARDFENCE :
    case GDS_SOFTWIRE :
    case GDS_HARDWIRE :
    case GDS_PATHPORT :
    case GDS_NODEPORT :
    case GDS_USERCONSTRAINT :
    case GDS_SPACER_ERROR :
    case GDS_CONTACT :
        break;

    ////////// UNUSED LEVEL 1
    case GDS_TAPENUM :
    case GDS_TAPECODE :
    case GDS_RESERVED :
        break;

    ////////// UNUSED LEVEL 2
    case GDS_UINTEGER :// Not currently used
    case GDS_USTRING :// Not currently used
    case GDS_SPACING :// Not currently used
    case GDS_TEXTNODE :// Text node is not supported in GDSII
    case GDS_STYPTABLE :// Unreleased feature
    case GDS_STRTYPE :// Unreleased feature
    case GDS_ELKEY :// Unreleased feature
    case GDS_LINKTYPE :// Unreleased feature
    case GDS_LINKKEYS :// Unreleased feature
        break;


    default:
        return false;
        break;
    }

    return true;
}






