package org.example;

import java.io.File;
import java.io.OutputStream;
import java.net.URI;
import java.net.URL;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class Request {

    public final String url;
    public final int method;
    public final int followLocation;
    public final String[] headers;
    public final String writeFile;
    public final OutputStream writeStream;
    public final String readString;
    public final byte[] readBytes;
    public final boolean accumulate;

    private Request(
        final String url,
        final int method,
        final int followLocation,
        final String[] headers,
        final String writeFile,
        final OutputStream writeStream,
        final String readString,
        final byte[] readBytes,
        final boolean accumulate
    ) {
        this.url = url;
        this.method = method;
        this.followLocation = followLocation;
        this.headers = headers;
        this.writeFile = writeFile;
        this.writeStream = writeStream;
        this.readString = readString;
        this.readBytes = readBytes;
        this.accumulate = accumulate;
    }

    public static Builder builder() {
        return new Builder();
    }

    public static class Builder {

        String url = null;
        int method = 1;
        int followLocation = 3;
        List<String> headers = null;
        String writeFile = null;
        OutputStream writeStream = null;
        String readString = null;
        byte[] readBytes = null;
        boolean accumulate = false;

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

        private void initHeaders() {
            if (headers == null) {
                headers = new ArrayList<>();
            }
        }

        public Builder addHeader(String header, String value) {
            initHeaders();
            headers.add(header + ":" + value); // TODO
            return this;
        }

        public Builder addHeaders(final List<String> listHeaders) {
            initHeaders();
            headers.addAll(listHeaders);
            return this;
        }

        public Builder addHeaders(final Map<String, String> mapHeaders) {
            initHeaders();
            for (Map.Entry<String, String> entry: mapHeaders.entrySet()) {
                headers.add(entry.getKey() + ":" + entry.getValue()); // TODO
            }
            return this;
        }

        public Builder writeFile(final String writeFile) {
            this.writeFile = writeFile;
            return this;
        }

        public Builder writeFile(final File writeFile) {
            this.writeFile = writeFile.getAbsolutePath();
            return this;
        }

        public Builder writeFile(final Path writeFile) {
            this.writeFile = writeFile.toAbsolutePath().toString();
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
                    (headers == null) ? null : headers.toArray(new String[0]),
                    writeFile,
                    writeStream,
                    readString,
                    readBytes,
                    accumulate
            );
        }

    }

}
