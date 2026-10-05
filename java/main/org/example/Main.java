package org.example;

public class Main {

    public static Response test(final CurlEasy c) {
        // final OutputStream out = new ByteArrayOutputStream();
        try(FILE f = FILE.open("foo.txt", "wb");
            Headers h = Headers.create(new String[] {"foo: bar"})) {
            final Request request = Request.builder()
                    .url("https://habr.com")
                    // .url("http://127.0.0.1:3000")
                    .method(1)
                    .followLocation(3)
                    .headers(h)
                    .writeFile(f)
                    // .accumulate(true)
                    // .writeFile("test.html")
                    // .writeStream(out)
                    .build();
            return c.perform(request);
        }

    }

    public static void main(String... args) {
        try (CurlEasy c = CurlEasy.make()) {
            System.out.println(test(c));
        }
    }
}