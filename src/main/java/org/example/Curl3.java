package org.example;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.OutputStream;

public class Curl3 {

    static {
        final String libPath = new File("curltest.dylib").getAbsolutePath();
        System.load(libPath);
    }

    native public static long perform(
            final String url,
            final int method,
            final int followLocation,
            final String[] headers,
            final String writeFile,
            final OutputStream writeStream
            );

    public static void main(final String... args) {
        final OutputStream out = new ByteArrayOutputStream();
        final long code = perform(
                "https://habr.com",
                1,
                3,
                new String[]{"foo: bar"},
                null, // "foo2.html",
                out
        );
        System.out.println(out.toString().substring(0, 10));
    }
}
