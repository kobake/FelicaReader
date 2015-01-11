/*
 * Copyright 2011 Sony Corporation
 */

#pragma once

class StrUtl
{
public:
	static unsigned long Str2Utf8(LPCTSTR pszStr, unsigned char* pBuffer = NULL, unsigned long ulLen = 0);
	static unsigned long Utf82Str(const unsigned char* pData, unsigned long ulLen, LPTSTR pszBuffer = NULL, unsigned long ulBufSize = 0);
};
