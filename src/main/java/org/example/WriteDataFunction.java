package org.example;

public record WriteDataFunction(long ptr, IWriteHandler handler) implements AutoCloseable {

    public static WriteDataFunction wrap(final IWriteHandler handler) {
        final long ptr = 123;
        return new WriteDataFunction(ptr, handler);
    }

    @Override
    public void close() {
        // release ptr, handler
    }
}
