# streams and filesystem

source "stdlib.tcl"

puts "current directory " (ltail (ltail (dirlist "."))) nl
puts "stats of first file " (filestat (first (dirlist "."))) nl

puts "writing data to file..." nl
set astream (openstream "test.txt" 'output 'text)
if (not (isgood astream)) (throw "invalid output stream")
writeline astream "this text " (tostr '(should be written)) " " (tostr (ljoin 1)) " time"
closestream astream

puts "reading back data = "
set bstream (openstream  "test.txt" 'input)
if (not (isgood bstream)) (throw "invalid input stream")

puts (readall bstream) nl

puts "rewind stream and read again = "
rewindstream bstream
puts (readall bstream) nl
closestream bstream
puts nl
