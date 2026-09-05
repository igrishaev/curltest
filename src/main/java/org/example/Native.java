package org.example;

import java.io.File;

public class Native {
    static {
        final String libPath = new File("curltest.dylib").getAbsolutePath();
        System.load(libPath);
    }
    static native long curl_easy_init();
    static native long curl_easy_setopt_CURLOPT_URL(final long curl, final String url);
    static native long curl_easy_setopt_CURLOPT_FOLLOWLOCATION(final long curl, final int code);
    static native void curl_easy_cleanup(final long curl);
    static native long curl_easy_perform(final long curl);
    static native long curl_easy_setopt_CURLOPT_WRITEDATA(final long curl, final String path);
}
