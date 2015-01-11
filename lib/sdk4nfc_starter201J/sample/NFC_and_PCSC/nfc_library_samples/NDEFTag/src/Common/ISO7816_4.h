/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "IAccessLib.h"

class ISO7816_4
{
public:
	ISO7816_4(IAccessLib* pLib);
	~ISO7816_4(void);
	enum Exception {
		ISO7816_4_ACCESS_FAILED,
	};

	void select(unsigned char* pId, unsigned char uLen, bool isV2);
	void select(unsigned short usId, bool isV2);
	void read(unsigned short usOffset, unsigned char* pBuffer, unsigned short* puLen);
	void write(unsigned short usOffset, unsigned char* pData, unsigned short uLen);
private:
	IAccessLib* m_pLib;
};
