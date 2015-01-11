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


// CFelicaReaderDlg メッセージ ハンドラー

BOOL CFelicaReaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CFelicaReaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
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
// エラーコードの定義
// ・FELICA_NFC_E_DRIVER_ERROR 0x80080000 ドライバでエラーが発生しました
//
// ドライバエラーコード
// ・0x00000032 API の発行シーケンスが正しくありません。
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

// 参考：http://codezine.jp/article/detail/7402?p=2
std::wstring GetFelicaInfo()
{
	//std::wstringbuf retbuf;
	//std::wostream ret(&retbuf);
	std::wostringstream ret;//(&retbuf);

	felicalib_nfc_start_logging("C:\\felica_test.log");
	
	// ライブラリの初期化
	bool bSuccess;
	bSuccess = felicalib_nfc_initialize();
	if(!bSuccess){
		ret << "initialize failure\r\n";
		ret << "Error: " << GetFelicaErrorInfo() << "\r\n";
		return ret.str();
	}

	// ポートのオープン
	char szPort[16] = "USB0";
	bSuccess = felicalib_nfc_open(szPort);
	if(!bSuccess){
		ret << "open failure\r\n";
		ret << "Error: " << GetFelicaErrorInfo() << "\r\n";
		return ret.str();
	}
	ret << "port = " << szPort << "\r\n"; //L"aa";//printf("port=%s\r\n", szPort);

	// デバイスの捕捉とアクセス権の取得
	DEVICE_INFO* pInfo = NULL;
	bSuccess = felicalib_nfc_poll_and_start_dev_access(
		DEVICE_TYPE_NFC_18092_212K    // FeliCa 212kbps
		| DEVICE_TYPE_NFC_18092_424K, // FeliCa 424kbps
		3000,                         // タイムアウト ms
		&pInfo
	);
	if(!bSuccess){
		ret << "access failure\r\n";
		ret << "Error: " << GetFelicaErrorInfo() << "\r\n";
		return ret.str();
	}

	// IDm の表示（システム 0）
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

	// 結果
	return ret.str();
}
#else
#pragma comment(lib ,"felica.lib")
#include "felica.h"

std::wstring GetFelicaInfo()
{
	std::wostringstream ret;
	bool b = false;

	// 初期化
	ret << "initialize...\r\n";
	b = initialize_library();
	if(!b){
		ret << "failed.\r\n";
		return ret.str();
	}

	// オープン
	ret << "open...\r\n";
	b = open_reader_writer_auto();
	if(!b){
		dispose_library();
		ret << "failed.\r\n";
		return ret.str();
	}

	// ポーリング
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

	// 結果
	ret << "IDm = ";
	for(int i = 0; i < 8; i++){
		char buf[32];
		sprintf(buf, "%02X ", card_idm[i]);
		ret << buf;
	}
	ret << "\r\n";

	// 結果
	ret << "PMm = ";
	for(int i = 0; i < 8; i++){
		char buf[32];
		sprintf(buf, "%02X ", card_pmm[i]);
		ret << buf;
	}
	ret << "\r\n";

	// 後始末
	close_reader_writer();
	dispose_library();

	// 結果
	return ret.str();
}

#endif

void CFelicaReaderDlg::OnBnClickedButton1()
{
	// FeliCa 情報を取得し、結果を表示
	m_edtInfo.SetWindowText(GetFelicaInfo().c_str());
}
