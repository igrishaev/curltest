package org.example;

import java.io.File;

public class Native {
    static final long NULL;

    static void loadLib() {
        final String libPath = new File("resources/curltest.dylib").getAbsolutePath();
        System.load(libPath);
    }

    static {
        loadLib();
        NULL = get_null();
    }
    native private static long get_null();
}
