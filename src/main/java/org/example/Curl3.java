package org.example;

import org.example.http.Request;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.OutputStream;

public class Curl3 {

    static {
        final String libPath = new File("curltest.dylib").getAbsolutePath();
        System.load(libPath);
    }

    native public static long perform(Request request);

    public static void main(final String... args) {
        final OutputStream out = new ByteArrayOutputStream();
        final Request request = Request.builder()
                .url("https://habr.com")
                .method(1)
                .followLocation(3)
                .addHeader("foo", "bar")
                // .writeFile("test.html")
                .writeStream(out)
                .build();
        final long code = perform(request);
        System.out.println(out.toString().substring(0, 10));
    }
}
