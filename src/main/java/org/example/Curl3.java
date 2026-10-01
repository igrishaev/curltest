package org.example;

import java.io.File;

public class Curl3 {

    static {
        final String libPath = new File("curltest.dylib").getAbsolutePath();
        System.load(libPath);
    }

    native public static long perform(
            final String url,
            final int method,
            final int followLocation,
            final String[] headers
    );

    public static void main(final String... args) {
        final long code = perform(
                "https://habr.com",
                1,
                3,
                new String[]{"foo: bar"}
        );
        System.out.println(code);
    }
}
