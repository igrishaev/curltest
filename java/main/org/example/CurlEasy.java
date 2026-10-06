package org.example;

public class CurlEasy implements IResource {

    static {
        Native.loadLib();
    }

    private final long ptr;
    private boolean isClosed;

    private CurlEasy(long ptr, boolean isClosed) {
        this.ptr = ptr;
        this.isClosed = isClosed;
    }

    public static CurlEasy make() {
        final long ptr = curl_easy_init();
        if (ptr == Native.NULL) {
            Err.error("failed to initialize cURL");
        }
        return new CurlEasy(ptr, false);
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

    @Override
    public void close() {
        if (isClosed) return;
        curl_easy_cleanup(ptr);
        isClosed = true;
    }
}
