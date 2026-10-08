package org.example;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Objects;

public class CurlEasy implements IResource {

    static {
        Native.loadLib();
        ERROR_SIZE = _get_CURL_ERROR_SIZE();
    }

    private final long ptr;
    private final ByteBuffer bb;
    private boolean isClosed;
    private final static int ERROR_SIZE;

    private CurlEasy(long ptr, ByteBuffer bb, boolean isClosed) {
        this.ptr = ptr;
        this.bb = bb;
        this.isClosed = isClosed;
    }

    public static CurlEasy make() {
        final long ptr = curl_easy_init();
        if (ptr == Native.NULL) {
            Err.error("failed to initialize cURL");
        }
        ByteBuffer bb = ByteBuffer.allocateDirect(ERROR_SIZE);
        return new CurlEasy(ptr, bb, false).setErrorBuffer();
    }

    // TODO delete
    public Response perform(final Request request) {
        final long code = curl_easy_perform(ptr, request);
        if (code != 0) {
            Err.error("failed with non-zero code: %s", code);
        }
        final Response response = new Response();
        response.from_curl(ptr);
        return response;
    }

    @Override
    public long ptr() {
        return ptr;
    }

    // TODO: delete
    native public static long curl_easy_init();
    native public static long curl_easy_cleanup(final long curlPtr);
    native public static long curl_easy_perform(final long curl, Request request);

    private CurlEasy checkClosed() {
        if (isClosed) {
            throw new RuntimeException("This CurlEasy resource has been closed");
        } else {
            return this;
        }
    }

    private static String readCString(final ByteBuffer bb) {
        bb.rewind();
        if (bb.get(0) == 0) {
            return null;
        }
        int len = bb.limit();
        int pos = 0;
        for (int i = 0; i < len; i++) {
            if (bb.get(pos) == 0) {
                break;
            }
            pos++;
        }
        ByteBuffer slice = bb.slice(0, pos);
        return StandardCharsets.UTF_8.decode(slice).toString();
    }

    private CurlEasy checkCode(final long code) {
        if (code == 0) {
            return this;
        } else {
            String errorDescription = _curl_easy_strerror(code);
            String errorExplanation = readCString(bb);
            throw new RuntimeException(
                    String.format("curl code: %d, description: %s, explanation: %s",
                            code, errorDescription, errorExplanation));
        }
    }

    native private static int _get_CURL_ERROR_SIZE();

    native private static long _set_url(final long curlPtr, final String url);
    public CurlEasy setUrl(final String url) {
        return checkClosed().checkCode(_set_url(ptr, url));
    }

    native private static long _set_method(final long curlPtr, final long method);
    public CurlEasy setMethod(final long method) {
        return checkClosed().checkCode(_set_method(ptr, method));
    }

    native private static long _perform(final long curlPtr);
    public CurlEasy perform() {
        return checkClosed().checkCode(_perform(ptr));
    }

    native private static long _set_error_buffer(long curlPtr, ByteBuffer bb);
    private CurlEasy setErrorBuffer() {
        return checkClosed().checkCode(_set_error_buffer(ptr, bb));
    }

    native private static long _curl_easy_reset(long curlPtr);
    public CurlEasy resetOptions() {
        return checkClosed().checkCode(_curl_easy_reset(ptr));
    }

    native private static String _curl_easy_strerror(long curlCode);

    native private static long _curl_set_headers(long curl, long headers);
    public CurlEasy setHeaders(Headers headers) {
        final long hhPtr = (headers == null) ? Native.NULL : headers.ptr();
        return checkClosed().checkCode(_curl_set_headers(ptr, hhPtr));
    }

    native private static long _set_write_file(long curl, long file);
    public CurlEasy setWriteFile(FILE file) {
        Objects.requireNonNull(file, "the file object cannot be null");
        return checkClosed().checkCode(_set_write_file(ptr, file.ptr()));
    }

    native private static long _set_write_stream(long curl, long stream);
    public CurlEasy setWriteStream(WriteStream stream) {
        Objects.requireNonNull(stream, "the stream object cannot be null");
        return checkClosed().checkCode(_set_write_stream(ptr, stream.ptr()));
    }

    native private static long _set_write_callback(long curl, long callback);
    public CurlEasy setWriteCallback(WriteCallback callback) {
        Objects.requireNonNull(callback, "write callback cannot be null");
        return checkClosed().checkCode(_set_write_callback(ptr, callback.ptr()));
    }

    native private static long _set_accumulator(long curl, long acc);
    public CurlEasy setAccumulator(Accumulator acc) {
        Objects.requireNonNull(acc, "the accumulator cannot be null");
        return checkClosed().checkCode(_set_accumulator(ptr, acc.ptr()));
    }

    // TODO: check for null
    native private static long _set_post_fields_bytes(long curl, byte[] buf, int len);
    public CurlEasy setPostData(byte[] buf) {
        return checkClosed().checkCode(_set_post_fields_bytes(ptr, buf, buf.length));
    }

    public CurlEasy setPostData(String string, Charset charset) {
        return setPostData(string.getBytes(charset));
    }

    public CurlEasy setPostData(String string, String charset) {
        final byte[] buf;
        try {
            buf = string.getBytes(charset);
        } catch (UnsupportedEncodingException e) {
            throw Err.error("TODO"); // TODO
        }
        return setPostData(buf);
    }

    native private static long _set_verbose(long curl, long value);
    public CurlEasy setVerbose(boolean flag) {
        long value = flag ? 1 : 0;
        return checkClosed().checkCode(_set_verbose(ptr, value));
    }

    native private static long _set_read_file(long curl, long file);
    public CurlEasy setReadFile(FILE file) {
        Objects.requireNonNull(file, "read file cannot be null");
        return checkClosed().checkCode(_set_read_file(ptr, file.ptr()));
    }

    native private static long _set_follow_location(long curl, long value);
    public CurlEasy setFollowLocation(long value) {
        return checkClosed().checkCode(_set_follow_location(ptr, value));
    }

    native private static long _set_read_stream(long curl, long value);
    public CurlEasy setReadStream(ReadStream stream) {
        Objects.requireNonNull(stream, "read stream cannot be null");
        return checkClosed().checkCode(_set_read_stream(ptr, stream.ptr()));
    }

    @Override
    public void close() {
        if (isClosed) return;
        curl_easy_cleanup(ptr);
        isClosed = true;
    }

    public static void main(String... args) throws IOException {
        try (CurlEasy c = CurlEasy.make();
             Headers hh = Headers.create(new String[] {"foo: bar"});
             FILE wf = FILE.open("aaa.txt", "wb");
             ByteArrayOutputStream out = new ByteArrayOutputStream(32);
             Accumulator acc = Accumulator.create(2048);
             WriteStream ws = WriteStream.create(out)
        ) {
            c
                    .resetOptions()
                    .setUrl("https://habr.com")
                    .setHeaders(hh)
                    .setAccumulator(acc)
                    // .setWriteFile(wf)
                    // .setWriteStream(ws)
                    .setMethod(1)
                    .setPostData(new byte[] {1, 2, 3})
                    .perform();
            System.out.println(acc.getString());
        }
    }
}
