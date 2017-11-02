#include "qlayer.h"

QString qLayer_num()
{
    int array[128];
    int array_x[64];
    int array_y[64];
    //QString strname;
    QString Layer_num;

    unsigned char no_byte_array[2];
    int no_read;
    int no_bytes;
    unsigned char *record;
    char indent_string[128];
    int record_type;
    int data_type;
    int expected_data_type;
    char ascii_record_type[128];
    char ascii_data_type[128];
    char ascii_record_description[128];
    char ascii_data_type_description[128];
    char ascii_expected_data_type[128];
    char ascii_expected_data_type_description[128];
    int ktr;
    int int_ktr;
    int data_ktr;
    int exponent_ktr;
    int corrupt_ktr;
    unsigned int display_integer;
    unsigned int hex_display_integer;
    char display_char_1;
    char display_char_2;
    int bit_array;
    int real_sign;
    int real_exponent;
    unsigned long long real_mantissa_int;
    double real_mantissa_float;
    double display_float;

    /* start out with no indent */
    strcpy (indent_string, "");//开始没有缩进


    int f1;
    f1=open("test_file.sf",O_RDWR | O_CREAT);

   while (1)
   {

       /* read 2 bytes, this will be two bytes telling how many bytes of
       * data are in this record (including these two bytes) */
       no_read = read (f1, no_byte_array, 2);//读取2个字节到no_byte_array数组中，no_read为实际写入到no_byte_array的字节数。

       /* if we can't read 2 bytes, we must be at the end of the file. If we
        * just get one byte and it's not  padding (a 0), something is wrong
        * with the file structure... */
       if (no_read != 2)
       {
           printf ("# ***WHOOPS*** We seem to have reached the end of the file.\n");
           if ((no_read == 1) && (no_byte_array[0] != 0))
           {
               printf ("# ***ERROR*** We read a single non-zero byte after the last record.\n");
               printf ("#             I'm suspecting that this isn't padding...\n");
           }
           break; /* this is how the loop exits on normal execution */
       }

       /* store the number of bytes as an integer */
       no_bytes = no_byte_array[0] * 256 + no_byte_array[1];//总的字节数，no_byte_array[0]为两字节的高位。

       /* we could be into the padding region at the end of the file... */
       if (no_bytes != 0)
       {
       /* make room for the rest of the record */
       record = (unsigned char *) malloc (no_bytes - 2);//分配no_bytes - 2个字节的空间。record为指向字符的指针

       /* read in the record */
       no_read = read (f1, record, no_bytes - 2);//读取no_bytes - 2个数据，存储在record所指向的空间

       /* we should always get all of the bytes we asked for */
       if (no_read != no_bytes - 2)   //no_read为所读取的字节数，no_bytes是总的字节数。
       {
           /* Tell the user that there is a problem */
           printf ("# ***ERROR*** Couldn't read all of record.\n");
           printf ("#             It should have had %d bytes, could only read %d of them.\n",
                   no_bytes, no_read + 2);
           printf ("#             This is a corrupt file...\n");

           /* dump what we got in raw form */
           for (corrupt_ktr = 0; corrupt_ktr < no_read; corrupt_ktr++)
               {
                   printf ("0x%02x     # RAW\n", record[corrupt_ktr]);//%02X表示输出的16进制使用两个位置，如果只有一位的前面添0，比如15就输出0F
               }

           break;
        }

       /* now find the record type, numeric and ascii */
       record_type = record[0];
       find_ascii_record_type (record_type, ascii_record_type, ascii_record_description,&expected_data_type);

       /* find the data type, numeric and ascii */
       data_type = record[1];
       find_ascii_data_type (data_type, ascii_data_type, ascii_data_type_description);

       /* if it's a ENDSTR or ENDEL, subtract from indent */
       if ((!strcmp (ascii_record_type, "ENDSTR")) ||
           (!strcmp (ascii_record_type, "ENDEL")))
       {
           if (strlen (indent_string) >= 2)
           {
               indent_string[strlen (indent_string) - 2] = 0;  //ascii值为0，对应的的字符为空格
           }
       }

       /* print it out */
       printf ("\n");
       printf ("%s0x%04x     # RECORD_LENGTH              Bytes of data in this record\n",
               indent_string, no_bytes);
       printf ("%s0x%02x       # RECORD_TYPE:  %-12s %s\n",
               indent_string, record_type, ascii_record_type, ascii_record_description);
       printf ("%s0x%02x       # DATA_TYPE:    %-12s %s\n",
               indent_string, data_type, ascii_data_type, ascii_data_type_description);

       /* If the record and data types don't match, print an error, but
        * keep right on processing. It seems redundant having both of
        * these when it looks like the record type would be enough, but
        * hey, I didn't create this format, I'm just parsing it and
        * looking for errors... */
       if ((expected_data_type != 0xffff) && (expected_data_type != data_type))
       {
           find_ascii_data_type (expected_data_type, ascii_expected_data_type,
                                 ascii_expected_data_type_description);
           printf ("%s# ***ERROR*** We were expecting data type 0x%02x (%s) to be specified, but\n",
                   indent_string, expected_data_type, ascii_expected_data_type_description);
           printf ("%s#             data type 0x%02x (%s) was specified.\n",
                   indent_string, data_type, ascii_data_type_description);
           printf ("%s#             I'll use the expected data type when I try to read this.\n",
                   indent_string);
        }

       /* now print the actual data (if present) */
       if (expected_data_type == 0x01)	/* BIT_ARRAY */
       {
           for (data_ktr = 2; data_ktr < no_read; data_ktr += 2)
           {
               bit_array = 256 * record[data_ktr] + record[data_ktr + 1];
               printf ("%s0x%04x     # DATA\n",
                       indent_string, bit_array);
               /* print a hopefully useful comment */
               print_bit_array_comments (ascii_record_type, bit_array, indent_string);
           }
       }
       else if (expected_data_type == 0x02)	/* INTEGER_2 */
       {
           for (data_ktr = 2; data_ktr < no_read; data_ktr += 2)
           {
               display_integer = record[data_ktr];
               display_integer <<= 8;
               display_integer += record[data_ktr + 1];
               hex_display_integer = display_integer;
               if (display_integer & 0x8000)	/* negative number, 2's 负数执行if * comp */
               {
                   display_integer &= 0x7fff;
                   display_integer ^= 0x7fff;
                   display_integer += 1;
                   display_integer *= -1;
               }
               printf ("%s0x%04x     # DATA: %d\n",indent_string, hex_display_integer, display_integer);

            }
           //若ascii_record_type为LAYER,读取出层数
           if(!strcmp(ascii_record_type,"LAYER"))   //注意两字符串相等返回零，所以前面加上一个！
              {
                  Layer_num = QString::number(display_integer,10);   //将层数由整形转化为QString型
                  qDebug()<<"层数变量：Layer_num"<<display_integer<<Layer_num;
              }
       }
       else if (expected_data_type == 0x03)	/* INTEGER_4 */
       {
           for (data_ktr = 2; data_ktr < no_read; data_ktr += 4)
           {
               display_integer = 0;
               for (int_ktr = 0; int_ktr < 4; int_ktr++)
                   {
                       display_integer <<= 8;
                       display_integer += record[data_ktr + int_ktr];
                   }
               hex_display_integer = display_integer;
               if (display_integer & 0x80000000)	/* negative number, 2's * comp */
                   {
                       display_integer &= 0x7fffffff;
                       display_integer ^= 0x7fffffff;
                       display_integer += 1;
                       display_integer *= -1;
                    }
               printf ("%s0x%08x # DATA: %d\n",indent_string, hex_display_integer, display_integer);
               array[(data_ktr-2)/4]=display_integer;//存储所有点的坐标
            }

           if(!strcmp(ascii_record_type,"XY"))
           {
               qDebug()<<endl;              //换行
               for(int i=0;i<(no_read-2)/8;i++)
               {
                   array_x[i] = array[2*i];     //存储每点坐标x值
                   array_y[i] = array[2*i+1];   //存储每点坐标y值
                   qDebug()<<"x的坐标："<<array_x[i]<<';'<<"y的坐标："<<array_y[i]<<endl;
               }
            }
       }
       else if (expected_data_type == 0x04)	/* REAL_4 */
       {
           for (data_ktr = 2; data_ktr < no_read; data_ktr += 4)
           {
               real_sign = record[data_ktr] & 0x80;
               real_exponent = (record[data_ktr] & 0x7f) - 64;
               real_mantissa_int = 0;
               for (exponent_ktr = 1; exponent_ktr < 4; exponent_ktr++)
               {
                   real_mantissa_int <<= 8;
                   real_mantissa_int += record[data_ktr + exponent_ktr];
               }
               real_mantissa_float = (double) real_mantissa_int / pow (2, 24);
               display_float = real_mantissa_float * pow (16, (float) real_exponent);
               if (real_sign)
               {
                   display_float *= -1;
               }
               printf ("%s%-.9f # DATA\n", indent_string, display_float);
            }
       }
       else if (expected_data_type == 0x05)	/* REAL_8 */
       {
           for (data_ktr = 2; data_ktr < no_read; data_ktr += 8)
           {
               real_sign = record[data_ktr] & 0x80;
               real_exponent = (record[data_ktr] & 0x7f) - 64;
               real_mantissa_int = 0;
               for (exponent_ktr = 1; exponent_ktr < 8; exponent_ktr++)
               {
                   real_mantissa_int <<= 8;
                   real_mantissa_int += record[data_ktr + exponent_ktr];
               }
               real_mantissa_float = (double) real_mantissa_int / pow (2, 56);
               display_float = real_mantissa_float * pow (16, (float) real_exponent);
               if (real_sign)
               {
                   display_float *= -1;
               }
               printf ("%s%-.18f # DATA\n", indent_string, display_float);
           }
        }
       else if (expected_data_type == 0x06)	/* STRING */
       {
           QString strname = 0;//结构体名的初始化
           for (data_ktr = 2; data_ktr < no_read; data_ktr += 2)
           {
               if(!strcmp(ascii_record_type,"STRNAME"))//将结构体名合成一个整体
               {
                   strname += record[data_ktr];
                   strname += record[data_ktr + 1];
               }
               display_char_1 = record[data_ktr];
               display_char_2 = record[data_ktr + 1];
               if (!isprint (display_char_1))
               {
                  display_char_1 = '.';
               }
               if (!isprint (display_char_2))
               {
                   display_char_2 = '.';
               }
               printf ("%s0x%02x 0x%02x  # DATA: %c%c\n",
                       indent_string, record[data_ktr], record[data_ktr + 1],
                       display_char_1, display_char_2);
               if (((!isprint (record[data_ktr])) && (record[data_ktr] != 0)) ||
                   ((!isprint (record[data_ktr + 1])) && (record[data_ktr + 1] != 0)))
               {
                 printf ("%s# ***ERROR*** There was a non-printable character in the last 2 byte word.\n",indent_string);
               }
           }
           if(!strcmp(ascii_record_type,"STRNAME"))
           {
               qDebug()<<"结构体变量名:strname  "<<"结构体名:"<<strname; //将结构体名作为一个整体输出。
           }
       }
       else
       {
           if (expected_data_type != 0x00)
           {
               for (data_ktr = 2; data_ktr < no_read; data_ktr++)
               {
                   printf ("0x%02x # RAW\n", record[data_ktr]);
               }
            }
       }

       /* if it's a BGNSTR or the beginning of an element, add to indent */
       if ((!strcmp (ascii_record_type, "BGNSTR")) ||
          (!strcmp (ascii_record_type, "BOUNDARY")) ||
          (!strcmp (ascii_record_type, "PATH")) ||
          (!strcmp (ascii_record_type, "SREF")) ||
          (!strcmp (ascii_record_type, "AREF")) ||
          (!strcmp (ascii_record_type, "TEXT")) ||
          (!strcmp (ascii_record_type, "TEXTNODE")) ||
          (!strcmp (ascii_record_type, "NODE")) ||
          (!strcmp (ascii_record_type, "BOX")))
        {
           for (ktr = 0; ktr < NO_SPACES_TO_INDENT; ktr++)
               {
                  strcat (indent_string, " ");
               }
        }
       /* free the record memory */
       free (record);
       }
       else
       {
           /* if it was a NULL record */
           printf ("%s0x%04x # PADDING\n",indent_string, no_bytes);
       }

     }
   return Layer_num;
   exit (0);
}

