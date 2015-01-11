// sample_pcsc_04_Initiator.cpp
//

#include <windows.h>
#include <winscard.h>
#include <stdio.h>
#include <tchar.h>
#include <assert.h>

#pragma comment (lib, "winscard.lib")

#define SAMPLE_TITLE		_T("DEP-Tareget (Initiator)")
#define PCSC_TRANS_BUFF_LEN	(262)
#define PCSC_RECV_BUFF_LEN	(262)
#define CONNECT_TIMEOUT		(10)

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
	TCHAR	*pszExpectedReaderName = _T("Sony FeliCa Port/PaSoRi 3.0");
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
	if (_tcsncmp(pszExpectedReaderName, lpszReaderName, _tcslen(pszExpectedReaderName)) != 0) {
		_ftprintf_s(stdout, _T("Reader/Writer is not Found.\n"));
		::SCardFreeMemory(hContext, lpszReaderName);
		::SCardReleaseContext(hContext);
		return EXIT_FAILURE;
	}

	SCARDHANDLE	hCard = NULL;
	DWORD		dwActiveProtocol = 0;
	for (UINT uiTryTime = 0; uiTryTime <= CONNECT_TIMEOUT; uiTryTime ++) {
		lResult = ::SCardConnect(hContext, lpszReaderName, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
		if (lResult == SCARD_S_SUCCESS) {
			break;
		} else if (lResult == SCARD_W_REMOVED_CARD) {
			_ftprintf_s(stdout, _T("%3d\r"), CONNECT_TIMEOUT - uiTryTime);
			if ((CONNECT_TIMEOUT - uiTryTime) > 0) {
				Sleep(1000);
			} else {
				_ftprintf_s(stdout, _T("Card is not Found.\n"));

				::SCardFreeMemory(hContext, lpszReaderName);
				::SCardReleaseContext(hContext);
				return EXIT_FAILURE;
			}
		} else {
			_ftprintf_s(stdout, _T("SCardConnect Error.\nErrorCode %08X\n"), lResult);

			::SCardFreeMemory(hContext, lpszReaderName);
			::SCardReleaseContext(hContext);
			return EXIT_FAILURE;
		}
	}

	SENDCOMM	SendComm[] = {
					{53, {0xFF, 0xFE, 0x02, 0x02, 0x30,
						  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
						  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
						  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
						  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
						  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
						  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F}},	/* Data Exchange: NFC-DEP */
					{69, {0xFF, 0xFE, 0x02, 0x03, 0x40,
						  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
						  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
						  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
						  0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
						  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
						  0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
						  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
						  0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F}},	/* Data Exchange: NFC-DEP (Chaining) */
					{85, {0xFF, 0xFE, 0x02, 0x01, 0x50,
						  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
						  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
						  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
						  0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
						  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
						  0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
						  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
						  0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
						  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
						  0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF}},	/* Data Exchange: NFC-DEP (Chaining) */
					{-1, NULL},
				};
	BYTE		bRecvBuf[PCSC_RECV_BUFF_LEN] = {0x00};
	DWORD		dwResponseSize;
	for (UINT uiCmdIdx = 0; SendComm[uiCmdIdx].iLength > -1; uiCmdIdx ++) {
		dwResponseSize = sizeof(bRecvBuf);
		lResult = ::SCardTransmit(hCard, CardProtocol2PCI(dwActiveProtocol),
			SendComm[uiCmdIdx].bCommand, SendComm[uiCmdIdx].iLength, NULL, bRecvBuf, &dwResponseSize);
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
