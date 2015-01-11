/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "NfcF.h"

NfcF::NfcF(NfcAccessLib* pLib) : m_pLib(pLib)
{
	this->m_pLib->connect();
}

NfcF::~NfcF(void)
{
	this->m_pLib->disconnect();
}

void NfcF::polling(unsigned short usSystem, unsigned char pIDm[IDM_SIZE])
{
	unsigned char aCommand[6] = {0x06, 0x00, 0xFF, 0xFF, 0x00, 0x00};
	aCommand[2] = static_cast<unsigned char>(usSystem >> 8);
	aCommand[3] = static_cast<unsigned char>(usSystem & 0xFF);
	unsigned char aResponse[0xFFFF];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, sizeof(aCommand), aResponse, &usLenResponse);
	memcpy_s(pIDm, IDM_SIZE, aResponse + 2, IDM_SIZE);
}

void NfcF::check(unsigned char pIDm[IDM_SIZE], unsigned short usService, unsigned short usFrom, unsigned char ucBlockCnt, unsigned char* pBuffer)
{
	unsigned char ucLen = 2 + IDM_SIZE + 1 + 2 + 1 + 3 * ucBlockCnt;
	unsigned char aCommand[0xFF] = {ucLen, 0x06};
	unsigned char* p = aCommand + 2;
	memcpy_s(p, sizeof(aCommand) - (p - aCommand), pIDm, IDM_SIZE);
	p += IDM_SIZE;
	*p++ = 0x01;
	*p++ = 0x0b;
	*p++ = 0x00;
	*p++ = ucBlockCnt;
	for (unsigned char i = 0; i < ucBlockCnt; i++) {
		*p++ = 0x00;
		unsigned short usIdx = usFrom + i;
		*p++ = static_cast<unsigned char>(usIdx & 0xFF);
		*p++ = static_cast<unsigned char>(usIdx >> 8);
	}
	unsigned char aResponse[0xFFFF];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, ucLen, aResponse, &usLenResponse);
	if (usLenResponse != 2 + IDM_SIZE + 2 + 1 + ucBlockCnt * BLOCK_SIZE) {
		throw NFC_F_ACCESS_FAILED;
	}
	memcpy_s(pBuffer, ucBlockCnt * BLOCK_SIZE, aResponse + 2 + IDM_SIZE + 2 + 1, ucBlockCnt * BLOCK_SIZE);
}

void NfcF::update(unsigned char pIDm[IDM_SIZE], unsigned short usService, unsigned short usFrom, unsigned char ucBlockCnt, unsigned char* pBuffer)
{
	unsigned char ucLen = 2 + IDM_SIZE + 1 + 2 + 1 + 3 * ucBlockCnt + ucBlockCnt * BLOCK_SIZE;
	unsigned char aCommand[0xFF] = {ucLen, 0x08};
	unsigned char* p = aCommand + 2;
	memcpy_s(p, sizeof(aCommand) - (p - aCommand), pIDm, IDM_SIZE);
	p += IDM_SIZE;
	*p++ = 0x01;
	*p++ = 0x09;
	*p++ = 0x00;
	*p++ = ucBlockCnt;
	for (unsigned char i = 0; i < ucBlockCnt; i++) {
		*p++ = 0x00;
		unsigned short usIdx = usFrom + i;
		*p++ = static_cast<unsigned char>(usIdx & 0xFF);
		*p++ = static_cast<unsigned char>(usIdx >> 8);
	}
	for (unsigned char i = 0; i < ucBlockCnt; i++) {
		memcpy_s(p, sizeof(aCommand) - (p - aCommand),  pBuffer, BLOCK_SIZE);
		p += BLOCK_SIZE;
	}
	unsigned char aResponse[0xFFFF];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, ucLen, aResponse, &usLenResponse);
	if (aResponse[2 + IDM_SIZE] != 0x00) {
		throw NFC_F_ACCESS_FAILED;
	}
}
