# {threds and sockets}

source "stdlib.tcl"

puts "launch a thread that will listen to socket..." nl
proc (listener) {
    udprecv "127.0.0.1" 10000
    puts "MESS RECEIVED!" nl
    exit
}

set t1 (thread '(listener))
puts "main thread is still running here..." nl

puts "starting a task that will send something to socket in 5 seconds..." nl

proc (sender) {
    udpsend "127.0.0.1" 10000 "hallo!"
    puts "hello" nl
}

schedule '(sender) 5000 1
gets

# eof


