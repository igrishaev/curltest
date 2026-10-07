#ifndef __WRITE_DATA_H__
#define __WRITE_DATA_H__

size_t write_callback_stream(char *data, size_t size, size_t nmemb, void *userdata);

size_t write_callback_handler(char *data, size_t size, size_t nmemb, void *userdata);

#endif /* __WRITE_DATA_H__ */
