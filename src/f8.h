// f8.h

#ifndef F8_H
#define F8_H

#include "core.h"
#include "system.h"
#include "numeric.h"
#include "plotting.h"

// interface
AtomPtr make_env () {
    AtomPtr env = make_node ();
    env->tail.push_back (make_node ()); // no parent env
    env->tail.push_back (make_entry (make_node ("#t"), make_node ("#t")));
    env->tail.push_back (make_entry (make_node ("#f"), make_node ("#f")));
    add_core (env);
    add_system (env);
    add_numeric (env);
	add_plotting (env);
    return env;
}
void repl (AtomPtr env, std::istream& in, std::ostream& out) {
	std::istream* current = &in;	
	while (true){
		out << ">> ";
		try {
			print (eval (read (*current), env), out);
			out << std::endl;
		} catch (std::exception& e) {
			out << RED << "error: " << e.what () << RESET << std::endl;
		} catch (AtomPtr& e) {
			out << RED << "error: uncaught expection " << RESET;
			print (e, out); out << std::endl;
		} catch (...) {
			out << RED << "fatal error: execution stopped" << RESET << std::endl;
		}
	} 
}

#endif // F8_H

// eof