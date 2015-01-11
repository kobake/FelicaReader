/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "AbstractRecord.h"

#define MAX_LANG	0x3F
#define MAX_TEXT	0xFF

class TextRecord :
	public AbstractRecord
{
public:
	TextRecord();
	~TextRecord(void);

	void setTextData(const char* pszLang, LPCTSTR pszText);
	void setPayloadData(unsigned char* pData, unsigned char uLen);
	const char* getLangCode(void);
	LPCTSTR getText(void);

	const char* getType(void);
protected:
	unsigned char getPayloadLength(void);
	void getPayloadData(unsigned char* pBuffer, unsigned char uLen);
private:
	char m_szLangCode[MAX_LANG];
	TCHAR m_szTextData[MAX_TEXT];
};
