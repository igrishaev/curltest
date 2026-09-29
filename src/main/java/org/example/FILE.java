package org.example;

import java.io.File;
import java.nio.file.Path;

public class FILE implements AutoCloseable {

    private final long ptr;
    private final String path;
    private final String mode;
    private boolean closed;

    public long fd() {
        return ptr;
    }

    private FILE(final long ptr, final String path, final String mode, final boolean closed) {
        this.ptr = ptr;
        this.path = path;
        this.mode = mode;
        this.closed = closed;
    }

    @Override
    public String toString() {
        return String.format("<FILE %s %s>", mode, path);
    }

    public static FILE open(final String path) {
        final String mode = "rw";
        final long fd = fopen(path, mode);
        if (fd < 0) { // TODO
            Err.error("fopen failed: %s", path); // TODO
        }
        return new FILE(fd, path, mode, false);
    }

    public native static long fopen(final String path, final String mode);

    public native static long fclose(final long ptr);

    public static FILE open(final File file) {
        return open(file.getAbsolutePath());
    }

    public static FILE open(final Path path) {
        return open(path.toAbsolutePath().toString());
    }

    @Override
    public void close() {
        if (!closed) {
            Native.fclose(ptr);
        }
        closed = true;
    }
}
