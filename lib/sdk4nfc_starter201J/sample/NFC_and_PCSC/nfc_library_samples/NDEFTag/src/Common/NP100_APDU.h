/*
 * Copyright 2012 Sony Corporation
 */

#pragma once
#include "PcscAccessLib.h"

class NP100_APDU
{
public:
	NP100_APDU(PcscAccessLib* pLib);
	~NP100_APDU(void);
	enum Exception {
		NP100_APDU_ACCESS_FAILED,
	};
	void selectFile(unsigned char ucP1, unsigned char ucP2, unsigned char ucLen, unsigned char* pData);
	void readBinary(unsigned char ucP1, unsigned char ucP2, unsigned char ucParamLen, unsigned char* pParam, unsigned char ucDataLen, unsigned char* pData);
	void updateBinary(unsigned char ucP1, unsigned char ucP2, unsigned char ucDataLen, unsigned char* pData);
private:
	PcscAccessLib* m_pLib;
};
