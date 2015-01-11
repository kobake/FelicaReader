/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "AbstractRecord.h"
#include "TextRecord.h"
#include "UriRecord.h"
#include "ActionRecord.h"

class SmartPosterRecord :
	public AbstractRecord
{
public:
	SmartPosterRecord(void);
	~SmartPosterRecord(void);

	void setData(unsigned char* pData, unsigned long uLen);
	void setData(const char* pszLang, LPCTSTR pszTitle, LPCTSTR pszUri);
	LPCTSTR getTitle(void);
	LPCTSTR getUri(void);

protected:
	void setPayloadData(unsigned char* pData, unsigned char uLen);
	const char* getType(void);
	unsigned char getPayloadLength(void);
	void getPayloadData(unsigned char* pBuffer, unsigned char uLen);

	TextRecord m_text;
	UriRecord m_uri;
	ActionRecord m_action;
};
