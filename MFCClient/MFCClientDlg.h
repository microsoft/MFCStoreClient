// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// CMFCClientDlg dialog
class CMFCClientDlg : public CDialogEx
{
// Construction
public:
	CMFCClientDlg(CWnd* pParent = nullptr);	// standard constructor
    ~CMFCClientDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCLIENT_DIALOG };
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

    static void StorePurchaseCallback(int error, const wchar_t* message, void* userData);
    static void StoreLicenseStateChangedCallback(int error, const wchar_t* message, void* userData);
    static void StoreLicenseStateCallback(int error, const wchar_t* message, void* userData);
    static void StoreGetPriceCallback(int error, const wchar_t* message, void* userData);

    // WindowsStore DLL function pointers
    WinRT::WindowsStoreInitializeFunc m_windowsStoreInitFunc;
    WinRT::WindowsStorePurchaseFunc m_windowsStorePurchaseFunc;
    WinRT::WindowsStoreLicenseStateFunc m_windowsStoreLicenseStateFunc;
    WinRT::WindowsStoreGetPriceFunc m_windowsStoreGetPriceFunc;
    WinRT::WindowsStoreFreeFunc m_windowsStoreFreeFunc;
    WinRT::WindowsStorePtr m_storePtr;
    HINSTANCE m_dllHandle;
public:
    afx_msg void OnBnClickedCheckLicense();
    afx_msg void OnBnClickedBuy();
    afx_msg void OnLaunchWin32App();
};
