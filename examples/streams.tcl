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
writeline astream "this text " (tostr '(should be written)) " " (tostr (lappend 1)) " time"
closestream astream

puts "reading back data: "
set bstream (openstream  "test.txt" 'input)
if (not (isgood bstream)) (throw "invalid input stream")

puts (readlines bstream) "\n"

puts "rewind stream and read again: "
rewindstream bstream
puts (readlines bstream) "\n"
closestream bstream
puts "\n"

# eof

