#ifndef __PROGDLG_H__
#define __PROGDLG_H__

class CProgressDlg : public CDialog
{
public:
    CProgressDlg(UINT nCaptionID = 0);
    ~CProgressDlg();

    BOOL Create(CWnd *pParent=NULL);

    BOOL CheckCancelButton();
    int GetStatus(LPTSTR lpszStringBuf, int nMaxCount );
    void SetStatus(LPCTSTR lpszMessage);
    void SetRange(int nLower,int nUpper);
    int  SetStep(int nStep);
    int  SetPos(int nPos);
    int  OffsetPos(int nPos);
    int  StepIt();
        
    //{{AFX_DATA(CProgressDlg)
	enum { IDD = CG_IDD_PROGRESS };
	CButton	m_ctrlCancel;
    CProgressCtrl	m_Progress;
	//}}AFX_DATA

    //{{AFX_VIRTUAL(CProgressDlg)
	public:
    virtual BOOL DestroyWindow();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	public:
		void DisableCancel();
    virtual void OnCancel();
protected:
	UINT m_nCaptionID;
    int m_nLower;
    int m_nUpper;
    int m_nStep;
	BOOL m_bEnableCancel;
    BOOL m_bCancel;
    BOOL m_bParentDisabled;

    void ReEnableParent();

    virtual void OnOK() {}; 
    void UpdatePercent(int nCurrent);
    void PumpMessages();

    //{{AFX_MSG(CProgressDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __PROGDLG_H__
