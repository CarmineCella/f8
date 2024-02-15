# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: streams and filesystem
#
# (c) www.carminecella.com
#
source "stdlib.tcl"

puts "current directory: " (cdr (cdr (dirlist "."))) "\n"
puts "stats for the first file: " (filestat (car (dirlist "."))) "\n"
puts "home folder is: " (getvar "HOME") "\n"
puts "writing data to file..." "\n"
set astream (openstream "test.txt" 'output 'text)
if (not (isgood astream)) (throw "invalid output stream")
writeline astream "this text " (tostr '(should be written)) " " (tostr (lappend () 1)) " time\n"
writeline astream "this is another line\n"
closestream astream

puts "reading back data: "
set bstream (openstream  "test.txt" 'input 'text)
if (not (isgood bstream)) (throw "invalid input stream")

set lines (readlines bstream)
puts lines "\n"

puts "rewind stream and read again: "
rewindstream bstream
puts (readlines bstream) "\n"
closestream bstream
puts "\n"


puts "write binary streams (double numbers)\n"
set cstream (openstream "test.bin" 'output 'binary)
writenumbers cstream (bpf 0 100 100)
closestream cstream

puts "read back binary streams (double numbers): "
set cstream (openstream "test.bin" 'input 'binary)
set vals (readnumbers cstream)
puts vals "\n"
puts "read back only 10 numbers: "
rewindstream cstream
set vals (readnumbers cstream 10)
puts vals "\n"

# eof

