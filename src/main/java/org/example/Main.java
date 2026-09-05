package org.example;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;

//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
public class Main {

    static {
        // System.load("/opt/homebrew/opt/curl/lib/libcurl.dylib");
        System.load("/Users/ivan/work/curltest/curltest.dylib");
        System.out.println(curlInit());
    }

    static native int curlInit();

    static native int foobar(OutputStream out);

    static native int test1();

    public static void main(String[] args) throws InterruptedException {

        var baos = new ByteArrayOutputStream();
        System.out.println(foobar(baos));
        System.out.println(new String(baos.toByteArray()).substring(0, 100));

        // System.out.println(test1());

        // Thread.sleep(1000);

        // System.out.println(Arrays.toString(baos.toByteArray()));

        // final ByteBuffer bb = ByteBuffer.allocateDirect(32000);



        //TIP Press <shortcut actionId="ShowIntentionActions"/> with your caret at the highlighted text
        // to see how IntelliJ IDEA suggests fixing it.

    }
}