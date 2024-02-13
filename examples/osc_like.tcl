source "stdlib.tcl"

schedule (udpsend "127.0.0.1" 10000 "/giveme some 44") 2000 0
schedule (udpsend "127.0.0.1" 10000 "/giveme some 55") 2000 0
schedule (udpsend "127.0.0.1" 10000 "/giveme some 66") 2000 0

# eof
