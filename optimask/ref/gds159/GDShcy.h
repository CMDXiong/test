//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDSREAD_H__678DFE79_317D_4855_985E_ACB17A911FA3__INCLUDED_)
#define AFX_GDSREAD_H__678DFE79_317D_4855_985E_ACB17A911FA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum rec_type{
	GDS_HEADER = 0,
	GDS_BGNLIB,
	GDS_LIBNAME,
	GDS_UNITS,
	GDS_ENDLIB,
	GDS_BGNSTR,
	GDS_STRNAME,
	GDS_ENDSTR,
	GDS_BOUNDARY,
	GDS_PATH,
	GDS_SREF,
	GDS_AREF,
	GDS_TEXT,
	GDS_LAYER,
	GDS_DATATYPE,
	GDS_WIDTH,
	GDS_XY,
	GDS_ENDEL,
	GDS_SNAME,
	GDS_COLROW,
	GDS_TEXTNODE,
	GDS_NODE,
	GDS_TEXTTYPE,
	GDS_PRESENTATION,
	GDS_SPACING,
	GDS_STRING,
	GDS_STRANS,
	GDS_MAG,
	GDS_ANGLE,
	GDS_UINTEGER,
	GDS_USTRING,
	GDS_REFLIBS,
	GDS_FONTS,
	GDS_PATHTYPE,
	GDS_GENERATIONS,
	GDS_ATTRTABLE,
	GDS_STYPTABLE,
	GDS_STRTYPE,
	GDS_ELFLAGS,
	GDS_ELKEY,
	GDS_LINKTYPE,
	GDS_LINKKEYS,
	GDS_NODETYPE,
	GDS_PROPATTR,
	GDS_PROPVALUE,
	GDS_BOX,
	GDS_BOXTYPE,
	GDS_PLEX,
	GDS_BGNEXTN,
	GDS_ENDEXTN,
	GDS_TAPENUM,
	GDS_TAPECODE,
	GDS_STRCLASS,
	GDS_RESERVED,
	GDS_FORMAT,
	GDS_MASK,
	GDS_ENDMASKS,
	GDS_LIBDIRSIZE,
	GDS_SRFNAME,
	GDS_LIBSECUR,
	GDS_BORDER,
	GDS_SOFTFENCE,
	GDS_HARDFENCE,
	GDS_SOFTWIRE,
	GDS_HARDWIRE,
	GDS_PATHPORT,
	GDS_NODEPORT,
	GDS_USERCONSTRAINT,
	GDS_SPACER_ERROR,
	GDS_CONTACT
};

enum strans{
	GDS_STRANS_REFRECT = 0x8000,
	GDS_STRANS_ABS_MAG = 0x0004,
	GDS_STRANS_ABS_ANG = 0x0002,
};

enum element{
	GDS_ELEMENT_TEMPLATE = 0x01,
	GDS_ELEMENT_EXTERNAL = 0x02,
};

#define PAI                     3.1415926535897932384626433832795
#define GDS_MAX_RECORD_LEN      (65536 + 1) // 16 bit
#define GDS_MAX_STR_LEN         (65536 + 1) // 16 bit

#define GDS_MAX_LAYER           1024 //512 //256
#define GDS_MAX_COL				32767
#define GDS_MAX_ROW				GDS_MAX_COL
#define GDS_MAX_DATA_TYPE		255

#define GDS_MAX_COORD           8192 
// Record length = 16bit = points up to 65536byte
// XY coord  = pair of 4byte integer= 8byte
// 65536/8 = 8192

