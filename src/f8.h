// f8.h

#ifndef F8_H
#define F8_H

#include <iostream>
#include <deque>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <regex>
#include <cmath>
#include <fstream>
#if defined (ENABLE_READLINE)
	#include <readline/readline.h>
	#include <readline/history.h>
#endif

#define BOLDWHITE   "\033[1m\033[37m"
#define RED     	"\033[31m" 
#define RESET   	"\033[0m"

// TODO:

// system
// audio I/O
// load db
// features + pitch + onsets
// rt playback

// clustering
// maple
// orchidea
// sparkle / blockconv / pvoc
// plotting (SVG)
// ambienc3
// ambidec (?)
// musicxml output

// examples

// ast
struct Atom;
typedef std::shared_ptr<Atom> AtomPtr;
typedef AtomPtr (*Op) (AtomPtr, AtomPtr);
#define make_node(type)(std::make_shared<Atom>(type))
enum AtomType {
    NUMBER,
    SYMBOL,
    STRING,
    LIST,
    PROCEDURE,
    LAMBDA
};
const char* TYPE_NAMES[] = {
	"number", "symbol", "string", "list", "proc", "lambda"
};
typedef double Real;
bool is_string (const std::string& s);
struct Atom {
    Atom () {type = LIST;}
    Atom (Real v) {type = NUMBER; val = v;}
    Atom (const std::string& s) {
        if (is_string (s)) {
            type = STRING; 
            lexeme = s.substr (1, s.size ()-1);
        } else {
            type = SYMBOL; 
            lexeme = s;
        }        
    }
    Atom (Op f) {type = PROCEDURE; action = f;}
    Atom (std::deque<AtomPtr> ll) {
        type = LAMBDA;
        tail.push_back (ll.at (0));
        tail.push_back (ll.at (1));
        tail.push_back (ll.at (2));
    }
    AtomType type;
    Real val;
    std::string lexeme;
    Op action;
    std::deque<AtomPtr> tail;
    int minargs;
};

// parsing
std::string next (std::istream& input) {
	std::stringstream accum;
	while (!input.eof ()) {
		char c = input.get ();
		switch (c) { 			
			case '(': case ')': case '\'':	
 				if (accum.str ().size ()) {
					input.unget();
					return accum.str ();
				} else {
					accum << c;
					return accum.str ();
				}
			break;
			case ';':
				while (c != '\n' && !input.eof ())  { c = input.get (); }
			break;		            
			case ' ': case '\t': case '\r': case '\n':
				if (accum.str ().size ()) return accum.str ();
				else continue;
			break;   
			case '\"':
				if (accum.str ().size ()) {
					input.putback(c);
					return accum.str ();
				} else {
					accum << c;
					while (!input.eof ()) {
						input.get (c);
						if (c == '\"') break;
						accum << c;
					}
					return accum.str ();
				}
			break;                   
			default:
				if (c > 0) accum << c;
		}
	}
	return accum.str ();
}
bool is_number (std::string token) {
    return std::regex_match(token, std::regex (("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?")));
}
bool is_string (const std::string& s) {
	return s.find ("\"") != std::string::npos;
}
AtomPtr read (std::istream& in) {
    std::string token = next (in);
    if (token == "(") {
        AtomPtr l = make_node();
        while (!in.eof ()) {
            AtomPtr a = read (in);
            if (a->lexeme == ")" && a->type != STRING) break;
            l->tail.push_back (a);
        }
        return l;
    } else if (is_number (token)) {
        return make_node (atof (token.c_str ()));
    } else if (token == "'") {
        AtomPtr list = make_node ();
        list->tail.push_back (make_node("quote"));
        list->tail.push_back (read (in));
        return list;			
    }  else {
        if (token.size ()) return make_node (token);
        else return make_node ();
    }
}

