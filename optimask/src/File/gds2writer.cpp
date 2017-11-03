#include <QFile>
#include <QDataStream>
#include <stdint.h>
#include <iostream>
#include "./File/gds2writer.h"
#include <QDebug>

void gds2writer::writeGDS(QString filename, COptimaskView *doc, CGDS *read)
{
    QFile file(filename);
    file.open(QIODevice::WriteOnly|QIODevice::Truncate);
    out.setDevice(&file);   // 打开设备

    // HEADER
    write_record_size(6);
    write_record(GDSHEADER);
    write_short(read->GetStreamVersion());

    // BGNLIB
    write_record_size (4 + 12 * 2);
    write_record (GDSBGNLIB);
    write_time (time_data);
    write_time (time_data);

    // LIBNAME
    write_string_record(GDSLIBNAME,read->GetLibName());

    // FONTS 要先判断FONTS记录存不存在
    if (read->GetFontsFlag()){
       write_string_record(GDSFONTS,read->GetFontName());
    }

    // UNITS
    write_record_size (4 + 8 * 2);
    write_record(GDSUNITS);
    write_double(read->GetUserUnit());
    write_double(read->GetDBUnit());

    // 迭代每一个构元
    QList<CGDSStructure*>::iterator iter_str;
    for(iter_str=(read->m_obaryStructures).begin();iter_str!=(read->m_obaryStructures).end();++iter_str)
    {
        CGDSStructure *structure=*iter_str;

        // BGNSTR
        write_record_size(4+12*2);
        write_record(GDSBGNSTR);
        write_time (time_data);//时间现在写的还不是时时更新
        write_time (time_data);

        // STRNAME
        write_string_record(GDSSTRNAME,structure->GetStructureName());

        // 迭代每一个构元中的每个element
        QList<CGDSElement*>::iterator iter;
        for(iter=(structure->m_obaryElements).begin();iter!=(structure->m_obaryElements).end();iter++)
        {
            CGDSElement *element=*iter;
            switch(element->GetElementType()){  //element类型匹配
            case GDS_SREF:
                write_sref(element);
                break;
            case GDS_AREF:
                write_aref(element);
                break;
            case GDS_PATH:
                write_path(element);
                break;
            case GDS_BOX:
                write_box(element);
                break;
            case GDS_BOUNDARY:
                write_boundary(element);
                break;
            case GDS_TEXT:
                write_text(element);
                break;
            case GDS_NODE:
                break;
            default:
                break;
            }
        }
       // ENDSTR
        write_record_size(4);
        write_record(GDSENDSTR);
    }

    if(doc->shapeStructureList1.size()!=0){
        QList<CGDSElement*>::iterator iter;
        for(iter=doc->shapeStructureList1.begin();iter!=doc->shapeStructureList1.end();++iter){
            // BGNSTR
            write_record_size(4+12*2);
            write_record(GDSBGNSTR);
            write_time (time_data);//时间现在写的还不是时时更新
            write_time (time_data);
            // STRNAME
            write_string_record(GDSSTRNAME,"box");

            write_box(*iter);

            // ENDSTR
             write_record_size(4);
             write_record(GDSENDSTR);
        }
    }

    // ENDLIB
    write_record_size(4);
    write_record(GDSENDLIB);



    //关闭设备
    file.close();
}

inline
void gds2writer::write_record_size(short i)
{
    out<<i;
}

inline
void gds2writer::write_record(short i)
{
    out<<i;
}

inline
void gds2writer::write_short(short i)
{
    out<<i;
}

inline
void gds2writer::write_int(int i)
{
    out<<i;
}

inline
void gds2writer::write_time(const short *time)
{
    for (unsigned int i = 0; i < 6; ++i) {
      write_short (time [i]);
    }
}

inline
void gds2writer::write_byte(unsigned char b)
{
    out<<b;
}

inline
void gds2writer::write_string(const char *str)
{
    size_t len = strlen(str);
    for(size_t i=0;i<len;i++){
        write_byte(*(str+i));
    }
    if((len&1)!=0){    //如果str所含有的有效字符是奇数，则添加一个形成偶数
        write_byte(0);
    }
}

inline
void gds2writer::write_string(const std::string &str)
{
    size_t len = str.size();
    for(size_t i=0;i<len;i++){
        write_byte(str[i]);
    }
    if((len&1)!=0){//如果str所含有的有效字符是奇数，则添加一个形成偶数
        write_byte(0);
    }
}