void find_ascii_record_type (int numeric, char *record_name, char *record_description, int *expected_data_type)
  {

  switch (numeric)
      {
      case 0x00:
          strcpy (record_name, "HEADER");
          strcpy (record_description, "Start of stream, contains version number of stream file");
          *expected_data_type = 0x02;
          break;
      case 0x01:
          strcpy (record_name, "BGNLIB");
          strcpy (record_description, "Beginning of library, plus mod and access dates");
          *expected_data_type = 0x02;
          break;
      case 0x02:
          strcpy (record_name, "LIBNAME");
          strcpy (record_description, "The name of the library");
          *expected_data_type = 0x06;
          break;
      case 0x03:
          strcpy (record_name, "UNITS");
          strcpy (record_description, "Size of db unit in user units and size of db unit in meters");
          *expected_data_type = 0x05;
          break;
      case 0x04:
          strcpy (record_name, "ENDLIB");
          strcpy (record_description, "End of the library");
          *expected_data_type = 0x00;
          break;
      case 0x05:
          strcpy (record_name, "BGNSTR");
          strcpy (record_description, "Begin structure, plus create and mod dates");
          *expected_data_type = 0x02;
          break;
      case 0x06:
          strcpy (record_name, "STRNAME");
          strcpy (record_description, "Name of a structure");
          *expected_data_type = 0x06;
          break;
      case 0x07:
          strcpy (record_name, "ENDSTR");
          strcpy (record_description, "End of a structure");
          *expected_data_type = 0x00;
          break;
      case 0x08:
          strcpy (record_name, "BOUNDARY");
          strcpy (record_description, "The beginning of a BOUNDARY element");
          *expected_data_type = 0x00;
          break;
      case 0x09:
          strcpy (record_name, "PATH");
          strcpy (record_description, "The beginning of a PATH element");
          *expected_data_type = 0x00;
          break;
      case 0x0a:
          strcpy (record_name, "SREF");
          strcpy (record_description, "The beginning of an SREF element");
          *expected_data_type = 0x00;
          break;
      case 0x0b:
          strcpy (record_name, "AREF");
          strcpy (record_description, "The beginning of an AREF element");
          *expected_data_type = 0x00;
          break;
      case 0x0c:
          strcpy (record_name, "TEXT");
          strcpy (record_description, "The beginning of a TEXT element");
          *expected_data_type = 0x00;
          break;
      case 0x0d:
          strcpy (record_name, "LAYER");
          strcpy (record_description, "Layer specification");
          *expected_data_type = 0x02;
          break;
      case 0x0e:
          strcpy (record_name, "DATATYPE");
          strcpy (record_description, "Datatype specification");
          *expected_data_type = 0x02;
          break;
      case 0x0f:
          strcpy (record_name, "WIDTH");
          strcpy (record_description, "Width specification, negative means absolute");
          *expected_data_type = 0x03;
          break;
      case 0x10:
          strcpy (record_name, "XY");
          strcpy (record_description, "An array of XY coordinates");
          *expected_data_type = 0x03;
          break;
      case 0x11:
          strcpy (record_name, "ENDEL");
          strcpy (record_description, "The end of an element");
          *expected_data_type = 0x00;
          break;
      case 0x12:
          strcpy (record_name, "SNAME");
          strcpy (record_description, "The name of a referenced structure");
          *expected_data_type = 0x06;
          break;
      case 0x13:
          strcpy (record_name, "COLROW");
          strcpy (record_description, "Columns and rows for an AREF");
          *expected_data_type = 0x02;
          break;
      case 0x14:
          strcpy (record_name, "TEXTNODE");
          strcpy (record_description,
                  "\"Not currently used\" per GDSII Stream Format Manual, Release 6.0");
          *expected_data_type = 0x00;
          break;
      case 0x15:
          strcpy (record_name, "NODE");
          strcpy (record_description, "The beginning of a NODE element");
          *expected_data_type = 0x00;
          break;
      case 0x16:
          strcpy (record_name, "TEXTTYPE");
          strcpy (record_description, "Texttype specification");
          *expected_data_type = 0x02;
          break;
      case 0x17:
          strcpy (record_name, "PRESENTATION");
          strcpy (record_description, "Text origin and font specification");
          *expected_data_type = 0x01;
          break;
      case 0x18:
          strcpy (record_name, "SPACING");
          strcpy (record_description,
                  "\"Discontinued\" per GDSII Stream Format Manual, Release 6.0");
          *expected_data_type = 0xffff;
          break;
      case 0x19:
          strcpy (record_name, "STRING");
          strcpy (record_description, "Character string");
          *expected_data_type = 0x06;
          break;
      case 0x1a:
          strcpy (record_name, "STRANS");
          strcpy (record_description,
                  "Refl, absmag, and absangle for SREF, AREF and TEXT");
          *expected_data_type = 0x01;
          break;
      case 0x1b:
          strcpy (record_name, "MAG");
          strcpy (record_description, "Magnification, 1 is the default");
          *expected_data_type = 0x05;
          break;
      case 0x1c:
          strcpy (record_name, "ANGLE");
          strcpy (record_description, "Angular rotation factor");
          *expected_data_type = 0x05;
          break;
      case 0x1d:
          strcpy (record_name, "UINTEGER");
          strcpy (record_description,
                  "User integer, used only in V2.0, translates to userprop 126 on instream");
          *expected_data_type = 0xffff;
          break;
      case 0x1e:
          strcpy (record_name, "USTRING");
          strcpy (record_description,
                  "User string, used only in V2.0, translates to userprop 127 on instream");
          *expected_data_type = 0xffff;
          break;
      case 0x1f:
          strcpy (record_name, "REFLIBS");
          strcpy (record_description, "Names of the reference libraries");
          *expected_data_type = 0x06;
          break;
      case 0x20:
          strcpy (record_name, "FONTS");
          strcpy (record_description, "Names of the textfont definition files");
          *expected_data_type = 0x06;
          break;
      case 0x21:
          strcpy (record_name, "PATHTYPE");
          strcpy (record_description, "Type of path ends");
          *expected_data_type = 0x02;
          break;
      case 0x22:
          strcpy (record_name, "GENERATIONS");
          strcpy (record_description, "Number of deleted or backed up structures to retain");
          *expected_data_type = 0x02;
          break;
      case 0x23:
          strcpy (record_name, "ATTRTABLE");
          strcpy (record_description, "Name of the attribute definition file");
          *expected_data_type = 0x06;
          break;
      case 0x24:
          strcpy (record_name, "STYPTABLE");
          strcpy (record_description,
                  "\"Unreleased feature\" per GDSII Stream Format Manual, Release 6.0");
          *expected_data_type = 0x06;
          break;
      case 0x25:
          strcpy (record_name, "STRTYPE");
          strcpy (record_description,
                  "\"Unreleased feature\" per GDSII Stream Format Manual, Release 6.0");
          *expected_data_type = 0x02;
          break;
      case 0x26:
          strcpy (record_name, "ELFLAGS");
          strcpy (record_description, "Flags for template and exterior data");
          *expected_data_type = 0x01;
          break;
      case 0x27:
          strcpy (record_name, "ELKEY");
          strcpy (record_description,
                  "\"Unreleased feature\" per GDSII Stream Format Manual, Release 6.0");
          *expected_data_type = 0x03;
          break;
      case 0x28:
          strcpy (record_name, "LINKTYPE");
          strcpy (record_description,
                  "\"Unreleased feature\" per GDSII Stream Format Manual, Release 6.0");
          *expected_data_type = 0xffff;
          break;
      case 0x29:
          strcpy (record_name, "LINKKEYS");
          strcpy (record_description,
                  "\"Unreleased feature\" per GDSII Stream Format Manual, Release 6.0");
          *expected_data_type = 0xffff;
          break;
      case 0x2a:
          strcpy (record_name, "NODETYPE");
          strcpy (record_description, "Nodetype specification");
          *expected_data_type = 0x02;
          break;
      case 0x2b:
          strcpy (record_name, "PROPATTR");
          strcpy (record_description, "Property number");
          *expected_data_type = 0x02;
          break;
      case 0x2c:
          strcpy (record_name, "PROPVALUE");
          strcpy (record_description, "Property value");
          *expected_data_type = 0x06;
          break;
      case 0x2d:
          strcpy (record_name, "BOX");
          strcpy (record_description, "The beginning of a BOX element");
          *expected_data_type = 0x00;
          break;
      case 0x2e:
          strcpy (record_name, "BOXTYPE");
          strcpy (record_description, "Boxtype specification");
          *expected_data_type = 0x02;
          break;
      case 0x2f:
          strcpy (record_name, "PLEX");
          strcpy (record_description, "Plex number and plexhead flag");
          *expected_data_type = 0x03;
          break;
      case 0x30:
          strcpy (record_name, "BGNEXTN");
          strcpy (record_description, "Path extension beginning for pathtype 4 in CustomPlus");
          *expected_data_type = 0x03;
          break;
      case 0x31:
          strcpy (record_name, "ENDTEXTN");	/* first 'T' a typo in GDSII
                                               * manual??? */
          strcpy (record_description, "Path extension end for pathtype 4 in CustomPlus");
          *expected_data_type = 0x03;
          break;
      case 0x32:
          strcpy (record_name, "TAPENUM");
          strcpy (record_description,
                  "Tape number for multi-reel stream file, you've got a really old file here");
          *expected_data_type = 0x02;
          break;
      case 0x33:
          strcpy (record_name, "TAPECODE");
          strcpy (record_description,
                  "Tape code to verify that you've loaded a reel from the proper set");
          *expected_data_type = 0x02;
          break;
      case 0x34:
          strcpy (record_name, "STRCLASS");
          strcpy (record_description,
                  "Calma use only, non-Calma programs should not use, or set to all 0");
          *expected_data_type = 0x01;
          break;
      case 0x35:
          strcpy (record_name, "RESERVED");
          strcpy (record_description,
                  "Used to be NUMTYPES per GDSII Stream Format Manual, Release 6.0");
          *expected_data_type = 0x03;
          break;
      case 0x36:
          strcpy (record_name, "FORMAT");
          strcpy (record_description, "Archive or Filtered flag");
          *expected_data_type = 0x02;
          break;
      case 0x37:
          strcpy (record_name, "MASK");
          strcpy (record_description,
                  "Only in filtered streams, lists layer and datatype mask used");
          *expected_data_type = 0x06;
          break;
      case 0x38:
          strcpy (record_name, "ENDMASKS");
          strcpy (record_description, "The end of mask descriptions");
          *expected_data_type = 0x00;
          break;
      case 0x39:
          strcpy (record_name, "LIBDIRSIZE");
          strcpy (record_description, "Number of pages in library director, a GDSII thing...");
          *expected_data_type = 0x02;
          break;
      case 0x3a:
          strcpy (record_name, "SRFNAME");
          strcpy (record_description, "Sticks rule file name");
          *expected_data_type = 0x06;
          break;
      case 0x3b:
          strcpy (record_name, "LIBSECUR");
          strcpy (record_description, "Access control list stuff for CalmaDOS, ancient!");
          *expected_data_type = 0x02;
          break;
      default:
          strcpy (record_name, "UNKNOWN");
          strcpy (record_description,
                  "***ERROR*** Unknown record type type");
          *expected_data_type = 0xffff;
          break;
      }
  }
