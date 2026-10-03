(ns server.bench
  (:import
   (java.io InputStream
            ByteArrayOutputStream)
   (org.example Curl3))
  (:use criterium.core)
  (:require
   [org.httpkit.client :as http]
   [babashka.http-client :as bb]
   [clj-http.conn-mgr :as conn]
   [clj-http.client :as client]))


(defn test-bb []
  (quick-bench
      (let [response
            (bb/get "http://127.0.0.1:3000"
                    {:as :string})])))

(defn test-clj-http []
  (let [cm (conn/make-reusable-conn-manager {})]

    (quick-bench
        (let [response
              (client/get "http://127.0.0.1:3000"
                          {:connection-manager cm
                           :cache true})
              {:keys [body]}
              response]

          (assert (string? body))))

    #_
    (quick-bench
        (let [out
              (new ByteArrayOutputStream)

              response
              (client/get "http://127.0.0.1:3000"
                          {:connection-manager cm
                           :as :stream
                           :cache true})

              {:keys [^InputStream body]}
              response]

          (.transferTo body out)
          (.close out)))))

(defn test-http-kit []
  (quick-bench
      @(http/get "http://127.0.0.1:3000")

      #_
      (let [out
            (new ByteArrayOutputStream)

            response
            @(http/get "http://127.0.0.1:3000"
                       {:as :stream})

            {:keys [^InputStream body]}
            response]

        (.transferTo body out)
        (.close out))))


(defn test-curl []
  (with-open [c (Curl3/create)]
    (quick-bench
        (Curl3/test c))))
