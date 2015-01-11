/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "NfcAccessLib.h"

#define	IDM_SIZE			8
#define	BLOCK_SIZE			16

class NfcF
{
public:
	NfcF(NfcAccessLib* pLib);
	~NfcF(void);
	enum Exception {
		NFC_F_ACCESS_FAILED,
	};

	void polling(unsigned short usSystem, unsigned char pIDm[IDM_SIZE]);
	void check(unsigned char pIDm[IDM_SIZE], unsigned short usService, unsigned short usFrom, unsigned char ucBlockCnt, unsigned char* pBuffer);
	void update(unsigned char pIDm[IDM_SIZE], unsigned short usService, unsigned short usFrom, unsigned char ucBlockCnt, unsigned char* pBuffer);
private:
	NfcAccessLib* m_pLib;
};
