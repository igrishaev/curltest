package org.example;

import java.io.IOException;
import java.io.InputStream;

public record ReadDataStream(long ptr, InputStream in) implements AutoCloseable {

    public static ReadDataStream wrap(final InputStream in) {
        long ptr = 42;
        return new ReadDataStream(ptr, in);
    }

    @Override
    public void close() throws IOException {
        Native.close_user_data(ptr);
        in.close();
    }
}
