/*
 * Copyright 2011 Sony Corporation
 */

#pragma once

class IAccessLib
{
public:
	virtual void connect(void) = 0;
	virtual void disconnect(void) = 0;
	virtual void transmit(unsigned char* pCom, unsigned short uLenCom, unsigned char* pRes, unsigned short* puLenRes) = 0;
};
