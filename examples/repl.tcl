# repl

  
proc (repl) {
    while true {
      puts {
        catch {
          puts ">> "
          puts (eval (gets))
        }
      } nl
    }
}

puts "[minimal f8 repl]" nl nl
repl