class CCoord
{
public :
	double x;
	double y;

public:
	inline CCoord(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
	inline CCoord()
	{
		x = y = 0;
	}
	inline CCoord(const CCoord &other)
	{
		x = other.x;
		y = other.y;
	}
	inline void Offset(double x, double y)
	{
		this->x += x;
		this->y += y;
	}
	inline void Offset(double* x, double* y)
	{
		this->x += *x;
		this->y += *y;
	}
	inline void Offset(const CCoord &add)
	{
		x += add.x;
		y += add.y;
	}
	inline void OffsetNeg(const CCoord &sub)
	{
		x -= sub.x;
		y -= sub.y;
	}
	inline void NegateY()
	{
		y = -y;
	}
	inline void NegateX()
	{
		x = -x;
	}
	inline void Negate()
	{
		x = -x;
		y = -y;
	}
	inline void Mul(const CCoord &add)
	{
		x *= add.x;
		y *= add.y;
	}
	inline void Mul(const double &ratio)
	{
		x *= ratio;
		y *= ratio;
	}

	inline void SetCoord(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
	inline void SetCoord(double* x, double* y)
	{
		this->x = *x;
		this->y = *y;
	}
	inline void SetCoord(CCoord& arg)
	{
		this->x = arg.x;
		this->y = arg.y;
	}
	inline BOOL operator==(CCoord& arg)
	{
		if(x == arg.x && y == arg.y)
			return TRUE;
		return FALSE;
	}
	inline BOOL Equals(CCoord& arg)
	{
		if(x == arg.x && y == arg.y)
			return TRUE;
		return FALSE;
	}
	inline BOOL Equals(CCoord* arg)
	{
		if(x == arg->x && y == arg->y)
			return TRUE;
		return FALSE;
	}
	inline BOOL operator!=(CCoord& arg)
	{
		if(x != arg.x || y != arg.y)
			return TRUE;
		return FALSE;
	}

	inline void Copy(CCoord* other)
	{
		x = other->x;
		y = other->y;
	}

	inline CCoord& operator=(CCoord& arg)
	{
		x = arg.x;
		y = arg.y;
		return *this;
	}

	inline CCoord& operator+=(CCoord& arg)
	{
		x += arg.x;
		y += arg.y;
		return *this;
	}
	inline CCoord& operator-=(CCoord& arg)
	{
		x -= arg.x;
		y -= arg.y;
		return *this;
	}
	inline CCoord& operator*=(double ratio)
	{
		x *= ratio;
		y *= ratio;
		return *this;
	}
	inline CCoord& operator/=(double ratio)
	{
		x /= ratio;
		y /= ratio;
		return *this;
	}
	inline CCoord operator* (double ratio)
	{
		return CCoord(x * ratio, y * ratio);
	}
};

class CiCoord
{
public :
	int x;
	int y;

public:
	inline CiCoord(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	inline CiCoord()
	{
		x = y = 0;
	}
	inline CiCoord(const CiCoord &other)
	{
		x = other.x;
		y = other.y;
	}
	inline void Offset(int x, int y)
	{
		this->x += x;
		this->y += y;
	}
	inline void Offset(int* x, int* y)
	{
		this->x += *x;
		this->y += *y;
	}
	inline void Offset(const CiCoord &add)
	{
		x += add.x;
		y += add.y;
	}
	inline void OffsetNeg(const CiCoord &sub)
	{
		x -= sub.x;
		y -= sub.y;
	}
	inline void NegateY()
	{
		y = -y;
	}
	inline void NegateX()
	{
		x = -x;
	}
	inline void Negate()
	{
		x = -x;
		y = -y;
	}
	inline void Mul(const CiCoord &add)
	{
		x *= add.x;
		y *= add.y;
	}
	inline void Mul(const int &ratio)
	{
		x *= ratio;
		y *= ratio;
	}
	inline void SetCoord(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	inline void SetCoord(int* x, int* y)
	{
		this->x = *x;
		this->y = *y;
	}
	inline void SetCoord(CiCoord& arg)
	{
		this->x = arg.x;
		this->y = arg.y;
	}
	inline BOOL operator==(CiCoord& arg)
	{
		if(x == arg.x && y == arg.y)
			return TRUE;
		return FALSE;
	}
	inline BOOL Equals(CiCoord& arg)
	{
		if(x == arg.x && y == arg.y)
			return TRUE;
		return FALSE;
	}
	inline BOOL Equals(CiCoord* arg)
	{
		if(x == arg->x && y == arg->y)
			return TRUE;
		return FALSE;
	}
	inline BOOL operator!=(CiCoord& arg)
	{
		if(x != arg.x || y != arg.y)
			return TRUE;
		return FALSE;
	}

	inline void Copy(CiCoord* other)
	{
		x = other->x;
		y = other->y;
	}

	inline CiCoord& operator=(CiCoord& arg)
	{
		x = arg.x;
		y = arg.y;
		return *this;
	}
	inline CiCoord& operator+=(CiCoord& arg)
	{
		x += arg.x;
		y += arg.y;
		return *this;
	}
	inline CiCoord& operator-=(CiCoord& arg)
	{
		x -= arg.x;
		y -= arg.y;
		return *this;
	}
	inline CiCoord& operator*=(int ratio)
	{
		x *= ratio;
		y *= ratio;
		return *this;
	}
	inline CiCoord& operator/=(int ratio)
	{
		x /= ratio;
		y /= ratio;
		return *this;
	}
	inline CiCoord operator* (int ratio)
	{
		return CiCoord(x * ratio, y * ratio);
	}
};

class CGDSProperty
{
protected :
	unsigned  m_uintPropAttr;
	char      *m_strPropValue;
public :
	CGDSProperty(){
		m_uintPropAttr = 0;
		m_strPropValue = NULL;
	}
	~CGDSProperty(){
		if(m_strPropValue)
			delete [] m_strPropValue;
	}
	void SetPropAttr(int var){m_uintPropAttr = var;}
	void SetPropValue(char* str)
	{
		if(m_strPropValue)
			delete [] m_strPropValue;
		int len = strlen(str) + 1;
		m_strPropValue = new char[len];
		strncpy(m_strPropValue, str, len);
	}
	unsigned GetPropAttr(){return m_uintPropAttr;}
	char* GetPropValue(){return m_strPropValue;}

};


class CGDSXY
{
protected :
	unsigned short	m_intNumCoords;
	unsigned long	m_ulSize;
	CiCoord			m_coordPos;
	CiCoord*		m_coordList;
public :
	CGDSXY()
	{
		m_coordList = NULL;
		m_intNumCoords = 0;
		m_ulSize = 0;
    }
	~CGDSXY();
	virtual void LoadCoords(CiCoord* list, int num)
	{
		if(num <= 0)
			return;
		m_intNumCoords = num;
		if(m_coordList)
			delete [] m_coordList;

		if(num == 1){
			m_coordPos = *(list + 0);
			return;
		}

		m_coordList = new CiCoord[num];
		long minx, maxx, miny, maxy;
		for(unsigned short i = 0;i < num;i++){
			*(m_coordList + i) = *(list + i);
			int x = (m_coordList + i)->x;
			int y = (m_coordList + i)->y;
			if(i == 0){
				minx = maxx = x;
				miny = maxy = y;
			}
			else{
				if(x < minx)
					minx = x;
				else if(x > maxx)
					maxx = x;
				if(y < miny)
					miny = y;
				else if(y > maxy)
					maxy = y;
			}
		}
		m_ulSize = max(maxx - minx, maxy - miny);
		m_coordPos.x = (maxx + minx)/2L;
		m_coordPos.y = (maxy + miny)/2L;
	}
	unsigned long GetSize()
	{
		return m_ulSize;
	}
	void GetPos(CCoord* pCoord)
	{
		pCoord->x = m_coordPos.x;
		pCoord->y = m_coordPos.y;
	}
	void GetNthCoord(int n, CCoord* pcoord)
	{
		if(m_coordList){
			pcoord->x = (m_coordList + n)->x;
			pcoord->y = (m_coordList + n)->y;
		}
		else{
			pcoord->x = m_coordPos.x;
			pcoord->y = m_coordPos.y;
		}
	}
	unsigned short GetNumCoords()
	{
		return m_intNumCoords;
	}
	CiCoord* GetCoordArray()
	{
		return m_coordList;
	}
};

class CGDSStructure;


class CGDSElement : public CObject, public CGDSXY
{
protected :
	//unsigned char	m_intAttrib;
	//int			m_intPlex;

	//unsigned char	m_intLayer;
	unsigned int	m_uintLayer;

	unsigned char	m_uintElementType;

	//CGDSProperty*	m_pProperties;
	//unsigned		m_intNumProperties;

public :
	CGDSElement()
	{
		m_uintLayer = 0;
		//m_intAttrib = 0;
		//m_intPlex = 0;
		//m_pProperties = NULL;
		//m_intNumProperties = 0;
	}
	~CGDSElement();
	virtual void SetDataType(int type){}
	virtual void SetWidth(int width){}
	virtual void SetSname(CString str){}
	virtual void SetColRow(int col, int row){}
	virtual void SetTextType(int type){}
	virtual void SetPresentation(int bits){}
	virtual void SetString(CString str){}
	virtual void SetStrans(int bits){}
	virtual void SetMag(double mag){}
	virtual void SetAngle(double angle){}
	virtual void SetPathType(int type){}
	virtual void SetNodeType(int type){}
	virtual void SetBoxType(int type){}
	virtual void SetBgnExtn(int len){}
	virtual void SetEndExtn(int len){}
	virtual char* GetSname(){return NULL;}
	virtual void SetRefStructure(CGDSStructure* ref){}
	virtual CGDSStructure* GetRefStructure(){return NULL;}
	void SetLayer(unsigned int layer);
	void SetPlex(int plex);
	void SetElflags(int elflags);
	//int  GetPlex(){return m_intPlex;}
	void SetElementType(int type){m_uintElementType = type;}
	unsigned int  GetLayer(){return m_uintLayer;}
	int  GetElementType(){return m_uintElementType;}

	void AddProperty(int var, char* str)
	{
		/*CGDSProperty* p = new CGDSProperty[m_intNumProperties + 1];
		if(m_pProperties){
			for(unsigned i = 0;i < m_intNumProperties;i++){
				(p + i)->SetPropAttr((m_pProperties + i)->GetPropAttr());
				(p + i)->SetPropValue((m_pProperties + i)->GetPropValue());
			}
			delete [] m_pProperties;
		}
		(p + m_intNumProperties)->SetPropAttr(var);
		(p + m_intNumProperties)->SetPropValue(str);
		m_pProperties = p;
		m_intNumProperties++; */
	}
};

class CGDSStrans : public CGDSElement
{
protected :
	double m_dblMag;
	double m_dblAng;
	unsigned short m_intAttrib;
public :
	CGDSStrans()
	{
		m_dblMag = 1.0;
		m_dblAng = 0.0;
		m_intAttrib = 0;
	}
	void SetStrans(int bits){m_intAttrib = bits;}
	void SetAngle(double ang){m_dblAng = ang;}
	void SetMag(double mag){m_dblMag = mag;}

	BOOL IsReflect()
	{
		return (m_intAttrib & GDS_STRANS_REFRECT) > 0 ? TRUE : FALSE;
	}
	BOOL IsAbsAng()
	{
		return (m_intAttrib & GDS_STRANS_ABS_ANG) > 0 ? TRUE : FALSE;
	}
	BOOL IsAbsMag()
	{
		return (m_intAttrib & GDS_STRANS_ABS_MAG) > 0 ? TRUE : FALSE;
	}
	double GetMag(){return m_dblMag;}
	void GetMag(double* pmag){*pmag = m_dblMag;}
	double GetAngle(){return m_dblAng;}
	void GetAngle(double* pang){*pang = m_dblAng;}
};

class CGDSBoundary : public CGDSElement
{
protected :
	unsigned char m_uintDataType;

public :
	CGDSBoundary(){m_uintDataType = 0;}
	~CGDSBoundary(){}
	void SetDataType(int type);
};

class CGDSPath : public CGDSElement
{
protected :
	unsigned char m_uintDataType;
	unsigned char m_uintPathType;
	int		  m_intBgnExtn;
	int		  m_intEndExtn;
	int       m_intWidth;

public :
	CGDSPath()
	{
		m_uintDataType = 0;
		m_uintPathType = m_intBgnExtn = m_intEndExtn = 0;
		m_intWidth = 0;
	}
	~CGDSPath(){}
	void SetDataType(int type);
	void SetWidth(int width);
	void SetPathType(int type);
	void SetBgnExtn(int len);
	void SetEndExtn(int len);
	int  GetPathType(){return m_uintPathType & 0x0f;}
	int  GetBgnExtn(){return m_intBgnExtn;}
	int  GetEndExtn(){return m_intEndExtn;}
	int  GetWidth(){return m_intWidth;}
	void LoadCoords(CiCoord* list, int num);
};

class CGDSSREF : public CGDSStrans
{
protected :
	char*           m_strSname;
	CGDSStructure* m_pStructure;

public :
	CGDSSREF()
	{
		m_strSname = NULL;
		m_pStructure = NULL;
	}
	~CGDSSREF()
	{
		if(m_strSname)
			delete [] m_strSname;
	}
	void SetSname(CString str)
	{
		int len = str.GetLength() + 1;
		m_strSname = new char[len];
		strncpy(m_strSname, str, len);
	}
	char* GetSname();
	void SetRefStructure(CGDSStructure* ref)
	{
		m_pStructure = ref;
		if(m_strSname){
			delete [] m_strSname;
			m_strSname = NULL;
		}
	}
	CGDSStructure* GetRefStructure(){return m_pStructure;}
};

class CGDSAREF : public CGDSSREF
{
protected :
	unsigned   m_uintCol;
	unsigned   m_uintRow;
public :
	CGDSAREF()
	{
		m_uintCol = m_uintRow = 0;
	}
	~CGDSAREF(){}
	void SetColRow(int col, int row)
	{
		if(col >= 0 && col <= GDS_MAX_COL && row >= 0 && row <= GDS_MAX_ROW){
			m_uintCol = col;
			m_uintRow = row;
			return;
		}
		return;
	}
	unsigned GetNumCol(){return m_uintCol;}
	unsigned GetNumRow(){return m_uintRow;}
};

class CGDSText : public CGDSStrans
{
protected :
	unsigned  m_uintFont;
	unsigned  m_uintVertJust;
	unsigned  m_uintHorzJust;

	unsigned char m_uintPathType;
	unsigned char m_uintTextType;
	int       m_intWidth;

	char*     m_strString;
public :
	CGDSText()
	{
		m_uintFont = m_uintVertJust = m_uintHorzJust = 0;
		m_uintPathType = m_intWidth = 0;
		m_uintTextType = 0;
		m_strString = NULL;
	}
	~CGDSText()
	{
		if(m_strString)
			delete [] m_strString;
	}
	void SetWidth(int width);
	void SetTextType(int type)
	{
		if(type >= 0 && type <= GDS_MAX_DATA_TYPE){
			m_uintTextType = type;
			return;
		}
		return;
	}
	void SetPresentation(int bits);
	void SetString(CString str)
	{
		if(m_strString)
			delete [] m_strString;
		int len = str.GetLength() + 1;
		m_strString = new char[len];
		strncpy(m_strString, str, len);
	}
	void SetPathType(int type);
	char* GetString(){return m_strString;}
	int GetWidth(){return m_intWidth;}
	int GetFont(){return m_uintFont;}
	int GetVertJust(){return m_uintVertJust;}
	int GetHorzJust(){return m_uintHorzJust;}
	int GetPathType(){return m_uintPathType;}
};

class CGDSNode : public CGDSElement
{
protected :
	unsigned char m_uintNodeType;
public:
	CGDSNode(){m_uintNodeType = 0;}
	~CGDSNode(){}
	void SetNodeType(int type);
};

class CGDSBox : public CGDSElement
{
protected :
	unsigned char m_uintBoxType;
public:
	CGDSBox(){m_uintBoxType = 0;}
	~CGDSBox(){}
	void SetBoxType(int type);
};

class CGDSElements
{
protected :
	CObList m_obaryElements;
public:
	CGDSElements(){}
	~CGDSElements();
	void AddElement(int type);
	void DeleteElement(CGDSElement* p);
	CObList* GetElementList(){return &m_obaryElements;}
	CGDSElement* GetLastElement()
	{
		POSITION pos = m_obaryElements.GetTailPosition();
		if(pos)
		    return (CGDSElement*)m_obaryElements.GetAt(pos);
	    return NULL;
	}
};

class CGDSStructures;
class CGDSRefStructure;

class CGDSStructure : public CGDSElements, public CObject
{
protected :
	CCoord			m_coordDimMin;
	CCoord			m_coordDimMax;
	char*			m_strStrName;
	//unsigned		m_uintStrClass;
    //struct tm		m_tmCreationDate;
    //struct tm		m_tmLastModifyDate;

	BOOL			m_bRefered;
	CObArray		m_obaryRefStructures;

	unsigned char	m_charLayerList[GDS_MAX_LAYER/8];
public :
	CGDSStructure()
	{
		//m_uintStrClass = 0; // Cadence use only
		m_bRefered = FALSE;
		m_strStrName = NULL;
		for(int i = 0;i < GDS_MAX_LAYER/8;i++)
			*(m_charLayerList + i) = 0;
	}
	~CGDSStructure();
	void SetVisibleOnLayer(int layer)
	{
		//if(layer < 0 || layer >= GDS_MAX_LAYER)
		//	return;
		unsigned char val = (0x01 << (layer % 8));
		*(m_charLayerList + layer / 8) |= val;
	}
	BOOL IsVisibleOnLayer(int layer)
	{
		//if(layer < 0 || layer >= GDS_MAX_LAYER)
		//	return FALSE;
		unsigned char val = (0x01 << (layer % 8));
		return (*(m_charLayerList + layer / 8) & val) > 0 ? TRUE : FALSE;
	}
	void MarkRefered(){m_bRefered = TRUE;}
	BOOL IsRefered(){return m_bRefered;}
	void SetStructureName(CString name)
	{
		if(m_strStrName)
			delete [] m_strStrName;

		int len = name.GetLength() + 1;
		m_strStrName = new char[len];
		strncpy(m_strStrName, name, len);
	}
	char* GetStructureName(){return m_strStrName;}
	/*struct tm* GetCreateDate()
	{
		return &m_tmCreationDate;
	}
	struct tm* GetModifyDate()
	{
		return &m_tmLastModifyDate;
	}*/
	void  GetDimMin(CCoord* dim)
	{
		dim->x = m_coordDimMin.x;
		dim->y = m_coordDimMin.y;
	}
	void  GetDimMax(CCoord* dim)
	{
		dim->x = m_coordDimMax.x;
		dim->y = m_coordDimMax.y;
	}
	CCoord*  GetDimMinP(){return &m_coordDimMin;}
	CCoord*  GetDimMaxP(){return &m_coordDimMax;}
	void	SetDimMin(CCoord coord){m_coordDimMin = coord;}
	void	SetDimMax(CCoord coord){m_coordDimMax = coord;}
	void    CalcDimension(CGDSStructures* strlist, BOOL* broken_link, CString* err = NULL);
	void    GetElementMinMax(CGDSElement* element, CCoord& min, CCoord& max);
	void    AddRefStructure(CGDSStructure* str);
	CGDSRefStructure* FindRefStructure(CGDSStructure* structure);
	CGDSRefStructure* FindRefStructure(CString name);
	int GetNumRefStructure();
	CObArray* GetRefStructureList(){return &m_obaryRefStructures;}
};


class CGDSRefStructure : public CObject
{
protected:
	CGDSStructure* m_pRefStructure;
	char*          m_strStrName;
public:
	CGDSRefStructure()
	{
		m_pRefStructure = NULL;
		m_strStrName = NULL;
	}
	~CGDSRefStructure()
	{
		if(m_strStrName)
			delete [] m_strStrName;
	}
	CGDSStructure* GetStructure(){return m_pRefStructure;}
	void SetStructure(CGDSStructure* str)
	{
		m_pRefStructure = str;
		if(m_strStrName){
			delete [] m_strStrName;
			m_strStrName = NULL;
		}
	}
	void SetName(CString name){
		if(m_strStrName)
			delete [] m_strStrName;
		int len = name.GetLength() + 1;
		m_strStrName = new char[len];
		strncpy(m_strStrName, name, len);
	}
	char* GetName()
	{
		return m_strStrName;
	}
};


class CGDSStructures
{
protected :
	CObArray m_obaryStructures;
	CGDSStructure* m_structActive;
public :
	CGDSStructure* FindTopStructure();
	void MakeCrossRef();
	CGDSStructures()
	{
		m_structActive = NULL;
	}
	~CGDSStructures();
	CObArray* GetStructureList(){return &m_obaryStructures;}
	void AddStructure();
	CGDSStructure* FindStructure(char* name);
	int GetNumStructure(){return m_obaryStructures.GetSize();}
	void ClearActiveStructure(){m_structActive = NULL;}
	CGDSStructure* GetActiveStructure(){return m_structActive;}
};

class CMask : public CObject
{
protected :
	CString m_strMask;
public :
	void SetMaskName(CString str){m_strMask = str;}
	CString GetMaskName(){return m_strMask;}
};

class CGDSFormatType
{
protected :
	CObArray m_obaryMasks;
public :
	CGDSFormatType(){}
	~CGDSFormatType();
	void AddFormatType(CString str);
};


class CGDShcy :  public CGDSStructures, public CGDSFormatType
{
protected : 
    struct tm      m_tmCreationDate;
    struct tm      m_tmLastAccessDate;

	unsigned       m_uintStreamVersion;
	unsigned       m_uintLibDirSize;
	CString 	   m_strSrfName;
	CString 	   m_strLibName;
	CString 	   m_strFonts;
	CString 	   m_strRefLibs;
	CString 	   m_strAttrTable;
	unsigned       m_uintGenerations;

	double         m_dblUserUnit;
	double         m_dblDBUnit;

protected :
	CCoord         m_coordMax;
	CCoord         m_coordMin;

public :
	CGDShcy();
	virtual ~CGDShcy();

	CString GetLibName(){return m_strLibName;}
	double  GetUserUnit(){return m_dblUserUnit;}
	double  GetDBUnit(){return m_dblDBUnit;}
	BOOL CalculateDimension();
	void CalcDimensionRecurse(CGDSStructure* structure, CCoord pos, BOOL reflect, double angle, double mag);
	void RotatePoint(CCoord* point, double& angle);
	void fmod360(double& angle);

	struct tm GetCreationDate(){return m_tmCreationDate;}
	struct tm GetAccessDate(){return m_tmLastAccessDate;}
};

class CGDS : public CGDShcy
{
protected :
	CFile         m_fileRead;
	CArchive*     m_readArchive;
	unsigned long m_ulongCacheSize;
	unsigned int  m_uintRecordLength;
	unsigned int  m_uintRecordType;
	unsigned int  m_uintDataType;
	unsigned int  m_uintBufPtr;
	unsigned char m_ucharBuf[GDS_MAX_RECORD_LEN];
	char          m_charBuf[GDS_MAX_STR_LEN];

	BOOL ValidateXY(int elementtype, int num_xy);
	unsigned      DecodeBitArray();
	inline double DecodeReal();
	inline int    DecodeInteger();
	BOOL          DecodeDateTime(struct tm *tm_set);

	BOOL          m_bLayerList[GDS_MAX_LAYER];

	CiCoord       m_aryCoordBuf[GDS_MAX_COORD];
	int           m_intCoordIdx;

	int           m_intPropAttr;
	CString       m_strPropVal;
	unsigned long m_ulNumStructure;
	unsigned long m_ulNumXY;
	unsigned long m_ulNumXYCoord;

public :
	CGDS();
	BOOL ReadGDS(CString path, CString& name, void (*pStepCancel)(void) = NULL,
		void (*pStepProgress)(void) = NULL,  
		int step_pcnt = 0, BOOL (*pCheckStepProgDlgCancel)(void) = NULL);
	BOOL ReadGDSPlain(CString path, void (*pStepProgress)(void) = NULL,  
		int step_pcnt = 0, BOOL (*pCheckStepProgDlgCancel)(void) = NULL);
	BOOL ReadGDSZip(CString path, CString& name, void (*pStepProgress)(void) = NULL,  
		int step_pcnt = 0, BOOL (*pCheckStepProgDlgCancel)(void) = NULL);
	BOOL ReadGDSGZip(CString path, void (*pStepProgress)(void) = NULL,  
		int step_pcnt = 0, BOOL (*pCheckStepProgDlgCancel)(void) = NULL);
	BOOL DecodeRecord();
	BOOL DecodeAscii(CString* set_to, unsigned len = 0);
	void AddLayer(int layer);
	BOOL DoesLayerExist(int layer);

};


#endif // !defined(AFX_GDSREAD_H__678DFE79_317D_4855_985E_ACB17A911FA3__INCLUDED_)
