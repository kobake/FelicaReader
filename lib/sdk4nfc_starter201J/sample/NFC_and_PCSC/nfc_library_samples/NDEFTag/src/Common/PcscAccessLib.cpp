/*
 * Copyright 2011 Sony Corporation
 */

#include "PcscAccessLib.h"
#pragma comment(lib, "winscard.lib")
#include "Exception.h"

PcscAccessLib::PcscAccessLib(void) : m_isInitialized(false), m_hPcscContext(NULL), m_hCard(NULL)
{
}

PcscAccessLib::~PcscAccessLib(void)
{
	this->uninit();
}

void PcscAccessLib::connect(void)
{
	// init
	this->init();
	// disconnect
	this->disconnect();
	// Get available R/W's name
	LPTSTR lpmszReaders = NULL;
	DWORD dwLen = SCARD_AUTOALLOCATE;
	if (SCARD_S_SUCCESS != SCardListReaders(this->m_hPcscContext, NULL, (LPTSTR)&lpmszReaders, &dwLen)) {
		throw EX_NO_RW;
	}
	// connect Tag
	DWORD dwProtocol;
	LONG lRes = SCardConnect(this->m_hPcscContext, lpmszReaders, SCARD_SHARE_SHARED, (SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1), &this->m_hCard, &dwProtocol);
	SCardFreeMemory(this->m_hPcscContext, lpmszReaders);
	if (SCARD_S_SUCCESS != lRes) {
		throw EX_NO_TAG;
	}
}

void PcscAccessLib::disconnect(void)
{
	SCardDisconnect(this->m_hPcscContext, SCARD_LEAVE_CARD);
}

void PcscAccessLib::transmit(unsigned char* pCom, unsigned short uLenCom, unsigned char* pRes, unsigned short* puLenRes)
{
	DWORD dwLenResponse = *puLenRes;
	if (SCARD_S_SUCCESS != SCardTransmit(this->m_hCard, SCARD_PCI_T1, pCom, uLenCom, NULL, pRes, &dwLenResponse)) {
		throw EX_ACCESS_FAILURE;
	}
	*puLenRes = static_cast<unsigned short>(dwLenResponse);
}

void PcscAccessLib::init(void)
{
	if (!this->m_isInitialized) {
		this->m_isInitialized = (SCARD_S_SUCCESS == SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &this->m_hPcscContext));
	}
}

void PcscAccessLib::uninit(void)
{
	if (this->m_isInitialized) {
		SCardReleaseContext(this->m_hPcscContext);
	}
}