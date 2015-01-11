/*
 * Copyright 2011 Sony Corporation
 */

#include <windows.h>
#include "NfcAccessLib.h"
#include "NfcF.h"
#include "Type3TagAccessor.h"
#include "SmartPosterRecord.h"
#include "UriRecord.h"
#include "UsageError.h"
#include "Exception.h"
#include <tchar.h>
#include <locale.h>

#define COMMAND_NAME TEXT("Type3Tag")

#define E_OK				0
#define E_USAGE				1
#define E_ACCESS			2

#define OPT_HEAD	TEXT("-")
#define OPT_TITLE	OPT_HEAD TEXT("T")
#define OPT_URI		OPT_HEAD TEXT("U")
#define OPT_DUMP	OPT_HEAD TEXT("D")
#define CMD_READ	TEXT("READ")
#define CMD_WRITE	TEXT("WRITE")

typedef struct {
	bool isReadMode;
	LPCTSTR pszTitle;
	LPCTSTR pszUri;
	bool isDumpMode;
} PARAMETER;

void parseCmdLine(int argc, TCHAR** argv, PARAMETER* pParam)
{
	pParam->isReadMode = true;
	pParam->pszTitle = TEXT("");
	pParam->pszUri = TEXT("");
	pParam->isDumpMode = false;

	enum {
		MD_CMD = 0,
		MD_OPT,
		MD_TITLE,
		MD_URI,
	} mode;
	mode = MD_CMD;

	if (argc == 1) {
		throw UE_NO_OPT;
	}

	for (int i = 1; i < argc; i++) {
		switch (mode) {
			case MD_CMD:
				if (lstrcmpi(argv[i], CMD_READ) == 0) {
					pParam->isReadMode = true;
					mode = MD_OPT;
				} else if (lstrcmpi(argv[i], CMD_WRITE) == 0) {
					pParam->isReadMode = false;
					mode = MD_OPT;
				} else {
					throw UE_BAD_CMD;
				}
				break;
			case MD_OPT:
				if (lstrcmpi(argv[i], OPT_TITLE) == 0) {
					mode = MD_TITLE;
				} else if (lstrcmpi(argv[i], OPT_URI) == 0) {
					mode = MD_URI;
				} else if (lstrcmpi(argv[i], OPT_DUMP) == 0) {
					pParam->isDumpMode = true;
				} else {
					throw UE_BAD_OPT;
				}
				break;
			case MD_TITLE:
				if (_tcsnccmp(OPT_HEAD, argv[i], _tcslen(OPT_HEAD)) == 0) {
					throw UE_BAD_DATA;
				}
				pParam->pszTitle = argv[i];
				mode = MD_OPT;
				break;
			case MD_URI:
				if (_tcsnccmp(OPT_HEAD, argv[i], _tcslen(OPT_HEAD)) == 0) {
					throw UE_BAD_DATA;
				}
				pParam->pszUri = argv[i];
				mode = MD_OPT;
				break;
		}
	}
	if (mode != MD_OPT) {
		throw UE_BAD_DATA;
	}
}

void usage(void)
{
	_tprintf_s(TEXT("%s [Command] <Oprion>\n"), COMMAND_NAME);
	_tprintf_s(TEXT("\n"));
	_tprintf_s(TEXT("Command:\n"));
	_tprintf_s(TEXT("  %s\n"), CMD_READ);
	_tprintf_s(TEXT("\t Read from the Type3 Tag.\n"));
	_tprintf_s(TEXT("  %s\n"), CMD_WRITE);
	_tprintf_s(TEXT("\t Write to the Type3 Tag.\n"));
	_tprintf_s(TEXT("\n"));
	_tprintf_s(TEXT("Option:\n"));
	_tprintf_s(TEXT("  %s [Title]\n"), OPT_TITLE);
	_tprintf_s(TEXT("\t is the string as the title.(for Write Command only)\n"));
	_tprintf_s(TEXT("  %s [URI]\n"), OPT_URI);
	_tprintf_s(TEXT("\t is the URI string.(for Write Command only)\n"));
}

void printUsageErrorMessage(UsageError e) 
{
	switch (e) {
		case UE_NO_OPT:
			break;
		case UE_BAD_CMD:
			_tprintf_s(TEXT("Invalid command was used.\n\n"));
			break;
		case UE_BAD_OPT:
			_tprintf_s(TEXT("Invalid option was used.\n\n"));
			break;
		case UE_BAD_DATA:
			_tprintf_s(TEXT("The necessary value to the option was lacked.\n\n"));
			break;
	}
	usage();
}

void printAccessErrorMessage(Exception e)
{
	_tprintf_s(TEXT("Failed.\n\n"));
	switch(e) {
		case EX_NO_RW:
			_tprintf_s(TEXT("Connect the R/W.\n\n"));
			break;
		case EX_NO_TAG:
			_tprintf_s(TEXT("Set a Type3 Tag.\n\n"));
			break;
		case EX_ACCESS_FAILURE:
			_tprintf_s(TEXT("Failed to access to the tag.\n\n"));
			break;
		case EX_UNSUPPORTED_TAG:
			_tprintf_s(TEXT("The tag is not a supported Type3 Tag.\n\n"));
			break;
		case EX_ACCESS_DENY:
			_tprintf_s(TEXT("The tag is read only.\n\n"));
			break;
		case EX_BAD_DATA:
			_tprintf_s(TEXT("Bad data for the tag.\n\n"));
			break;
	}
}

void printBytes(unsigned char* pData, unsigned int uLen)
{
	for (unsigned int i = 0; i < uLen; i++) {
		_tprintf_s(TEXT("%02X"), pData[i]);
	}
	_tprintf_s(TEXT("\n"));
}

int _tmain(int argc, TCHAR** argv)
{
	int iRes = 0;
	_tsetlocale(LC_ALL, _T(""));

	int iRet = E_OK;
	PARAMETER param;

	try {
		parseCmdLine(argc, argv, &param);

		NfcAccessLib lib(NfcAccessLib::TypeF);
		NfcF tag(&lib);
		Type3TagAccessor accessor(&tag);
		SmartPosterRecord record;
		unsigned char aBuf[0xFFFF];
		unsigned long ulLen = sizeof(aBuf);
		if (param.isReadMode) {
			accessor.read(aBuf, &ulLen);
			if (param.isDumpMode) {
				printBytes(aBuf, ulLen);
			}
			record.setData(aBuf, ulLen);
			_tprintf_s(TEXT("TITLE:%s\n"), record.getTitle());
			_tprintf_s(TEXT("URI:  %s\n"), record.getUri());
		} else {
			record.setData("ja", param.pszTitle, param.pszUri);
			record.getData(aBuf, ulLen);
			ulLen = record.getDataLength();
			accessor.write(aBuf, ulLen);
			_tprintf_s(TEXT("Succeeded.\n"));
		}
	} catch (UsageError e) {
		printUsageErrorMessage(e);
		iRet = E_USAGE;
	} catch (Exception e) {
		printAccessErrorMessage(e);
		iRet = E_ACCESS;
	}
	return iRet;
}