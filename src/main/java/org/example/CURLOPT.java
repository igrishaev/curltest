package org.example;

public class CURLOPT {

    public static final int URL;
    public static final int FOLLOWLOCATION;
    public static final int ACCEPTTIMEOUT_MS;
    public static final int ACCEPT_ENCODING;
    public static final int CONNECTTIMEOUT;

    static {

        final Arena arena = Arena.of(128);
        // System.out.println(arena.ptr());
        Native.read_curl_constants(arena.ptr());

        arena.orderJNI();
        arena.rewind();

        URL = arena.getInt();
        FOLLOWLOCATION = arena.getInt();
        ACCEPTTIMEOUT_MS = arena.getInt();
        ACCEPT_ENCODING = arena.getInt();
        CONNECTTIMEOUT = arena.getInt();

        // arena.debug(64);

    }
}
