package org.example;

import java.io.File;
import java.nio.file.Path;

public class FILE implements AutoCloseable {

    private final long ptr;
    private final String path;
    private final String mode;
    private boolean closed;

    static {
        final String libPath = new File("curltest.dylib").getAbsolutePath();
        System.load(libPath);
    }

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

    private static RuntimeException error(final String template, final Object... args) {
        return new RuntimeException(String.format(template, args));
    }

    public static FILE open(final String path, final String mode) {
        final long fd = fopen(path, mode);
        if (fd < 0) {
            throw error("fopen failed, code: %s, mode: %s, path: %s", fd, mode, path);
        }
        return new FILE(fd, path, mode, false);
    }

    public native static long fopen(final String path, final String mode);

    public native static long fclose(final long ptr);

    public static FILE open(final File file, final String mode) {
        return open(file.getAbsolutePath(), mode);
    }

    public static FILE open(final Path path, final String mode) {
        return open(path.toAbsolutePath().toString(), mode);
    }

    @Override
    public void close() {
        if (!closed) {
            final long code = fclose(ptr);
            if (code != 0) {
                throw error("fclose failed, code: %s, mode: %s, path: %s", code, mode, path);
            }
        }
        closed = true;
    }

    public static void main(final String... args) {
        try (final FILE f = open("hello.txt", "wb")) {
            System.out.println(f);
        }
    }
}
