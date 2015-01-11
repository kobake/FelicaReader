/*
* Copyright 2012 Sony Corporation
*/

#include <windows.h>
#include "NP100_Type2TagAccessor.h"
#include "Exception.h"
#include <memory.h>

#define MAGIC_NFC_FORUM_TAG	0xE1
#define VERSION_MAJOR		1
#define TAG_NDEF_MESSAGE	0x03
#define TAG_TERMINATOR		0xFE
#define SECTOR_SIZE			(256 * 4)
#define BLOCK_SIZE			4

NP100_Type2TagAccessor::NP100_Type2TagAccessor(NP100_APDU* pTag) : m_ulMemSize(64), m_isReadOnly(false), m_ulPos(0), m_ulBufferPos(0), m_pTag(pTag)
{
	static const unsigned long START_BLOCK = 3;
	this->m_ulPos = START_BLOCK * BLOCK_SIZE;
	this->readBuffer(this->m_ulPos);
	unsigned char aCc[4];
	aCc[0] = this->readByte();
	aCc[1] = this->readByte();
	aCc[2] = this->readByte();
	aCc[3] = this->readByte();
	// check CC
	if (aCc[0] != MAGIC_NFC_FORUM_TAG) {
		throw EX_UNSUPPORTED_TAG;
	}
	if ((aCc[1] >> 4) != VERSION_MAJOR) {
		throw EX_UNSUPPORTED_TAG;
	}
	this->m_ulMemSize = aCc[2] * 8 + 16;
	if (aCc[3] == 0x00) {
		this->m_isReadOnly = false;
	} else if (aCc[3] == 0x0F) {
		this->m_isReadOnly = true;
	} else {
		throw EX_UNSUPPORTED_TAG;
	}
}

NP100_Type2TagAccessor::~NP100_Type2TagAccessor(void)
{
}

void NP100_Type2TagAccessor::read(unsigned char* pData, unsigned short* pusLenData)
{
	this->searchNdef();
	unsigned short usLen = this->readLength();
	this->readValue(pData, usLen);
	*pusLenData = usLen;
}

void NP100_Type2TagAccessor::write(unsigned char* pData, unsigned short usLenData)
{
	if (this->m_isReadOnly) {
		throw EX_ACCESS_DENY;
	}
	this->searchNdef();
	if (this->m_ulPos + ((usLenData > 0xFF) ? 3 : 1) + usLenData > this->m_ulMemSize) {
		throw EX_BAD_DATA;
	}
	this->writeLength(usLenData);
	this->writeValue(pData, usLenData);
	this->writeByte(TAG_TERMINATOR);
	this->writeBuffer();
}


void NP100_Type2TagAccessor::updateBuffer(void)
{
	if (this->m_ulPos < this->m_ulBufferPos) {
		this->readBuffer(this->m_ulPos);
	} else if (this->m_ulPos >= this->m_ulBufferPos + READ_BLOCK_SIZE) {
		unsigned long ulNewPos = this->m_ulPos;
		if ((this->m_ulPos % SECTOR_SIZE) > SECTOR_SIZE - READ_BLOCK_SIZE) {
			ulNewPos = (1 + this->m_ulPos / SECTOR_SIZE) * SECTOR_SIZE - READ_BLOCK_SIZE;
		} else if (this->m_ulPos > this->m_ulMemSize - READ_BLOCK_SIZE) {
			ulNewPos = this->m_ulMemSize - READ_BLOCK_SIZE;
		}
		this->readBuffer(ulNewPos);
	}
}

unsigned char NP100_Type2TagAccessor::readByte(void)
{
	if (this->m_ulPos >= this->m_ulMemSize) {
		throw EX_UNSUPPORTED_TAG;
	}
	this->updateBuffer();
	unsigned char ucRet = this->m_aBuffer[this->m_ulPos - this->m_ulBufferPos];
	this->m_ulPos++;
	return ucRet;
}

unsigned short NP100_Type2TagAccessor::readLength(void)
{
	unsigned short usRet = this->readByte();
	if (usRet == 0xFF) {
		usRet = this->readByte();
		usRet = (usRet << 8) + this->readByte();
	}
	return usRet;
}

