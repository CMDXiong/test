//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gds.h"
#include "GDShcy.h"

#include <math.h>
#include <limits.h>
#include <float.h>


#include "zlib.h"
#include "unzip.h"
#include "ProgDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define GDS_HEADER_LEN        (unsigned long int)4
#define ARCHIVE_CACHE_SIZE    (1048576L)

enum dat_type{
	GDS_NoData = 0,
	GDS_BitArray,
	GDS_Integer2,
	GDS_Integer4,
	GDS_Real4, // not used
	GDS_Real8,
	GDS_Ascii
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
char* CGDSSREF::GetSname()
{
	return m_strSname;
}

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

CGDS::CGDS()
{
	for(int i = 0;i < sizeof(m_bLayerList)/sizeof(BOOL);i++)
		*(m_bLayerList + i) = FALSE;
	m_intPropAttr = -1;
	m_strPropVal = "";
	m_ulNumStructure = 0;
	m_ulNumXY = 0;
	m_ulNumXYCoord = 0;
}

BOOL CGDS::DoesLayerExist(int layer)
{
	if(layer < 0 || layer >= GDS_MAX_LAYER)
		return FALSE;
	if(*(m_bLayerList + layer) == TRUE)
		return TRUE;
	else
		return FALSE;
}

void CGDS::AddLayer(int layer)
{
	if(layer < 0 || layer >= GDS_MAX_LAYER)
		return;
	*(m_bLayerList + layer) = TRUE;
}

BOOL CGDS::ReadGDS(CString path, CString& name,  void (*pStepCancel)(void), void (*pStepProgress)(void),  
				   int step_pcnt, BOOL (*pCheckStepProgDlgCancel)(void))
{
	CString patha = path;
	patha.MakeUpper();
	name = path;
	if(patha.Find("ZIP") == patha.GetLength() - 3){
		CString namesel = "";
		if(!ReadGDSZip(path, namesel, pStepProgress, step_pcnt, pCheckStepProgDlgCancel))
			return FALSE;
		if(namesel.GetLength() > 0){
			name += "\\" + namesel;
		}
	}
	else if(patha.Find("GZ") == patha.GetLength() - 2){
		if(!ReadGDSGZip(path, pStepProgress, step_pcnt, pCheckStepProgDlgCancel))
			return FALSE;
	}
	else if(!ReadGDSPlain(path, pStepProgress, step_pcnt, pCheckStepProgDlgCancel))
		return FALSE;

	pStepProgress();
	pStepCancel();

#ifdef DEBUG_MSG
MessageBox(AfxGetApp()->GetMainWnd()->m_hWnd, "File was read.", "Info.", MB_ICONINFORMATION | MB_OK);
#endif

	MakeCrossRef();

#ifdef DEBUG_MSG
MessageBox(AfxGetApp()->GetMainWnd()->m_hWnd, "Made x-ref mapping.", "Info.", MB_ICONINFORMATION | MB_OK);
#endif

	if(!CalculateDimension())
		return FALSE;

#ifdef DEBUG_MSG
MessageBox(AfxGetApp()->GetMainWnd()->m_hWnd, "Made structure size info.", "Info.", MB_ICONINFORMATION | MB_OK);
#endif

	if(m_ulNumStructure == 0) // empty file may not be detected to be invalid
		return FALSE;
#if 0
	CString msg;
	msg.Format("structure=%ld, xy=%ld, xycoord=%ld", m_ulNumStructure, m_ulNumXY, m_ulNumXYCoord);
	MessageBox(AfxGetApp()->GetMainWnd()->m_hWnd, msg, "Info.", MB_ICONINFORMATION | MB_OK);
#endif

	return TRUE;
}

BOOL CGDS::ReadGDSPlain(CString path, void (*pStepProgress)(void),  
				   int step_pcnt, BOOL (*pCheckStepProgDlgCancel)(void))
{
	if(!m_fileRead.Open(path, CFile::modeRead | CFile::typeBinary))
		return FALSE;

	BOOL anyFault = FALSE;
	long readSize = 0;

	m_ulongCacheSize = ARCHIVE_CACHE_SIZE;
	BYTE* buf = new BYTE[m_ulongCacheSize];
	m_readArchive = new CArchive(&m_fileRead, CArchive::load, m_ulongCacheSize, buf);

	m_intCoordIdx = 0;

	long length = 0;
    if(pStepProgress)
        length = m_readArchive->GetFile()->GetLength();

	int pcnt, pcnt_now;
	pcnt = pcnt_now = 0;
	while(1){
		if(pCheckStepProgDlgCancel){
			if(pCheckStepProgDlgCancel()){
				anyFault = TRUE;
				break;
			}
		}
        if(pStepProgress){
            pcnt_now = (int)(100.0 * readSize / length);
            if(pcnt_now >= pcnt + step_pcnt){
                pStepProgress();
                pcnt = pcnt_now;
            }
        }
		unsigned len = m_readArchive->Read((void*)m_ucharBuf, GDS_HEADER_LEN);
		readSize += len;
		if(len < GDS_HEADER_LEN)
			break;

		m_uintRecordLength  = (((unsigned int) *(m_ucharBuf + 0)) << 8);
		m_uintRecordLength += (unsigned int) *(m_ucharBuf + 1);
		if(m_uintRecordLength == 0)
			break;
		m_uintRecordLength -= GDS_HEADER_LEN;

		m_uintRecordType = *(m_ucharBuf + 2);
		m_uintDataType = *(m_ucharBuf + 3);

		len = m_readArchive->Read((void*)m_ucharBuf, m_uintRecordLength);
		if(len < m_uintRecordLength)
			break;
		readSize += len;
		if(DecodeRecord() == FALSE){
			anyFault = TRUE;
			break;
		}
	}
	delete m_readArchive;
	delete [] buf;
	m_fileRead.Close();

	if(anyFault)
		return FALSE;
	return TRUE;
}

#include "ZipSelDlg.h"
BOOL CGDS::ReadGDSZip(CString path, CString& name, void (*pStepProgress)(void),  
				   int step_pcnt, BOOL (*pCheckStepProgDlgCancel)(void))
{
	name = "";

	unzFile zfile = unzOpen(path);
	if(zfile == NULL)
		return FALSE;
	unz_global_info global_info;
	if(unzGetGlobalInfo(zfile, &global_info) != UNZ_OK){
		unzClose(zfile);
		return FALSE;
	}

    if(unzGoToFirstFile(zfile) != UNZ_OK){
		unzClose(zfile);
		return FALSE;
	}

	if(global_info.number_entry > 1){
		CZipSelDlg dlg(zfile, global_info.number_entry);
		if(dlg.DoModal() == IDCANCEL){
			unzClose(zfile);
			return FALSE;
		}
		name = dlg.GetSelected();
		if(name.GetLength() <= 0){
			unzClose(zfile);
			return FALSE;
		}
		unz_file_info file_info;
		char filename[_MAX_PATH];
		for(unsigned i = 0;i < global_info.number_entry;i++){
			unzGetCurrentFileInfo(zfile, &file_info, filename, _MAX_PATH, NULL, 0, NULL, 0);
			if(name == filename)
				break;
			unzGoToNextFile(zfile);
		}
	}
	if(unzOpenCurrentFile(zfile) != UNZ_OK){
		unzClose(zfile);
		return FALSE;
	}

	long length = 0;
	int pcnt, pcnt_now;
	pcnt = pcnt_now = 0;
 
	if(pStepProgress){
		unz_file_info info;
		if(unzGetCurrentFileInfo(zfile, &info, NULL, 0, NULL, 0, NULL, 0 ) != UNZ_OK){
			unzClose(zfile);
			return FALSE;
		}
        length = info.uncompressed_size;
	}

	BOOL anyFault = FALSE;
	long readSize = 0;
	m_intCoordIdx = 0;

	while(1){
		if(pCheckStepProgDlgCancel){
			if(pCheckStepProgDlgCancel()){
				anyFault = TRUE;
				break;
			}
		}
        if(pStepProgress){
            pcnt_now = (int)(100.0 * readSize / length);
            if(pcnt_now >= pcnt + step_pcnt){
                pStepProgress();
                pcnt = pcnt_now;
            }
        }
		unsigned len = unzReadCurrentFile(zfile, (void*)m_ucharBuf, GDS_HEADER_LEN);
		readSize += len;
		if(len < GDS_HEADER_LEN)
			break;

		m_uintRecordLength  = (((unsigned int) *(m_ucharBuf + 0)) << 8);
		m_uintRecordLength += (unsigned int) *(m_ucharBuf + 1);
		if(m_uintRecordLength == 0)
			break;
		m_uintRecordLength -= GDS_HEADER_LEN;

		m_uintRecordType = *(m_ucharBuf + 2);
		m_uintDataType = *(m_ucharBuf + 3);

		len = unzReadCurrentFile(zfile, (void*)m_ucharBuf, m_uintRecordLength);
		readSize += len;
		if(len < m_uintRecordLength)
			break;
		if(DecodeRecord() == FALSE){
			anyFault = TRUE;
			break;
		}
	}

	unzCloseCurrentFile(zfile);
	unzClose(zfile);

	if(anyFault)
		return FALSE;
	return TRUE;
}

BOOL CGDS::ReadGDSGZip(CString path, void (*pStepProgress)(void),  
				   int step_pcnt, BOOL (*pCheckStepProgDlgCancel)(void))
{
	gzFile zfile = gzopen(path, "rb");
	if(zfile == NULL)
		return FALSE;

	long length = 0;
	int pcnt, pcnt_now;
	pcnt = pcnt_now = 0;
	if(pStepProgress){
		CWaitCursor cursor;
		BYTE* buf = new BYTE[ARCHIVE_CACHE_SIZE];
		length = 0;
		while(1){
			unsigned len = gzread(zfile, (void*)buf, ARCHIVE_CACHE_SIZE);
			if(len > 0)
				length += len;
			else
				break;
		}
		gzrewind(zfile);
		delete [] buf;
	}
 
	BOOL anyFault = FALSE;
	long readSize = 0;
	m_intCoordIdx = 0;

	while(1){
		if(pCheckStepProgDlgCancel){
			if(pCheckStepProgDlgCancel()){
				anyFault = TRUE;
				break;
			}
		}
        if(pStepProgress){
            pcnt_now = (int)(100.0 * readSize / length);
            if(pcnt_now >= pcnt + step_pcnt){
                pStepProgress();
                pcnt = pcnt_now;
            }
        }
		unsigned len = gzread(zfile, (void*)m_ucharBuf, GDS_HEADER_LEN);
		readSize += len;
		if(len < GDS_HEADER_LEN)
			break;

		m_uintRecordLength  = (((unsigned int) *(m_ucharBuf + 0)) << 8);
		m_uintRecordLength += (unsigned int) *(m_ucharBuf + 1);
		if(m_uintRecordLength == 0)
			break;
		m_uintRecordLength -= GDS_HEADER_LEN;

		m_uintRecordType = *(m_ucharBuf + 2);
		m_uintDataType = *(m_ucharBuf + 3);

		len = gzread(zfile, (void*)m_ucharBuf, m_uintRecordLength);
		readSize += len;
		if(len < m_uintRecordLength)
			break;
		if(DecodeRecord() == FALSE){
			anyFault = TRUE;
			break;
		}
	}

	gzclose(zfile);

	if(anyFault)
		return FALSE;
	return TRUE;
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
	BYTE* ptr = (BYTE*)&mantissa;

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

unsigned CGDS::DecodeBitArray()
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
	BYTE* ptr = (BYTE*)&ret_val;

	for(unsigned i = 0;i < len;i++)
		*(ptr + len - i - 1) = *(m_ucharBuf + m_uintBufPtr++);
	

	return (int)ret_val;
}

BOOL CGDS::DecodeAscii(CString* set_to, unsigned len)
{
	char *cptr = m_charBuf;

	if(len == 0)
		len = m_uintRecordLength;
	else if(m_uintRecordLength - m_uintBufPtr < len)
		return FALSE;

	for(unsigned i = 0;i < len;i++){
		if(*(m_ucharBuf + m_uintBufPtr) == 0x00)
			break;
		*cptr++ = *(m_ucharBuf + m_uintBufPtr++);
	}
	*cptr = '\0';
	if(set_to)
		*set_to = m_charBuf;


	return TRUE;
}

BOOL CGDS::DecodeDateTime(struct tm *tm_set)
{
	const unsigned length = 12;

	if(m_uintRecordLength - m_uintBufPtr < 0)
		return FALSE;

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
	return TRUE;
}

BOOL CGDS::ValidateXY(int elementtype, int num_xy)
{
	switch(elementtype){
	case GDS_PATH:
		if(num_xy >= 2 && num_xy <= GDS_MAX_COORD)
			return TRUE;
		break;
	case GDS_BOUNDARY:
		if(num_xy >= 4 && num_xy <= GDS_MAX_COORD){
			if(*(m_aryCoordBuf + 0) == *(m_aryCoordBuf + num_xy - 1))
				m_intCoordIdx--;// reduce memory usage
			return TRUE;
		}
		break;
	case GDS_TEXT:
	case GDS_CONTACT:
	case GDS_SREF:
		if(num_xy == 1)
			return TRUE;
		break;
	case GDS_NODE:
		if(num_xy >= 1 && num_xy <= 50)
			return TRUE;
		break;
	case GDS_BOX:
		if(num_xy == 5){
			if(*(m_aryCoordBuf + 0) == *(m_aryCoordBuf + num_xy - 1)){
				m_intCoordIdx--;// reduce memory usage
				return TRUE;
			}
		}
		break;
	case GDS_AREF:
		if(num_xy == 3)
			return TRUE;
		break;
	default:
		break;
	}

	return FALSE;
}


BOOL CGDS::DecodeRecord()
{
	long int     int_val;
	double       double_val;
	CiCoord      coord;

	CGDSStructure* structure = GetActiveStructure();
	CGDSElement* element = NULL;
	if(structure)
		element = structure->GetLastElement();

	m_uintBufPtr = 0;

	switch(m_uintRecordType){
	////////// ELEMENT COMPONENT
	case GDS_XY :
		if(element == NULL)
			break; 
		for(int_val = 0;(unsigned)int_val < m_uintRecordLength/8;int_val++){
			coord.x = DecodeInteger(); // 4byte integer
			coord.y = DecodeInteger();
			if(m_intCoordIdx > 1 && (m_aryCoordBuf + m_intCoordIdx - 1)->Equals(coord))
				continue; // remove redundunt coordinate
			(m_aryCoordBuf + m_intCoordIdx++)->SetCoord(coord);
		}
		if(m_intCoordIdx > 0 && ValidateXY(element->GetElementType(), m_intCoordIdx)){
			((CGDSXY*)element)->LoadCoords(m_aryCoordBuf, m_intCoordIdx);
			m_ulNumXYCoord += m_intCoordIdx;
			m_ulNumXY++; 
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
		int_val = DecodeInteger();
		element->SetPlex(int_val);
		break;

	case GDS_ELFLAGS :
		if(element == NULL)
			break;
		int_val = DecodeBitArray();
		element->SetElflags(int_val);
		break;

	case GDS_WIDTH :
		if(element == NULL)
			break;
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
		int_val = DecodeBitArray();
		((CGDSStrans*)element)->SetStrans(int_val);
		break;

	case GDS_MAG :
		if(element == NULL)
			break;
		double_val = DecodeReal();
		element->SetMag(double_val);
		break;

	case GDS_ANGLE :
		if(element == NULL)
			break;
		double_val = DecodeReal();
		element->SetAngle(double_val);
		break;

	case GDS_PATHTYPE :
		if(element == NULL)
			break;
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
		int_val = DecodeInteger();
		element->SetBgnExtn(int_val);
		break;

	case GDS_ENDEXTN :
		if(element == NULL)
			break;
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

	////////// STRUCTURE 
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
			element->AddProperty(m_intPropAttr, m_strPropVal.GetBuffer(0));
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
		return FALSE;
		break;
	}
 
	return TRUE;
}

void CGDSStructures::AddStructure()
{
	m_structActive = new CGDSStructure;
	m_obaryStructures.Add(m_structActive); 
}

CGDSStructure* CGDSStructures::FindStructure(char* name)
{
	if(!name || *name == '\0')
		return NULL;

	for(int pos = 0;pos < m_obaryStructures.GetSize();pos++){
		CGDSStructure* ptr = (CGDSStructure*)m_obaryStructures.GetAt(pos);
		if(strcmp(ptr->GetStructureName(), name) == 0)
			return ptr;
	}
		
	return NULL;
}

CGDSStructure::~CGDSStructure()
{
	if(m_strStrName)
		delete [] m_strStrName;

	for(int pos = 0;pos < m_obaryRefStructures.GetSize();pos++){
		CGDSRefStructure* ptr = (CGDSRefStructure*)m_obaryRefStructures.GetAt(pos);
		delete ptr;
	}
	m_obaryRefStructures.RemoveAll();
}

CGDSStructures::~CGDSStructures()
{
	for(int pos = 0;pos < m_obaryStructures.GetSize();pos++){
		CGDSStructure* ptr = (CGDSStructure*)m_obaryStructures.GetAt(pos);
		delete ptr;
	}
	m_obaryStructures.RemoveAll();
}

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
	m_obaryElements.AddTail(p);
}

void CGDSElements::DeleteElement(CGDSElement* p)
{
	if(!p)
		return;

	POSITION pos = m_obaryElements.GetHeadPosition();
	while(pos){
		CGDSElement* ptr = (CGDSElement*)m_obaryElements.GetAt(pos);
		if(ptr == p){
			delete p;
			m_obaryElements.RemoveAt(pos);
			break;
		}
		m_obaryElements.GetNext(pos);
	}
}


CGDSElements::~CGDSElements()
{
	POSITION pos = m_obaryElements.GetHeadPosition();
	while(pos){
		CGDSElement* ptr = (CGDSElement*)m_obaryElements.GetNext(pos);
		delete ptr;
	}
	m_obaryElements.RemoveAll();

}

void CGDSFormatType::AddFormatType(CString str)
{
	CMask* p = new CMask;
	p->SetMaskName(str);
	m_obaryMasks.Add(p);
}

CGDSFormatType::~CGDSFormatType()
{
	CMask* ptr;

	for(int pos = 0;pos < m_obaryMasks.GetSize();pos++){
		ptr = (CMask*)m_obaryMasks.GetAt(pos);
		delete ptr;
	}
	m_obaryMasks.RemoveAll();
}

CGDSXY::~CGDSXY()
{
	if(m_coordList)
		delete [] m_coordList;
}

CGDSElement::~CGDSElement()
{
	//if(m_pProperties)
	//	delete m_pProperties;
}

void CGDSElement::SetLayer(unsigned int layer)
{
	if(layer < 0 || layer >= GDS_MAX_LAYER)
		return;
	m_uintLayer = layer;
	return;
}

void CGDSElement::SetPlex(int plex)
{
//	static double lim = pow(2.0, 24.0);
//	m_intPlex = plex;
	return;
}

void CGDSElement::SetElflags(int elflags)
{
//	if(elflags & 0x01)
//		m_intAttrib |= GDS_ELEMENT_TEMPLATE;
//	if(elflags & 0x02)
//		m_intAttrib |= GDS_ELEMENT_TEMPLATE;
	return;
}


void CGDSBoundary::SetDataType(int type)
{
	if(type < 0 || type > GDS_MAX_DATA_TYPE)
		return;
	m_uintDataType = type;
}

void CGDSPath::SetDataType(int type)
{
	if(type < 0 || type > GDS_MAX_DATA_TYPE)
		return;
	m_uintDataType = type;
	return;
}

void CGDSPath::LoadCoords(CiCoord* list, int num)
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

void CGDSText::SetWidth(int width)
{
	m_intWidth = width; 
}

void CGDSText::SetPresentation(int bits)
{
	m_uintFont     = (bits & 0x0030) >> 4;
	m_uintVertJust = (bits & 0x000c) >> 2;
	m_uintHorzJust = (bits & 0x0003);
}

void CGDSBox::SetBoxType(int type)
{
	if(type >= 0 && type <= GDS_MAX_DATA_TYPE){
		m_uintBoxType = type;
		return;
	}
	return;
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

void CGDSNode::SetNodeType(int type)
{
	if(type >= 0 && type <= GDS_MAX_DATA_TYPE)
		m_uintNodeType = type;
}



BOOL CGDShcy::CalculateDimension()
{
	BOOL broken_link = FALSE;

	int step = 5;
	CProgressDlg dlg(0);
	dlg.Create(AfxGetApp()->GetMainWnd());
	dlg.SetStep(step);
	dlg.SetStatus("Calculating dimension ...");
	dlg.DisableCancel();


	CObArray* structures = GetStructureList();
	for(int pos = 0;pos < structures->GetSize();pos++){
		m_structActive = (CGDSStructure*)structures->GetAt(pos);
		if(!m_structActive)
			continue;
		if(m_structActive->GetStructureName() == NULL){
			structures->RemoveAt(pos);
			delete m_structActive;
			continue;
		}
		m_structActive->CalcDimension(this, &broken_link);//, &err);
	}
	double pcnt_now, pcnt = 0.0;
	for(pos = 0;pos < structures->GetSize();pos++){
		pcnt_now = (int)(100.0 * pos / structures->GetSize());
		if(pcnt_now >= pcnt + step){
			dlg.StepIt();
			pcnt = pcnt_now;
		}
		m_structActive = (CGDSStructure*)structures->GetAt(pos);
		if(!m_structActive)
			continue;
		CalcDimensionRecurse(m_structActive, CCoord(0, 0), FALSE, 0.0, 1.0);
	}
	dlg.StepIt();
	dlg.DestroyWindow();

	int nowarn = AfxGetApp()->GetProfileInt("Misc", "No warning on broken link", 0);
	if(broken_link && !nowarn){
		MessageBox(AfxGetApp()->GetMainWnd()->m_hWnd, "Broken link detected!", "Warning", MB_ICONEXCLAMATION | MB_OK);
	}

	return TRUE;
}


int CGDSStructure::GetNumRefStructure()
{
	return m_obaryRefStructures.GetSize();
}

CGDSRefStructure* CGDSStructure::FindRefStructure(CString name)
{
	CGDSRefStructure* str;

	for(int pos = 0;pos < m_obaryRefStructures.GetSize();pos++){
		str = (CGDSRefStructure*)m_obaryRefStructures.GetAt(pos);
		if(str->GetName() == name)
			return str;
	}
	return NULL;
}

CGDSRefStructure* CGDSStructure::FindRefStructure(CGDSStructure* structure)
{
	if(!structure)
		return NULL;

	CGDSRefStructure* str;

	for(int pos = 0;pos < m_obaryRefStructures.GetSize();pos++){
		str = (CGDSRefStructure*)m_obaryRefStructures.GetAt(pos);
		if(str && str->GetStructure() == structure)
			return str;
	}
	return NULL;
}

void CGDSStructure::AddRefStructure(CGDSStructure* str)
{
	CGDSRefStructure* refstructure;
	for(int pos = 0;pos < m_obaryRefStructures.GetSize();pos++){
		refstructure = (CGDSRefStructure*)m_obaryRefStructures.GetAt(pos);
		if(refstructure->GetStructure() == str)
			return;
	}

	refstructure = new CGDSRefStructure;
	refstructure->SetStructure(str);
	refstructure->SetName(str->GetStructureName());

	m_obaryRefStructures.Add(refstructure);
}

void CGDSStructure::CalcDimension(CGDSStructures* strlist, BOOL* broken_link, CString* err)
{
	CCoord min, max;
	BOOL flg = FALSE;

	POSITION pos = m_obaryElements.GetHeadPosition();
	CGDSElement* element;
	CGDSStructure* ref; 
	while(pos){
		element = (CGDSElement*)m_obaryElements.GetNext(pos);
		switch(element->GetElementType()){
		case GDS_SREF:
		case GDS_AREF:
			ref = strlist->FindStructure(element->GetSname());
			if(ref){
				element->SetRefStructure(ref);
				AddRefStructure(ref);
			}
			else{
				if(err){
					CString msg;
					msg.Format("%s=>%s\n", GetStructureName(), element->GetSname());
					*err += msg;
				}
				*broken_link = TRUE;
			}
			break;
		case GDS_TEXT:
			break;
		default:
			GetElementMinMax(element, min, max);
			if(!flg){
				m_coordDimMin = min;
				m_coordDimMax = max;
				flg = TRUE;
			}
			if(m_coordDimMin.x > min.x)
				m_coordDimMin.x = min.x;
			if(m_coordDimMin.x > max.x)
				m_coordDimMin.x = max.x;

			if(m_coordDimMax.x < max.x)
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

}

void CGDShcy::CalcDimensionRecurse(CGDSStructure* structure, CCoord pos, BOOL reflect, double angle, double mag)
{
	CGDSStructure* refStructure;
	CCoord coord;
	double eangle, emag;
	BOOL   ereflect;

	if(!structure)
		return;

	CObList* elements = structure->GetElementList();
	POSITION idx = elements->GetHeadPosition();
	CGDSElement* element;
	while(idx){
		element = (CGDSElement*)elements->GetNext(idx);
		if(!element)
			continue;
		switch(element->GetElementType()){
		case GDS_SREF:
			refStructure = element->GetRefStructure();
			if(!refStructure)
				break;
			element->GetPos(&coord);
			if(reflect)
				coord.y *= -1;
    		RotatePoint(&coord, angle);
			coord += pos;

			((CGDSSREF*)element)->GetMag(&emag);
			emag *= mag;
			ereflect = reflect ^ (((CGDSStrans*)element)->IsReflect());
			if(ereflect != ((CGDSStrans*)element)->IsReflect())
				eangle = angle - ((CGDSSREF*)element)->GetAngle();
			else
				eangle = angle + ((CGDSSREF*)element)->GetAngle();
			fmod360(eangle);
			CalcDimensionRecurse(refStructure, coord, ereflect, eangle, emag);
			break;
		case GDS_AREF:
			refStructure = element->GetRefStructure();
			if(!refStructure)
				break;
			{
			int numRow, numCol;
			CCoord coordOrg, coordCol, coordRow;
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

			double xstep = (coordCol.x - coordOrg.x)/(double)numCol;
			double ystep = (coordRow.y - coordOrg.y)/(double)numRow;
			if(xstep == 0.0 && ystep == 0.0){
				int tmp = numCol;
				numCol = numRow;
				numRow = tmp;
				xstep = (coordRow.x - coordOrg.x)/(double)numCol;
				ystep = (coordCol.y - coordOrg.y)/(double)numRow;
			}
			((CGDSAREF*)element)->GetMag(&emag);
			emag *= mag;
			ereflect = reflect ^ ((CGDSStrans*)element)->IsReflect();
			if(ereflect != ((CGDSStrans*)element)->IsReflect())
				eangle = angle - ((CGDSSREF*)element)->GetAngle();
			else
				eangle = angle + ((CGDSSREF*)element)->GetAngle();
			fmod360(eangle);
			for(int i = 0;i < numRow;i += numRow > 1 ? numRow - 1 : 1){
				for(int j = 0;j < numCol;j += numCol > 1 ? numCol - 1 : 1){
					coord.SetCoord(coordOrg.x + j * xstep, coordOrg.y + i * ystep);
    				RotatePoint(&coord, angle);
					coord += pos;
					CalcDimensionRecurse(refStructure, coord, ereflect, eangle, emag);
				}
			}
			}
			break;
		default:
			{
			CCoord cmin, cmax;
			m_structActive->SetVisibleOnLayer(element->GetLayer());
			structure->SetVisibleOnLayer(element->GetLayer());
			structure->GetDimMin(&cmin);
			structure->GetDimMax(&cmax);
			coord.SetCoord(pos.x, pos.y);
			if(reflect){
				cmin.y *= -1.0;
				cmax.y *= -1.0;
			}
    		RotatePoint(&cmin, angle);
    		RotatePoint(&cmax, angle);
			cmin *= mag;
			cmax *= mag;
			cmin += coord;
			cmax += coord;

			CCoord* min = m_structActive->GetDimMinP();
			CCoord* max = m_structActive->GetDimMaxP();
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

void CGDShcy::fmod360(double& angle)
{
	if(angle >= 360.0)
		angle -= 360.0;
	else if(angle <= -360.0)
		angle += 360.0;
}

void CGDShcy::RotatePoint(CCoord* point, double& angle)
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

void CGDSStructure::GetElementMinMax(CGDSElement *element, CCoord& min, CCoord& max)
{
	CCoord coord;

	for(int i = 0;i < ((CGDSXY*)element)->GetNumCoords();i++){
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
}




void CGDSStructures::MakeCrossRef()
{
	CGDSStructure* structure;
	CGDSElement* element;
	CObList* elements;
	POSITION idx;


	int step = 5;
	CProgressDlg dlg(0);
	dlg.Create(AfxGetApp()->GetMainWnd());
	dlg.SetStep(step);
	dlg.SetStatus("Making AREF/SREF table ...");
	dlg.DisableCancel();

	double pcnt_now, pcnt = 0.0;
	for(int pos = 0;pos < m_obaryStructures.GetSize();pos++){
		structure = (CGDSStructure*)m_obaryStructures.GetAt(pos);

		pcnt_now = (int)(100.0 * pos / m_obaryStructures.GetSize());
		if(pcnt_now >= pcnt + step){
			dlg.StepIt();
			pcnt = pcnt_now;
		}

		elements = structure->GetElementList();
		idx = elements->GetHeadPosition();
		while(idx){
			element = (CGDSElement*)elements->GetNext(idx);
			if(!element)
				continue;
			switch(element->GetElementType()){
			case GDS_SREF:
			case GDS_AREF:
				if((structure = FindStructure(element->GetSname())) == NULL)
					break;
				structure->MarkRefered();
				break;
			default:
				break;
			}
		}
	}
	dlg.StepIt();
	dlg.DestroyWindow();
}

CGDSStructure* CGDSStructures::FindTopStructure()
{
	CGDSStructure* str_found = NULL;

	double s_max, s_now;
	CCoord min_now, max_now;
	CGDSStructure* structure;
	for(int idx = 0;idx < m_obaryStructures.GetSize();idx++){
		if(!(structure = (CGDSStructure*)m_obaryStructures.GetAt(idx)))
			continue;
		if(!structure->IsRefered()){
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
