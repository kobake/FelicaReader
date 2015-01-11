/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "NfcAccessLib.h"
#include "felica_nfc_library.h"
#pragma comment(lib ,"felica_nfc_library.lib")
#include "Exception.h"

#define TIMEOUT 2000L

NfcAccessLib::NfcAccessLib(NfcType type) : m_isInitialized(false), m_isConnected(false), m_type(type)
{
}

NfcAccessLib::~NfcAccessLib(void)
{
	this->uninit();
}

void NfcAccessLib::init(void)
{
	if (!this->m_isInitialized) {
		this->m_isInitialized = felicalib_nfc_initialize();
	}
}

void NfcAccessLib::uninit(void)
{
	this->disconnect();
	if (this->m_isInitialized) {
		felicalib_nfc_uninitialize();
		this->m_isInitialized = false;
	}
}

void NfcAccessLib::connect(void)
{
	// init
	this->init();
	// disconnect
	this->disconnect();
	// open r/w
	char szPort[16] = "";
	if (!felicalib_nfc_open(szPort)) {
		throw EX_NO_RW;
	}
	// connect to tag
	unsigned long aType[] = {
		DEVICE_TYPE_NFC_14443A_18092_106K,
		DEVICE_TYPE_NFC_14443B_106K,
		DEVICE_TYPE_NFC_18092_212K | DEVICE_TYPE_NFC_18092_424K
	};
	DEVICE_INFO* pInfo;
	if (!felicalib_nfc_poll_and_start_dev_access(aType[this->m_type], TIMEOUT, &pInfo)) {
		felicalib_nfc_close();
		throw EX_NO_TAG;
	}
	this->m_isConnected = true;
}

void NfcAccessLib::disconnect(void)
{
	if (this->m_isConnected) {
		felicalib_nfc_stop_dev_access(RE_NOTIFICATION_SAME_DEVICE);
		felicalib_nfc_close();
		this->m_isConnected = false;
	}
}

void NfcAccessLib::transmit(unsigned char* pCom, unsigned short uLenCom, unsigned char* pRes, unsigned short* puLenRes)
{
	if (!felicalib_nfc_thru(pCom, uLenCom, pRes, puLenRes)) {
		throw EX_ACCESS_FAILURE;
	}
}
