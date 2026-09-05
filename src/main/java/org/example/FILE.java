package org.example;

public record FILE(long ptr, String path) implements AutoCloseable {

    // TODO: pass mode
    public static FILE open(final String path) {
        final long ptr = Native.fopen(path);
        if (ptr == Native.NULL) {
            Err.error("fopen failed: %s", path);
        }
        return new FILE(ptr, path);
    }

    @Override
    public void close() {
        Native.fclose(ptr);
    }
}
