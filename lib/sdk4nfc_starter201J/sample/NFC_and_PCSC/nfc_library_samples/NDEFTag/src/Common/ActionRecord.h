/*
 * Copyright 2011 Sony Corporation
 */

#pragma once
#include "AbstractRecord.h"

class ActionRecord :
	public AbstractRecord
{
public:
	ActionRecord(void);
	~ActionRecord(void);

	void setAction(const unsigned char ucAction);
	void setPayloadData(unsigned char* pData, unsigned char uLen);

	unsigned char getAction(void);
	const char* getType(void);
	
protected:
	unsigned char getPayloadLength(void);
	void getPayloadData(unsigned char* pBuffer, unsigned char uLen);
private:
	unsigned char m_ucAction;
};
