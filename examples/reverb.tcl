source "stdlib.tcl"

set scale 0.1
set mix 0.8

set sig_file (openwav "../sounds/anechoic1.wav" 'input)
set ir_file  (openwav "../sounds/Concertgebouw-s.wav" 'input)

set sig (readwav sig_file)
set ir (readwav ir_file)  

set sr (first (infowav sig_file))

set outsigL (conv (first ir) (first sig) scale mix)
set outsigR (conv (second ir) (first sig) scale mix)

set ir_file  (openwav "../sounds/Concertgebouw-s.wav" 'input)

set out (openwav "reverb.wav" 'output sr 2)
writewav out 2 outsigL outsigR
closewav out



