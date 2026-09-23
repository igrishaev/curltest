package org.example;

public record FILE(long fd, String path) implements AutoCloseable {

    // TODO: pass mode
    public static FILE open(final String path) {
        final long fd = Native.fopen(path);
        if (fd == Native.NULL) {
            Err.error("fopen failed: %s", path);
        }
        return new FILE(fd, path);
    }

    @Override
    public void close() {
        Native.fclose(fd);
    }
}
