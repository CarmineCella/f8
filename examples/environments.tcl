# Environment manipulation

source "stdlib.tcl"

set e0 10 ; creates a new variable
puts "e0 = " e0 nl
puts "dees e0 exist? " (info 'exists 'e0) nl

puts "removing e0" nl
unset 'e0
puts "does e0 exist? " (info 'exists 'e0) nl

let {
	puts "define e1 locally" nl
	set e1 10
	puts "does e1 exist? " (info 'exists 'e1) nl
}
puts "does e1 exist? " (info 'exists 'e1) nl

puts "all variables: " (info 'vars) nl nl