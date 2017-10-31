#if !defined(AFX_DLGLAYER_H__E9E6EBEC_21A1_472A_8C26_15AB8C4C6E76__INCLUDED_)
#define AFX_DLGLAYER_H__E9E6EBEC_21A1_472A_8C26_15AB8C4C6E76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GDShcy.h"
#include "GdsDoc.h"
#include "LayerInfo.h"


class CGdsView;

class CDlgLayer : public CDialog
{
	BOOL* m_pEnableSwitch;
	CGdsView* m_pCaller;

	int m_intWindowWidth;
	int m_intClientWidth;

	BOOL m_bDragging;
	CPoint m_pointDrag;
	CImageList* m_pDragImage;
	int m_intDragItem;
	int m_intDragTo;

public:
	void EndDragItem();
	void DragItem();
	void UpdateItemCheck();
	void MakeListControl();
	void FillPattern(CDC* pDC, CRect rect, CLayerSet* pLayer);
	void AdjustWindowSize();
	void SetEnableSwitch(BOOL* pSwitch)
	{
		m_pEnableSwitch = pSwitch;
	}
	void ExitDialog()
	{
		*m_pEnableSwitch = FALSE;
		CDialog::OnCancel();
	}
	void OnCancel()
	{
		if(m_pLayerInEdit)
			OnKillfocusEditLayerName();
		else
			ExitDialog();
	}
	void CheckAdditionalLayer();
	int GetItemIndex(int index);
	void SetTecFilePath(CString path){m_strTecFileName = path;}
	CString GetTecFilePath(){return m_strTecFileName;}

	void Sort();
	void OnOK()
	{
		OnApply();
	}
	void OnApply();
	void ShowPage();
	CDlgLayer(CWnd* pParent = NULL);
	~CDlgLayer();
	void SetLayerAndColor(int* order, CString* names, BOOL* stat, COLORREF* colors, int* fill, 
										BOOL* exist, BOOL* tec);
	void OnChangeEditLayer(int idx);
	void SetCaller(CGdsView* caller){m_pCaller = caller;}
	void SetHatchDensity(double val)
	{
		if(val > 100.0)
			val = 100.0;
		else if(val < 0.0)
			val = 0.0;
		m_intDensity = (int)val;
	}
	double GetHatchDensity(){return m_intDensity;}
	//{{AFX_DATA(CDlgLayer)
	enum { IDD = IDD_LAYER_DLG };
	CEdit	m_editLayerName;
	CListCtrl	m_listCtrlLayer;
	CButton	m_ctrlHideNonExist;
	CButton	m_ctrlSaveNonExist;
	CEdit	m_editTecFileName;
	CSliderCtrl	m_sliderDensity;
	int		m_intDensity;
	CString	m_strTecFileName;
	BOOL	m_bHideNonExisting;
	BOOL	m_bSaveNonExisting;
	CString	m_strNumLayer;
	CString	m_strEditLayerName;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CDlgLayer)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CDlgLayer)
	afx_msg void OnAllOff();
	afx_msg void OnAllOn();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSaveLayer();
	afx_msg void OnLoadLayer();
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomdrawSliderDensity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnCheckHideNonexist();
	afx_msg void OnLayerApply();
	afx_msg void OnClickLayerList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditLayerName();
	afx_msg void OnCloseLayerSetup();
	afx_msg void OnKillfocusEditLayerName();
	afx_msg void OnBegindragLayerList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnCustomdrawList( NMHDR* pNMHDR, LRESULT* pResult );
	DECLARE_MESSAGE_MAP()

private:
	// Pointers to original
	CString*	m_pStrNameList;
	BOOL*		m_pBShowList;
	COLORREF*	m_pColorColorList;
	int*		m_pIntFillList;
	int*		m_pIntOrderList;
	BOOL*		m_pBoolLayerExistList;
	BOOL*	    m_pBoolLayerInTecList;
	CLayerSet*  m_pLayerInEdit;

	CLayerInfo	m_layerInfo;
	CLayerSet   m_arrayLayers[GDS_MAX_LAYER];
	BOOL      m_bLayerInTec[GDS_MAX_LAYER];
	int			m_intNumLayer;
	int			m_intNumLayerTec;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DLGLAYER_H__E9E6EBEC_21A1_472A_8C26_15AB8C4C6E76__INCLUDED_)
