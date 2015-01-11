/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "Type4TagAccessor.h"
#include "Exception.h"

#define CC_SIZE				15
#define TYPE4_TAG_VER_1		0x10
#define TYPE4_TAG_VER_2		0x20
#define MAGIC_NFC_FORUM_TAG	0x04
#define NDEF_TAG_SIZE		6

Type4TagAccessor::Type4TagAccessor(ISO7816_4* pTag)
{
	this->m_pTag = pTag;
	// Select App
	bool isV2 = true;
	unsigned char aidV1[] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x00};
	unsigned char aidV2[] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};
	try {
		this->m_pTag->select(aidV2, sizeof(aidV2), true);
	} catch(ISO7816_4::Exception) {
		isV2 = false;
		this->m_pTag->select(aidV1, sizeof(aidV1), false);
	}
	// Select CC
	this->m_pTag->select(0xe103, isV2);
	// Read CC
	unsigned char cc[CC_SIZE];
	unsigned short uLen = sizeof(cc);
	this->m_pTag->read(0, cc, &uLen);
	// Check CC
	unsigned char* p = cc;
	unsigned short usCcLen = *p++;
	usCcLen = (usCcLen << 8) + *p++;
	if (usCcLen != CC_SIZE) {
		throw EX_UNSUPPORTED_TAG;
	}
	unsigned char ucVer = *p++;
	if (TYPE4_TAG_VER_1 != ucVer && TYPE4_TAG_VER_2 != ucVer) {
		throw EX_UNSUPPORTED_TAG;
	}
	this->m_usMle = *p++;
	this->m_usMle = (this->m_usMle << 8) + *p++;
	this->m_usMlc = *p++;
	this->m_usMlc = (this->m_usMlc << 8) + *p++;
	if (MAGIC_NFC_FORUM_TAG != *p++) {
		throw EX_NDEF_ERROR;
	}
	if (NDEF_TAG_SIZE != *p++) {
		throw EX_NDEF_ERROR;
	}
	unsigned short usFile = *p++;
	usFile = (usFile << 8) + *p++;
	this->m_usDataLen = *p++;
	this->m_usDataLen = (this->m_usDataLen << 8) + *p++;
	if (0x00 != *p++) {
		throw EX_UNSUPPORTED_TAG;
	}
	unsigned char ucWriteSec = *p++;
	if (ucWriteSec == 0x00) {
		this->m_isReadOnly = false;
	} else if (ucWriteSec == 0xFF) {
		this->m_isReadOnly = true;
	} else {
		throw EX_UNSUPPORTED_TAG;
	}
	// Select NDEF FILE
	this->m_pTag->select(usFile, isV2);
}

Type4TagAccessor::~Type4TagAccessor(void)
{
}

void Type4TagAccessor::read(unsigned char* pData, unsigned short* pusLenData)
{
	unsigned char aNdefLen[2];
	unsigned short usLen = sizeof(aNdefLen);
	this->m_pTag->read(0, aNdefLen, &usLen);
	unsigned short usNdefLen = aNdefLen[0];
	usNdefLen = (usNdefLen << 8) + aNdefLen[1];
	unsigned short uReadCnt = 0;
	while (uReadCnt < usNdefLen) {
		unsigned short uReadSize = (usNdefLen - uReadCnt < this->m_usMle) ? usNdefLen - uReadCnt : this->m_usMle;
		this->m_pTag->read(2 + uReadCnt, pData + uReadCnt, &uReadSize);
		uReadCnt += uReadSize;
	}
	*pusLenData = uReadCnt;
}

void Type4TagAccessor::write(unsigned char* pData, unsigned short usLenData)
{
	if (this->m_isReadOnly) {
		throw EX_ACCESS_DENY;
	}
	unsigned char aNdefLen[2] = {0, 0};
	this->m_pTag->write(0, aNdefLen, 2);
	unsigned short uWroteCnt = 0;
	while (uWroteCnt < usLenData) {
		unsigned short uWriteSize = (usLenData - uWroteCnt < this->m_usMlc) ? usLenData - uWroteCnt : this->m_usMlc;
		this->m_pTag->write(2 + uWroteCnt, pData + uWroteCnt, uWriteSize);
		uWroteCnt += uWriteSize;
	}
	aNdefLen[0] = usLenData >> 8;
	aNdefLen[1] = usLenData & 0xFF;
	this->m_pTag->write(0, aNdefLen, 2);
}
