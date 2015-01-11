/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "AbstractRecord.h"

#define MAX_URI	255

class UriRecord :
	public AbstractRecord
{
public:
	UriRecord(void);
	~UriRecord(void);

	void setUri(LPCTSTR pszUri);
	void setPayloadData(unsigned char* pData, unsigned char uLen);

	LPCTSTR getUri(void);
	const char* getType(void);

protected:
	unsigned char getPayloadLength(void);
	void getPayloadData(unsigned char* pBuffer, unsigned char uLen);

private:
	TCHAR m_szUriData[MAX_URI];
	static unsigned char getId(LPCTSTR pszUri);

	static LPCTSTR s_aAbbreviations[];
};
