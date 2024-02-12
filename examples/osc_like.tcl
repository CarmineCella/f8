udpsend "127.0.0.1" 10000 "/gimme some 1000"

schedule (udpsend "127.0.0.1" 10000 "/freq 1000") 1000 1
schedule (udpsend "127.0.0.1" 10000 "/freq 2000") 2000 1
schedule (udpsend "127.0.0.1" 10000 "/freq 10000") 5000 1

puts "press ENTER..."
gets
