#pragma once

#include "Resource.h"

// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O
class CAboutDlg : public CDialogEx{
public:
	enum { IDD = IDD_ABOUTBOX };

public:
	CAboutDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	DECLARE_MESSAGE_MAP()
};

