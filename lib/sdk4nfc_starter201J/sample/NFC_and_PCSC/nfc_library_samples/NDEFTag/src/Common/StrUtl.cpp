/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "StrUtl.h"

unsigned long StrUtl::Str2Utf8(LPCTSTR pszStr, unsigned char* pBuffer, unsigned long ulLen)
{
	unsigned long ulMBSize = static_cast<unsigned long>(::WideCharToMultiByte(CP_UTF8, 0, pszStr, -1 , NULL, 0 , NULL, NULL));
	if (pBuffer != NULL) {
		::WideCharToMultiByte(CP_UTF8, 0, pszStr, -1, reinterpret_cast<char*>(pBuffer), ulLen, NULL, NULL);
	}
	return ulMBSize - 1;
}

unsigned long StrUtl::Utf82Str(const unsigned char* pData, unsigned long ulLen, LPTSTR pszBuffer, unsigned long ulBufSize)
{
	unsigned long ulRet = static_cast<unsigned long>(::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCSTR>(pData), ulLen, NULL, 0));
	if (pszBuffer != NULL) {
		::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCSTR>(pData), ulLen, pszBuffer, ulBufSize);
		pszBuffer[ulRet] = '\0';
	}
	return ulRet;
}
