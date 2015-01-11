
// FelicaReader.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CFelicaReaderApp:
// このクラスの実装については、FelicaReader.cpp を参照してください。
//

class CFelicaReaderApp : public CWinApp
{
public:
	CFelicaReaderApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CFelicaReaderApp theApp;