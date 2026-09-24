package org.example;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public record WriteDataStream(long ptr, OutputStream out) implements AutoCloseable {

    public static WriteDataStream wrap(final OutputStream out) {
        final long ptr = Native.init_write_data_stream(out);
        return new WriteDataStream(ptr, out);
    }

    public static WriteDataStream allocate(final int len) {
        return wrap(new ByteArrayOutputStream(len));
    }

    public static WriteDataStream allocate() {
        return wrap(new ByteArrayOutputStream());
    }

    @Override
    public void close() throws IOException {
        Native.close_user_data(ptr);
        out.close();
    }

    public static void main(final String... args) throws IOException {
        try (ByteArrayOutputStream out = new ByteArrayOutputStream(32);
             WriteDataStream wd = WriteDataStream.wrap(out)) {
            System.out.println(wd);
        }
    }
}
