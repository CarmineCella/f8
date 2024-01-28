; f8 - standard library
;

;; system
(function {exists fname}
	{first (filestat fname)}
)

(function {udpmonitor addr port}
	{do
	 	(def {r} (udprecv addr port))
		(if (== r "disconnect")
			{do (join 0)}
			{do	(prn r (nl))
				(udpmonitor addr port)
			}
		)
	}
)
