/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "NfcA.h"

NfcA::NfcA(NfcAccessLib* pLib) : m_pLib(pLib)
{
	this->m_pLib->connect();
}

NfcA::~NfcA(void)
{
	this->m_pLib->disconnect();
}

void NfcA::read(unsigned char ucBlock, unsigned char pBuffer[READ_BLOCK_SIZE])
{
	unsigned char aCommand[2] = {0x30};
	aCommand[1] = ucBlock;
	unsigned char aResponse[0xFFFF];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, sizeof(aCommand), aResponse, &usLenResponse);
	memcpy_s(pBuffer, READ_BLOCK_SIZE, aResponse, READ_BLOCK_SIZE);
}

void NfcA::write(unsigned char ucBlock, unsigned char pBuffer[WRITE_BLOCK_SIZE])
{
	unsigned char aCommand[2 + WRITE_BLOCK_SIZE] = {0xA2};
	aCommand[1] = ucBlock;
	memcpy_s(aCommand + 2, sizeof(aCommand) - 2, pBuffer, WRITE_BLOCK_SIZE);
	unsigned char aResponse[0xFFFF];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, sizeof(aCommand), aResponse, &usLenResponse);
}

void NfcA::sectorSelect(unsigned char ucSector)
{
	unsigned char aCommand[2] = {0xC2, 0xFF};
	unsigned char aResponse[0xFFFF];
	unsigned short usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, sizeof(aCommand), aResponse, &usLenResponse);
	unsigned char aCommand2[4] = {0x00, 0x00, 0x00, 0x00};
	aCommand2[0] = ucSector;
	usLenResponse = sizeof(aResponse);
	this->m_pLib->transmit(aCommand, sizeof(aCommand), aResponse, &usLenResponse);
}