package org.example;

import java.io.File;
import java.net.URI;
import java.net.URL;

public record Curl(long ptr) implements AutoCloseable {

    private void error(final String message) {
        throw new RuntimeException(message);
    }

    private void error(final String template, final Object... args) {
        throw new RuntimeException(String.format(template, args));
    }

    private void checkResult(final long result, final String message) {
        if (result != 0) {
            error("cURL operation has failed, code: %s, message: %s", message);
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

    public void curlOptWriteData(final String path) {
        checkResult(Native.curl_easy_setopt_CURLOPT_WRITEDATA(ptr, path), "CURLOPT_WRITEDATA");
    }

    public void perform() {
        checkResult(Native.curl_easy_perform(ptr), "curl_easy_perform");
    }

    @Override
    public void close() {
        Native.curl_easy_cleanup(ptr);
    }

    public static void main(final String... args) {
        final long t1 = System.currentTimeMillis();
        try(Curl curl = Curl.init()) {
            curl.curlOptFollowLocation(1);
            curl.curlOptUtl("https://habr.com");
            curl.curlOptWriteData("foobar.html");
            curl.perform();
        }
        final long t2 = System.currentTimeMillis();
        System.out.println(t2 - t1);
    }
}