void gds2writer::write_double(double d)
{
    char b[8];

    b[0] = 0;
    if (d < 0) {
      b[0] = char (0x80);
      d = -d;
    }

    //  compute the next power of 16 that that value will fit in
    int e = 0;
    if (d < 1e-77 /*~16^-64*/) {
      d = 0;
    } else {
      double lg16 = log (d) / log (16.0);
      e = int (ceil (log (d) / log (16.0)));
      if (e == lg16) {
        ++e;
      }
    }

    d /= pow (16.0, e - 14);

    //tl_assert (e >= -64 && e < 64);
    b[0] |= ((e + 64) & 0x7f);

    uint64_t m = uint64_t (d + 0.5);
    for (int i = 7; i > 0; --i) {
      b[i] = (m & 0xff);
      m >>= 8;
    }
    for(int i=0;i<8;i++){
        write_byte(b[i]);
    }

}

inline
void gds2writer::write_string_record(short record, const string &name)
{
    write_record_size (4 + (short (name.size () + 1) / 2) * 2);
    write_record (record);
    write_string (name);
}

void gds2writer::write_box(CGDSElement* element)
{
    // BOX
    write_record_size (4);
    write_record (GDSBOX);

    // ELFLAGS AND PLEX
    write_elflags_plex(element);

    // LAYER
    write_record_size (4 + 2);
    write_record (GDSLAYER);
    write_short (element->GetLayer());

    // DATATYPE
    write_record_size(4 + 2);
    write_record(GDSDATATYPE);
    write_short(element->GetDataType());

    // XY
    write_record_size(4 + 5 * 2 * 4);
    write_record(GDSXY);
    for(unsigned int i=0;i<element->GetNumCoords();++i){// 因为为了节省空间，在存的时候没有存入最后一个点

        write_int((element->GetCoordArray())[i].x);
        write_int((element->GetCoordArray())[i].y);
    }
    write_int((element->GetCoordArray())->x);           // 添加上最后一个点
    write_int((element->GetCoordArray())->y);

    // ENDEL
    finish();
}

void gds2writer::write_boundary(CGDSElement *element)
{
    //GDSBOUNDARY
    write_record_size(4);
    write_record(GDSBOUNDARY);

    // ELFLAGS AND PLEX
    write_elflags_plex(element);

    // LAYER
    write_record_size(4 + 2);
    write_record(GDSLAYER);
    write_short(element->GetLayer());

    // DATATYPE
    write_record_size(4 + 2);
    write_record(GDSDATATYPE);
    write_short(element->GetDataType());

    // XY
    write_record_size(4 + int16_t(element->GetNumCoords()+1) * 2 * 4);
    write_record(GDSXY);
    for(unsigned int i=0;i<element->GetNumCoords();++i){

        write_int((element->GetCoordArray())[i].x);
        write_int((element->GetCoordArray())[i].y);
    }
    write_int((element->GetCoordArray())->x);
    write_int((element->GetCoordArray())->y);

    // ENDEL
    finish();
}

void gds2writer::write_path(CGDSElement *element)
{
    // PATH
    write_record_size(4);
    write_record(GDSPATH);

    // ELFLAGS AND PLEX
    write_elflags_plex(element);

    // LAYER
    write_record_size (4 + 2);
    write_record (GDSLAYER);
    write_short (element->GetLayer());

    // DATATYPE
    write_record_size (4 + 2);
    write_record (GDSDATATYPE);
    write_short (element->GetDataType());

    // PATHTYPE
    if(element->GetPathTypeFlag()) {
        write_record_size (4 + 2);
        write_record(GDSPATHTYPE);
        write_short(element->GetPathType());
    }

    // WIDTH
    if(element->GetWidthFlag()) {
        write_record_size (4 + 4);
        write_record (GDSWIDTH);
        write_int(element->GetWidth());
    }

    // 如果PATHTYPE的类型为4时
    if (element->GetPathType() == 4) {
        // BGNEXTN
        if(element->GetBgnextnFlag()) {
            write_record_size(4 + 4);
            write_record(GDSBGNEXTN);
            write_int(element->GetBgnExtn());
        }

        // ENDEXTN
        if(element->GetEdextnFlag()) {
            write_record_size(4 + 4);
            write_record(GDSENDEXTN);
            write_int(element->GetEndExtn());
        }
    }

    // GDSXY
    write_record_size (4 + int16_t(element->GetNumCoords()+1) * 2 * 4);
    write_record (GDSXY);
    for(unsigned int i=0;i<element->GetNumCoords();++i){
        write_int((element->GetCoordArray())[i].x);
        write_int((element->GetCoordArray())[i].y);
    }

    //ENDEL
    finish();
}

