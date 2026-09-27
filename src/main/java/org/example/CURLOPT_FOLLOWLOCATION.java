package org.example;

// https://curl.se/libcurl/c/CURLOPT_FOLLOWLOCATION.html

public enum CURLOPT_FOLLOWLOCATION {

    DEFAULT(0),
    CURLFOLLOW_ALL(1),
    CURLFOLLOW_OBEYCODE(2),
    CURLFOLLOW_FIRSTONLY(3);

    public final int code;

    CURLOPT_FOLLOWLOCATION(final int code) {
        this.code = code;
    }


}
