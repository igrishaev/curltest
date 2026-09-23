package org.example;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public record WriteStream(long ptr, OutputStream out) implements AutoCloseable {

    public static WriteStream wrap(final OutputStream out) {
        final long ptr = Native.init_write_data_stream(out);
        return new WriteStream(ptr, out);
    }

    public static WriteStream allocate(final int len) {
        return wrap(new ByteArrayOutputStream(len));
    }

    public static WriteStream allocate() {
        return wrap(new ByteArrayOutputStream());
    }

    @Override
    public void close() throws IOException {
        Native.close_write_data_stream(ptr);
        out.close();
    }

    public static void main(final String... args) throws IOException {
        try (ByteArrayOutputStream out = new ByteArrayOutputStream(32);
             WriteStream wd = WriteStream.wrap(out)) {
            System.out.println(wd);
        }
    }
}
