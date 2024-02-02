// f8.h

#ifndef F8_H
#define F8_H

#include "core.h"
#include "system.h"
#include "numeric.h"
#include "plotting.h"

AtomPtr make_env () {
    AtomPtr env = make_node ();
    env->tail.push_back (make_node ()); // no parent env
    env->tail.push_back (make_entry (make_node ("#t"), make_node ("#t")));
    env->tail.push_back (make_entry (make_node ("#f"), make_node ("#f")));
    env->tail.push_back (make_entry (make_node ("nl"), make_node ("\n")));
    env->tail.push_back (make_entry (make_node ("tab"), make_node ("\t")));
    add_core (env);
    add_system (env);
    add_numeric (env);
	add_plotting (env);
    return env;
}

#endif // F8_H

// eof