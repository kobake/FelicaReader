/*
 * Copyright 2012 Sony Corporation
 */

#pragma once
#include "NP100_APDU.h"

#define	READ_BLOCK_SIZE		16
#define	WRITE_BLOCK_SIZE	4

class NP100_Type2TagAccessor
{
public:
	NP100_Type2TagAccessor(NP100_APDU* pTag);
	virtual ~NP100_Type2TagAccessor(void);
	
	void read(unsigned char* pData, unsigned short* pusLenData);
	void write(unsigned char* pData, unsigned short usLenData);
	
private:
	/// NFPPort100 APDU
	NP100_APDU* m_pTag;
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

	void selectFile(unsigned char ucSector);
	void readBinary(unsigned char ucPageNum, unsigned char* pBuffer);
	void updateBinary(unsigned char ucPageNum, unsigned char* pData);
};
