/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "IAccessLib.h"

class NfcAccessLib : public IAccessLib
{
public:
	enum NfcType {
		TypeA = 0,
		TypeB,
		TypeF
	};

	NfcAccessLib(NfcType type);
	~NfcAccessLib(void);

	void connect(void);
	void disconnect(void);
	void transmit(unsigned char* pCom, unsigned short uLenCom, unsigned char* pRes, unsigned short* puLenRes);
private:
	void init(void);
	void uninit(void);

	NfcType m_type;
	bool m_isInitialized;
	bool m_isConnected;
};
