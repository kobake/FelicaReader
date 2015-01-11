/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "NfcAccessLib.h"

#define	BLOCK_SIZE			4
#define	READ_BLOCK_SIZE		16
#define	WRITE_BLOCK_SIZE	4

class NfcA
{
public:
	NfcA(NfcAccessLib* pLib);
	~NfcA(void);

	void read(unsigned char ucBlock, unsigned char pBuffer[READ_BLOCK_SIZE]);
	void write(unsigned char ucBlock, unsigned char pBuffer[WRITE_BLOCK_SIZE]);
	void sectorSelect(unsigned char ucSector);
private:
	NfcAccessLib* m_pLib;
};
