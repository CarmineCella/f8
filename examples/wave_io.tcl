# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: wave files I/O
#
# (c) www.carminecella.com
#
source "stdlib.tcl"

set sr 44100
set ch 2
set freq 440
set dur 0.1
set samples (* dur sr)

set time (tolist (bpf 0 samples (/ samples sr)))

# arrays and lists are connected via torray/tolist
set sig1 (toarray (-> sin (map (\(x)(* TWOPI freq x)) time)))
set sig2 (toarray (-> cos (map (\(x)(* TWOPI freq x)) time)))

puts "write wave data..."
set a (openwav "test.wav" 'output sr ch)
puts (writewav a sig1 sig2) " samples written\n"
closewav a

puts "read wave data..." 
set a (openwav "test.wav" 'input)
# by default reads all file
set sigr (readwav a) 
puts (* (size (car sigr)) (llength sigr)) " samples read\n"
closewav a

puts  "write again data..."
set a (openwav "test2.wav" 'output sr ch)
puts (writewav a (car sigr) (second sigr)) " samples written\n"
closewav a

puts "read sounds/archeos-bell.wav..." 
set a (openwav "../sounds/archeos-bell.wav" 'input)
set winfo (infowav a)
set sigr (readwav a) 
closewav a
puts "sr: " (car winfo) ", ch: "(second winfo) "\n"

set c (openwav "copy.wav" 'output (car winfo) (second winfo))
puts (writewav c (car sigr)) " samples copied\n"
closewav c

# eof