void NP100_Type2TagAccessor::readValue(unsigned char* pValue, unsigned short usLen)
{
	for (unsigned short i = 0; i < usLen; i++) {
		pValue[i] = this->readByte();
	}
}

void NP100_Type2TagAccessor::writeByte(unsigned char ucData)
{
	if (this->m_ulPos >= this->m_ulMemSize) {
		throw EX_UNSUPPORTED_TAG;
	}
	this->updateBuffer();
	unsigned long ulIdx = this->m_ulPos - this->m_ulBufferPos;
	this->m_aBuffer[ulIdx] = ucData;
	if ((ulIdx % WRITE_BLOCK_SIZE) == (WRITE_BLOCK_SIZE - 1)) {
		this->writeBuffer();
	}
	this->m_ulPos++;
}

void NP100_Type2TagAccessor::writeLength(unsigned short usLen)
{
	if (usLen < 0xFF) {
		this->writeByte(static_cast<unsigned char>(usLen));
	} else {
		this->writeByte(0xFF);
		this->writeByte(usLen >> 8);
		this->writeByte(usLen & 255);
	}
}

void NP100_Type2TagAccessor::writeValue(unsigned char* pValue, unsigned short usLen)
{
	for (unsigned short i = 0; i < usLen; i++) {
		this->writeByte(pValue[i]);
	}
}

void NP100_Type2TagAccessor::readBuffer(unsigned long ulPos)
{
	if (this->m_ulBufferPos / SECTOR_SIZE != ulPos / SECTOR_SIZE) {
		this->selectFile(static_cast<unsigned char>(ulPos / SECTOR_SIZE));
	}
	this->readBinary(static_cast<unsigned char>((ulPos % SECTOR_SIZE) / BLOCK_SIZE), this->m_aBuffer);
	this->m_ulBufferPos = ulPos / BLOCK_SIZE * BLOCK_SIZE;
}

void NP100_Type2TagAccessor::writeBuffer(void)
{
	unsigned long ulIdx = (this->m_ulPos - this->m_ulBufferPos) / WRITE_BLOCK_SIZE * WRITE_BLOCK_SIZE;
	this->updateBinary(static_cast<unsigned char>((this->m_ulPos % SECTOR_SIZE) / BLOCK_SIZE), this->m_aBuffer + ulIdx);
}

void NP100_Type2TagAccessor::searchNdef(void)
{
	bool bFound = false;
	this->m_ulPos = 16;
	enum {
		MD_T,
		MD_L,
		MD_V,
	} mode;
	mode = MD_T;
	unsigned short usLen = 0;
	for (unsigned long i = this->m_ulPos; i < this->m_ulMemSize; i++) {
		if (mode == MD_T) {
			unsigned char ucT = this->readByte();
			if (ucT == TAG_NDEF_MESSAGE) {
				bFound = true;
				break;
			} else {
				mode = MD_L;
			}
		} else if (mode == MD_L) {
			usLen = this->readLength();
			mode = MD_V;
		} else if (mode == MD_V) {
			this->m_ulPos += usLen;
			i += usLen - 1;
			mode = MD_T;
		}
	}
	if (!bFound) {
		throw EX_NDEF_ERROR;
	}
}

void NP100_Type2TagAccessor::selectFile(unsigned char ucSector)
{
	try {
		this->m_pTag->selectFile(0, 0, 1, &ucSector);
	} catch (NP100_APDU::Exception) {
		throw EX_ACCESS_FAILURE;
	}
}

void NP100_Type2TagAccessor::readBinary(unsigned char ucPageNum, unsigned char* pBuffer)
{
	try {
		this->m_pTag->readBinary(0, ucPageNum, 0, NULL, READ_BLOCK_SIZE, pBuffer);
	} catch (NP100_APDU::Exception) {
		throw EX_ACCESS_FAILURE;
	}
}

void NP100_Type2TagAccessor::updateBinary(unsigned char ucPageNum, unsigned char* pData)
{
	try {
		this->m_pTag->updateBinary(0, ucPageNum, WRITE_BLOCK_SIZE, pData);
	} catch (NP100_APDU::Exception) {
		throw EX_ACCESS_FAILURE;
	}
}
