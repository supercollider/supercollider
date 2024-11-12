#!/bin/sh
"exec" "clj" "-Sdeps" "{:deps,{org.clojure/clojure,{:mvn/version,\"1.12.0\"},org.jmdns/jmdns,{:mvn/version,\"3.6.0\"}}}" "-M" "$0" "$@"
;; Author: Ambrose Bonnaire-Sergeant
;; Dependencies: https://clojure.org/guides/install_clojure
;; Usage:
;;   SCLANG=sclang SCSYNTH=scsynth ./test_zeroconf.sh
;;   SCLANG=../../build/Install/SuperCollider/SuperCollider.app/Contents/MacOS/sclang SCSYNTH=../../build/Install/SuperCollider/SuperCollider.app/Contents/Resources/scsynth ./test_zeroconf.sh
(ns test-zeroconf
  (:require [clojure.java.process :as p])
  (:import [javax.jmdns JmDNS ServiceListener ServiceInfo ServiceEvent]
           [java.util.concurrent TimeUnit]
           [java.net ServerSocket Socket]))

(set! *warn-on-reflection* true)

(defn- get-free-port []
  (with-open [socket (ServerSocket. 0)]
    (.getLocalPort socket)))

(defn pass [& args] (apply println "Pass:" args) 0)
(defn fail [& args] (apply println "Fail:" args) 1)

(defn run-test [{:keys [requested-udp-port requested-tcp-port]}]
  (let [udp-promise (promise)
        tcp-promise (promise)
        sclang (or (System/getenv "SCLANG") (throw (ex-info "Must define $SCLANG to point to sclang" {})))
        scsynth (or (System/getenv "SCSYNTH") (throw (ex-info "Must define $SCSYNTH to point to scsynth" {})))
        timeout (Object.)
        timeout-seconds 5]
    (with-open [jmdns (JmDNS/create)]
      (doto jmdns
        (.addServiceListener
          "_osc._udp.local."
          (reify ServiceListener
            (serviceAdded [_ e])
            (serviceRemoved [_ e])
            (serviceResolved [_ e] (deliver udp-promise e))))
        (.addServiceListener
          "_osc._tcp.local."
          (reify ServiceListener
            (serviceAdded [_ e])
            (serviceRemoved [_ e])
            (serviceResolved [_ e] (deliver tcp-promise e)))))
      (let [proc (p/start {:err :inherit
                           :out :inherit}
                          scsynth "-u" (pr-str requested-udp-port) "-t" (pr-str requested-tcp-port)
                          "-i" "0" "-o" "0")]
        (try (let [^ServiceEvent udp-event (deref udp-promise (* 1000 timeout-seconds) timeout)
                   ^ServiceEvent tcp-event (deref tcp-promise (* 1000 timeout-seconds) timeout)
                   test-port (fn [{:keys [type ^ServiceEvent event requested-port]}]
                               (let [actual-port (-> event .getInfo .getPort)]
                                 (if (identical? event timeout)
                                   (fail (format "Timeout waiting for %s event!" type))
                                   (let [actual-port (-> event .getInfo .getPort)
                                         dbg-str #(str (format "\n---------START %s EVENT----------\n" type)
                                                       event
                                                       (format "\n---------END %s EVENT------------" type))]
                                     (if (zero? requested-port)
                                       (if (pos? actual-port)
                                         ;; TODO connect to server from sclang
                                         (pass (format "%s server was allocated a free port when passed 0" type))
                                         (fail (format "%s server was not allocated a free port: %s"
                                                       type actual-port)
                                               (dbg-str)))
                                       (if (= requested-port actual-port)
                                         (pass (format "%s port matched requested port" type))
                                         (fail (format "%s port did not match requested port: expected %s actual %s"
                                                       type requested-port actual-port)
                                               (dbg-str))))))))]
               (max (test-port {:type "UDP" :event udp-event :requested-port requested-udp-port})
                    (test-port {:type "TCP" :event tcp-event :requested-port requested-tcp-port})))
             (finally
               (.destroy proc)
               (or (.waitFor proc timeout-seconds TimeUnit/SECONDS)
                   (throw (ex-info "Failed to gracefully shut down scsynth!" {})))))))))

(defn -main [& args]
  (when args (throw (ex-info "No arguments supported" {:args args})))
  (try (System/exit
         (max (run-test {:requested-udp-port (get-free-port)
                         :requested-tcp-port 0})
              (run-test {:requested-udp-port 0
                         :requested-tcp-port (get-free-port)})))
       (catch Throwable e
         (.printStackTrace e)
         (System/exit 1))
       (finally
         (println "Unknown error!")
         (System/exit 1))))

(apply -main *command-line-args*)
