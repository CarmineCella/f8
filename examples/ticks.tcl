# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: internal clock
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
source "system.tcl"

set tic (clock)
puts "current clock " tic "\n"
set toc (clock)
puts "current clock " toc "\n"
puts "ticks elapsed " (- toc tic) "\n\n"

# they should be different
test '(<> toc tic) '1

# eof

