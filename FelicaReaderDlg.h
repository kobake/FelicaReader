
// FelicaReaderDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"


// CFelicaReaderDlg �_�C�A���O
class CFelicaReaderDlg : public CDialogEx{
public:
	enum { IDD = IDD_FELICAREADER_DIALOG };
	
public:
	CFelicaReaderDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

	// �C�x���g
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();

protected:
	// �V�X�e���C�x���g
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

private:
	// �A�C�R��
	HICON m_hIcon;

	// �R���g���[���Q
	CEdit m_edtInfo;
};
