(ns curl
  (:import
   (java.util Map)
   (org.example CurlEasy
                IResource
                Response
                Headers
                Dummy
                FILE
                Accumulator
                Request
                Request$Builder)))

(defn ->request ^Request [opts]

  (let [{:keys [url
                method
                follow-location
                resource-headers
                resource-write-file
                resource-accum]}
        opts]

    (cond-> (Request/builder)

      ;; TODO URL URI
      (string? url)
      (.url ^String url)

      method
      (.method method)

      follow-location
      (.followLocation follow-location)

      resource-write-file
      (.writeFile resource-write-file)

      resource-headers
      (.headers resource-headers)

      resource-accum
      (.accum resource-accum)

      :finally
      (.build))))

(defn init ^CurlEasy []
  (CurlEasy/make))

(defn perform [^CurlEasy curl opts]
  (.perform curl (->request opts)))

(defn open-headers ^IResource [headers]
  (if headers
    (Headers/create ^Map headers)
    Dummy/INSTANCE))

(defn open-write-file ^IResource [write-file]
  (if write-file
    (FILE/open ^String write-file "wb")
    Dummy/INSTANCE))

(defn open-accum ^IResource [accumulate?]
  (if accumulate?
    (Accumulator/create 2048)
    Dummy/INSTANCE))

(defn perform2 [^CurlEasy curl opts]
  (let [{:keys [headers
                write-file
                accumulate?]}
        opts]
    (with-open [h (open-headers headers)
                f (open-write-file write-file)
                a (open-accum accumulate?)]
      (let [request
            (-> opts
                (assoc :resource-headers h
                       :resource-write-file f
                       :resource-accum a)
                (->request))

            response
            (.perform curl request)]

        {:status (.-status response)
         :headers (.-headers response)
         :body (when accumulate?
                 (.getString ^Accumulator a))}

        #_
        response))))

(comment

  (with-open [c (init)]
    (perform2 c {:url "http://127.0.0.1:3000" #_"https://habr.com"
                 :method 1
                 :follow-redirects 3
                 :headers {"foo" "bar"}
                 :accumulate? true})))
