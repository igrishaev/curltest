package org.example;

import java.io.OutputStream;
import java.net.URI;
import java.net.URL;

public class Request {

    public final String url;
    public final int method;
    public final int followLocation;
    public final long headersPtr;
    public final long writeFilePtr;
    public final OutputStream writeStream;
    public final String readString;
    public final byte[] readBytes;
    public final boolean accumulate;

    private Request(
        final String url,
        final int method,
        final int followLocation,
        final long headersPtr,
        final long writeFilePtr,
        final OutputStream writeStream,
        final String readString,
        final byte[] readBytes,
        final boolean accumulate
    ) {
        this.url = url;
        this.method = method;
        this.followLocation = followLocation;
        this.headersPtr = headersPtr;
        this.writeFilePtr = writeFilePtr;
        this.writeStream = writeStream;
        this.readString = readString;
        this.readBytes = readBytes;
        this.accumulate = accumulate;
    }

    public static Builder builder() {
        return new Builder();
    }

    public static class Builder {

        private String url = null;
        private int method = 1;
        private int followLocation = 3;
        private long headersPtr = Native.NULL;
        private long writeFilePtr = Native.NULL;
        private OutputStream writeStream = null;
        private String readString = null;
        private byte[] readBytes = null;
        private boolean accumulate = false;

        public Builder url(final String url) {
            this.url = url;
            return this;
        }

        public Builder url(final URL url) {
            this.url = url.toString();
            return this;
        }

        public Builder url(final URI uri) {
            this.url = uri.toString();
            return this;
        }

        public Builder followLocation(final int followLocation) {
            this.followLocation = followLocation;
            return this;
        }

        public Builder method(final int method) {
            this.method = method;
            return this;
        }

        public Builder headers(final Headers headers) {
            this.headersPtr = headers.ptr();
            return this;
        }

        public Builder writeFile(final FILE writeFile) {
            this.writeFilePtr = writeFile.ptr();
            return this;
        }

        public Builder writeStream(final OutputStream stream) {
            this.writeStream = stream;
            return this;
        }

        public Builder accumulate(final boolean accumulate) {
            this.accumulate = accumulate;
            return this;
        }

        public Request build() {
            return new Request(
                    url,
                    method,
                    followLocation,
                    headersPtr,
                    writeFilePtr,
                    writeStream,
                    readString,
                    readBytes,
                    accumulate
            );
        }

    }

}
