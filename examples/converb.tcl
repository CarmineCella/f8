# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: reverberation by convolution
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

set scale 0.1
set mix 0.8

set sig_file (openwav "../data/anechoic1.wav" 'input)
set ir_file  (openwav "../data/Concertgebouw-s.wav" 'input)

set sig (readwav sig_file)
set ir (readwav ir_file)  

set sr (car (infowav sig_file))

closewav sig_file
closewav ir_file

puts "applying convolution..."
set outsigL (conv (car ir) (car sig) scale mix)
set outsigR (conv (second ir) (car sig) scale mix)
puts "done\n"

set out (openwav "reverb.wav" 'output sr 2)
writewav out (list outsigL outsigR)
closewav out

# eof


