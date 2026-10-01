package org.example.http;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

public record Request (String method, String[] headers, String[] bodyParts) {

    public static void request(
            String url,
            boolean isGet,
            boolean isPost,
            boolean isPut,
            String method,
            String postData,
            String[] postFields,
            String[] headers,
            int followRedirects,
            OutputStream writeStream,
            String writeFile,
            Object writeHandler,
            InputStream readStream,
            String readFile,
            Object readHandler
    ) {

    }

    public static Builder builder() {
        return new Builder();
    }

    public static class Builder {
        List<String> headers = null;
        List<String> bodyParts = null;

        private void initHeaders() {
            if (headers == null) {
                headers = new ArrayList<>();
            }
        }

        private void initBodyParts() {
            if (bodyParts == null) {
                bodyParts = new ArrayList<>();
            }
        }

        public Builder addHeader(String header, String value) {
            initHeaders();
            headers.add(header);
            headers.add(value);
            return this;
        }

        public Builder addBodyPart(String name, String value) {
            initBodyParts();
            bodyParts.add(name);
            bodyParts.add(value);
            return this;
        }

        public Request build() {
            return new Request(
                    "sdf",
                    (headers == null) ? null : headers.toArray(new String[0]),
                    (bodyParts == null) ? null : bodyParts.toArray(new String[0])
            );
        }

    }

}
