/*
 * Copyright 2011 Sony Corporation
 */

#include "AbstractRecord.h"
#include <string.h>

AbstractRecord::AbstractRecord(void) : m_isStart(false), m_isEnd(false)
{
}

AbstractRecord::~AbstractRecord(void)
{
}

unsigned short AbstractRecord::getDataLength(void)
{
	return static_cast<unsigned short>(1 + 1 + 1 + strlen(this->getType()) + this->getPayloadLength());
}

void AbstractRecord::getData(unsigned char* pBuffer, unsigned long uLen)
{
	unsigned char ucFlags = 0x11;
	ucFlags |= (this->m_isStart) ? 0x80 : 0x00;
	ucFlags |= (this->m_isEnd) ? 0x40 : 0x00;
	unsigned char ucTypeLen = static_cast<unsigned char>(strlen(this->getType()));
	unsigned char ucPayloadLen = this->getPayloadLength();

	unsigned char* p = pBuffer;
	*p++ = ucFlags;
	*p++ = ucTypeLen;
	*p++ = ucPayloadLen;
	memcpy_s(p, uLen - (p - pBuffer), this->getType(), ucTypeLen);
	p += ucTypeLen;
	this->getPayloadData(p, static_cast<unsigned char>(uLen - (p - pBuffer)));
}
