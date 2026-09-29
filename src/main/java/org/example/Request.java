package org.example;

import java.net.URI;
import java.net.URL;

public record Request(
        String url,
        CURLOPT_FOLLOWLOCATION followlocation,
        HTTPMethod httpMethod
) {
    public static Builder builder() {
        return new Builder();
    }

    public static class Builder {

        private String url = null;
        private CURLOPT_FOLLOWLOCATION followlocation = CURLOPT_FOLLOWLOCATION.DEFAULT;
        private HTTPMethod httpMethod;

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

        public Builder followlocation(final CURLOPT_FOLLOWLOCATION followlocation) {
            this.followlocation = followlocation;
            return this;
        }

        public Builder httpMethod(final HTTPMethod httpMethod) {
            this.httpMethod = httpMethod;
            return this;
        }

        public Request build() {
            return new Request(
                    url,
                    followlocation,
                    httpMethod
            );
        }
    }
}
