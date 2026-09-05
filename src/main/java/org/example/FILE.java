package org.example;

public record FILE(long ptr, String path) implements AutoCloseable {

    public FILE open(final String path) {
        final long ptr = Native.fopen(path);
        return new FILE(ptr, path);
    }

    @Override
    public void close() {
        Native.fclose(ptr);
    }
}
