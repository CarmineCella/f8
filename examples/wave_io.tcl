# wave files io
#

source "stdlib.tcl"

set sr 44100
set ch 2
set freq 440
set dur 0.1
set samples (* dur sr)

set time (bpf 0 samples (/ samples sr))

set sig1 (-> sin (map (\(x)(* TWOPI freq x)) time))
set sig2 (-> cos (map (\(x)(* TWOPI freq x)) time))

puts "write wave data..."
set a (openwav "test.wav" 'output sr ch)
puts (writewav a ch sig1 sig2) " samples written" nl
closewav a

puts "read wave data..." 
set a (openwav "test.wav" 'input)
# by default reads all file
set sigr (readwav a) 
puts (* (len (first sigr)) (len sigr)) " samples read" nl
closewav a

puts  "write again data..."
set a (openwav "test2.wav" 'output sr ch)
puts (writewav a ch (first sigr) (second sigr)) " samples written" nl
closewav a

puts "read sounds/archeos-bell.wav..." 
set a (openwav "../sounds/archeos-bell.wav" 'input)
set winfo (infowav a)
set sigr (readwav a) 
closewav a
puts "sr: " (first winfo) ", ch: "(second winfo) nl

set c (openwav "copy.wav" 'output (first winfo) (second winfo))
puts (writewav c (second winfo) (first sigr)) " samples copied" nl
closewav c


# eof
