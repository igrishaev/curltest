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

    public void putNULL() {
        bb.putLong(NULL);
    }

    public void putNULL(final int index) {
        bb.putLong(index, NULL);
    }

    static {
        final String libPath = new File("org_example_Arena.dylib").getAbsolutePath();
        System.load(libPath);
    }

    private static native int initByteBuffer(final ByteBuffer bb);

    private final static byte TERM = 0;

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
        byte lead;

        // byte order for JVM
        bb.putLong(1);
        lead = bb.get(0);
        final ByteOrder BO_JVM = (lead == 1) ? ByteOrder.LITTLE_ENDIAN : ByteOrder.BIG_ENDIAN;

        final int initStatus = initByteBuffer(bb);
        if (initStatus != 0) {
            throw new RuntimeException("failed to init byte buffer, code: " + initStatus);
        }
        bb.rewind();

        // byte order for JNI
        lead = bb.get();
        final ByteOrder BO_JNI = (lead == 1) ? ByteOrder.LITTLE_ENDIAN : ByteOrder.BIG_ENDIAN;

        // other fields
        bb.order(BO_JNI);
        final long NULL = bb.getLong();
        final long ptr = bb.getLong();

        // TODO
        bb.rewind();
        bb.putLong(0);
        bb.putLong(0);
        bb.putLong(0);
        bb.rewind();

        return new Arena(size, bb, ptr, BO_JVM, BO_JNI, NULL);
    }

    public int putCString(final String s) {
        final byte[] buf = s.getBytes(StandardCharsets.UTF_8);
        bb.put(buf);
        bb.put(TERM);
        return buf.length + 1;
    }

    public int putCString(final int index, final String s) {
        final byte[] buf = s.getBytes(StandardCharsets.UTF_8);
        bb.put(index, buf);
        bb.put(index + buf.length, TERM);
        return buf.length + 1;
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

    public void putLong(final long l) {
        bb.putLong(l);
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

    public void get(final byte[] ba) {
        bb.get(ba);
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
        System.out.println(a.BO_JVM);
        System.out.println(a.BO_JNI);
        System.out.println(a.bbLen);
        System.out.println(a.ptr);
        System.out.println(a.NULL);

    }

}