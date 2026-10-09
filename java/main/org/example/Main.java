package org.example;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class Main {

    public static void main(String... args) throws IOException {
        try (CurlEasy c = CurlEasy.make();
             Headers hh = Headers.create(new String[] {"foo: bar"});
             FILE wf = FILE.open("aaa.txt", "wb");
             OutputStream out = new ByteArrayOutputStream(32);
             Accumulator acc = Accumulator.create(2048);
             WriteStream ws = WriteStream.create(out)
        ) {
            c
                    .resetOptions()
                    .setUrl("https://habr.com")
                    .setHeaders(hh)
                    .setFollowLocation(3)
                    .setAccumulator(acc)
                    // .setWriteFile(wf)
                    // .setWriteStream(ws)
                    .setMethod(1)
                    // .setVerbose(true)
                    // .setPostData(new byte[] {1, 2, 3})
                    .perform();
            System.out.println(acc.getString());
        }
    }
}