package org.example;

import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public class Arena {

    final int bbLen;
    final private ByteBuffer bb;
    final private long ptr;
    final private ByteOrder BO_JVM;
    final private ByteOrder BO_JNI;
    final private long NULL;

    public long ptr() {
        return ptr;
    }

    static {
        final String libPath = new File("arena.dylib").getAbsolutePath();
        System.load(libPath);
    }

    private static native int initByteBuffer(final ByteBuffer bb);

    private Arena(final int bbLen, final ByteBuffer bb, final long ptr, final ByteOrder BO_JVM,
                  final ByteOrder BO_JNI, final long NULL) {
        this.bbLen = bbLen;
        this.bb = bb;
        this.ptr = ptr;
        this.BO_JVM = BO_JVM;
        this.BO_JNI = BO_JNI;
        this.NULL = NULL;
    }

    public static Arena of(final int size) {
        // TODO: check min size
        final ByteBuffer bb = ByteBuffer.allocateDirect(size);
        final int initStatus = initByteBuffer(bb);
        if (initStatus != 0) {
            throw new RuntimeException("failed to init byte buffer, code: " + initStatus);
        }

        final ByteOrder BO_JVM = ByteOrder.BIG_ENDIAN;

        // byte order
        final byte lead = bb.get();
        final ByteOrder BO_JNI = (lead == 1) ? ByteOrder.LITTLE_ENDIAN : ByteOrder.BIG_ENDIAN;

        // other fields
        bb.order(BO_JNI);
        final long NULL = bb.getLong();
        final long ptr = bb.getLong();

        return new Arena(size, bb, ptr, BO_JVM, BO_JNI, NULL);
    }

    public void rewind() {
        bb.rewind();
    }

    public void orderJVM() {
        bb.order(BO_JVM);
    }

    public void orderJNI() {
        bb.order(BO_JNI);
    }

    public int getInt() {
        return bb.getInt();
    }

    public long getLong() {
        return bb.getLong();
    }

    public void putInt(final int i) {
        bb.putInt(i);
    }

    public void putInt(final int index, final int i) {
        bb.putInt(index, i);
    }

    public void putLong(final int index, final long l) {
        bb.putLong(index, l);
    }

    public int position() {
        return bb.position();
    }

    public void skip(final int len) {
        final int pos = bb.position();
        bb.position(pos + len);
    }

    public String getLenString() {
        final int len = bb.getInt();
        final byte[] ba = new byte[len];
        bb.get(ba);
        return new String(ba, StandardCharsets.UTF_8);
    }

    public void get(final byte[] ba) {
        bb.get(ba);
    }

    public String getString(final int len) {
        final byte[] ba = new byte[len];
        bb.get(ba);
        return new String(ba, StandardCharsets.UTF_8);
    }

    public String getString(final int index, final int len) {
        final byte[] ba = new byte[len];
        bb.get(index, ba);
        return new String(ba, StandardCharsets.UTF_8);
    }

    @SuppressWarnings("unused")
    void debug(final int len) {
        final byte[] ba = new byte[len];
        bb.get(0, ba);
        System.out.println(Arrays.toString(ba));
    }

    public static void main(final String... args) {
        final Arena a = Arena.of(64);
        a.debug(64);
        System.out.println(a.BO_JNI);
        System.out.println(a.BO_JVM);
        System.out.println(a.bbLen);
        System.out.println(a.ptr);
        System.out.println(a.NULL);

    }

}