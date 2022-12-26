
// AnnoPythonAPIToolDlg.h : header file
//

#pragma once


// CAnnoPythonAPIToolDlg dialog
class CAnnoPythonAPIToolDlg : public CDialogEx
{
// Construction
public:
	CAnnoPythonAPIToolDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANNOPYTHONAPITOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CListBox ListBox1;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	CComboBox ComboBox1;
	afx_msg void OnBnClickedButton8();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedButton9();
	CButton CheckBox1;
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton12();
};
