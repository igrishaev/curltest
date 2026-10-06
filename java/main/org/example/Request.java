package org.example;

import java.net.URI;
import java.net.URL;

public class Request {

    public final String url;
    public final int method;
    public final int followLocation;
    public final long headersPtr;
    public final long writeFilePtr;
    public final long writeStreamPtr;
    public final String readString;
    public final byte[] readBytes;
    public final long accumPtr;
    public final long verbose;

    private Request(
        final String url,
        final int method,
        final int followLocation,
        final long headersPtr,
        final long writeFilePtr,
        final long writeStreamPtr,
        final String readString,
        final byte[] readBytes,
        final long accumPtr,
        final long verbose
    ) {
        this.url = url;
        this.method = method;
        this.followLocation = followLocation;
        this.headersPtr = headersPtr;
        this.writeFilePtr = writeFilePtr;
        this.writeStreamPtr = writeStreamPtr;
        this.readString = readString;
        this.readBytes = readBytes;
        this.accumPtr = accumPtr;
        this.verbose = verbose;
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
        private long writeStreamPtr = Native.NULL;
        private String readString = null;
        private byte[] readBytes = null;
        private long accumPtr = Native.NULL;
        private long verbose = 0;

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

        public Builder headers(final IResource resource) {
            this.headersPtr = resource.ptr();
            return this;
        }

        public Builder writeFile(final IResource resource) {
            this.writeFilePtr = resource.ptr();
            return this;
        }

        public Builder writeStream(final IResource writeStream) {
            this.writeStreamPtr = writeStream.ptr();
            return this;
        }

        public Builder accum(final IResource resource) {
            this.accumPtr = resource.ptr();
            return this;
        }

        public Builder verbose(boolean isVerbose) {
            this.verbose = (isVerbose ? 1 : 0);
            return this;
        }

        public Request build() {
            return new Request(
                    url,
                    method,
                    followLocation,
                    headersPtr,
                    writeFilePtr,
                    writeStreamPtr,
                    readString,
                    readBytes,
                    accumPtr,
                    verbose
            );
        }

    }

}
