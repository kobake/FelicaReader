// FalpSend.cpp -*-c++-*-
// Copyright 2011 Sony Corporation

#include <cstdio>
#include <cstdlib>

#include <string.h>
#include "felica.h"

int main(void);
void error_routine(void);
void print_vector(char* title, unsigned char* vector, int length);

int main(void)
{
	//
	FILE *fp = NULL;
	errno_t err = ::fopen_s(&fp, "test.jpg", "rb");
	if (err != 0) {
        fprintf_s(stderr, "fopen_s() error.\n");
        return EXIT_FAILURE;
	}
	fpos_t fsize;
	::fseek(fp, 0, SEEK_END); 
	::fgetpos(fp, &fsize); 	
	::fseek(fp, 0, SEEK_SET);

	unsigned char *pBuf = (unsigned char *)malloc((size_t)fsize);
	if (pBuf == NULL) {
        fprintf_s(stderr, "Failed to allocate memory.\n");
        return EXIT_FAILURE;
	}
	memset (pBuf, NULL, (size_t)fsize);

	size_t read_bytes = ::fread(pBuf, sizeof(unsigned char), (size_t)fsize, fp);
	::fclose(fp);
	if (read_bytes != fsize) {
        fprintf_s(stderr, "Failed to read the data.\n");
		::free(pBuf);
        return EXIT_FAILURE;
	}
	//

    // initialize library
	fprintf_s(stdout, "initialize_library()...\n");
    if (!initialize_library()) {
        fprintf_s(stderr, "initialize_library() error.\n");
		::free(pBuf);
        return EXIT_FAILURE;
    }

    // open reader writer
	fprintf_s(stdout, "open_reader_writer_auto()...\n");
    if (!open_reader_writer_auto()) {
        fprintf_s(stderr, "open_reader_writer_auto() error.\n");
		::free(pBuf);
        return EXIT_FAILURE;
	}

    // transaction lock
	fprintf_s(stdout, "transaction_lock()...\n");
    if (!transaction_lock()) {
        fprintf_s(stderr, "transaction_lock() error.\n");
		::free(pBuf);
        return EXIT_FAILURE;
    }

    // polling and get card information
    structure_polling polling;
    unsigned char system_code[2] = {0xff, 0xff};
    polling.system_code = system_code;
    polling.time_slot = 0x00;

    structure_card_information card_information;
    unsigned char card_idm[8];
    unsigned char card_pmm[8];
    card_information.card_idm = card_idm;
    card_information.card_pmm = card_pmm;
    unsigned char number_of_cards = 0;

	fprintf_s(stdout, "polling_and_get_card_information()...\n");
    if (!polling_and_get_card_information(&polling,
                &number_of_cards,
                &card_information)) {
        fprintf_s(stderr, "polling_and_get_card_information() error.\n");
		::free(pBuf);
        return EXIT_FAILURE;
    }

    int lngErrorTimeout = 0;
    // falp open
	fprintf_s(stdout, "falp_open()...\n");
    if (!falp_open()) {
        fprintf_s(stderr, "falp_open() error.\n");
		::free(pBuf);
        return EXIT_FAILURE;
    }

    unsigned short propose_timeout = 3000; // msec
    unsigned short handshake_timeout = 3000; // msec
    unsigned char appid[] = {0x02, 0x00, 0x54, 0x45, 0x53, 0x54, 0x49, 0x44 };
    unsigned char appid_length = sizeof(appid);

	unsigned long send_data_length = (unsigned long)read_bytes;

	fprintf_s(stdout, "falp_connect()...\n");
    if (!falp_connect(
                propose_timeout,
                handshake_timeout,
                appid,
                appid_length,
                pBuf,
                &send_data_length)) {
        fprintf_s(stderr, "falp_connect() error.\n");
		::free(pBuf);
        return EXIT_FAILURE;
    }
	::free(pBuf);

    // falp wait event
    unsigned short timeout_value = 10000; // msec
    unsigned long falp_event = 0;
    unsigned long falp_event_end_empty = FALP_EVENT_SEND_EMPTY | FALP_EVENT_SHUTDOWNED;
	fprintf_s(stdout, "falp_wait_event()...\n");
    if (!falp_wait_event(
                timeout_value,
                &falp_event,
                falp_event_end_empty)) {
        fprintf_s(stderr, "falp_wait_event error.\n");
        if (lngErrorTimeout != 1) {
            return EXIT_FAILURE;
        }
    }
    fprintf_s(stdout, "falp wait event: %d\n", falp_event);

    // falp shutdown
    unsigned int shutdown_flag = FALP_SHUTDOWN_FLAG_BOTH;
	fprintf_s(stdout, "falp_shutdown()...\n");
    if (!falp_shutdown(shutdown_flag)) {
        fprintf_s(stderr, "falp_shutdown error().\n");
        return EXIT_FAILURE;
    }

    // falp wait event
    timeout_value = 5000; 
    falp_event = 0;
    unsigned long falp_event_sd_ready = FALP_EVENT_SHUTDOWNED;
    if (!falp_wait_event(
                timeout_value,
                &falp_event,
                falp_event_sd_ready)) {
        fprintf_s(stderr, "Can't wait event.\n");
        if (lngErrorTimeout != 1) {
            return EXIT_FAILURE; // It isn't always error when falp_wait_event is time out.
        }
    }
    fprintf_s(stdout, "falp wait event: %d\n", falp_event);

    // falp close
	fprintf_s(stdout, "falp_close()...\n");
    if (!falp_close()) {
        fprintf_s(stderr, "Can't falp close.\n");
        return EXIT_FAILURE;
    }

    // transaction unlock
	fprintf_s(stdout, "transaction_unlock()...\n");
    if (!transaction_unlock()) {
        fprintf_s(stderr, "Can't unlock transaction.\n");
        return EXIT_FAILURE;
    }

    // close reader writer
	fprintf_s(stdout, "close_reader_writer()...\n");
    if (!close_reader_writer()) {
        fprintf_s(stderr, "Can't close reader writer.\n");
        return EXIT_FAILURE;
    }

    // dispose library
    if (!dispose_library()) {
        fprintf_s(stderr, "Can't dispose library.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void error_routine(void)
{
    enumernation_felica_error_type felica_error_type;
    enumernation_rw_error_type rw_error_type;
    get_last_error_types(&felica_error_type, &rw_error_type);
    fprintf_s(stderr, "felica_error_type: %d\n", felica_error_type);
    fprintf_s(stderr, "rw_error_type: %d\n", rw_error_type);

    close_reader_writer();
    dispose_library();
}

void print_vector(char* title, unsigned char* vector, int length)
{
    if (title != NULL) {
        fprintf_s(stdout, "%s ", title);
    }

    int i;
    for (i = 0; i < length - 1; i++) {
        fprintf_s(stdout, "%02x ", vector[i]);
    }
    fprintf_s(stdout, "%02x", vector[length - 1]);
    fprintf_s(stdout, "\n");
}