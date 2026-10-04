package org.example;

import java.io.File;

public class CurlEasy implements AutoCloseable {

    static {
        final String libPath = new File("resources/curltest.dylib").getAbsolutePath();
        System.load(libPath);
    }

    private final long ptr;
    private boolean closed;

    private CurlEasy(long ptr, boolean closed) {
        this.ptr = ptr;
        this.closed = closed;
    }

    public static CurlEasy make() {
        final long ptr = curl_easy_init();
        return new CurlEasy(ptr, false);
    }

    public long getPtr() {
        return ptr;
    }

    native public static long curl_easy_init();
    native public static long curl_easy_cleanup(final long curlPtr);
    native public static long perform(final long curl, Request request);

    @Override
    public void close() {
        if (!closed) {
            curl_easy_cleanup(ptr);
        }
        closed = true;
    }
}
