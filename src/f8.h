// f8.h

#ifndef F8_H
#define F8_H

#include "core.h"
#include "system.h"
#include "numeric.h"

// interface
AtomPtr make_env () {
    AtomPtr env = make_node ();
    env->tail.push_back (make_node ()); // no parent env
    env->tail.push_back (make_entry (make_node ("#t"), make_node ("#t")));
    env->tail.push_back (make_entry (make_node ("#f"), make_node ("#f")));
    add_core (env);
    add_system (env);
    add_numeric (env);
    return env;
}
void repl (AtomPtr env, std::istream& in, std::ostream& out) {
	#if defined (ENABLE_READLINE)
	char* line_read = 0;
	#endif
	std::istream* current = &in;
	while (true){
		#if defined (ENABLE_READLINE)
			if (line_read) {
				delete [] line_read;
				line_read = 0;
			}
			line_read = readline (">> ");
			if (!line_read) break;
			if (line_read && *line_read) add_history (line_read);
			std::string input (line_read);
			if (!input.size ()) { continue; }
			std::stringstream tmp_str; 
			tmp_str << line_read << "\n"; // lf added
			current = &tmp_str;
		#else
			out << ">> ";
		#endif	
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