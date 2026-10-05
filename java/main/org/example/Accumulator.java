package org.example;

public class Accumulator implements AutoCloseable {

    private final long ptr;
    private boolean isClosed;

    static {
        Native.loadLib();
    }

    public long ptr() {
        return ptr;
    }

    private Accumulator(long ptr, boolean isClosed) {
        this.ptr = ptr;
        this.isClosed = isClosed;
    }

    public static Accumulator create(long initSize) {
        long ptr = _allocate(initSize);
        if (ptr == Native.NULL) {
            Err.error("failed to allocate accumulator");
        }
        return new Accumulator(ptr, false);
    }

    @Override
    public String toString() {
        return String.format("<Accumulator %s>", ptr);
    }

    private native static long _allocate(final long initSize);
    private native static long _free(final long ptr);

    @Override
    public void close() {
        if (!isClosed) {
            final long code = _free(ptr);
            isClosed = true;
            if (code != 0) {
                Err.error("failed to close accumulator, code: %s", code);
            }
        }
    }
}
