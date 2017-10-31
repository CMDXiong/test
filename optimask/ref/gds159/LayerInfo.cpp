//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gds.h"
#include "LayerInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CLayerInfo::CLayerInfo()
{

}

CLayerInfo::~CLayerInfo()
{

}

BOOL CLayerInfo::ReadLayerSetup(CLayerSet* pSet, CString path)
{

	if(path.GetLength() == 0){
		CString filt;
		filt.Format("TechnologyMap Files (*.%s)|*.%s|All Files (*.*)|*.*||", DEFAULT_EXTENSION, DEFAULT_EXTENSION);
		CFileDialog dlg(TRUE, DEFAULT_EXTENSION, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filt, NULL);

		if(dlg.DoModal() == IDCANCEL)
			return FALSE;
		path = dlg.GetPathName();
	}

	CFile file;
	if(!file.Open(path, CFile::modeRead))
		return -1;
	CArchive archive(&file, CArchive::load);

	m_strTecFilePath = path;

	CString line, word;
	int idx, length;

	int visible = -1;
	int layernum = -1;
	int layerpat = -1;
	CString layername = "NOSUCHNAME";
	COLORREF layercolor = -1;

	for(int i = 0;i < GDS_MAX_LAYER;i++){
		(pSet + i)->m_bValid = FALSE;
		(pSet + i)->m_intOrder = -1;
	}

	int order_idx = 0;
	while(archive.ReadString(line)){
		length = line.GetLength();
		if((idx = line.Find("Layer")) >= 0 && 
			(idx = line.Find("=", idx)) >= 0 && idx < length - 1){
			word = line.Right(length - idx - 1);
			layernum = atoi(word);
		}
		if((idx = line.Find("Color")) >= 0 && 
			(idx = line.Find("=", idx)) >= 0 && idx < length - 1){
			word = line.Right(length - idx - 1);
			layercolor = atol(word);
		}
		if((idx = line.Find("Pattern")) >= 0 && 
			(idx = line.Find("=", idx)) >= 0 && idx < length - 1){
			word = line.Right(length - idx - 1);
			layerpat = atoi(word);
		}
		if((idx = line.Find("Name")) >= 0 && 
			(idx = line.Find("=", idx)) >= 0 && idx < length - 1){
			layername = line.Right(length - idx - 1);
			if((idx = layername.Find('"')) >= 0)
				layername = layername.Right(layername.GetLength() - idx - 1);
			if((idx = layername.ReverseFind('"')) >= 0)
				layername = layername.Left(idx);
			layername = layername;
		}
		if((idx = line.Find("Visible")) >= 0 && 
			(idx = line.Find("=", idx)) >= 0 && idx < length - 1){
			word = line.Right(length - idx - 1);
			visible = atoi(word);
		}

		if(layernum >= 0 && layernum < GDS_MAX_LAYER &&
			layerpat >= 0 && layername != "NOSUCHNAME" && layercolor >= 0){
			for(int i = 0;i < GDS_MAX_LAYER;i++){
				if((pSet + i)->m_intNumber == layernum)
					break;
			}
			if(i < 0 || i >= GDS_MAX_LAYER)
				continue;
			(pSet + i)->m_color = layercolor;
			(pSet + i)->m_intFill = layerpat;
			(pSet + i)->m_strName = layername;


			if(visible > 0)
				(pSet + i)->m_bShow = TRUE;
			else
				(pSet + i)->m_bShow = FALSE;
			(pSet + i)->m_intOrder = order_idx++;
			(pSet + i)->m_bValid = TRUE;

			visible = -1;
			layernum = -1;
			layerpat = -1;
			layername = "NOSUCHNAME";
			layercolor = -1; 
		}
	}

	archive.Close();
	file.Close();

	return TRUE;
}


BOOL CLayerInfo::SaveLayerSetup(CLayerSet* pSet, CString path, BOOL* storeList)
{
	if(path.GetLength() == 0){
		if(m_strTecFilePath.GetLength() > 0)
			path = m_strTecFilePath;
		CString filt;
		filt.Format("TechnologyMap Files (*.%s)|*.%s|All Files (*.*)|*.*||", DEFAULT_EXTENSION, DEFAULT_EXTENSION);
		CFileDialog dlg(FALSE, DEFAULT_EXTENSION, path, OFN_OVERWRITEPROMPT, filt, NULL);
		if(dlg.DoModal() == IDCANCEL)
			return FALSE;
		path = dlg.GetPathName();
	}


	CFile file;
	file.Open(path, CFile::modeCreate | CFile::modeWrite);
	CArchive archive(&file, CArchive::store);

	CString line;
	for(int i = 0;i < GDS_MAX_LAYER;i++){
		if(! *(storeList + (pSet + i)->m_intNumber) && ! (pSet + i)->m_bValid)
			continue;
		line.Format("Layer = %d\r\n", (pSet + i)->m_intNumber);
		archive.WriteString(line);

		line.Format("Visible = %d\r\n", (pSet + i)->m_bShow ? 1 : 0);
		archive.WriteString(line);
		line.Format("Color = %ld\r\n", (pSet + i)->m_color);
		archive.WriteString(line);
		line.Format("Pattern = %d\r\n", (pSet + i)->m_intFill);
		archive.WriteString(line);
		line.Format("Name = \"%s\"\r\n", (pSet + i)->m_strName);
		archive.WriteString(line);
	}
	archive.Close();
	file.Close();
	m_strTecFilePath = path;
	return TRUE;
}

