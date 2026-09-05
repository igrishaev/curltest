package org.example;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.OutputStream;

public class Main {

    static {
        final String libPath = new File("curltest.dylib").getAbsolutePath();
        System.load(libPath);
        System.out.println(curlInit());
    }

    static native int curlInit();

    static native int foobar(OutputStream out);

    static native int test1();

    public static void main(String[] args) throws InterruptedException {
        var baos = new ByteArrayOutputStream();
        System.out.println(foobar(baos));
        System.out.println(baos.toString().substring(0, 100));
    }
}