package org.example;

import java.io.IOException;
import java.io.InputStream;

public record ReadStream(long ptr, InputStream in) implements AutoCloseable {

    public static ReadStream wrap(final InputStream in) {
        long ptr = Native.init_read_data_stream(in);
        return new ReadStream(ptr, in);
    }

    @Override
    public void close() throws IOException {
        Native.close_user_data(ptr);
        in.close();
    }
}
