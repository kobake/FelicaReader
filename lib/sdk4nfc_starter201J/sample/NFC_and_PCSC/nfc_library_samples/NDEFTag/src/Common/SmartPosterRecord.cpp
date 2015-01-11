/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "SmartPosterRecord.h"
#include "Exception.h"
#include <string.h>

SmartPosterRecord::SmartPosterRecord(void)
{
}

SmartPosterRecord::~SmartPosterRecord(void)
{
}

void SmartPosterRecord::setPayloadData(unsigned char* pData, unsigned char uLen)
{
	unsigned char* p = pData;
	unsigned char uLeft = uLen;
	AbstractRecord* pRecord[] = {&this->m_text, &this->m_uri, &this->m_action};
	unsigned char aFlags[] = {0x91, 0x11, 0x51};

	for (int i = 0 ;i < 3; i++) {
		if (uLeft < 3) {
			throw EX_NDEF_ERROR;
		}
		uLeft -= 3;
		unsigned char ucFlags = *p++;
		unsigned char ucTypeLen = *p++;
		unsigned char ucPayloadLen = *p++;
		if (ucFlags != aFlags[i]) {
			throw EX_UNSUPPORTED_TAG;
		}
		if (uLeft < ucTypeLen + ucPayloadLen) {
			throw EX_NDEF_ERROR;
		}
		char szType[255];
		strncpy_s(szType, _countof(szType), reinterpret_cast<char*>(p), ucTypeLen);
		p += ucTypeLen;
		if (strcmp(szType, pRecord[i]->getType()) != 0) {
			throw EX_UNSUPPORTED_TAG;
		}
		pRecord[i]->setPayloadData(p, ucPayloadLen);
		uLeft -= ucTypeLen + ucPayloadLen;
		p += ucPayloadLen;
	}
}

const char* SmartPosterRecord::getType(void)
{
	return "Sp";
}

unsigned char SmartPosterRecord::getPayloadLength(void)
{
	return this->m_text.getDataLength() + this->m_uri.getDataLength() + this->m_action.getDataLength();
}

void SmartPosterRecord::getPayloadData(unsigned char* pBuffer, unsigned char uLen)
{
	unsigned char* p = pBuffer;
	unsigned char uLeft = uLen;
	this->m_text.getData(p, uLeft);
	p += this->m_text.getDataLength();
	uLeft -= this->m_text.getDataLength();
	this->m_uri.getData(p, uLeft);
	p += this->m_uri.getDataLength();
	uLeft -= this->m_uri.getDataLength();
	this->m_action.getData(p, uLeft);
	p += this->m_action.getDataLength();
}

void SmartPosterRecord::setData(unsigned char* pData, unsigned long uLen)
{
	unsigned char* p = pData;
	unsigned long uLeft = uLen;

	if (uLeft < 3) {
		throw EX_NDEF_ERROR;
	}
	uLeft -= 3;
	unsigned char ucFlags = *p++;
	unsigned char ucTypeLen = *p++;
	unsigned char ucPayloadLen = *p++;
	if (ucFlags != 0xD1) {
		throw EX_UNSUPPORTED_TAG;
	}
	if (uLeft < static_cast<unsigned long>(ucTypeLen + ucPayloadLen)) {
		throw EX_NDEF_ERROR;
	}
	char szType[255];
	strncpy_s(szType, _countof(szType), reinterpret_cast<char*>(p), ucTypeLen);
	p += ucTypeLen;
	if (strcmp(szType, this->getType()) != 0) {
		throw EX_UNSUPPORTED_TAG;
	}
	this->setPayloadData(p, ucPayloadLen);
}

void SmartPosterRecord::setData(const char* pszLang, LPCTSTR pszTitle, LPCTSTR pszUri)
{
	this->m_isStart = true;
	this->m_isEnd = true;
	this->m_text.setTextData(pszLang, pszTitle);
	this->m_text.m_isStart = true;
	this->m_uri.setUri(pszUri);
	this->m_action.setAction(0);
	this->m_action.m_isEnd = true;
}

LPCTSTR SmartPosterRecord::getTitle(void)
{
	return this->m_text.getText();
}

LPCTSTR SmartPosterRecord::getUri(void)
{
	return this->m_uri.getUri();
}
