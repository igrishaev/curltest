(ns curl
  (:import
   (java.util Map)
   (java.lang AutoCloseable)
   (org.example CurlEasy
                Request
                Request$Builder)))

(defn ->request ^Request [opts]

  (let [{:keys [url
                method
                follow-location
                headers
                accumulate]}
        opts]

    (cond-> (Request/builder)

      ;; TODO URL URI
      (string? url)
      (.url ^String url)

      method
      (.method method)

      follow-location
      (.followLocation follow-location)

      (map? headers)
      (.addHeaders ^Map headers)

      (some? accumulate)
      (.accumulate accumulate)

      :finally
      (.build))))

(defn init ^CurlEasy []
  (CurlEasy/make))

(defn perform [^CurlEasy curl opts]
  (.perform curl (->request opts)))
