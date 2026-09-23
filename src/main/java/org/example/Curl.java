package org.example;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.net.URL;

public record Curl(long ptr, byte[] buf) implements AutoCloseable {

    private void checkResult(final long result, final String message) {
        if (result != 0) {
            Err.error("cURL operation has failed, code: %s, message: %s", result, message);
        }
    }

    public static Curl init() {
        final long ptr = Native.curl_easy_init();
        final byte[] buf = new byte[32000];
        return new Curl(ptr, buf);
    }

    public void curlOptFollowLocation(final int code) {
        checkResult(Native.curl_easy_setopt_CURLOPT_FOLLOWLOCATION(ptr, code), "CURLOPT_FOLLOWLOCATION");
    }

    public void curlOptURL(final String url) {
        checkResult(Native.curl_easy_setopt_CURLOPT_URL(ptr, url), "CURLOPT_URL");
    }

    public void curlOptURL(final URL url) {
        curlOptURL(url.toString());
    }

    public void curlOptURL(final URI uri) {
        curlOptURL(uri.toString());
    }

    public void curlOptWriteData(final FILE file) {
        checkResult(Native.curl_easy_setopt_CURLOPT_WRITEDATA_file(ptr, file.fd()), "CURLOPT_WRITEDATA");
    }

    public void curlOptWriteData(final WriteDataStream writeDataStream) {
        checkResult(Native.curl_easy_setopt_CURLOPT_WRITEDATA_stream(ptr, writeDataStream.ptr()), "CURLOPT_WRITEDATA");
    }

    public void perform() {
        checkResult(Native.curl_easy_perform(ptr), "curl_easy_perform");
    }

    @Override
    public void close() {
        Native.curl_easy_cleanup(ptr);
    }

    private static void test(final Curl curl, final int i) {
        final long t1 = System.currentTimeMillis();
//        try(Curl curl = Curl.init()) {
//            curl.curlOptFollowLocation(1);
//            curl.curlOptUtl("https://habr.com");
//            curl.curlOptWriteData("foobar" + i + ".html");
//            curl.perform();
//        }
        try (FILE f = FILE.open("foobar" + i + ".html") ) {
            curl.curlOptWriteData(f);
            curl.perform();
        }
        final long t2 = System.currentTimeMillis();
        System.out.println(t2 - t1);
    }

    public static void main(final String... args) throws IOException {
//        try (FILE f = FILE.open("/SSSS")) {
//            System.out.println(f);
//        }

        try (Curl curl = Curl.init()) {
            curl.curlOptFollowLocation(1);
            curl.curlOptURL("https://habr.com");
            try (FILE f = FILE.open("foobar.txt")) {
                curl.curlOptWriteData(f);
                curl.perform();

            }
//            try (ByteArrayOutputStream baos = new ByteArrayOutputStream(32);
//                WriteDataStream writeDataStream = WriteDataStream.wrap(baos)) {
//                curl.curlOptWriteData(writeDataStream);
//                curl.perform();
//                System.out.println(baos);
//            }
        }

//        final Runtime runtime = Runtime.getRuntime();
//        long m1 = runtime.totalMemory() - runtime.freeMemory();
//        try (Curl curl = Curl.init()) {
//            curl.curlOptFollowLocation(1);
//            curl.curlOptUtl("https://habr.com");
//            for (int i = 0; i < 10; i++) {
//                test(curl, i);
//            }
//        }
//        long m2 = runtime.totalMemory() - runtime.freeMemory();
//        System.out.println("Memory increased: " + (m2 - m1) / 1000000);
    }
}
