#include "stdafx.h"
#include "FelicaReader.h"
#include "FelicaReaderDlg.h"
#include "afxdialogex.h"
#include "CAboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFelicaReaderDlg::CFelicaReaderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFelicaReaderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFelicaReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edtInfo);
}

BEGIN_MESSAGE_MAP(CFelicaReaderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFelicaReaderDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CFelicaReaderDlg ���b�Z�[�W �n���h���[

BOOL CFelicaReaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CFelicaReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CFelicaReaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CFelicaReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include <string>
#include <iostream>
#include <sstream>

#if 0
#pragma comment(lib ,"felica_nfc_library.lib")
#include "felica_nfc_library.h"
// �G���[�R�[�h�̒�`
// �EFELICA_NFC_E_DRIVER_ERROR 0x80080000 �h���C�o�ŃG���[���������܂���
//
// �h���C�o�G���[�R�[�h
// �E0x00000032 API �̔��s�V�[�P���X������������܂���B
std::wstring GetFelicaErrorInfo()
{
	unsigned long e[2];
	felicalib_nfc_get_last_error(e);
	std::wostringstream ret;
	char buf[64];
	sprintf(buf, "0x%08X, 0x%08X", e[0], e[1]);
	ret << buf;
	//ret << e[0] << ", " << e[1];
	return ret.str();
}

// �Q�l�Fhttp://codezine.jp/article/detail/7402?p=2
std::wstring GetFelicaInfo()
{
	//std::wstringbuf retbuf;
	//std::wostream ret(&retbuf);
	std::wostringstream ret;//(&retbuf);

	felicalib_nfc_start_logging("C:\\felica_test.log");
	
	// ���C�u�����̏�����
	bool bSuccess;
	bSuccess = felicalib_nfc_initialize();
	if(!bSuccess){
		ret << "initialize failure\r\n";
		ret << "Error: " << GetFelicaErrorInfo() << "\r\n";
		return ret.str();
	}

	// �|�[�g�̃I�[�v��
	char szPort[16] = "USB0";
	bSuccess = felicalib_nfc_open(szPort);
	if(!bSuccess){
		ret << "open failure\r\n";
		ret << "Error: " << GetFelicaErrorInfo() << "\r\n";
		return ret.str();
	}
	ret << "port = " << szPort << "\r\n"; //L"aa";//printf("port=%s\r\n", szPort);

	// �f�o�C�X�̕ߑ��ƃA�N�Z�X���̎擾
	DEVICE_INFO* pInfo = NULL;
	bSuccess = felicalib_nfc_poll_and_start_dev_access(
		DEVICE_TYPE_NFC_18092_212K    // FeliCa 212kbps
		| DEVICE_TYPE_NFC_18092_424K, // FeliCa 424kbps
		3000,                         // �^�C���A�E�g ms
		&pInfo
	);
	if(!bSuccess){
		ret << "access failure\r\n";
		ret << "Error: " << GetFelicaErrorInfo() << "\r\n";
		return ret.str();
	}

	// IDm �̕\���i�V�X�e�� 0�j
	DEVICE_DATA_NFC_18092_212_424K* pdevf = NULL;
	pdevf = (DEVICE_DATA_NFC_18092_212_424K*)pInfo->dev_info;
	ret << "IDm = ";
	for (int i = 0; i < 8; i++)
	{
		char buf[32];
		sprintf(buf, "%02x ", pdevf->NFCID2[i]);
		ret << buf;
	}
	ret << "\r\n";

	// ����
	return ret.str();
}
#else
#pragma comment(lib ,"felica.lib")
#include "felica.h"

std::wstring GetFelicaInfo()
{
	std::wostringstream ret;
	bool b = false;

	// ������
	ret << "initialize...\r\n";
	b = initialize_library();
	if(!b){
		ret << "failed.\r\n";
		return ret.str();
	}

	// �I�[�v��
	ret << "open...\r\n";
	b = open_reader_writer_auto();
	if(!b){
		dispose_library();
		ret << "failed.\r\n";
		return ret.str();
	}

	// �|�[�����O
	ret << "polling...\r\n";
	unsigned char system_code[2] = {0xff, 0xff};
	structure_polling polling;
	{
		polling.system_code = system_code;
		polling.time_slot = 0x00;
	}
	unsigned char number_of_cards = 0;
	structure_card_information card_information;
	unsigned char card_idm[8] = {0}; 
	unsigned char card_pmm[8] = {0}; 
	card_information.card_idm = card_idm;
	card_information.card_pmm = card_pmm;
	b = ::polling_and_get_card_information(&polling, &number_of_cards, &card_information);
	if(!b){
		close_reader_writer();
		dispose_library();
		ret << "failed.\r\n";
		return ret.str();
	}

	// ����
	ret << "IDm = ";
	for(int i = 0; i < 8; i++){
		char buf[32];
		sprintf(buf, "%02X ", card_idm[i]);
		ret << buf;
	}
	ret << "\r\n";

	// ����
	ret << "PMm = ";
	for(int i = 0; i < 8; i++){
		char buf[32];
		sprintf(buf, "%02X ", card_pmm[i]);
		ret << buf;
	}
	ret << "\r\n";

	// ��n��
	close_reader_writer();
	dispose_library();

	// ����
	return ret.str();
}

#endif

void CFelicaReaderDlg::OnBnClickedButton1()
{
	// FeliCa �����擾���A���ʂ�\��
	m_edtInfo.SetWindowText(GetFelicaInfo().c_str());
}