// helpers
bool is_nil (AtomPtr node) {
    return node == nullptr || (node->type == LIST && node->tail.size () == 0);
}
std::ostream& print (AtomPtr node, std::ostream& out) {
    if (node->type == NUMBER) out << node->val;
    if (node->type == SYMBOL || node->type == STRING) {
        out << node->lexeme;
    }
    if (node->type == PROCEDURE) {
        //  out << <"<operator " << node->lexeme << " @" << (std::hex) << node << ">";
        out << "<proc>";
    }
    if (node->type == LAMBDA) {
        out << "(lambda ";
        print (node->tail.at (0), out) << " ";
        print (node->tail.at (1), out);
        out << ")";
    }
    if (node->type == LIST) {
        out << "(";
        for (unsigned i = 0; i < node->tail.size (); ++i) {
            print (node->tail.at (i), out);
            if (i != node->tail.size () - 1) out << " ";
        }
        out << ")";
    }
    return out;
}
void error (const std::string& msg, AtomPtr ctx) {
    std::stringstream err;
    err << msg << " ";
    if (!is_nil (ctx)) {
        err << "[";
        print (ctx, err);
        err << "]";
    }
    throw std::runtime_error (err.str ());
}
AtomPtr argnum_check (AtomPtr node, int sz) {
    if (node->tail.size () < sz) {
        std::stringstream msg;
        msg << "insufficient arguments (expected " << sz << ", found " << node->tail.size () << ")";    
        error (msg.str (), node);
    }
    return node;
}
AtomPtr type_check (AtomPtr node, AtomType type) {
    if (node->type != type) {
        std::stringstream msg;
        msg << "invalid type (expected " << TYPE_NAMES[type] << ")";        
        error (msg.str (), node);
    }
    return node;
}

