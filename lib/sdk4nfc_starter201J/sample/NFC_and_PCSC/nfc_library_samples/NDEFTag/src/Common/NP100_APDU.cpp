/*
 * Copyright 2012 Sony Corporation
 */

#include <windows.h>
#include "NP100_APDU.h"

#define NFC_F_MAX_BLOCK_CNT 16
#define NFC_F_MAX_NODES	20
#define NFC_F_BLOCK_SIZE	16


NP100_APDU::NP100_APDU(PcscAccessLib* pLib) : m_pLib(pLib)
{
	this->m_pLib->connect();
}

NP100_APDU::~NP100_APDU(void)
{
	this->m_pLib->disconnect();
}

void NP100_APDU::selectFile(unsigned char ucP1, unsigned char ucP2, unsigned char ucLen, unsigned char* pData)
{
	if (ucLen > NFC_F_MAX_NODES * 2) {
		throw NP100_APDU_ACCESS_FAILED;
	}
	unsigned char aCommand[5 + NFC_F_MAX_NODES * 2] = {0xff, 0xa4};
	unsigned char* p = aCommand + 2;
	*p++ = ucP1;
	*p++ = ucP2;
	*p++ = ucLen;
	memcpy_s(p, sizeof(aCommand) - (p - aCommand), pData, ucLen);
	p += ucLen;
	unsigned short usLenCommand = static_cast<unsigned short>(p - aCommand);
	unsigned char aResponse[2];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, usLenCommand, aResponse, &usLenResponse);
	if (aResponse[0] != 0x90 || aResponse[1] != 0x00) {
		throw NP100_APDU_ACCESS_FAILED;
	}
}

void NP100_APDU::readBinary(unsigned char ucP1, unsigned char ucP2, unsigned char ucParamLen, unsigned char* pParam, unsigned char ucDataLen, unsigned char* pData)
{
	unsigned char aCommand[6 + NFC_F_MAX_BLOCK_CNT * 2] = {0xff, 0xb0};
	unsigned char* p = aCommand + 2;
	*p++ = ucP1;
	*p++ = ucP2;
	if (ucParamLen != 0) {
		*p++ = ucParamLen;
		memcpy_s(p, sizeof(aCommand) - (p - aCommand), pParam, ucParamLen);
		p += ucParamLen;
	}
	*p++ = ucDataLen;
	unsigned short usLenCommand = static_cast<unsigned short>(p - aCommand);
	unsigned char aResponse[NFC_F_MAX_BLOCK_CNT * NFC_F_BLOCK_SIZE + 2];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, usLenCommand, aResponse, &usLenResponse);
	if (aResponse[usLenResponse - 2] != 0x90 || aResponse[usLenResponse - 1] != 0x00) {
		throw NP100_APDU_ACCESS_FAILED;
	}
	memcpy_s(pData, ucDataLen, aResponse, usLenResponse - 2);
}

void NP100_APDU::updateBinary(unsigned char ucP1, unsigned char ucP2, unsigned char ucDataLen, unsigned char* pData)
{
	unsigned char aCommand[5 + NFC_F_BLOCK_SIZE * NFC_F_MAX_BLOCK_CNT] = {0xff, 0xd6};
	unsigned char* p = aCommand + 2;
	*p++ = ucP1;
	*p++ = ucP2;
	*p++ = ucDataLen;
	memcpy_s(p, sizeof(aCommand) - (p - aCommand), pData, ucDataLen);
	p += ucDataLen;
	unsigned short usLenCommand = static_cast<unsigned short>(p - aCommand);
	unsigned char aResponse[2];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, usLenCommand, aResponse, &usLenResponse);
	if (aResponse[0] != 0x90 || aResponse[1] != 0x00) {
		throw NP100_APDU_ACCESS_FAILED;
	}
}
