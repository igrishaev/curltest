package org.example;

import java.net.URI;
import java.net.URL;

public record Request(
        String url,
        FollowLocation followLocation,
        HTTPMethod httpMethod
) {
    public static Builder builder() {
        return new Builder();
    }

    public static class Builder {

        private String url = null;
        private FollowLocation followLocation = FollowLocation.DEFAULT;
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

        public Builder followLocation(final FollowLocation followLocation) {
            this.followLocation = followLocation;
            return this;
        }

        public Builder httpMethod(final HTTPMethod httpMethod) {
            this.httpMethod = httpMethod;
            return this;
        }

        public Request build() {
            return new Request(
                    url,
                    followLocation,
                    httpMethod
            );
        }
    }
}