// evaluation
AtomPtr make_entry (AtomPtr sym, AtomPtr v) {
    AtomPtr entry = make_node ();
    entry->tail.push_back (sym);
    entry->tail.push_back (v);
    return entry;
}
AtomPtr assoc (AtomPtr node, AtomPtr env) {
    for (unsigned i = 1; i < env->tail.size (); ++i) {
        AtomPtr s = env->tail.at (i);
        if (s->tail.at (0)->lexeme == node->lexeme) return s->tail.at (1);
    }
    if (!is_nil (env->tail.at (0))) return assoc (node, env->tail.at (0));

    error ("unbound identifier", node);
    return make_node (); // not reached
}
AtomPtr extend (AtomPtr sym, AtomPtr v, AtomPtr env, bool recurse) {
    for (unsigned i = 1; i < env->tail.size (); ++i) {
        AtomPtr e = env->tail.at (i);
        if (e->tail.at (0)->lexeme == sym->lexeme) {
            e->tail.at (1) = v;
            return v;
        }
    }
    if (recurse) { // set!
        if (!is_nil (env->tail.at (0))) return extend (sym, v, env->tail.at (0), recurse);
        error ("unbound identifier", sym);
    } else { // def
        env->tail.push_back (make_entry (sym, v));
        return v;
    }
    return make_node ();
}
void browse (AtomPtr env, AtomPtr output) {
    for (unsigned i = 1; i < env->tail.size (); ++i) {
        AtomPtr s = env->tail.at (i);
        output->tail.push_back (s->tail.at (0));
    }
    if (!is_nil (env->tail.at (0))) return browse (env->tail.at (0), output);
}
bool atom_eq (AtomPtr x, AtomPtr y) {
	if (x->type != y->type) { return 0; }
	switch (x->type) {
    case NUMBER: return (x->val == y->val);
    case SYMBOL: case STRING: return (x->lexeme == y->lexeme);
    case LIST: {
		if (x->tail.size () != y->tail.size ()) { return 0; }
		for (unsigned i = 0; i < x->tail.size (); ++i) {
			if (!atom_eq (x->tail[i], y->tail[i])) { return 0; }
		}
		return 1;
	}
    case PROCEDURE: return (x->action == y->action);
	case LAMBDA: return (atom_eq (x->tail[0], y->tail[0]) 
		&& atom_eq (x->tail[1], y->tail[1]));
	default:
		return 0;
	}
}
AtomPtr fn_quote (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
AtomPtr fn_def (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
AtomPtr fn_set (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
AtomPtr fn_if (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
template <bool dynamic> AtomPtr fn_lambda (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
AtomPtr fn_do (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
AtomPtr fn_catch (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
AtomPtr fn_eval (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
AtomPtr fn_apply (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
AtomPtr eval (AtomPtr node, AtomPtr env) {
tail_call:
    if (is_nil (node)) return make_node ();
    if (node->type == SYMBOL) return assoc (node, env);
    if (node->type != LIST) return node;
    // composite type
    AtomPtr car = eval (node->tail.at (0), env);
    if (car->action == &fn_quote) {
        std::cout << "quote" << std::endl;
        argnum_check (node, 2);
        return node->tail.at (1);
    }
    if (car->action == &fn_def) {
        argnum_check (node, 3);
        return extend (type_check (node->tail.at (1), SYMBOL), eval (node->tail.at (2), env), env, false); // not recursive
    }
    if (car->action == &fn_set) {
        argnum_check (node, 3);
        return extend (type_check (node->tail.at (1), SYMBOL), eval (node->tail.at (2), env), env, true); // recursive
    }        
    if (car->action == &fn_lambda<0> || car->action == &fn_lambda<1>) {
        argnum_check (node, 3);
        AtomPtr ss =  make_node (std::deque<AtomPtr> ({type_check (node->tail.at (1), LIST), 
            type_check (node->tail.at (2), LIST), (car->action == &fn_lambda<0> ? env : make_node ())}));
        return ss;
    }
    if (car->action == &fn_if) {
        argnum_check (node, 3);
        if (eval (node->tail.at (1), env)->lexeme == "#t") node = node->tail.at (2);
        else {
            if (node->tail.size () == 4) node = node->tail.at (3);
            else return make_node ();
        }
        goto tail_call;
    }
    if (car->action == &fn_catch) {
        argnum_check(node, 3);
        try {
            return eval (node->tail.at (2), env);
        } catch (AtomPtr& p) {
            if (atom_eq(p, node->tail.at (1))) {
                node = node->tail.at (3);
                goto tail_call;
            } else throw p;
        }
        return node; 
    }        
    if (car->action == &fn_do) {
        argnum_check (node, 2);
        AtomPtr res = make_node ();
        for (unsigned i = 1; i < node->tail.size () - 1; ++i) {
            res = eval (node->tail.at (i), env);
        }
        node = node->tail.at (node->tail.size () - 1);
        goto tail_call;
    }        

    // executable
    AtomPtr params = make_node ();
    for (unsigned i = 1; i < node->tail.size (); ++i) {
        params->tail.push_back (eval (node->tail.at (i), env));
    }
    AtomPtr exec = eval (car, env);
    if (exec->type == PROCEDURE) {
        argnum_check (params, exec->minargs);        
        if (exec->action == &fn_eval) {
            node = params->tail.at (0);
            goto tail_call;
        } 
        if (exec->action == &fn_apply) {
			type_check (params->tail.at (1), LIST)->tail.push_front (params->tail.at(0));
			node = params->tail.at(1);
            goto tail_call;
        }         
        return exec->action (params, env);
    } else if (exec->type == LAMBDA) {
		AtomPtr args = exec->tail.at (0);
		AtomPtr body = exec->tail.at (1);
		AtomPtr closure = is_nil (exec->tail.at (2)) ? env : exec->tail.at (2);
        AtomPtr nenv = make_node ();
        nenv->tail.push_back (closure);
		int minargs = args->tail.size () < params->tail.size () ? args->tail.size () 
			: params->tail.size ();
		for (unsigned i = 0; i < minargs; ++i) {
		    extend (args->tail. at (i), params->tail.at (i), nenv, false); // not recursive
		}
		if (args->tail.size () > params->tail.size ()) {
			AtomPtr args_cut = make_node ();
			for (unsigned i = minargs; i < args->tail.size (); ++i) {
				args_cut->tail.push_back (args->tail.at (i));
			}
			return make_node (std::deque<AtomPtr> ({args_cut, body, nenv})); // return lambda
		}
        env = nenv;
        node = body;
        goto tail_call;
    }
    error ("function expected", node);
    return make_node (); // not reached
}

// procedures
AtomPtr fn_env (AtomPtr node, AtomPtr env) {
    AtomPtr coll = make_node ();
    browse (env, coll);
    return coll;
}
AtomPtr fn_typeof (AtomPtr node, AtomPtr env) {
	return make_node (TYPE_NAMES[node->tail.at (0)->type]);
}
std::string unbind (AtomPtr k, AtomPtr env) {
	for (std::deque<AtomPtr>::iterator it = env->tail.begin() + 1; it != env->tail.end (); ++it) {
		if (atom_eq ((*it)->tail.at (0), k)) {
			env->tail.erase(it);
			return "#t";
		}
	}
	if (!is_nil(env->tail.at(0))) return unbind(k, env->tail.at(0));
	return "#f";
}
AtomPtr fn_unbind (AtomPtr n, AtomPtr env) {
	return make_node(unbind(type_check (n->tail.at(0), SYMBOL), env));
}
AtomPtr fn_throw (AtomPtr node, AtomPtr env) {
	throw node->tail.at (0);
	return make_node ();
}
AtomPtr fn_list (AtomPtr node, AtomPtr env) {
    return node;
}
AtomPtr fn_car (AtomPtr node, AtomPtr env) {
	AtomPtr o = type_check (node->tail.at (0), LIST);
	if (!o->tail.size ()) return make_node  ();
	else return o->tail.at (0);
}
AtomPtr fn_cdr (AtomPtr node, AtomPtr env) {
	AtomPtr l = make_node ();
	AtomPtr o = type_check (node->tail[0], LIST);
	if (o->tail.size () < 2) return make_node ();
	for (unsigned i = 1; i < o->tail.size (); ++i) {
		l->tail.push_back (o->tail[i]);
	}
	return l;
}
AtomPtr fn_join (AtomPtr node, AtomPtr env) {
    AtomPtr ctr = type_check (node->tail.at (0), LIST);
    for (unsigned i = 0; i < type_check (node->tail.at (1), LIST)->tail.size (); ++i) {
        ctr->tail.push_back (node->tail.at (1)->tail.at (i));
    }
    return ctr;
}
AtomPtr fn_eqp (AtomPtr node, AtomPtr env) {
	if (atom_eq (node->tail[0], node->tail[1])) return make_node ("#t");
    else return make_node ("#f");	
}
#define MAKE_BINOP(op,name, unit)									\
	AtomPtr name (AtomPtr n, AtomPtr env) {							\
		if (n->tail.size () == 1) {									\
			return make_node(unit						        	\
				op (type_check (n->tail[0], NUMBER)->val));			\
		}															\
		AtomPtr c = n;		                                        \
		Real s = (type_check (n->tail[0], NUMBER)->val);			\
		for (unsigned i = 1; i < n->tail.size (); ++i) {			\
			s = s op type_check (n->tail[i], NUMBER)->val;			\
		}									   						\
		return make_node (s);				        				\
	}																\

MAKE_BINOP(+,fn_add,0);
MAKE_BINOP(*,fn_mul,1);
MAKE_BINOP(-,fn_sub,0);
MAKE_BINOP(/,fn_div,1);

#define MAKE_CMPOP(f,o) \
	AtomPtr f (AtomPtr node, AtomPtr env) { \
		std::string v = "#f"; \
		for (unsigned i = 0; i < node->tail.size () - 1; ++i) { \
			if (type_check (node->tail[i], NUMBER)->val o node->tail[i + 1]->val) v = "#t"; \
			else return make_node ("#f"); \
		} \
 		return make_node (v); \
	} \

MAKE_CMPOP(fn_less,<);
MAKE_CMPOP(fn_lesseq,<=);
MAKE_CMPOP(fn_greater,>);
MAKE_CMPOP(fn_greatereq,>=);

#define MAKE_SINGOP(f,o) \
	AtomPtr f (AtomPtr node, AtomPtr env) { \
        AtomPtr r = make_node (); \
        for (unsigned i = 0; i < node->tail.size (); ++i) { \
            r->tail.push_back (make_node (o (type_check (node->tail[i], NUMBER)->val))); \
        } \
 		return r; \
	} \

MAKE_SINGOP(fn_sqrt, sqrt);
MAKE_SINGOP(fn_sin, sin);
MAKE_SINGOP(fn_cos, cos);
MAKE_SINGOP(fn_tan, tan);
MAKE_SINGOP(fn_log, log);
MAKE_SINGOP(fn_abs, fabs);
MAKE_SINGOP(fn_exp, exp);
MAKE_SINGOP(fn_floor, floor);
MAKE_SINGOP (fn_log10, log10);

template <int mode>
AtomPtr fn_format (AtomPtr node, AtomPtr env) {
	std::stringstream tmp;
	for (unsigned i = mode == 2 ? 1 : 0; i < node->tail.size (); ++i) {
		print (node->tail.at (i), tmp);
	}
	switch (mode) {
		case 0: // print
			std::cout << tmp.str ();
			return make_node ("");
		break;
		case 1: // to string
			return make_node ((std::string) "\"" + tmp.str ());
		break;
		case 2: // save
			std::string fname = node->tail.at (0)->lexeme;
			std::ofstream out (fname);
			if (!out.good ()) return make_node ("#f");
			out << tmp.str ();
			out.close ();
			return make_node("#t");
		break;	
	}
}
AtomPtr fn_nl (AtomPtr node, AtomPtr env) {
	return make_node ("\"\n");
}
AtomPtr fn_read (AtomPtr node, AtomPtr env) {
	return read (std::cin);
}
AtomPtr load (const std::string& filename, AtomPtr env) {
	std::ifstream in (filename);
	if (!in.good ()) return make_node ("#f");
    AtomPtr res = make_node ();
	while (!in.eof ()) {
		res = eval (read (in), env);
	}
	return make_node ("#t");
}
AtomPtr fn_load (AtomPtr node, AtomPtr env) {
	return load (type_check (node->tail.at (0), STRING)->lexeme, env);
}
void replace (std::string &s, std::string from, std::string to) {
    int idx = 0;
    int next;
    while ((next = s.find (from, idx)) != std::string::npos) {
        s.replace (next, from.size (), to);
        idx = next + to.size ();
    } 
}
template <int cmd>
AtomPtr fn_string (AtomPtr node, AtomPtr env) {
	AtomPtr l = make_node();
	std::regex r;
	if (cmd == 0) { // length
		return make_node(type_check (node->tail.at(0), STRING)->lexeme.size ());
	} else if (cmd == 1) { // find
		unsigned long pos = type_check (node->tail.at(0), STRING)->lexeme.find (
			type_check (node->tail.at(1), STRING)->lexeme);
		if (pos == std::string::npos) return make_node (-1);
		else return make_node (pos);		
    } else if (cmd == 2) { // range
		std::string tmp = type_check (node->tail.at(0), STRING)->lexeme.substr(
			type_check (node->tail.at(1), NUMBER)->val, 
			type_check (node->tail.at(2), NUMBER)->val);
		return make_node ((std::string) "\"" + tmp);
	} else if (cmd == 3) { // replace
		std::string tmp = type_check (node->tail.at(0), STRING)->lexeme;
		replace (tmp,
			type_check (node->tail.at(1), STRING)->lexeme, 
			type_check (node->tail.at(2), STRING)->lexeme);
		return make_node((std::string) "\"" + tmp);
	} else if (cmd == 4) { // regex
		std::string str = type_check (node->tail.at(0), STRING)->lexeme;
		std::regex r (type_check (node->tail.at(1), STRING)->lexeme);
	    std::smatch m; 
	    std::regex_search(str, m, r);
	    AtomPtr l = make_node();
	    for(auto v: m) {
	    	l->tail.push_back (make_node ((std::string) "\"" + v.str()));
	    }
		return l;		
	} 
    return l;
}
AtomPtr fn_exec (AtomPtr node, AtomPtr env) {
	return make_node (system (type_check (node->tail.at (0), STRING)->lexeme.c_str ()));
}
AtomPtr fn_exit (AtomPtr node, AtomPtr env) {
    std::cout << std::endl;
	exit (0);
	return make_node ();
}
AtomPtr make_op (const std::string& name, Op action, int minargs, AtomPtr env) {
    AtomPtr op = make_node (action);
    op->lexeme = name;
    op->minargs = minargs;
    return extend (make_node (name), op, env, false);;
}
AtomPtr make_env () {
    AtomPtr env = make_node ();
    env->tail.push_back (make_node ()); // no parent env
    env->tail.push_back (make_entry (make_node ("#t"), make_node ("#t")));
    env->tail.push_back (make_entry (make_node ("#f"), make_node ("#f")));
    
    make_op ("quote", &fn_quote, -1, env);
    make_op ("def", &fn_def, -1, env);
    make_op ("set!", &fn_set, -1, env);
    make_op ("if", &fn_if, -1, env);
    make_op ("lambda", &fn_lambda<0>, -1, env);
    make_op ("dynamic", &fn_lambda<1>, -1, env);
    make_op ("do", &fn_do, -1, env);
    make_op ("catch", &fn_catch, -1, env);
    make_op ("eval", &fn_eval, -1, env);
    make_op ("apply", &fn_apply, -1, env);
    make_op ("env", &fn_env, 0, env);
    make_op ("typeof", &fn_typeof, 1, env);
    make_op ("unbind", &fn_unbind, 1, env);
    make_op ("throw", &fn_throw, 1, env);
    make_op ("list", &fn_list, 0, env);
    make_op ("join", &fn_join, 2, env);
    make_op ("car", &fn_car, 1, env);
    make_op ("cdr", &fn_cdr, 1, env);
    make_op ("eq", &fn_eqp, 2, env);
    make_op ("+", &fn_add, 1, env);
    make_op ("*", &fn_mul, 1, env);
    make_op ("-", &fn_sub, 1, env);
    make_op ("/", &fn_div, 1, env);
    make_op ("<", &fn_less, 1, env);
    make_op ("<=", &fn_lesseq, 1, env);
    make_op (">", &fn_greater, 1, env);
    make_op (">=", &fn_greatereq, 1, env);
    make_op ("sqrt", &fn_less, 1, env);
    make_op ("sin", &fn_sin, 1, env);
    make_op ("cos", &fn_cos, 1, env);
    make_op ("tan", &fn_tan, 1, env);
    make_op ("log", &fn_log, 1, env);
    make_op ("log10", &fn_log10, 1, env);
    make_op ("exp", &fn_exp, 1, env);
    make_op ("abs", &fn_abs, 1, env);
    make_op ("print", &fn_format<0>, 1, env);
    make_op ("nl", &fn_nl, 0, env);
    make_op ("str", &fn_format<1>, 1, env);
    make_op ("read", &fn_read, 0, env);
    make_op ("load", &fn_load, 1, env);
    make_op ("save", &fn_format<2>, 2, env);
    make_op ("length", &fn_string<0>, 1, env);
    make_op ("find", &fn_string<1>, 2, env);
    make_op ("range", &fn_string<2>, 3, env);
    make_op ("replace", &fn_string<3>, 3, env);
    make_op ("regex", &fn_string<4>, 2, env);
    make_op ("exec", &fn_exec, 1, env);
    make_op ("exit", &fn_exit, 0, env);
    return env;
}

// interface
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