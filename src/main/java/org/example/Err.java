package org.example;

public class Err {

    public static void error(final String template, final Object... args) {
        throw new RuntimeException(String.format(template, args));
    }
}
