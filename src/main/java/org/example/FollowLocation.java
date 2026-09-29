package org.example;

// https://curl.se/libcurl/c/CURLOPT_FOLLOWLOCATION.html

public enum FollowLocation {

    DEFAULT(0),
    ALL(H.CURLFOLLOW_ALL),
    OBEY_CODE(H.CURLFOLLOW_OBEYCODE),
    FIRST_ONLY(H.CURLFOLLOW_FIRSTONLY);

    public final int code;

    FollowLocation(final int code) {
        this.code = code;
    }

}
