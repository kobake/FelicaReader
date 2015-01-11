/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "UriRecord.h"
#include "StrUtl.h"
#include "Exception.h"
#include <tchar.h>

LPCTSTR UriRecord::s_aAbbreviations[] = {
	TEXT(""),
	TEXT("http://www."),
	TEXT("https://www."),
	TEXT("http://"),
	TEXT("https://"),
};


UriRecord::UriRecord(void)
{
	this->m_szUriData[0] = '\0';
}

UriRecord::~UriRecord(void)
{
}

void UriRecord::setUri(LPCTSTR pszUri)
{
	int iUriLen = lstrlen(pszUri);
	if (iUriLen > _countof(this->m_szUriData)) {
		throw EX_BAD_DATA;
	}
	unsigned long ulUtf8UriLen = StrUtl::Str2Utf8(pszUri);
	if (ulUtf8UriLen > 0xFF - 1) {
		throw EX_BAD_DATA;
	}
	_tcsncpy_s(this->m_szUriData, _countof(this->m_szUriData), pszUri, _TRUNCATE);

}

void UriRecord::setPayloadData(unsigned char* pData, unsigned char uLen)
{
	if (uLen < 1) {
		throw EX_NDEF_ERROR;
	}
	unsigned char* p = pData;
	unsigned char ucIdCode = *p++;
	if (ucIdCode > _countof(UriRecord::s_aAbbreviations)) {
		throw EX_UNSUPPORTED_TAG;
	}
	lstrcpy(this->m_szUriData, UriRecord::s_aAbbreviations[ucIdCode]);
	unsigned char ucAbbreviationLen = lstrlen(UriRecord::s_aAbbreviations[ucIdCode]);
	unsigned long ulUriLen = StrUtl::Utf82Str(p, uLen - static_cast<unsigned char>(p - pData));
	if (ulUriLen > _countof(this->m_szUriData) - ucAbbreviationLen) {
		throw EX_UNSUPPORTED_TAG;
	}
	StrUtl::Utf82Str(p, uLen - static_cast<unsigned char>(p - pData), this->m_szUriData + ucAbbreviationLen, _countof(this->m_szUriData) - ucAbbreviationLen);
}

LPCTSTR UriRecord::getUri(void)
{
	return this->m_szUriData;
}

const char* UriRecord::getType(void)
{
	return "U";
}

unsigned char UriRecord::getPayloadLength(void)
{
	unsigned char ucIdCode = UriRecord::getId(this->m_szUriData);
	unsigned char ucAbbreviationLen = lstrlen(UriRecord::s_aAbbreviations[ucIdCode]);
	return static_cast<unsigned char>(1 + StrUtl::Str2Utf8(this->m_szUriData + ucAbbreviationLen));
}

void UriRecord::getPayloadData(unsigned char* pBuffer, unsigned char uLen)
{
	unsigned char* p = pBuffer;
	unsigned char ucIdCode = UriRecord::getId(this->m_szUriData);
	*p++ = ucIdCode;
	unsigned char ucAbbreviationLen = lstrlen(UriRecord::s_aAbbreviations[ucIdCode]);
	StrUtl::Str2Utf8(this->m_szUriData + ucAbbreviationLen, p, uLen - static_cast<unsigned char>(p - pBuffer));
}

unsigned char UriRecord::getId(LPCTSTR pszUri)
{
	unsigned char uRet = 0;
	unsigned char uCnt = _countof(UriRecord::s_aAbbreviations);
	for (unsigned char i = 1; i < uCnt; i++) {
		LPCTSTR lpAbbreviatedString = UriRecord::s_aAbbreviations[i];
		if (_tcsncmp(lpAbbreviatedString, pszUri, lstrlen(lpAbbreviatedString)) == 0) {
			uRet = i;
			break;
		}
	}
	return uRet;
}
