// sample_pcsc_02.cpp
//

#include <windows.h>
#include <winscard.h>
#include <stdio.h>
#include <tchar.h>
#include <assert.h>

#pragma comment (lib, "winscard.lib")

#define SAMPLE_TITLE		_T("READ BINARY/UPDATE BINARY (FeliCa Sample5)")
#define PCSC_TRANS_BUFF_LEN	(262)
#define PCSC_RECV_BUFF_LEN	(262)

typedef struct {
	INT		iLength;
	BYTE	bCommand[PCSC_TRANS_BUFF_LEN];
} SENDCOMM;


SCARD_IO_REQUEST *CardProtocol2PCI(DWORD dwProtocol)
{
	if (dwProtocol == SCARD_PROTOCOL_T0) {
		return (SCARD_IO_REQUEST *)SCARD_PCI_T0;
	} else if (dwProtocol == SCARD_PROTOCOL_T1) {
		return (SCARD_IO_REQUEST *)SCARD_PCI_T1;
	} else if (dwProtocol == SCARD_PROTOCOL_RAW) {
		return (SCARD_IO_REQUEST *)SCARD_PCI_RAW;
	} else if (dwProtocol == SCARD_PROTOCOL_UNDEFINED) {
		assert(false);
		return NULL;
	}

	return (SCARD_IO_REQUEST *)SCARD_PCI_T1;
}

int main(int argc, _TCHAR* argv[])
{
	_ftprintf_s(stdout, _T("%s\n"), SAMPLE_TITLE);

	SCARDCONTEXT	hContext = 0;
	LONG lResult = ::SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext);
	if (lResult != SCARD_S_SUCCESS) {
		if (lResult == SCARD_E_NO_SERVICE) {
			_ftprintf_s(stdout, _T("Smart Card Servise is not Started.\n"));
		} else {
			_ftprintf_s(stdout, _T("SCardEstablishContext Error.\nErrorCode %08X\n"), lResult);
		}
		return EXIT_FAILURE;
	}

	LPTSTR	lpszReaderName = NULL;
	DWORD	dwAutoAllocate = SCARD_AUTOALLOCATE;
	TCHAR	*pszExtectedReaderName = _T("Sony FeliCa Port/PaSoRi 3.0");

	lResult = ::SCardListReaders(hContext, NULL, (LPTSTR)&lpszReaderName, &dwAutoAllocate);
	if (lResult != SCARD_S_SUCCESS) {
		if (lResult == SCARD_E_NO_READERS_AVAILABLE)  {
			_ftprintf_s(stdout, _T("Reader/Writer is not Found.\n"));
		} else {
			_ftprintf_s(stdout, _T("SCardListReaders Error.\nErrorCode %08X\n"), lResult);
		}
		::SCardReleaseContext(hContext);
		return EXIT_FAILURE;
	}
	if (_tcsncmp(pszExtectedReaderName, lpszReaderName, _tcslen(pszExtectedReaderName)) != 0) {
		_ftprintf_s(stdout, _T("Reader/Writer is not Found.\n"));
		::SCardFreeMemory(hContext, lpszReaderName);
		::SCardReleaseContext(hContext);
		return EXIT_FAILURE;
	}

	SCARDHANDLE	hCard = NULL;
	DWORD		dwActiveProtocol = 0;
	lResult = ::SCardConnect(hContext, lpszReaderName, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
	if (lResult != SCARD_S_SUCCESS) {
		if (lResult == SCARD_W_REMOVED_CARD) {
			_ftprintf_s(stdout, _T("Card is not Found.\n"));
		} else {
			_ftprintf_s(stdout, _T("SCardConnect Error.\nErrorCode %08X\n"), lResult);
		}

		::SCardFreeMemory(hContext, lpszReaderName);
		::SCardReleaseContext(hContext);
		return EXIT_FAILURE;
	}

	SENDCOMM	SendComm[] = {
					{8,  {0xFF, 0xA4, 0x00, 0x01, 0x02, 0x09, 0x10}},		/* SelectFile ServiceCode:1009 */
					{5,  {0xFF, 0xB0, 0x00, 0x00, 0x00}},					/* ReadBinary Block0 */
					{21, {0xFF, 0xD6, 0x00, 0x00, 0x10,
						  0x0F, 0x1E, 0x2D, 0x3C, 0x4B, 0x5A, 0x69, 0x78,
						  0x87, 0x96, 0xA5, 0xB4, 0xC3, 0xD2, 0xE1, 0xF0}},	/* UpdateBinary Block0 */
					{5,  {0xFF, 0xB0, 0x00, 0x00, 0x00}},					/* ReadBinary Block0 */
					{-1, NULL},
				};
	BYTE	bRecvBuf[PCSC_RECV_BUFF_LEN] = {0x00};
	DWORD	dwResponseSize;
	for (UINT uiCmdIdx = 0; SendComm[uiCmdIdx].iLength > -1; uiCmdIdx ++) {
		dwResponseSize = sizeof(bRecvBuf);
		lResult = ::SCardTransmit(hCard, CardProtocol2PCI(dwActiveProtocol), SendComm[uiCmdIdx].bCommand, SendComm[uiCmdIdx].iLength, NULL, bRecvBuf, &dwResponseSize);
		if (lResult != SCARD_S_SUCCESS) {
			_ftprintf_s(stdout, _T("SCardTransmit Error.\nErrorCode:0x%08X\n"), lResult);

			::SCardDisconnect(hCard, SCARD_LEAVE_CARD);	
			::SCardFreeMemory(hContext, lpszReaderName);
			::SCardReleaseContext(hContext);
			return EXIT_FAILURE;
		}

		for (UINT uiRespIdx = 0; uiRespIdx < dwResponseSize; uiRespIdx ++) {
			_ftprintf_s(stdout, _T("%02X"), bRecvBuf[uiRespIdx]);
			if ((uiRespIdx + 1) >= dwResponseSize) {
				_ftprintf_s(stdout, _T("\n"));
			} else {
				_ftprintf_s(stdout, _T(" "));
			}
		}
	}

	::SCardDisconnect(hCard, SCARD_LEAVE_CARD);	
	::SCardFreeMemory(hContext, lpszReaderName);
	::SCardReleaseContext(hContext);

	return EXIT_SUCCESS;
}
