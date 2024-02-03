;; {threds and sockets}
;;
;; (c) 2019 www.arch-programming.org
;;

(load "system.scm")

(prn "launch a thread that will listen to socket..." (nl))
(def {listener} (\{}{do (udprecv "127.0.0.1" 10000) (do (prn "MESS RECEIVED!" (nl)) (quit))})) 
(def {t1} (thread {listener}))
(prn "main thread is still running here..." (nl))


(prn "starting a task that will send something to socket in 5 seconds..." (nl))
(def {sender}(\{}{udpsend "127.0.0.1" 10000 "hallo!"}));
(schedule {sender} 5000 1)
(read)