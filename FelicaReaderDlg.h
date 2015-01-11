
// FelicaReaderDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CFelicaReaderDlg ダイアログ
class CFelicaReaderDlg : public CDialogEx{
public:
	enum { IDD = IDD_FELICAREADER_DIALOG };
	
public:
	CFelicaReaderDlg(CWnd* pParent = NULL);	// 標準コンストラクター

	// イベント
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();

protected:
	// システムイベント
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

private:
	// アイコン
	HICON m_hIcon;

	// コントロール群
	CEdit m_edtInfo;
};
