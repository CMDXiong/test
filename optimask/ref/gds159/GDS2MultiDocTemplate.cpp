#include "stdafx.h"
#include "gds.h"
#include "GDS2MultiDocTemplate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CGDS2MultiDocTemplate::~CGDS2MultiDocTemplate()
{

}

BOOL CGDS2MultiDocTemplate::GetDocString(CString& rString, enum DocStringIndex i) const
{
	CString strTemp,strLeft,strRight;
	int nFindPos;
	AfxExtractSubString(strTemp, m_strDocStrings, (int)i);

	int start = 0;
	if(i == CDocTemplate::filterExt){
		while((nFindPos = strTemp.Find(';', start)) >= 0){
			 strLeft = strTemp.Left(nFindPos+1);
			 strRight = "*" + strTemp.Right(lstrlen((const char*)strTemp) - nFindPos - 1);
			 strTemp = strLeft + strRight;
			 start = nFindPos + 1;
		}
	}
	rString = strTemp;
	return TRUE;
}

CDocTemplate::Confidence CGDS2MultiDocTemplate::MatchDocType(const char* pszPathName, CDocument*& rpDocMatch)
{
	ASSERT(pszPathName != NULL);
	rpDocMatch = NULL;

	POSITION pos = GetFirstDocPosition();
	while (pos != NULL)
	{
		CDocument* pDoc = GetNextDoc(pos);
		if (pDoc->GetPathName() == pszPathName) {
			rpDocMatch = pDoc;
			return yesAlreadyOpen;
		}
	}

	CString strFilterExt;
	if (GetDocString(strFilterExt, CDocTemplate::filterExt) &&
											  !strFilterExt.IsEmpty())
	{
		 ASSERT(strFilterExt[0] == '.');
		 CString ext;
		 int nDot = CString(pszPathName).ReverseFind('.');
		 const char* pszDot = nDot < 0 ? NULL : pszPathName + nDot;

		 int nSemi = strFilterExt.Find(';');
		 if(nSemi < 0){
			 if (nDot >= 0 && (lstrcmpi(pszPathName+nDot, strFilterExt)==0))
				 return yesAttemptNative;
		 }
		 else{
			 int start = 0;
			 while((nSemi = strFilterExt.Find(';', start)) >= 0){
				 ext = strFilterExt.Mid(start, nSemi - start);
				 if(ext.Find('*') >= 0)
					 ext = ext.Right(ext.GetLength() - 1);
				 if (nDot >= 0 && lstrcmpi(pszPathName+nDot, ext) == 0)
					 return yesAttemptNative;
				 start = nSemi + 1;
			 }
			 nSemi = strFilterExt.GetLength();
			 ext = strFilterExt.Mid(start, nSemi - start);
			 if(ext.Find('*') >= 0)
				 ext = ext.Right(ext.GetLength() - 1);
			 if (nDot >= 0 && lstrcmpi(pszPathName+nDot, ext) == 0)
				 return yesAttemptNative;
		 }
	}
	return yesAttemptForeign;
}
