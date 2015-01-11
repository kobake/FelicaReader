/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "IAccessLib.h"
#include <Winscard.h>

class PcscAccessLib : public IAccessLib
{
public:
	PcscAccessLib(void);
	~PcscAccessLib(void);

	void connect(void);
	void disconnect(void);
	void transmit(unsigned char* pCom, unsigned short uLenCom, unsigned char* pRes, unsigned short* puLenRes);
private:
	void init(void);
	void uninit(void);

	bool m_isInitialized;

	SCARDCONTEXT m_hPcscContext;
	SCARDHANDLE	m_hCard;
};
