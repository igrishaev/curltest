#ifndef __READ_DATA_H__
#define __READ_DATA_H__

size_t read_callback_stream(char *ptr, size_t size, size_t nmemb, void *userdata);

size_t read_callback_file(char *ptr, size_t size, size_t nmemb, void *userdata);

#endif /* __READ_DATA_H__ */
