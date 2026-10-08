package org.example;

import java.nio.ByteBuffer;

public class CurlEasy implements IResource {

    static {
        Native.loadLib();
    }

    private final long ptr;
    private final ByteBuffer bb;
    private boolean isClosed;

    private CurlEasy(long ptr, ByteBuffer bb, boolean isClosed) {
        this.ptr = ptr;
        this.bb = bb;
        this.isClosed = isClosed;
    }

    public static CurlEasy make() {
        final long ptr = curl_easy_init();
        if (ptr == Native.NULL) {
            Err.error("failed to initialize cURL");
        }

        ByteBuffer bb = ByteBuffer.allocateDirect(2048);
        return new CurlEasy(ptr, bb, false);
    }

    public Response perform(final Request request) {
        final long code = curl_easy_perform(ptr, request);
        if (code != 0) {
            Err.error("failed with non-zero code: %s", code);
        }
        final Response response = new Response();
        response.from_curl(ptr);
        return response;
    }

    @Override
    public long ptr() {
        return ptr;
    }

    native public static long curl_easy_init();
    native public static long curl_easy_cleanup(final long curlPtr);
    native public static long curl_easy_perform(final long curl, Request request);

    private CurlEasy checkClosed() {
        if (isClosed) {
            throw new RuntimeException("This CurlEasy resource has been closed");
        } else {
            return this;
        }
    }

    private CurlEasy checkCode(final long code) {
        if (code == 0) {
            return this;
        } else {
            // curl_easy_strerror
            bb.rewind();
            byte[] buf = new byte[2048];
            bb.get(buf);
            String msg = new String(buf);
            System.out.println(msg);
            throw new RuntimeException(String.format("curl code: %d", code));
        }
    }

    native private static long _set_url(final long curlPtr, final String url);
    public CurlEasy setUrl(final String url) {
        return checkClosed().checkCode(_set_url(ptr, url));
    }

    native private static long _set_method(final long curlPtr, final long method);
    public CurlEasy setMethod(final long method) {
        return checkClosed().checkCode(_set_method(ptr, method));
    }

    native private static long _perform(final long curlPtr);
    public CurlEasy perform() {
        return checkClosed().checkCode(_perform(ptr));
    }

    native private static long _set_error_buffer(long curlPtr, ByteBuffer bb);
    public CurlEasy setErrorBuffer() {
        return checkClosed().checkCode(_set_error_buffer(ptr, bb));
    }

    @Override
    public void close() {
        if (isClosed) return;
        curl_easy_cleanup(ptr);
        isClosed = true;
    }

    public static void main(String... args) {
        try (CurlEasy c = CurlEasy.make()) {
            c
                    .setErrorBuffer()
                    .setUrl(null)
                    .setMethod(1)
                    .perform();
        }
    }
}
