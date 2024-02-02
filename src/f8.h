// f8.h

#ifndef F8_H
#define F8_H

#include "core.h"
#include "system.h"
#include "signals.h"
#include "plotting.h"

namespace f8 {
    AtomPtr make_env () {
        AtomPtr env = make_node ();
        env->tail.push_back (make_node ()); // no parent env
        env->tail.push_back (make_entry (make_node ("true"), make_node ("true")));
        env->tail.push_back (make_entry (make_node ("false"), make_node ("false")));
        env->tail.push_back (make_entry (make_node ("nl"), make_node ("\n")));
        env->tail.push_back (make_entry (make_node ("tab"), make_node ("\t")));
        add_core (env);
        add_system (env);
        add_signals (env);
        add_plotting (env);
        return env;
    }
}

#endif // F8_H

// eof