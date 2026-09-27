package org.example;

public record Request(
        String url,
        CURLOPT_FOLLOWLOCATION followlocation
) {
    public static class Builder {

        private String url = null;
        private CURLOPT_FOLLOWLOCATION followlocation = CURLOPT_FOLLOWLOCATION.DEFAULT;

        public Builder builder() {
            return new Builder();
        }

        public Builder url(final String url) {
            this.url = url;
            return this;
        }

        public Builder followlocation(final CURLOPT_FOLLOWLOCATION followlocation) {
            this.followlocation = followlocation;
            return this;
        }

        public Request build() {
            return new Request(url, followlocation);
        }
    }
}