void gds2writer::write_text (CGDSElement *element)
{
    // TEXT
    write_record_size (4);
    write_record (GDSTEXT);

    // ELFLAGS AND PLEX
    write_elflags_plex (element);

    // LAYER
    write_record_size (4 + 2);
    write_record (GDSLAYER);
    write_short (element->GetLayer());

    // TEXTTYPE
    write_record_size (4 + 2);
    write_record (GDSTEXTTYPE);
    write_short (dynamic_cast<CGDSText*>(element)->GetTextType());

    // PRESENTATION 
    if (element->GetPresentationFlag()) {
        write_record_size (4 + 2);
        write_record (GDSPRESENTATION);
        write_short (dynamic_cast<CGDSText*>(element)->GetPresentation());
    }

    // PATHTYPE
    if(element->GetPathTypeFlag()) {
        write_record_size (4 + 2);
        write_record(GDSPATHTYPE);
        write_short(element->GetPathType());
    }

    // WIDTH
    if(element->GetWidthFlag()) {
        write_record_size (4 + 4);
        write_record (GDSWIDTH);
        write_int(element->GetWidth());
    }

    // STRANS MAG ANGLE
    write_strans_mag_angle(element);

    // XY
    write_record_size (4 + 2 * 4);
    write_record (GDSXY);
    write_int (element->GetPos().x);
    write_int (element->GetPos().y);

    // STRING
    write_string_record (GDSSTRING, dynamic_cast<CGDSText*>(element)->GetString());

    // ENDEL
    finish ();
}

void gds2writer::write_sref(CGDSElement *element)
{
    // SREF
    write_record_size(4);
    write_record(GDSSREF);

    // ELFLAGS AND PLEX
    write_elflags_plex (element);

    //SNAME
    write_string_record(GDSSNAME,element->GetSname());

    // STRANS MAG ANGLE
    write_strans_mag_angle(element);

    // XY
    write_record_size(4+2*4);
    write_record(GDSXY);
    write_int(element->GetPos().x);
    write_int(element->GetPos().y);

    //ENDEL
    finish();
}

void gds2writer::write_aref(CGDSElement *element)
{
    // AREF
    write_record_size(4);
    write_record (GDSAREF);

    // ELFLAGS PLEX
    write_elflags_plex(element);

    // SNAME
    write_string_record (GDSSNAME,element->GetSname());

    // STRANS MAG ANGLE
    write_strans_mag_angle(element);

    // GDSCOLROW
    write_record_size (4 + 2 * 2);
    write_record (GDSCOLROW);
    if (dynamic_cast<CGDSAREF*>(element)->GetNumCol() > 32767 || dynamic_cast<CGDSAREF*>(element)->GetNumRow() > 32767) {
        cout<<"Cannot write array references with more than 32767 columns or rows to GDS2 streams"<<endl;
    }
    write_short (dynamic_cast<CGDSAREF*>(element)->GetNumCol()); // 列 尽量不要用向下转型
    write_short (dynamic_cast<CGDSAREF*>(element)->GetNumRow()); // 行 尽量不要用向下转型

    // XY
    write_record_size (4 + 3 * 2 * 4);
    write_record (GDSXY);
    for(unsigned int i=0;i<element->GetNumCoords();++i){

        write_int((element->GetCoordArray())[i].x);
        write_int((element->GetCoordArray())[i].y);
    }

    // ENDEL
    finish ();
}

// 写ELFLAGS PLEX
void gds2writer::write_elflags_plex(CGDSElement *element)
{
    // ELFLAGS 要先判断有没有
    if(element->GetELFLAGS_flag()){
        write_record_size (4 + 2);
        write_record (GDSELFLAGS);
        write_short (element->GetElflags());
    }
    // PLEX 要先判断有没有
    if(element->GetPLEX_flag()){
        write_record_size (4 + 2);
        write_record (GDSPLEX);
        write_short (element->GetPlex());
    }
}

// 写STRANS MAG ANGLE
void gds2writer::write_strans_mag_angle(CGDSElement *element)
{
    // STRANS 要先判断有没有
    if(element->GetStransFlag()) {
        write_record_size(6);
        write_record (GDSSTRANS);
        write_short (element->GetStrans());

        // MAG 要先判断有没有
        if(element->GetMagFlag()) {
            write_record_size (4 + 8);
            write_record (GDSMAG);
            write_double (element->GetMag());
        }
        // ANGLE 要先判断有没有
        if(element->GetAngleFlag()) {
            write_record_size (4 + 8);
            write_record (GDSANGLE);
            write_double (element->GetAngle());
        }
    }
}


// ENDEL
inline
void gds2writer::finish()
{
    write_record_size(4);
    write_record(GDSENDEL);
}
