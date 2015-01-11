/*
 * Copyright 2011,2012 Sony Corporation
 */

#include <windows.h>
#include "ISO7816_4.h"

ISO7816_4::ISO7816_4(IAccessLib* pLib) : m_pLib(pLib)
{
	this->m_pLib->connect();
}

ISO7816_4::~ISO7816_4(void)
{
	this->m_pLib->disconnect();
}

void ISO7816_4::select(unsigned char* pId, unsigned char uLen, bool isV2)
{
	unsigned char aCommand[6 + 255] = {0x00, 0xa4, 0x04, 0x00};
	unsigned char* p = aCommand + 4;
	*p++ = uLen;
	memcpy_s(p, 255, pId, uLen);
	p += uLen;
	if (isV2) {
		*p++ = 0x00;
	}
	unsigned short usLenCommand = static_cast<unsigned short>(p - aCommand);
	unsigned char aResponse[2];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, usLenCommand, aResponse, &usLenResponse);
	if (aResponse[0] != 0x90 || aResponse[1] != 0x00) {
		throw ISO7816_4_ACCESS_FAILED;
	}
}

void ISO7816_4::select(unsigned short usId, bool isV2)
{
	unsigned char aCommand[5 + 2] = {0x00, 0xa4, 0x00, 0x00};
	if (isV2) {
		aCommand[3] = 0x0c;
	}
	unsigned char* p = aCommand + 4;
	*p++ = 2;
	*p++ = static_cast<unsigned char>(usId >> 8);
	*p++ = static_cast<unsigned char>(usId & 255);
	unsigned short usLenCommand = static_cast<unsigned short>(p - aCommand);
	unsigned char aResponse[2];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, usLenCommand, aResponse, &usLenResponse);
	if (aResponse[0] != 0x90 || aResponse[1] != 0x00) {
		throw ISO7816_4_ACCESS_FAILED;
	}
}

void ISO7816_4::read(unsigned short usOffset, unsigned char* pBuffer, unsigned short* puLen)
{
	unsigned char aCommand[7] = {0x00, 0xb0};
	unsigned char* p = aCommand + 2;
	*p++ = (usOffset >> 8);
	*p++ = (usOffset & 0xFF);
	if (*puLen > 255) {
		*p++ = 0;
		*p++ = static_cast<unsigned char>(*puLen >> 8);
		*p++ = static_cast<unsigned char>(*puLen & 0xFF);
	} else {
		*p++ = static_cast<unsigned char>(*puLen);
	}
	unsigned short usLenCommand = static_cast<unsigned short>(p - aCommand);
	unsigned char aResponse[65535];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, usLenCommand, aResponse, &usLenResponse);
	if (aResponse[usLenResponse - 2] != 0x90 || aResponse[usLenResponse - 1] != 0x00) {
		throw ISO7816_4_ACCESS_FAILED;
	}
	memcpy_s(pBuffer, *puLen, aResponse, usLenResponse - 2);
	*puLen = usLenResponse - 2;
}

void ISO7816_4::write(unsigned short usOffset, unsigned char* pData, unsigned short uLen)
{
	unsigned char aCommand[7 + 65535] = {0x00, 0xd6};
	unsigned char* p = aCommand + 2;
	*p++ = (usOffset >> 8);
	*p++ = (usOffset & 0xFF);
	if (uLen > 255) {
		*p++ = 0;
		*p++ = (uLen >> 8);
		*p++ = (uLen & 0xFF);
	} else {
		*p++ = static_cast<unsigned char>(uLen);
	}
	memcpy_s(p, 65535, pData, uLen);
	p += uLen;
	unsigned short usLenCommand = static_cast<unsigned short>(p - aCommand);
	unsigned char aResponse[2];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, usLenCommand, aResponse, &usLenResponse);
	if (aResponse[0] != 0x90 || aResponse[1] != 0x00) {
		throw ISO7816_4_ACCESS_FAILED;
	}
}
