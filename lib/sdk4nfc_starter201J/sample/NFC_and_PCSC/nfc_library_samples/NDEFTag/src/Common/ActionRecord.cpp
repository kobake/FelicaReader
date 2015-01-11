/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "ActionRecord.h"
#include "StrUtl.h"
#include "Exception.h"
#include <tchar.h>

ActionRecord::ActionRecord(void) : m_ucAction(0)
{
}

ActionRecord::~ActionRecord(void)
{
}

void ActionRecord::setAction(const unsigned char ucAction)
{
	if (ucAction != 0) {
		throw EX_BAD_DATA;
	}
	this->m_ucAction = ucAction;

}

void ActionRecord::setPayloadData(unsigned char* pData, unsigned char uLen)
{
	if (uLen != 1) {
		throw EX_NDEF_ERROR;
	}
	if (*pData != 0) {
		throw EX_UNSUPPORTED_TAG;
	}
	this->m_ucAction = *pData;
}

unsigned char ActionRecord::getAction(void)
{
	return this->m_ucAction;
}

const char* ActionRecord::getType(void)
{
	return "act";
}

unsigned char ActionRecord::getPayloadLength(void)
{
	return 1;
}

void ActionRecord::getPayloadData(unsigned char* pBuffer, unsigned char uLen)
{
	*pBuffer = this->m_ucAction;
}
