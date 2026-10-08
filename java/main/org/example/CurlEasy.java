package org.example;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class CurlEasy implements IResource {

    static {
        Native.loadLib();
        ERROR_SIZE = _get_CURL_ERROR_SIZE();
    }

    private final long ptr;
    private final ByteBuffer bb;
    private boolean isClosed;
    private final static int ERROR_SIZE;

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
        ByteBuffer bb = ByteBuffer.allocateDirect(ERROR_SIZE);
        return new CurlEasy(ptr, bb, false).setErrorBuffer();
    }

    // TODO delete
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

    // TODO: delete
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

    private static String readCString(final ByteBuffer bb) {
        bb.rewind();
        if (bb.get(0) == 0) {
            return null;
        }
        int len = bb.limit();
        int pos = 0;
        for (int i = 0; i < len; i++) {
            if (bb.get(pos) == 0) {
                break;
            }
            pos++;
        }
        ByteBuffer slice = bb.slice(0, pos);
        return StandardCharsets.UTF_8.decode(slice).toString();
    }

    private CurlEasy checkCode(final long code) {
        if (code == 0) {
            return this;
        } else {
            String errorDescription = _get_str_error(code);
            String errorExplanation = readCString(bb);
            throw new RuntimeException(
                    String.format("curl code: %d, description: %s, explanation: %s",
                            code, errorDescription, errorExplanation));
        }
    }

    native private static int _get_CURL_ERROR_SIZE();

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
    private CurlEasy setErrorBuffer() {
        return checkClosed().checkCode(_set_error_buffer(ptr, bb));
    }

    native private static String _get_str_error(long curlCode);

    @Override
    public void close() {
        if (isClosed) return;
        curl_easy_cleanup(ptr);
        isClosed = true;
    }

    public static void main(String... args) {
        try (CurlEasy c = CurlEasy.make()) {
            c
                    .setUrl("https://habr.com")
                    .setMethod(1)
                    .perform();
        }
    }
}
