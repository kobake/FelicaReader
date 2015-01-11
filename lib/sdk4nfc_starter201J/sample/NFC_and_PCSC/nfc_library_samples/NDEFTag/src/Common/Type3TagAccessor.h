/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "NfcF.h"

class Type3TagAccessor
{
public:
	Type3TagAccessor(NfcF* pTag);
	virtual ~Type3TagAccessor(void);
	
	void read(unsigned char* pData, unsigned long* pulLenData);
	void write(unsigned char* pData, unsigned long ulLenData);
	
private:
	/// Nfc TypeF
	NfcF* m_pTag;
	/// IDm
	unsigned char m_aIDm[IDM_SIZE];
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
};
