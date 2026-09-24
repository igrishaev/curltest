package org.example;

public record WriteFunction(long ptr, IWriteHandler handler) implements AutoCloseable {

    public static WriteFunction wrap(final IWriteHandler handler) {
        final long ptr = Native.init_write_data_handler(handler);
        return new WriteFunction(ptr, handler);
    }

    @Override
    public void close() {
        Native.close_write_data_handler(ptr);
    }
}
