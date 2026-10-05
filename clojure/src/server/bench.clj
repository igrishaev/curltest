(ns server.bench
  (:import
   (java.io InputStream
            ByteArrayOutputStream))
  (:use criterium.core)
  (:require
   [curl]
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

          #_
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
  (with-open [c (curl/init)]
    (let [req {:url "http://127.0.0.1:3000"
               :method 1
               :follow-redirects 3
               :headers {"foo" "bar"}
               :accumulate? true}]
      (quick-bench
          (curl/perform2 c req))))

  #_
  (with-open [c (CurlEasy/make)]
    (quick-bench
        (Main/test c))))
