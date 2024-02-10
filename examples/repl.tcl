# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Example: self-boostrap in f8
#
# (c) www.carminecella.com
#

source "stdlib.tcl"
  
proc (repl) {
    while true {
      puts {
        catch {
          puts ">> "
          puts (eval (gets))
        }
      } "\n"
    }
}

puts "[minimal f8 repl]\n\n"
repl
