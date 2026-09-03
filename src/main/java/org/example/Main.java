package org.example;

import java.nio.ByteBuffer;

//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
public class Main {

    static {
        // System.load("/opt/homebrew/opt/curl/lib/libcurl.dylib");
        System.load("/Users/ivan/work/curltest/curltest.dylib");
        System.out.println(curlInit());
        System.out.println(foobar());
    }

    static native int curlInit();

    static native int foobar();

    public static void main(String[] args) {

        // final ByteBuffer bb = ByteBuffer.allocateDirect(32000);



        //TIP Press <shortcut actionId="ShowIntentionActions"/> with your caret at the highlighted text
        // to see how IntelliJ IDEA suggests fixing it.

    }
}