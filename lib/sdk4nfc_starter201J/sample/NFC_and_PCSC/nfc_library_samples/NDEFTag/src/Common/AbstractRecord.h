/*
 * Copyright 2011 Sony Corporation
 */

#pragma once

class AbstractRecord
{
public:
	AbstractRecord(void);
	~AbstractRecord(void);
	unsigned short getDataLength(void);
	void getData(unsigned char* pBuffer, unsigned long uLen);

	virtual void setPayloadData(unsigned char* pData, unsigned char uLen) = 0;
	virtual const char* getType(void) = 0;

	bool m_isStart;
	bool m_isEnd;
protected:
	virtual unsigned char getPayloadLength(void) = 0;
	virtual void getPayloadData(unsigned char* pBuffer, unsigned char uLen) = 0;
private:
};
