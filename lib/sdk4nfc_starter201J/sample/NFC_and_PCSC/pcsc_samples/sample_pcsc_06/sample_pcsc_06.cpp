// sample_pcsc_06.cpp
//

#include <windows.h>
#include <winscard.h>
#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "winscard.lib")

#define SAMPLE_TITLE          _T("SNEP Receive")
#define PCSC_CMD_BUFF_LEN     (262)
#define PCSC_RESP_BUFF_LEN    (262)

typedef struct {
    INT     iLength;
    BYTE    byCommand[PCSC_CMD_BUFF_LEN];
} PCSC_CMD;

int main(int argc, _TCHAR* argv[])
{
    _ftprintf_s(stdout, _T("%s\n"), SAMPLE_TITLE);

    SCARDCONTEXT    hContext = 0;
    LONG            lResult;

    lResult = ::SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext);
    if (lResult != SCARD_S_SUCCESS) {
        if (lResult == SCARD_E_NO_SERVICE) {
            _ftprintf_s(stdout, _T("Smart Card Servise is not started.\n"));
        } else {
            _ftprintf_s(stdout, _T("SCardEstablishContext Error.\nErrorCode %08X\n"), lResult);
        }
        return EXIT_FAILURE;
    }

    LPTSTR    lpszReaderName = NULL;
    DWORD     dwAutoAllocate = SCARD_AUTOALLOCATE;
    LPTSTR    lpszExpectedReaderName = _T("Sony FeliCa Port/PaSoRi 3.0");

    lResult = ::SCardListReaders(hContext, NULL, (LPTSTR)&lpszReaderName, &dwAutoAllocate);
    if (lResult != SCARD_S_SUCCESS) {
        _ftprintf_s(stdout, _T("SCardListReaders Error.\nErrorCode:0x%08X\n"), lResult);
        ::SCardReleaseContext(hContext);
        return EXIT_FAILURE;
    }
    if (_tcsncmp(lpszExpectedReaderName, lpszReaderName, _tcslen(lpszExpectedReaderName)) != 0) {
        _ftprintf_s(stdout, _T("Reader/Writer is not found.\n"));
        ::SCardFreeMemory(hContext, lpszReaderName);
        ::SCardReleaseContext(hContext);
        return EXIT_FAILURE;
    }

    SCARDHANDLE    hCard = NULL;
    DWORD          dwActiveProtocol = 0;

    lResult = ::SCardConnect(hContext, lpszReaderName, SCARD_SHARE_DIRECT, 0, &hCard, &dwActiveProtocol);
    if (lResult != SCARD_S_SUCCESS) {
        _ftprintf_s(stdout, _T("SCardConnect Error.\nErrorCode:0x%08X\n"), lResult);

        ::SCardFreeMemory(hContext, lpszReaderName);
        ::SCardReleaseContext(hContext);
        return EXIT_FAILURE;
    }

    PCSC_CMD    PcscCmd[] = {{2, {0xC6, 0x01}},                                        /* Start SNEP     */
                             {8, {0xC6, 0x05, 0x00, 0x02, 0x10, 0x27, 0x00, 0x00}},    /* Set timeout    */
                             {3, {0xC6, 0x04, 0xFF}},                                  /* Receive NDEF   */
                             {2, {0xC6, 0x02}},                                        /* Stop SNEP      */
                             {-1, NULL},};
    BYTE        byResponseBuf[PCSC_RESP_BUFF_LEN] = {0x00};
    DWORD       dwResponseSize;

    for (UINT uiCmdIdx = 0; PcscCmd[uiCmdIdx].iLength > -1; uiCmdIdx++) {
        dwResponseSize = 0;
        lResult = ::SCardControl(hCard, SCARD_CTL_CODE(3500),
                                 PcscCmd[uiCmdIdx].byCommand, PcscCmd[uiCmdIdx].iLength,
                                 byResponseBuf, PCSC_RESP_BUFF_LEN, &dwResponseSize);
        if (lResult != SCARD_S_SUCCESS) {
            _ftprintf_s(stdout, _T("SCardTransmit Error.\nErrorCode:0x%08X\n"), lResult);

            ::SCardDisconnect(hCard, SCARD_LEAVE_CARD);	
            ::SCardFreeMemory(hContext, lpszReaderName);
            ::SCardReleaseContext(hContext);
            return EXIT_FAILURE;
        }

        for (UINT uiRespIdx = 0; uiRespIdx < dwResponseSize; uiRespIdx++) {
            _ftprintf_s(stdout, _T("%02X"), byResponseBuf[uiRespIdx]);
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

