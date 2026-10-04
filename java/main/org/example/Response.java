package org.example;

import java.util.Map;

public class Response {

    static {
        Native.loadLib();
    }

    public int status = -1;
    public Map<String, String> headers;
    public Object body;
    public long contentLength = -1;
    public String effectiveUrl;

    native long from_curl(final long ptr);

}
