/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "NfcA.h"

class Type2TagAccessor
{
public:
	Type2TagAccessor(NfcA* pTag);
	virtual ~Type2TagAccessor(void);
	
	void read(unsigned char* pData, unsigned short* pusLenData);
	void write(unsigned char* pData, unsigned short usLenData);
	
private:
	/// Nfc TypeA
	NfcA* m_pTag;
	/// Memory size
	unsigned long m_ulMemSize;
	/// Writable or not
	bool m_isReadOnly;
	/// Position to access
	unsigned long m_ulPos;
	/// Read buffer
	unsigned char m_aBuffer[READ_BLOCK_SIZE];
	/// Position of read buffer
	unsigned long m_ulBufferPos;

	void updateBuffer(void);

	unsigned char readByte(void);
	unsigned short readLength(void);
	void readValue(unsigned char* pValue, unsigned short usLen);

	void writeByte(unsigned char ucData);
	void writeLength(unsigned short usLen);
	void writeValue(unsigned char* pValue, unsigned short usLen);

	void readBuffer(unsigned long ulPos);
	void writeBuffer(void);

	void searchNdef(void);
};
