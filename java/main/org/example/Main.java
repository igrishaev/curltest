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
        final long code = CurlEasy.perform(c.getPtr(), request);
        if (code != 0) {
            throw new RuntimeException("non zero code");
        }
    }

    public static void main(String... args) throws InterruptedException {
        try (CurlEasy c = CurlEasy.make()) {
            test(c);
        }
    }
}