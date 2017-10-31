#ifndef GDS2WRITER
#define GDS2WRITER

#include "./File/gds2reader.h"
#include "./Draw/graphicsview.h"

// 每种记录的二进制数据表示
const short GDSHEADER       = 0x0002;
const short GDSBGNLIB       = 0x0102;
const short GDSLIBNAME      = 0x0206;
const short GDSUNITS        = 0x0305;
const short GDSENDLIB       = 0x0400;
const short GDSBGNSTR       = 0x0502;
const short GDSSTRNAME      = 0x0606;
const short GDSENDSTR       = 0x0700;
const short GDSBOUNDARY     = 0x0800;
const short GDSPATH         = 0x0900;
const short GDSSREF         = 0x0a00;
const short GDSAREF         = 0x0b00;
const short GDSTEXT         = 0x0c00;
const short GDSLAYER        = 0x0d02;
const short GDSDATATYPE     = 0x0e02;
const short GDSWIDTH        = 0x0f03;
const short GDSXY           = 0x1003;
const short GDSENDEL        = 0x1100;
const short GDSSNAME        = 0x1206;
const short GDSCOLROW       = 0x1302;
const short GDSTEXTNODE     = 0x1400;
const short GDSNODE         = 0x1500;
const short GDSTEXTTYPE     = 0x1602;
const short GDSPRESENTATION = 0x1701;
const short GDSSTRING       = 0x1906;
const short GDSSTRANS       = 0x1a01;
const short GDSMAG          = 0x1b05;
const short GDSANGLE        = 0x1c05;
const short GDSREFLIBS      = 0x1f06;
const short GDSFONTS        = 0x2006;
const short GDSPATHTYPE     = 0x2102;
const short GDSGENERATIONS  = 0x2202;
const short GDSATTRTABLE    = 0x2306;
const short GDSSTYPTABLE    = 0x2406;
const short GDSSTRTYPE      = 0x2502;
const short GDSELFLAGS      = 0x2601;
const short GDSELKEY        = 0x2703;
const short GDSNODETYPE     = 0x2a02;
const short GDSPROPATTR     = 0x2b02;
const short GDSPROPVALUE    = 0x2c06;
const short GDSBOX          = 0x2d00;
const short GDSBOXTYPE      = 0x2e02;
const short GDSPLEX         = 0x2f03;
const short GDSBGNEXTN      = 0x3003;
const short GDSENDEXTN      = 0x3103;
const short GDSTAPENUM      = 0x3202;
const short GDSTAPECODE     = 0x3302;
const short GDSSTRCLASS     = 0x3401;
const short GDSRESERVED     = 0x3503;
const short GDSFORMAT       = 0x3602;
const short GDSMASK         = 0x3706;
const short GDSENDMASKS     = 0x3800;
const short GDSLIBDIRSIZE   = 0x3902;
const short GDSSRFNAME      = 0x3a06;

// 因为还不知道如何获取同步的时间，先用固定的时间来代替
const short time_data[12]={98,2,18,14,33,0,98,2,18,14,33,0};

class gds2writer
{
public:
    gds2writer () {}

    ~gds2writer() {}

    void writeGDS(QString filename, COptimaskView *doc = NULL, CGDS *read = NULL);   // 写函数的入口，filename:存放文件的路径，read:打开文件的对象指针

    void write_record_size(short i);              // 写一个记录的长度到文件中
    void write_record(short i);                   // 写一个记录类型到文件中
    void write_short(short i);                    // 写一个short型数据写到文件中
    void write_int(int i);                        // 写一个int型数据写到文件中
    void write_time(const short *time);           // 写一个时间数据写到文件中,这个函数还没有写
    void write_byte(unsigned char b);             // 写一个字节的数据，一般作为实现write_string的中间函数
    void write_string(const char *str);           // 写一个字符串数据到文件中
    void write_string(const std::string &str);    // 写一个字符串数据到文件中
    void write_double(double d);                  // 写一个double型数据到文件中
    void write_string_record(short record, const std::string &name);// 将带有名字的record如LIBNAEM,STRNAEM写到文件中

    // 以下是将六种构元写到文件的实现函数，本来是7种，write_node没写
    void write_box(CGDSElement* element);
    void write_boundary(CGDSElement* element);
    void write_path(CGDSElement* element);
    void write_text(CGDSElement* element);
    void write_sref(CGDSElement* element);
    void write_aref(CGDSElement* element);

    void write_elflags_plex(CGDSElement* element);    // 写ELFLAGS PLEX到文件中。多次用到，单独写出来，以减少代码
    void write_strans_mag_angle(CGDSElement* element);// 写STRANS MAG ANGLE到文件中。多次用到，单独写出来，以减少代码
    void finish();                                    // 写ENDEL到文件中

private:
    QDataStream out;
};


#endif // GDS2WRITER

