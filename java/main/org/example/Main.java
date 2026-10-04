package org.example;

public class Main {

    public static void test(final CurlEasy c) {
        // final OutputStream out = new ByteArrayOutputStream();
        final Request request = Request.builder()
                .url("https://habr.com")
                // .url("http://127.0.0.1:3000")
                .method(1)
                .followLocation(3)
                .addHeader("foo", "bar")
                .accumulate(true)
                // .writeFile("test.html")
                // .writeStream(out)
                .build();
        final long code = c.perform(request);
        if (code != 0) {
            Err.error("non zero code");
        }
    }

    public static void main(String... args) {
        try (CurlEasy c = CurlEasy.make()) {
            test(c);
        }
    }
}