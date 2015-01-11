/*
 * nfc_sample_01.cpp
 * Copyright 2009 Sony Corporation
 */

#include <windows.h>
#include <stdio.h>

#include "felica_nfc_library.h"
#include "felica_nfc_library_error.h"

#define EXIT_FAILED		1
#define EXIT_SUCCEED	0

void printErrorInformation(void);
HWND createWindow(void);
void destroyWindow(HWND hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool pollingCard(HWND hWnd);

int main(void)
{
	bool bRet = false;

	HWND hWnd = createWindow();
	if (hWnd == NULL) {
		printf("Failed:CreateWindow\n");
		return EXIT_FAILED;
	}

	bRet = ::felicalib_nfc_initialize();
	if (bRet == false) {
		printf("Failed:felcalib_nfc_initialize\n");
		::printErrorInformation();
		::destroyWindow(hWnd);
		return EXIT_FAILED;
	}

	char port_name[16] = {"USB0"};

	bRet = ::felicalib_nfc_open(port_name);
	if (bRet == false) {
		printf("Failed:felcalib_nfc_open\n");
		::printErrorInformation();
		::destroyWindow(hWnd);
		return EXIT_FAILED;
	}

	bRet = ::pollingCard(hWnd);
	if (bRet == false) {
		::destroyWindow(hWnd);
		return EXIT_FAILED;
	}

	const unsigned char command_packet_data[2] ={0x30, 0x00}; //command for Mifare ultralight, 0x30:16byte Reading command, 0x00:start address
	unsigned short command_packet_data_length = 2;
	unsigned char response_packet_data[BUFSIZ];
	memset(response_packet_data, NULL, sizeof(response_packet_data));
	unsigned char* presponse_packet_data = response_packet_data;
	unsigned short response_packet_data_length = sizeof(response_packet_data);
	unsigned short* presponse_packet_data_length = &response_packet_data_length;

	bRet = ::felicalib_nfc_thru(command_packet_data, command_packet_data_length, presponse_packet_data, presponse_packet_data_length);
	if (bRet == false) {
		printf("Failed:felcalib_nfc_thru\n");
		::printErrorInformation();
		::destroyWindow(hWnd);
		return EXIT_FAILED;
	}

	for (int idx = 0; idx < 16; idx++) {
		printf("response_packet_data[%d]:0x%02x\n",idx, response_packet_data[idx]);
	}

	unsigned long stop_mode = RE_NOTIFICATION_SAME_DEVICE;

	bRet = ::felicalib_nfc_stop_dev_access(stop_mode);
	if (bRet == false) {
		printf("Failed:felcalib_nfc_stop_dev_access\n");
		::printErrorInformation();
		::destroyWindow(hWnd);
		return EXIT_FAILED;
	}

	bRet = ::felicalib_nfc_stop_poll_mode();
	if (bRet == false) {
		printf("Failed:felcalib_nfc_stop_poll_mode\n");
		::printErrorInformation();
		::destroyWindow(hWnd);
		return EXIT_FAILED;
	}

	bRet = ::felicalib_nfc_close();
	if (bRet == false) {
		printf("Failed:felcalib_nfc_close\n");
		::printErrorInformation();
		::destroyWindow(hWnd);
		return EXIT_FAILED;
	}

	bRet = ::felicalib_nfc_uninitialize();
	if (bRet == false) {
		printf("Failed:felcalib_nfc_uninitialize\n");
		::printErrorInformation();
		::destroyWindow(hWnd);
		return EXIT_FAILED;
	}

	::destroyWindow(hWnd);

	printf("Succeed\n");

	return EXIT_SUCCEED;
}

void printErrorInformation(void)
{
	unsigned long error_info[2];
	::felicalib_nfc_get_last_error(error_info);
	printf("error_info[0]:0x%08x\nerror_info[1]:0x%08x\n", error_info[0], error_info[1]);

	return;
}

HWND createWindow(void)
{
	HINSTANCE hInst;
	WNDCLASSEX wc;
	hInst = (HINSTANCE)GetModuleHandle(NULL);

	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_DESKTOP+1);
	wc.hCursor = ::LoadCursor(NULL,IDC_ARROW);
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hInstance = hInst;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.lpszClassName = "Sample";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW|CS_VREDRAW;

	::RegisterClassEx(&wc);

	return CreateWindow("Sample", "Sample_Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						CW_USEDEFAULT, NULL, NULL, hInst, NULL);
}

void destroyWindow(HWND hWnd)
{
	MSG msg;
	::DestroyWindow(hWnd);
	while (::GetMessage(&msg, NULL, 0, 0)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return;
}

bool pollingCard(HWND hWnd)
{

	const unsigned char msg_str_of_find[5] = {"find"};
	const unsigned char msg_str_of_enable[7] = {"enable"};
	MSG msg;
	bool bRet = false;

	bRet = ::felicalib_nfc_set_poll_callback_parameters(hWnd, msg_str_of_find, msg_str_of_enable);
	if (bRet == false) {
		printf("Failed:felcalib_nfc_set_poll_callback_parameters\n");
		::printErrorInformation();
		return bRet;
	}

	unsigned int card_find_message = ::RegisterWindowMessage((LPCTSTR)msg_str_of_find);
	unsigned int card_enable_message = ::RegisterWindowMessage((LPCTSTR)msg_str_of_enable);

	unsigned long target_device = DEVICE_TYPE_NFC_14443A_18092_106K;

	bRet = ::felicalib_nfc_start_poll_mode(target_device);
	if (bRet == false) {
		printf("Failed:felcalib_nfc_start_poll_mode\n");
		::printErrorInformation();
		return bRet;
	}

	while (true) {
		if (PeekMessage (&msg, 0, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage (&msg, hWnd, 0, 0)) {
				bRet = false;
				return bRet;
			}
			else if ((msg.message == card_find_message)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				bRet = true;

				DEVICE_INFO* pDevInfo = (DEVICE_INFO*)msg.lParam;
				DEVICE_DATA_NFC_14443A_18092_106K* pDeviceData_A = (DEVICE_DATA_NFC_14443A_18092_106K*)pDevInfo->dev_info;
				unsigned long target_number = pDeviceData_A->target_number;

				bRet = ::felicalib_nfc_start_dev_access(target_number);
				if (bRet == false) {
					printf("Failed:felcalib_nfc_start_dev_access\n");
					::printErrorInformation();
					return bRet;
				}
			}
			else if (msg.message == card_enable_message) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				bRet = true;
				break;
			}
		}
	}
	return bRet;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return (0L);
}