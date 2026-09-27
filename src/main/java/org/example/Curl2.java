package org.example;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public record Curl2() {

    final static int CURLOPT_URL = 10001;
    final static int CURLOPT_FOLLOWLOCATION = 10002;
    final static int NULL = 0;
    final static byte TERM = 0;

    public void foo(final Request request, final Arena arena) {

        arena.orderJNI();

        // allocate counter
        int counter = 0;
        arena.putInt(counter);

        int dataOffset = 1024;

        // CURLOPT_URL
        arena.putInt(CURLOPT_URL);
        if (request.url() == null) {
            arena.putInt(NULL);
        } else {
            byte[] url = request.url().getBytes(StandardCharsets.UTF_8);
//            arena.put(dataOffset, url);
//            arena.put(dataOffset + url.length, TERM);
            dataOffset += url.length + 1;
            arena.putInt(dataOffset);
        }
        counter++;

        // CURLOPT_FOLLOWLOCATION
        arena.putInt(CURLOPT_FOLLOWLOCATION);
        arena.putInt(request.followlocation().code);

        // set the final
        arena.putInt(0, counter);
    }

}
