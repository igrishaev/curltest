package org.example;

import java.io.File;
import java.io.OutputStream;

public class Native {

    static long NULL = 0;

    static {
        final String libPath = new File("curltest.dylib").getAbsolutePath();
        System.load(libPath);
    }

    static native long curl_easy_init();
    static native long curl_easy_setopt_CURLOPT_URL(final long curl, final String url);
    static native long curl_easy_setopt_CURLOPT_FOLLOWLOCATION(final long curl, final int code);
    static native void curl_easy_cleanup(final long curl);
    static native long curl_easy_perform(final long curl);
    static native long curl_easy_setopt_CURLOPT_WRITEDATA(final long curl, final long fp);
    static native long curl_easy_setopt_CURLOPT_WRITEDATA_OUT(final long curl, final long wdPtr);
    static native long fopen(final String path);
    static native void fclose(final long ptr);
    static native long init_write_data_out(final OutputStream out);
    static native void free(final long ptr);
}
