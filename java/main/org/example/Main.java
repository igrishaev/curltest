package org.example;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class Main {

    public static Response test(final CurlEasy c) {
        final OutputStream out = new ByteArrayOutputStream();
        final ByteArrayInputStream in = new ByteArrayInputStream(new byte[]{1, 2 ,3, 4, 5});
        try(// FILE f = FILE.open("foo.txt", "wb");
            FILE f = FILE.open("pom.xml", "rb");
            ReadStream rs = ReadStream.create(in);
            Headers h = Headers.create(new String[] {"foo: bar"});
            Accumulator acc = Accumulator.create(2048);
            WriteStream ws = WriteStream.create(out);
            WriteCallback wc = WriteCallback.create((buf, off, len) -> {
                System.out.printf("lead: %s, off: %s, len: %s%n", buf[0], off, len);
            })
        ) {
            System.out.println(f.ptr());
            final Request request = Request.builder()
                    .url("https://habr.com")
                    // .url("http://127.0.0.1:3000")
                    .method(2)
                    .readFile(f)
                    .followLocation(3)
                    // .writeStream(ws)
                    .verbose(false)
                    // .writeCallback(wc)
//                    .headers(h)
//                    .accum(acc)
                    // .writeFile(f)
                    // .writeStream(out)
                    .build();
            Response r = c.perform(request);
//            System.out.println("----------");
//            System.out.println(acc.getString().substring(0, 100));
//            System.out.println("----------");
            return r;
        }

    }

    public static void main(String... args) throws IOException {
        try (CurlEasy c = CurlEasy.make()) {
            System.out.println(test(c));
        }
    }
}