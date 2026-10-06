package org.example;

import java.util.HashMap;
import java.util.Map;

public class Response {

    static {
        Native.loadLib();
    }

    public int status = -1;
    public Map<String, String> headers = new HashMap<>();
    public long contentLength = -1;
    public String effectiveUrl;

    @SuppressWarnings("unused")
    public void addHeader(final String name, final String value) {
        headers.put(name, value);
    }

    @Override
    public String toString() {
        return String.format("<Response [status: %s, content-length: %s, effective URL: %s, headers: %s]>",
                status, contentLength, effectiveUrl, headers.toString()
        );
    }

    native long from_curl(final long ptr);

}
