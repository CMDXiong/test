#if !defined(AFX_PRINTSCALEDLG_H__17A73A6C_E37B_4FF5_8A55_07563597341F__INCLUDED_)
#define AFX_PRINTSCALEDLG_H__17A73A6C_E37B_4FF5_8A55_07563597341F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CPrintScaleDlg : public CDialog
{
public:
	int DoModal(CRect* prect)
	{
		pRect = prect;
		return CDialog::DoModal();
	}
	void SetLegendAngle(double angle){m_dblLegendDir = angle;}
	double GetLegendAngle(){return m_dblLegendDir;}
	void SetLegendSize(double size){m_dblLegendSize = size * 1000.0;}
	double GetLegendSize(){return m_dblLegendSize / 1000.0;}
	BOOL IsColorPen()
	{
		return m_bUseColorPen;
	}
	void SetMisc(BOOL leg, BOOL sc, BOOL ft, BOOL colorpen)
	{
		m_bPrintLegend = leg;
		m_bPrintScale = sc;
		m_bFooter = ft;
		m_bUseColorPen = colorpen;
	}
	BOOL GetLegendSwitch(){return m_bPrintLegend;}
	BOOL GetScaleSwitch(){return m_bPrintScale;}
	BOOL GetFooterSwitch(){return m_bFooter;}
	REAL GetHatchPenWidth()
	{
		m_dblHatchPenWidth *= 1000.0;
		m_dblHatchPenWidth = (int)(m_dblHatchPenWidth + 0.5);
		m_dblHatchPenWidth /= 1000.0;
		return (REAL)(m_dblHatchPenWidth * 1.0e-3);
	}
	REAL GetHatchSpacing()
	{
		m_dblHatchSpacing *= 1000.0;
		m_dblHatchSpacing = (int)(m_dblHatchSpacing + 0.5);
		m_dblHatchSpacing /= 1000.0;
		return (REAL)(m_dblHatchSpacing * 1.0e-3);
	}
	REAL GetLineWidth()
	{
		m_dblLineWidth *= 1000.0;
		m_dblLineWidth = (int)(m_dblLineWidth + 0.5);
		m_dblLineWidth /= 1000.0;
		return (REAL)(m_dblLineWidth * 1.0e-3);
	}
	void SetHatch(REAL pw, REAL space)
	{
		m_dblHatchPenWidth = pw * 1.0e+3;
		m_dblHatchPenWidth *= 1000.0;
		m_dblHatchPenWidth = (int)(m_dblHatchPenWidth + 0.5);
		m_dblHatchPenWidth /= 1000.0;

		m_dblHatchSpacing = space * 1.0e+3;
		m_dblHatchSpacing *= 1000.0;
		m_dblHatchSpacing = (int)(m_dblHatchSpacing + 0.5);
		m_dblHatchSpacing /= 1000.0;
	}
	void SetLineWidth(REAL width)
	{
		m_dblLineWidth = width * 1.0e+3;

		m_dblLineWidth *= 1000.0;
		m_dblLineWidth = (int)(m_dblLineWidth + 0.5);
		m_dblLineWidth /= 1000.0;
	}
	BOOL GetShrink(){return m_bEnableShrink;}
	void SetShrinkRate(BOOL enable, double ratio)
	{
		if(ratio > 0.0)
			m_bEnableShrink = enable;
		else
			m_bEnableShrink = FALSE;
		m_dblShrink = ratio;
	}
	void OnCancel()
	{
		CDialog::OnCancel();
	}
	void SetPrintMode(int mode);
	void ShowRatio(BOOL enable=TRUE);
	void SetFitScale(double ratio, double ratio_area);
	double GetPrintScale(BOOL whole);
	int GetPrintMode();
	CPrintScaleDlg(CWnd* pParent = NULL);
	//{{AFX_DATA(CPrintScaleDlg)
	enum { IDD = IDD_PRINT_SCALE_DLG };
	CEdit	m_ctrlLineWidth;
	CSpinButtonCtrl	m_ctrlPenWidth;
	CComboBox	m_comboLegendPos;
	CEdit	m_editLegendSize;
	CSpinButtonCtrl	m_spinLegendSize;
	CComboBox	m_comboPenColor;
	CButton	m_buttonShrink;
	CSpinButtonCtrl	m_spinRatioArea;
	CEdit	m_editRatioArea;
	CButton	m_buttonAutoArea;
	CButton	m_radioFull;
	CButton	m_radioViewArea;
	CButton	m_buttonAuto;
	CEdit	m_editRatio;
	CSpinButtonCtrl	m_spinRatio;
	int		m_intRadioMode;
	UINT	m_uintRatio;
	int		m_intFull;
	int		m_intViewArea;
	UINT	m_uintRatioArea;
	BOOL	m_bEnableShrink;
	CString	m_strShrinkRate;
	double	m_dblLineWidth;
	CString	m_strShrink;
	BOOL	m_bPrintScale;
	BOOL	m_bPrintLegend;
	BOOL	m_bFooter;
	double	m_dblLegendSize;
	double	m_dblHatchPenWidth;
	double	m_dblHatchSpacing;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CPrintScaleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	CRect* pRect;
	CWnd* pParent;
	double m_dblShrink;
	BOOL m_bUseColorPen;
	double m_dblLegendDir;
	//{{AFX_MSG(CPrintScaleDlg)
	afx_msg void OnRadioWysiwyg();
	afx_msg void OnDeltaposSpinRatio(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioFull();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnButtonAuto();
	afx_msg void OnRadioScaling();
	afx_msg void OnRadioViewArea();
	afx_msg void OnButtonAutoArea();
	afx_msg void OnDeltaposSpinRatioArea(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckShrink();
	afx_msg void OnDeltaposSpinPenwidth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckLegend();
	afx_msg void OnDeltaposSpinLegSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboLegLoc();
	afx_msg void OnDeltaposSpinHatchpenwidth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinHatchspacing(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PRINTSCALEDLG_H__17A73A6C_E37B_4FF5_8A55_07563597341F__INCLUDED_)
