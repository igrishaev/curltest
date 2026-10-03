package org.example;

import org.example.http.Request;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.OutputStream;

public class Curl3 implements AutoCloseable {

    private final long ptr;

    private Curl3(long ptr) {
        this.ptr = ptr;
    }

    public static Curl3 create() {
        final long ptr = _init();
        return new Curl3(ptr);
    }

    static {
        final String libPath = new File("curltest.dylib").getAbsolutePath();
        System.load(libPath);
    }

    native public static long _init();
    native public static long _free(final long curlPtr);

    native public static long perform(final long curl, Request request);

    public static void test(final Curl3 c) {
        // final OutputStream out = new ByteArrayOutputStream();
        final Request request = Request.builder()
                // .url("https://habr.com")
                .url("http://127.0.0.1:3000")
                .method(1)
                .followLocation(3)
                .addHeader("foo", "bar")
                .accumulate(true)
                // .writeFile("test.html")
                // .writeStream(out)
                .build();
        final long code = perform(c.ptr, request);
        if (code != 0) {
            throw new RuntimeException("non zero code");
        }
    }

    public static void main(final String... args) {
        try(Curl3 curl = create()) {
            test(curl);
        }
    }

    @Override
    public void close() {
        _free(ptr);
    }
}
