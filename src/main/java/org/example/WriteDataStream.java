package org.example;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public record WriteDataStream(long ptr, OutputStream out) implements AutoCloseable {

    public static WriteDataStream wrap(final OutputStream out) {
        final long ptr = Native.init_write_data_out(out);
        return new WriteDataStream(ptr, out);
    }

    @Override
    public void close() {
        Native.close_write_data_out(ptr);
        // TODO out.close();
    }

    public static void main(final String... args) throws IOException {
        try (ByteArrayOutputStream out = new ByteArrayOutputStream(32);
             WriteDataStream wd = WriteDataStream.wrap(out)) {
            System.out.println(wd);
        }
    }
}
