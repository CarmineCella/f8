# {ticks}

source "stdlib.tcl"

set tic (ticks)
puts "current ticks " tic nl
set toc (ticks)
puts "current ticks " toc nl
puts "ticks elapsed " (- toc tic) nl nl

# they should be different
test '(<> toc tic)'(1) 