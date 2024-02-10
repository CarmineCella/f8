# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: threads and socket programming
#
# (c) www.carminecella.com
#

source "stdlib.tcl"

# NB: threads are not exception safe!
puts "launch a thread that will listen to socket...\n" 
proc (listener) {
    udprecv "127.0.0.1" 10000
    puts "MSG RECEIVED!\n"
    exit
}

set t1 (thread (listener))
puts "main thread is still running here...\n"

puts "starting a task that will send something via socket in 5 seconds...\n"

proc (sender) {
    udpsend "127.0.0.1" 10000 "hallo!"
    puts "hello\n"
}

schedule (sender) 5000 1
gets

# eof


