# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: internal clock
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

set tic (ticks)
puts "current ticks " tic "\n"
set toc (ticks)
puts "current ticks " toc "\n"
puts "ticks elapsed " (- toc tic) "\n\n"

# they should be different
test '(<> toc tic)'(1) 

# eof

