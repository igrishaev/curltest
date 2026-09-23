package org.example;

import java.io.File;
import java.nio.file.Path;

public record FILE(long fd, String path) implements AutoCloseable {

    // TODO: pass mode
    public static FILE open(final String path) {
        final long fd = Native.fopen(path);
        if (fd == Native.NULL) {
            Err.error("fopen failed: %s", path);
        }
        return new FILE(fd, path);
    }

    public static FILE open(final File file) {
        return open(file.getAbsolutePath());
    }

    public static FILE open(final Path path) {
        return open(path.toAbsolutePath().toString());
    }

    @Override
    public void close() {
        Native.fclose(fd);
    }
}
