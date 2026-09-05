package org.example;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.net.URI;
import java.net.URL;

public record Curl(long ptr) implements AutoCloseable {

    private void checkResult(final long result, final String message) {
        if (result != 0) {
            Err.error("cURL operation has failed, code: %s, message: %s", result, message);
        }
    }

    public static Curl init() {
        final long ptr = Native.curl_easy_init();
        return new Curl(ptr);
    }

    public void curlOptFollowLocation(final int code) {
        checkResult(Native.curl_easy_setopt_CURLOPT_FOLLOWLOCATION(ptr, code), "CURLOPT_FOLLOWLOCATION");
    }

    public void curlOptUtl(final String url) {
        checkResult(Native.curl_easy_setopt_CURLOPT_URL(ptr, url), "CURLOPT_URL");
    }

    public void curlOptUtl(final URL url) {
        curlOptUtl(url.toString());
    }

    public void curlOptUtl(final URI uri) {
        curlOptUtl(uri.toString());
    }

    public void curlOptWriteData(final FILE file) {
        checkResult(Native.curl_easy_setopt_CURLOPT_WRITEDATA(ptr, file.ptr()), "CURLOPT_WRITEDATA");
    }

    public void curlOptWriteData(final WriteDataOut wdOut) {
        checkResult(Native.curl_easy_setopt_CURLOPT_WRITEDATA_OUT(ptr, wdOut.ptr()), "CURLOPT_WRITEDATA");
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
            curl.curlOptUtl("https://habr.com");
            try (ByteArrayOutputStream out = new ByteArrayOutputStream(32);
                WriteDataOut writeDataOut = WriteDataOut.allocate(out)) {
                curl.curlOptWriteData(writeDataOut);
                curl.perform();
            }
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
