/*
 * Copyright 2012 Sony Corporation
 */

#pragma once
#include "NP100_APDU.h"

class NP100_Type3TagAccessor
{
public:
	NP100_Type3TagAccessor(NP100_APDU* pTag);
	virtual ~NP100_Type3TagAccessor(void);
	
	void read(unsigned char* pData, unsigned long* pulLenData);
	void write(unsigned char* pData, unsigned long ulLenData);
	
private:
	/// NFPPort100 APDU
	NP100_APDU* m_pTag;
	/// NBR
	unsigned char m_ucNbr;
	/// NBW
	unsigned char m_ucNbw;
	/// Max block count
	unsigned short m_usMaxBlockCount;
	/// Writable or not
	bool m_isReadOnly;
	/// Data length
	unsigned long m_ulDataLen;

	unsigned short mkChecksum(unsigned char pData[14]);
	void updateAttribute(bool isFinished);

	void selectFile(unsigned short usServiceCode);
	void readBinary(unsigned short usFrom, unsigned char ucBlockCnt, unsigned char* pBuffer);
	void updateBinary(unsigned short usFrom, unsigned char ucBlockCnt, unsigned char* pData);
};
