/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "Type3TagAccessor.h"
#include "Exception.h"
#include <memory.h>

#define SYSTEM_CODE		0x12FC
#define SERVICE_CODE_W	0x0009
#define SERVICE_CODE_R	0x000B
#define VERSION_MAJOR	1
#define VERSION_MINOR	0
#define WF_OFF			0x00
#define WF_ON			0x0F
#define RWF_READ_ONLY	0x00
#define RWF_READ_WRITE	0x01

#define	READ_BLOCK_SIZE		(4 * BLOCK_SIZE)
#define	WRITE_BLOCK_SIZE	(1 * BLOCK_SIZE)


Type3TagAccessor::Type3TagAccessor(NfcF* pTag) : m_pTag(pTag)
{
	this->m_pTag->polling(SYSTEM_CODE, this->m_aIDm);
	unsigned char aAttrib[BLOCK_SIZE];
	this->m_pTag->check(this->m_aIDm, SERVICE_CODE_R, 0, 1, aAttrib);
	// check Attribute
	if (aAttrib[0] >> 4 != VERSION_MAJOR) {
		throw EX_UNSUPPORTED_TAG;
	}
	this->m_ucNbr = aAttrib[1];
	if (this->m_ucNbr == 0) {
		throw EX_NDEF_ERROR;
	}
	this->m_ucNbw = aAttrib[2];
	if (this->m_ucNbw == 0) {
		throw EX_NDEF_ERROR;
	}
	this->m_usMaxBlockCount = aAttrib[3];
	this->m_usMaxBlockCount = (this->m_usMaxBlockCount << 8) + aAttrib[4];
	if (aAttrib[9] != WF_OFF) {
		throw EX_UNSUPPORTED_TAG;
	}
	if (aAttrib[10] == RWF_READ_ONLY) {
		this->m_isReadOnly = true;
	} else if (aAttrib[10] == RWF_READ_WRITE) {
		this->m_isReadOnly = false;
	} else {
		throw EX_NDEF_ERROR;
	}
	this->m_ulDataLen = aAttrib[11];
	this->m_ulDataLen = (this->m_ulDataLen << 8) + aAttrib[12];
	this->m_ulDataLen = (this->m_ulDataLen << 8) + aAttrib[13];
}

Type3TagAccessor::~Type3TagAccessor(void)
{
}

void Type3TagAccessor::read(unsigned char* pData, unsigned long* pulLenData)
{
	unsigned char aBuf[READ_BLOCK_SIZE];
	unsigned long ulLeft = this->m_ulDataLen;
	unsigned char* p = pData;
	unsigned char ucCnt = this->m_ucNbr;
	unsigned long ulSize = this->m_ucNbr * BLOCK_SIZE;
	while (ulLeft > 0) {
		unsigned short usFrom = static_cast<unsigned short>((p - pData) / BLOCK_SIZE + 1);
		if (ulLeft < ulSize) {
			ucCnt = static_cast<unsigned char>((ulLeft / BLOCK_SIZE) + ((ulLeft % BLOCK_SIZE) == 0 ? 0 : 1));
			ulSize = ulLeft;
		}
		this->m_pTag->check(this->m_aIDm, SERVICE_CODE_R, usFrom, ucCnt, aBuf);
		memcpy_s(p, *pulLenData - (p - pData), aBuf, ulSize);
		p += ulSize;
		ulLeft -= ulSize;
	}
	*pulLenData = this->m_ulDataLen;
}

void Type3TagAccessor::write(unsigned char* pData, unsigned long ulLenData)
{
	if (this->m_isReadOnly) {
		throw EX_ACCESS_DENY;
	}
	unsigned char aBuf[WRITE_BLOCK_SIZE];
	/// update attribute
	this->updateAttribute(false);
	/// update data
	unsigned long ulLeft = ulLenData;
	unsigned char* p = pData;
	unsigned char ucCnt = this->m_ucNbw;
	unsigned long ulSize = this->m_ucNbw * BLOCK_SIZE;
	while (ulLeft > 0) {
		unsigned short usFrom = static_cast<unsigned short>((p - pData) / BLOCK_SIZE + 1);
		if (ulLeft < ulSize) {
			ucCnt = static_cast<unsigned char>((ulLeft / BLOCK_SIZE) + ((ulLeft % BLOCK_SIZE) == 0 ? 0 : 1));
			ulSize = ulLeft;
		}
		memcpy_s(aBuf, sizeof(aBuf), p, ulSize);
		memset(aBuf + ulSize, 0, sizeof(aBuf) - ulSize);
		this->m_pTag->update(this->m_aIDm, SERVICE_CODE_W, usFrom, ucCnt, aBuf);
		p += ulSize;
		ulLeft -= ulSize;
	}
	this->m_ulDataLen = ulLenData;
	/// update attribute(2)
	this->updateAttribute(true);
}

unsigned short Type3TagAccessor::mkChecksum(unsigned char pData[14])
{
	unsigned short usRet = 0;
	for (int i = 0; i < 14; i++) {
		usRet += pData[i];
	}
	return usRet;
}

void Type3TagAccessor::updateAttribute(bool isFinished)
{
	unsigned char aBuf[BLOCK_SIZE];
	aBuf[0] = (VERSION_MAJOR << 4) + VERSION_MINOR;
	aBuf[1] = this->m_ucNbr;
	aBuf[2] = this->m_ucNbw;
	aBuf[3] = (this->m_usMaxBlockCount >> 8) & 0xFF;
	aBuf[4] = (this->m_usMaxBlockCount >> 0) & 0xFF;
	aBuf[5] = 0;
	aBuf[6] = 0;
	aBuf[7] = 0;
	aBuf[8] = 0;
	aBuf[9] = (isFinished ? WF_OFF : WF_ON);
	aBuf[10] = this->m_isReadOnly ? RWF_READ_ONLY : RWF_READ_WRITE;
	aBuf[11] = static_cast<unsigned char>((this->m_ulDataLen >> 16) & 0xFF);
	aBuf[12] = static_cast<unsigned char>((this->m_ulDataLen >>  8) & 0xFF);
	aBuf[13] = static_cast<unsigned char>((this->m_ulDataLen >>  0) & 0xFF);
	unsigned short usChecksum = this->mkChecksum(aBuf);
	aBuf[14] = (usChecksum >> 8) & 0xFF;
	aBuf[15] = (usChecksum >> 0) & 0xFF;
	this->m_pTag->update(this->m_aIDm, SERVICE_CODE_W, 0, 1, aBuf);
}
