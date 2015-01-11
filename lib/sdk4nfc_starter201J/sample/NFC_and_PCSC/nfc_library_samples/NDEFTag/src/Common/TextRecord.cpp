/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "TextRecord.h"
#include "StrUtl.h"
#include "Exception.h"
#include <tchar.h>

TextRecord::TextRecord(void)
{
	this->m_szLangCode[0] = '\0';
	this->m_szTextData[0] = '\0';
}

TextRecord::~TextRecord(void)
{
}

void TextRecord::setTextData(const char* pszLang, LPCTSTR pszText)
{
	size_t uLangLen = strlen(pszLang);
	if (uLangLen > _countof(this->m_szLangCode)) {
		throw EX_BAD_DATA;
	}
	int iTextLen = lstrlen(pszText);
	if (iTextLen > _countof(this->m_szTextData)) {
		throw EX_BAD_DATA;
	}
	unsigned long ulUtf8TextLen = StrUtl::Str2Utf8(pszText);
	if (ulUtf8TextLen > 0xFF - 1 - uLangLen) {
		throw EX_BAD_DATA;
	}
	strncpy_s(this->m_szLangCode, _countof(this->m_szLangCode), pszLang, _TRUNCATE);
	_tcsncpy_s(this->m_szTextData, _countof(this->m_szTextData), pszText, _TRUNCATE);
}

void TextRecord::setPayloadData(unsigned char* pData, unsigned char uLen)
{
	if (uLen < 1) {
		throw EX_NDEF_ERROR;
	}
	unsigned char* p = pData;
	unsigned char ucStat = *p++;
	if ((ucStat & 0xC0) != 0) {
		throw EX_UNSUPPORTED_TAG;
	}
	unsigned char ucLangCodeLen = ucStat & 0x3F;
	if (uLen < 1 + ucLangCodeLen) {
		throw EX_NDEF_ERROR;
	}
	strncpy_s(this->m_szLangCode, _countof(this->m_szLangCode), reinterpret_cast<const char*>(p), ucLangCodeLen);
	p += ucLangCodeLen;
	unsigned long ulTextLen = StrUtl::Utf82Str(p, uLen - static_cast<unsigned char>(p - pData));
	if (ulTextLen > _countof(this->m_szTextData)) {
		throw EX_UNSUPPORTED_TAG;
	}
	StrUtl::Utf82Str(p, uLen - static_cast<unsigned char>(p - pData), this->m_szTextData, _countof(this->m_szTextData));
}


const char* TextRecord::getLangCode(void)
{
	return this->m_szLangCode;
}

LPCTSTR TextRecord::getText(void)
{
	return this->m_szTextData;
}

const char* TextRecord::getType(void)
{
	return "T";
}

unsigned char TextRecord::getPayloadLength(void)
{
	return static_cast<unsigned char>(1 + strlen(this->m_szLangCode) + StrUtl::Str2Utf8(this->m_szTextData));
}

void TextRecord::getPayloadData(unsigned char* pBuffer, unsigned char uLen)
{
	unsigned char* p = pBuffer;
	*p++ = static_cast<unsigned char>(strlen(this->m_szLangCode));
	memcpy_s(p, uLen - 1, this->m_szLangCode, strlen(this->m_szLangCode));
	p += strlen(this->m_szLangCode);
	StrUtl::Str2Utf8(this->m_szTextData, p, uLen - static_cast<unsigned char>(p - pBuffer));
}
