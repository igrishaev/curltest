#ifndef __READ_DATA_H__
#define __READ_DATA_H__

#include "curl/curl.h"

size_t read_callback_stream(char *ptr, size_t size, size_t nmemb, void *userdata);

size_t read_callback_file(char *ptr, size_t size, size_t nmemb, void *userdata);

int read_seek_file(void *userdata, curl_off_t offset, int origin);

int read_seek_cannot(void *userdata, curl_off_t offset, int origin);

#endif /* __READ_DATA_H__ */
