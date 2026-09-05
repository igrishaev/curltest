package org.example;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public record WriteDataOut(long ptr, OutputStream out) implements AutoCloseable {

    public static WriteDataOut allocate(final OutputStream out) {
        final long ptr = Native.init_write_data_out(out);
        return new WriteDataOut(ptr, out);
    }

    @Override
    public void close() throws IOException {
        Native.free(ptr);
        out.close();
    }

    public static void main(final String... args) throws IOException {
        try (ByteArrayOutputStream out = new ByteArrayOutputStream(32);
             WriteDataOut wd = WriteDataOut.allocate(out)) {
            System.out.println(wd);
        }
    }
}
