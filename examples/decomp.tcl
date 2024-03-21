source "stdlib.tcl"
source "signals.tcl"
source "learning.tcl"
source "plotting.tcl"

puts "loading dictionary..."
set data (make-dictionary "../data/clarinet")
puts (llength data) " elements\n"

set target (car (sndread "../data/onsets.wav"))

set N 4096
set hop (/ N 4)
set SR 44100
set frame (/ N SR)
set threshold 0.5
set timegate 0.01
set comps 1

puts "computing onsets....."
set o (onsets target SR N hop threshold timegate)
puts (llength o) " fonud\n\n"

# FIXME: handle multichannel case
set i 0
set out (zeros 1)
puts "decomposing...\n"
while (< i (llength o)) {
    set next_p (size target)
    if (< i (- (llength o 1) 1)) {
        set n (lindex o (+ i 1))
        = next_p (* hop (car n)) 
    }
        
    set pos (* (car (lindex o i)) hop)
    puts "\tsegment " i " @ " pos "\n"
    set dec (mpdecomp target pos comps data)
    set w (car (mprebuild (car dec) data))
    = w (slice w 0 next_p)
    set out (mix 0 out 0 w)
    = i (+ 1 i)
}
puts "\n"
sndwrite "rebuild.wav" SR (list (* out 100))

# eof