//这个函数主要是为了找到data_type
void find_ascii_data_type (int numeric, char *data_name, char *data_description)
  {

  switch (numeric)
      {
      case 0x00:
          strcpy (data_description, "No data present (nothing after the record header)");
          strcpy (data_name, "NO_DATA");
          break;
      case 0x01:
          strcpy (data_description, "Bit array (2 bytes)");
          strcpy (data_name, "BIT_ARRAY");
          break;
      case 0x02:
          strcpy (data_description, "Two byte signed integer");
          strcpy (data_name, "INTEGER_2");
          break;
      case 0x03:
          strcpy (data_description, "Four byte signed integer");
          strcpy (data_name, "INTEGER_4");
          break;
      case 0x04:
          strcpy (data_description, "Four byte real (not used?)");
          strcpy (data_name, "REAL_4");
          break;
      case 0x05:
          strcpy (data_description, "Eight byte real");
          strcpy (data_name, "REAL_8");
          break;
      case 0x06:
          strcpy (data_description, "ASCII string (padded to an even byte count with NULL)");
          strcpy (data_name, "STRING");
          break;
      default:
          strcpy (data_description, "UNKNOWN");
          strcpy (data_name, "UNKNOWN");
          break;
      }

  }

void print_bit_array_comments (char *ascii_record_type, int bit_array, char *indent_string)
  {
  int half_nyble;

  if (!strcmp (ascii_record_type, "STRANS"))
      {
      printf ("%s           # bits 15 to 0, l to r\n",
              indent_string);
      printf ("%s           # 15=refl, 2=absmag, 1=absangle, others unused\n",
              indent_string);
      if (bit_array & 0x8000)
          {
          printf ("%s           # Reflected on X axis before rotation\n",
                  indent_string);
          }
      if (bit_array & 0x0002)
          {
          printf ("%s           # Magnitude is absolute\n",
                  indent_string);
          }
      if (bit_array & 0x0001)
          {
          printf ("%s           # Angle is absolute\n",
                  indent_string);
          }
      if (bit_array & 0x7ff8)
          {
          printf ("%s           # ***ERROR*** Undefined bits set in STRANS\n",
                  indent_string);
          }
      }
  else if (!strcmp (ascii_record_type, "PRESENTATION"))
      {
      printf ("%s           # bits 15 to 0, l to r\n",
              indent_string);
      printf ("%s           # bits 0 and 1: 00 left, 01 center, 10 right\n",
              indent_string);
      printf ("%s           # bits 2 and 3: 00 top 01, middle, 10 bottom\n",
              indent_string);
      printf ("%s           # bits 4 and 5: 00 font 0, 01 font 1, 10 font 2, 11 font 3,\n",
              indent_string);
      half_nyble = bit_array;
      half_nyble &= 0x30;
      half_nyble >>= 4;
      printf ("%s           # Font %d\n",
              indent_string, half_nyble);
      half_nyble = bit_array;
      half_nyble &= 0x0c;
      half_nyble >>= 2;
      if (half_nyble == 0x00)
          {
          printf ("%s           # Top justification\n",
                  indent_string);
          }
      else if (half_nyble == 0x01)
          {
          printf ("%s           # Middle justification\n",
                  indent_string);
          }
      else if (half_nyble == 0x02)
          {
          printf ("%s           # Bottom justification\n",
                  indent_string);
          }
      else
          {
          printf ("%s           # ***ERROR*** Illegal justification\n",
                  indent_string);
          }
      half_nyble = bit_array;
      half_nyble &= 0x03;
      if (half_nyble == 0x00)
          {
          printf ("%s           # Left justification\n",
                  indent_string);
          }
      else if (half_nyble == 0x01)
          {
          printf ("%s           # Center justification\n",
                  indent_string);
          }
      else if (half_nyble == 0x02)
          {
          printf ("%s           # Right justification\n",
                  indent_string);
          }
      else
          {
          printf ("%s           # ***ERROR*** Illegal justification\n",
                  indent_string);
          }
      if (bit_array & 0xffc0)
          {
          printf ("%s           # ***ERROR*** Undefined bits set in PRESENTATION\n",
                  indent_string);
          }
      }
  else if (!strcmp (ascii_record_type, "ELFLAGS"))
      {
      printf ("%s           # bits 15 to 0, l to r\n",
              indent_string);
      printf ("%s           # 0=template, 1=external data, others unused\n",
              indent_string);
      if (bit_array & 0x0001)
          {
          printf ("%s           # This is template data\n",
                  indent_string);
          }
      if (bit_array & 0x0002)
          {
          printf ("%s           # This is external data\n",
                  indent_string);
          }
      if (bit_array & 0xfffc)
          {
          printf ("%s           # ***ERROR*** Undefined bits in ELFLAGS\n",
                  indent_string);
          }
      }
  else if (!strcmp (ascii_record_type, "STRCLASS"))
      {
      printf ("%s           # Calma internal use only, should be all 0\n",
              indent_string);
      printf ("%s           # if this file was generated by non-Calma software\n",
              indent_string);
      if (bit_array)
          {
          printf ("%s           # ***WARNING*** The STRCLASS record type is used\n",
                  indent_string);
          }
      }
  else
      {
      printf ("\n");
      }
}
