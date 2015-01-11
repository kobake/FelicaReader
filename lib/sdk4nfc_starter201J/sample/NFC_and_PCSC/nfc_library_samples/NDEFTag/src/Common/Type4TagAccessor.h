/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "ISO7816_4.h"

class Type4TagAccessor
{
public:
	Type4TagAccessor(ISO7816_4* pTag);
	virtual ~Type4TagAccessor(void);
	
	void read(unsigned char* pData, unsigned short* pusLenData);
	void write(unsigned char* pData, unsigned short usLenData);
	
private:
	/// ISO7816_4
	ISO7816_4* m_pTag;
	/// Writable or not
	bool m_isReadOnly;
	/// Data length
	unsigned short m_usDataLen;
	/// MLe
	unsigned short m_usMle;
	/// Mlc
	unsigned short m_usMlc;
};
