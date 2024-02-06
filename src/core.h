// core.h

#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <deque>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <regex>
#include <cmath>
#include <fstream>
#include <valarray>
#include <dlfcn.h>

#define BOLDWHITE   "\033[1m\033[37m"
#define RED     	"\033[31m" 
#define RESET   	"\033[0m"

///~ ## Types
///~ The valid types are: number, symbol, string, list, operator, lambda, object
namespace f8 {
    // ast
    struct Atom;
    typedef std::shared_ptr<Atom> AtomPtr;
    typedef AtomPtr (*Op) (AtomPtr, AtomPtr);
    #define make_node(type)(std::make_shared<Atom>(type))
    enum AtomType {
        NUMBER, SYMBOL, STRING, LIST, OPERATOR, LAMBDA, OBJECT
    };
    const char* TYPE_NAMES[] = {
        "number", "symbol", "string", "list", "operator", "lambda", "object"
    };
    typedef double Real;
    bool is_string (const std::string& s);
    struct Atom {
        Atom () {type = LIST;}
        Atom (std::valarray<Real>& values) {
            type = LIST;
            for (unsigned i = 0; i < values.size (); ++i) tail.push_back (make_node (values[i]));
        }
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
        Atom (Op f) {type = OPERATOR; action = f;}
        Atom (std::deque<AtomPtr> ll) {
            type = LAMBDA;
            tail.push_back (ll.at (0));
            tail.push_back (ll.at (1));
            tail.push_back (ll.at (2));
        }
        Atom (void * o) { 
            type = OBJECT;
            obj = o;
        }    
        AtomType type;
        Real val;
        std::string lexeme;
        Op action;
        std::deque<AtomPtr> tail;
        int minargs;
        void* obj;
    };

    // helpers
    AtomPtr make_obj (const std::string& otype, void* ptr, AtomPtr cb) {
        AtomPtr o = make_node (ptr);
        o->lexeme = otype;
        o->tail.push_back (cb);
        return o;
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
        case OPERATOR: return (x->action == y->action);
        case LAMBDA: return (atom_eq (x->tail[0], y->tail[0]) 
            && atom_eq (x->tail[1], y->tail[1]));
        case OBJECT: return (x->lexeme == y->lexeme && x->obj == y->obj);        
        default:
            return 0;
        }
    }
    bool is_nil (AtomPtr node) {
        return node == nullptr || (node->type == LIST && node->tail.size () == 0);
    }
    std::ostream& print (AtomPtr node, std::ostream& out, bool write = false) {
        if (node->type == NUMBER) out << std::fixed  << node->val;
        if (node->type == SYMBOL || node->type == STRING) {
            if (node->type == STRING && write) out << "\"";
            out << node->lexeme;
            if (node->type == STRING && write) out << "\"";
        }
        if (node->type == OPERATOR) {
            if (write) out << node->lexeme;
            else out << "<operator " << node->lexeme << " @" << (std::hex) << node << ">";
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
        if (node->type == OBJECT) {
            out << "<object: " << node->lexeme << " @" << &node->obj << ">";    
        }
        out.flush ();
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
        throw make_node (err.str ());
    }
    AtomPtr argnum_check (AtomPtr node, int sz) {
        if (node->tail.size () < sz && sz != -1) {
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

    // parsing
    std::string next (std::istream& input) {
        std::stringstream accum;
        while (!input.eof ()) {
            char c = input.get ();
            switch (c) { 			
                case '(': case ')': case '\'': case '{': case '}': case '\n':
                    if (accum.str ().size ()) {
                        input.unget();
                        return accum.str ();
                    } else {
                        accum << c;
                        return accum.str ();
                    }
                break;
                case '#':
                    while (c != '\n' && !input.eof ())  { c = input.get (); }
                break;		            
                case ' ': case '\t': case '\r':  case '\0':
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
    AtomPtr read_line (std::istream& in);
    AtomPtr read (std::istream& in) {
        std::string token = next (in);
        if (token == "(") {
            AtomPtr l = make_node();
            AtomPtr a = make_node ();
            while (!in.eof ()) {
                a = read (in);
                if (a->lexeme == "\n" && a->type != STRING) continue;
                if (a->lexeme == ")" && a->type != STRING) break;
                l->tail.push_back (a);
            }
            return l;
        } else if (token == "{") {
            AtomPtr l = make_node();
            l->tail.push_back (make_node ("do"));
            AtomPtr a = make_node ();
            while (!in.eof ()) {
                a = read_line (in);
                if (a->tail.size () && atom_eq (a->tail.at (a->tail.size () - 1),
                    make_node ("}"))) {
                    a->tail.pop_back ();
                    if (!is_nil (a)) l->tail.push_back (a);
                    break;
                }
                if (!is_nil (a)) l->tail.push_back (a);
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
    AtomPtr read_line (std::istream& in) {
        AtomPtr l = make_node ();
        while (!in.eof ()) {
            AtomPtr a = read (in);
            if (in.eof ()) break;
            if (atom_eq (a, make_node ("}"))) {
                l->tail.push_back (a);
                break;
            }
            if (atom_eq (a, make_node ("\n"))) {
                if (l->tail.size () == 0) continue;
                else break;        
            }
            l->tail.push_back (a);
        }
        return l;
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
    void browse_env (AtomPtr env, AtomPtr output) {
        if (!is_nil (env->tail.at (0))) browse_env (env->tail.at (0), output);
        for (unsigned i = 1; i < env->tail.size (); ++i) {
            AtomPtr s = env->tail.at (i);
            output->tail.push_back (s->tail.at (0));
        }
    }
    ///~ ## Built-in operators
    ///~ `quote` *`expr`* <br>
    ///~ The quote operator indicates literal data; it suppresses evaluation.
    
    AtomPtr fn_quote (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_set (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_updef (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_reset (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_proc (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_dynamic (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_if (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_while (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    template <bool dynamic> AtomPtr fn_lambda (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_do (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_catch (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_break (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_eval (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr fn_apply (AtomPtr node, AtomPtr env) { return make_node (); } // dummy
    AtomPtr eval (AtomPtr node, AtomPtr env) {
    tail_call:
        if (is_nil (node)) return make_node ();
        if (node->type == SYMBOL) { // && node->lexeme.size ()) {
            return assoc (node, env); 
        }
        if (node->type != LIST) return node;
        // composite type
        AtomPtr car = eval (node->tail.at (0), env);
        if (car->action == &fn_quote) {
            argnum_check (node, 2);
            return node->tail.at (1);
        }
        if (car->action == &fn_set || car->action == &fn_updef) {
            argnum_check (node, 3);
            AtomPtr cenv = env;
            if (car->action == &fn_updef) {
                cenv = env->tail.at (0);
                if (is_nil (cenv)) error ("[updef] parent enviroment undefined", node);
            }
            return extend (type_check (node->tail.at (1), SYMBOL), eval (node->tail.at (2), env), cenv, false); // not recursive
        }
        if (car->action == &fn_reset) {
            argnum_check (node, 3);
            return extend (type_check (node->tail.at (1), SYMBOL), eval (node->tail.at (2), env), env, true); // recursive
        }        
        if (car->action == &fn_proc || car->action == &fn_dynamic) {
            argnum_check (type_check (node->tail.at (1), LIST), 1);
            AtomPtr name = node->tail.at (1)->tail.at (0);
            AtomPtr args = make_node ();
            for (unsigned i = 1; i < node->tail.at (1)->tail.size (); ++i) {
                args->tail.push_back (type_check (node->tail.at (1)->tail.at (i), SYMBOL));
            }
            AtomPtr body = node->tail.at (2);
            return extend (name, make_node (std::deque<AtomPtr> ({args, body, car->action == &fn_dynamic ? make_node () : env})), env, false);
        }
        if (car->action == &fn_lambda<0> || car->action == &fn_lambda<1>) {
            argnum_check (node, 3);
            AtomPtr ss =  make_node (std::deque<AtomPtr> ({type_check (node->tail.at (1), LIST), 
                type_check (node->tail.at (2), LIST), (car->action == &fn_lambda<0> ? env : make_node ())}));
            return ss;
        }
        if (car->action == &fn_if) {
            argnum_check (node, 3);
            if (eval (node->tail.at (1), env)->val != 0) node = node->tail.at (2);
            else {
                if (node->tail.size () > 3) {
                    argnum_check (node, 5);
                    if (type_check (node->tail.at (3), SYMBOL)->lexeme == "else") {
                        node = node->tail.at (4);
                    } else error ("invalid if/else syntax in ", node);
                } else return make_node ();
            }
            goto tail_call;
        }
        if (car->action == &fn_while) {
            argnum_check (node, 2);
            AtomPtr res = make_node ();
            try {
                while (eval (node->tail.at (1), env)->val != 0) {
                    res = eval (node->tail.at (2), env);
                }
            } catch (Op& e) {
                if (e == &fn_break) return res;
            }
            return res;
        }    
        if (car->action == &fn_catch) {
            argnum_check(node, 2);
            try {
                return eval (node->tail.at (1), env);
            } catch (AtomPtr p) {
                return p;
            }
        }        
        if (car->action == &fn_do) {
            argnum_check (node, 2);
            AtomPtr res = make_node ();

            for (unsigned i = 1; i < node->tail.size () - 1; ++i) {
                res = eval (node->tail.at (i), env);
            }
            node = node->tail.at (node->tail.size () - 1);
            goto tail_call;
            return res;
        }        

        // executable
        AtomPtr params = make_node ();
        for (unsigned i = 1; i < node->tail.size (); ++i) {
            params->tail.push_back (eval (node->tail.at (i), env));
        }
        AtomPtr exec = eval (car, env);
        if (exec->type == OPERATOR) {
            argnum_check (params, exec->minargs);      
            if (exec->action == &fn_break) {
			   throw &fn_break;
		    }  
            if (exec->action == &fn_eval) {
                node = params->tail.at (0);
                goto tail_call;
            } 
            if (exec->action == &fn_apply) {
                AtomPtr cp = make_node ();
                
                cp->tail.push_back (params->tail.at(0));
                for (unsigned i = 0; i < type_check (params->tail.at (1), LIST)->tail.size (); ++i) {
                    cp->tail.push_back (params->tail.at (1)->tail.at (i));
                }
                node = cp;
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
    AtomPtr fn_info (AtomPtr b, AtomPtr env) {
        std::string cmd = b->tail.at (0)->lexeme;
        AtomPtr l = make_node();
        std::regex r;
        if (cmd == "vars") {
            if (b->tail.size () > 1) {
                r.assign (b->tail.at(1)->lexeme);
            } else r.assign (".*");
            AtomPtr vars = make_node (); 
            browse_env (env, vars);
            for (unsigned i = 0; i < vars->tail.size () ; ++i) {
                std::string k = vars->tail.at (i)->lexeme;
                if (std::regex_match(k, r)) {
                    l->tail.push_back(make_node(k));
                }
            }
        } else if (cmd == "exists") {
            for (unsigned i = 1; i < b->tail.size (); ++i) {
                AtomPtr key = b->tail.at (i);		
                Real ans = 1;
                try {
                    AtomPtr r = assoc (key, env);
                } catch (...) {
                    ans = 0;
                }    	
                l->tail.push_back(make_node(ans));
            }
        } else if (cmd == "typeof") {
            for (unsigned i = 1; i < b->tail.size (); ++i) {
                l->tail.push_back(make_node(TYPE_NAMES[b->tail.at (i)->type]));
            }
        } else {
            error ("[info] invalid request", b->tail.at (0));
        }
        return l;
    }
    bool unbind (AtomPtr k, AtomPtr env) {
        for (std::deque<AtomPtr>::iterator it = env->tail.begin() + 1; it != env->tail.end (); ++it) {
            if (atom_eq ((*it)->tail.at (0), k)) {
                env->tail.erase(it);
                return true;
            }
        }
        if (!is_nil(env->tail.at(0))) return unbind(k, env->tail.at(0));
        return false;
    }
    AtomPtr fn_unset (AtomPtr n, AtomPtr env) {
        return make_node(unbind(type_check (n->tail.at(0), SYMBOL), env));
    }
    AtomPtr fn_throw (AtomPtr node, AtomPtr env) {
        throw node->tail.at (0);
        return make_node ();
    }
    AtomPtr fn_list (AtomPtr node, AtomPtr env) {
        return node;
    }
    AtomPtr fn_lindex (AtomPtr node, AtomPtr env) {
        int p  = (int) type_check (node->tail.at (0), NUMBER)->val;
        AtomPtr o = type_check (node->tail.at (1), LIST);
        if (!o->tail.size ()) return make_node  ();
        if (p < 0 || p >= o->tail.size ()) error ("[lget] invalid index", node);
        return o->tail.at (p);
    }
    AtomPtr fn_llength (AtomPtr node, AtomPtr env) {
        return make_node (node->tail.at (0)->tail.size ());
    }
    AtomPtr append (AtomPtr dst, AtomPtr ll) {
        if (ll->type == LIST) {
            for (unsigned i = 0; i < ll->tail.size (); ++i) {
                dst->tail.push_back (ll->tail.at (i));
            }
        } else dst->tail.push_back (ll);
        return dst;
    }
    AtomPtr fn_lappend (AtomPtr n, AtomPtr env) {
        AtomPtr dst = n->tail.at(0);
        for (unsigned i = 1; i < n->tail.size (); ++i){
            dst = append (dst, n->tail.at(i));
        }	
        return dst;
    }
    AtomPtr fn_lrange (AtomPtr params, AtomPtr env) {
        AtomPtr l = type_check (params->tail.at (0), AtomType::LIST);
        int i = (int) (type_check(params->tail.at (1), AtomType::NUMBER)->val);
        int len = (int) (type_check(params->tail.at (2), AtomType::NUMBER)->val);
        int end = i + len;
        int stride = 1;
        if (params->tail.size () == 4) {
            stride  = (int) (type_check(params->tail.at (3), AtomType::NUMBER)->val);
        }
        if (i < 0) i = 0;
        if (len < i) len = i;
        if (end > l->tail.size ()) end = l->tail.size ();
        AtomPtr nl = make_node();
        for (int j = i; j < end; j += stride) nl->tail.push_back(l->tail.at (j));
        return nl;
    }
    AtomPtr fn_lreplace (AtomPtr params, AtomPtr env) {
        AtomPtr l = type_check (params->tail.at (0), AtomType::LIST);
        AtomPtr r = type_check (params->tail.at (1), AtomType::LIST);
        int i = (int) (type_check(params->tail.at (2), AtomType::NUMBER)->val);
        int len = (int) (type_check(params->tail.at (3), AtomType::NUMBER)->val);
        int stride = 1;
        if (params->tail.size () == 5) {
            stride  = (int) (type_check(params->tail.at (4), AtomType::NUMBER)->val);
        }
        if (i < 0 || len < 0 || stride < 1 || i + len  > l->tail.size () || (int) (len / stride) > r->tail.size ()) {
            return make_node();
        }
        AtomPtr nl = make_node();
        int p = 0;
        for (int j = i; j < i + len; j += stride) {
            l->tail.at(j) = r->tail.at (p);
            ++p;
        }
        return r;
    }    
    AtomPtr fn_eqp (AtomPtr node, AtomPtr env) {
        if (atom_eq (node->tail[0], node->tail[1])) return make_node (1);
        else return make_node (0);	
    }
	void list2array (AtomPtr list, std::valarray<Real>& out) {
		int sz = list->tail.size (); 
		out.resize (sz ? sz : 1);
		if (sz) {
			for (unsigned i = 0; i < list->tail.size (); ++i) {
				out[i] = type_check (list->tail.at (i), NUMBER)->val;
			}
		} else out[0]= type_check (list, NUMBER)->val;
	}
	AtomPtr array2list (const std::valarray<Real>& out) {
		AtomPtr list = make_node ();
		for (unsigned i = 0; i < out.size (); ++i) {
			list->tail.push_back (make_node (out[i]));
		}
		return list->tail.size () == 1 ? list->tail.at (0) : list;
	}    
	#define MAKE_ARRAYBINOP(op,name) 					\
		AtomPtr name (AtomPtr n, AtomPtr env) { 	\
			std::valarray<Real> res; \
			list2array (n->tail.at (0), res); \
			for (unsigned i = 1; i < n->tail.size (); ++i) {  \
				std::valarray<Real> a; \
				list2array (n->tail.at (i), a); \
				if (a.size () == 1) res = res op a[0]; \
				else res = res op a; \
			} \
			return array2list (res); \
		} \

	MAKE_ARRAYBINOP (+, fn_add);
	MAKE_ARRAYBINOP (-, fn_sub);
	MAKE_ARRAYBINOP (*, fn_mul);
	MAKE_ARRAYBINOP (/, fn_div);
	MAKE_ARRAYBINOP (>, fn_greater);
	MAKE_ARRAYBINOP (>=, fn_greatereq);
	MAKE_ARRAYBINOP (<, fn_less);
	MAKE_ARRAYBINOP (<=, fn_lesseq);

	#define MAKE_ARRAYMETHODS(op,name)									\
		AtomPtr name (AtomPtr n, AtomPtr env) {						\
			AtomPtr res = make_node (); \
			for (unsigned i = 0; i < n->tail.size (); ++i) { \
				std::valarray<Real> v; \
				list2array (n->tail.at (i), v); \
				res->tail.push_back (make_node (v.op ())); \
			}\
			return res->tail.size () == 1 ? res->tail.at (0) : res; \
		}\

	MAKE_ARRAYMETHODS (min, fn_min);
	MAKE_ARRAYMETHODS (max, fn_max);
	MAKE_ARRAYMETHODS (sum, fn_sum);
	MAKE_ARRAYMETHODS (size, fn_size);

	#define MAKE_ARRAYSINGOP(op,name)									\
		AtomPtr name (AtomPtr n, AtomPtr env) {						\
			AtomPtr res = make_node (); \
			for (unsigned i = 0; i < n->tail.size (); ++i) { \
				std::valarray<Real> v; \
				list2array (n->tail.at (i), v); \
				v = op (v); \
				res->tail.push_back (array2list (v)); \
			}\
			return res->tail.size () == 1 ? res->tail.at (0) : res; \
		}\

	MAKE_ARRAYSINGOP (std::abs, fn_abs);
	MAKE_ARRAYSINGOP (exp, fn_exp);
	MAKE_ARRAYSINGOP (log, fn_log);
	MAKE_ARRAYSINGOP (log10, fn_log10);
	MAKE_ARRAYSINGOP (sqrt, fn_sqrt);
	MAKE_ARRAYSINGOP (sin, fn_sin);
	MAKE_ARRAYSINGOP (cos, fn_cos);
	MAKE_ARRAYSINGOP (tan, fn_tan);
   	MAKE_ARRAYSINGOP (asin, fn_asin);
	MAKE_ARRAYSINGOP (acos, fn_acos);
	MAKE_ARRAYSINGOP (atan, fn_atan);
	MAKE_ARRAYSINGOP (sinh, fn_sinh);
	MAKE_ARRAYSINGOP (cosh, fn_cosh);
	MAKE_ARRAYSINGOP (tanh, fn_tanh);

    // MAKE_ARRAYSINGOP (floor, fn_floor);

    AtomPtr fn_neg (AtomPtr n, AtomPtr env) {						
        AtomPtr res = make_node (); 
        for (unsigned i = 0; i < n->tail.size (); ++i) { 
            std::valarray<Real> v; 
            list2array (n->tail.at (i), v); 
            v = -(v); 
            res->tail.push_back (array2list (v)); 
        }\
        return res->tail.size () == 1 ? res->tail.at (0) : res; 
    }
    AtomPtr fn_floor (AtomPtr n, AtomPtr env) {						
        AtomPtr res = make_node (); 
        for (unsigned i = 0; i < n->tail.size (); ++i) { 
            std::valarray<Real> v; 
            list2array (n->tail.at (i), v); 
            for (unsigned j = 0; j < v.size (); ++j) v[j] = floor (v[j]); 
            res->tail.push_back (array2list (v)); 
        }\
        return res->tail.size () == 1 ? res->tail.at (0) : res; 
    }
    template <int mode>
    AtomPtr fn_format (AtomPtr node, AtomPtr env) {
        std::stringstream tmp;
        for (unsigned i = mode == 2 ? 1 : 0; i < node->tail.size (); ++i) {
            print (node->tail.at (i), tmp, mode == 2); // set write flag if necessary
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
                if (!out.good ()) return make_node (0);
                out << tmp.str ();
                out.close ();
                return make_node(1);
            break;	
        }
    }
    AtomPtr fn_read (AtomPtr node, AtomPtr env) {
        return read_line (std::cin);
    }
    AtomPtr load (const std::string& name, AtomPtr env) {
        std::ifstream in (name.c_str ());
        if (!in.good ()) {
            std::string longname = getenv("HOME");
            longname += "/.f8/" + name;
            in.open (longname.c_str());
            if (!in.good ()) return make_node(0);
        }
        AtomPtr res = make_node ();
        while (!in.eof ()) {
            res = read_line (in);
            if (!is_nil (res)) eval (res, env);
        }
        return make_node (1);
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
    AtomPtr fn_string (AtomPtr node, AtomPtr env) {
        std::string cmd = type_check (node->tail.at (0), SYMBOL)->lexeme;
        AtomPtr l = make_node();
        std::regex r;
        if (cmd == "length") { // argnum checked by default
            return make_node(type_check (node->tail.at(1), STRING)->lexeme.size ());
        } else if (cmd == "find") {
            argnum_check (node, 3);
            unsigned long pos = type_check (node->tail.at(1), STRING)->lexeme.find (
                type_check (node->tail.at(2), STRING)->lexeme);
            if (pos == std::string::npos) return make_node (0);
            else return make_node (pos);		
        } else if (cmd == "range") {
            argnum_check (node, 4);
            std::string tmp = type_check (node->tail.at(1), STRING)->lexeme.substr(
                type_check (node->tail.at(2), NUMBER)->val, 
                type_check (node->tail.at(3), NUMBER)->val);
            return make_node ((std::string) "\"" + tmp);
        } else if (cmd == "replace") {
            argnum_check (node, 4);
            std::string tmp = type_check (node->tail.at(1), STRING)->lexeme;
            replace (tmp,
                type_check (node->tail.at(2), STRING)->lexeme, 
                type_check (node->tail.at(3), STRING)->lexeme);
            return make_node((std::string) "\"" + tmp);
        } else if (cmd == "regex") {
            argnum_check (node, 3);
            std::string str = type_check (node->tail.at(1), STRING)->lexeme;
            std::regex r (type_check (node->tail.at(2), STRING)->lexeme);
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
    #define DECLFFI(name, f) \
        AtomPtr name (AtomPtr node, AtomPtr env) { \
            AtomPtr out = nullptr; 	\
            f (node, env, out);\
            return out;\
        }

    AtomPtr add_operator (const std::string& name, Op action, int minargs, AtomPtr env) {
        AtomPtr op = make_node (action);
        op->lexeme = name;
        op->minargs = minargs;
        return extend (make_node (name), op, env, false);;
    }
    AtomPtr fn_import (AtomPtr params, AtomPtr env) {
            std::string name = getenv ("HOME");
    #ifdef __APPLE__
            name += "/.f8/" + type_check (params->tail.at(0), AtomType::STRING)->lexeme + ".so";
    #elif __linux
            name += "/.f8/" + type_check (params->tail.at(0), AtomType::STRING)->lexeme + ".so";
    #else
            name += "/.f8/" + type_check (params->tail.at(0), AtomType::STRING)->lexeme + ".dll";
    #endif
        void* handle = dlopen (name.c_str (), RTLD_NOW);
        if (!handle) {
            error ((std::string) "[import] " + dlerror (), params);
        }
        unsigned ct = 0;
        for (unsigned i = 0; i < params->tail.at(1)->tail.size() / 2; ++i) {
            Op f = (Op) dlsym (handle, 
                type_check (params->tail.at(1)->tail.at(2 * i), AtomType::SYMBOL)->lexeme.c_str ());
            
            if (f) {
                add_operator(params->tail.at(1)->tail.at(2 * i)->lexeme, 
                    f, 
                    type_check (params->tail.at(1)->tail.at(2 * i + 1), NUMBER)->val, env); // silent error
                ++ct;
            }
        }
        return make_node(ct); // number of operator imported
    }
    AtomPtr add_core (AtomPtr env) {
        env->tail.push_back (make_entry (make_node ("nl"), make_node ("\n")));
        env->tail.push_back (make_entry (make_node ("tab"), make_node ("\t")));        
        add_operator ("quote", &fn_quote, -1, env); // -1 are checked in the eval function
        add_operator ("set", &fn_set, -1, env);
        add_operator ("updef", &fn_updef, -1, env);
        add_operator ("!", &fn_reset, -1, env);
        add_operator ("proc", &fn_proc, -1, env);
        add_operator ("dynamic", &fn_dynamic, -1, env);
        add_operator ("if", &fn_if, -1, env);
        add_operator ("while", &fn_while, -1, env);
        add_operator ("\\", &fn_lambda<0>, -1, env);
        add_operator ("@", &fn_lambda<1>, -1, env);
        add_operator ("do", &fn_do, -1, env);
        add_operator ("catch", &fn_catch, -1, env);
        add_operator ("break", &fn_break, -1, env);
        add_operator ("eval", &fn_eval, 1, env);
        add_operator ("->", &fn_apply, 2, env);
        add_operator ("info", &fn_info, 1, env);
        add_operator ("unset", &fn_unset, 1, env);
        add_operator ("throw", &fn_throw, 1, env);
        add_operator ("list", &fn_list, 0, env);
        add_operator ("lappend", &fn_lappend, 1, env);
        add_operator ("lreplace", &fn_lreplace, 4, env);
        add_operator ("lrange", &fn_lrange, 3, env);
        add_operator ("lindex", &fn_lindex, 1, env);
        add_operator ("llength", &fn_llength, 1, env);
        add_operator ("eq", &fn_eqp, 2, env);
        add_operator ("+", &fn_add, 1, env);
        add_operator ("*", &fn_mul, 1, env);
        add_operator ("-", &fn_sub, 1, env);
        add_operator ("/", &fn_div, 1, env);
        add_operator ("<", &fn_less, 1, env);
        add_operator ("<=", &fn_lesseq, 1, env);
        add_operator (">", &fn_greater, 1, env);
        add_operator (">=", &fn_greatereq, 1, env);
        add_operator ("sqrt", &fn_sqrt, 1, env);
        add_operator ("sin", &fn_sin, 1, env);
        add_operator ("cos", &fn_cos, 1, env);
        add_operator ("tan", &fn_tan, 1, env);
        add_operator ("log", &fn_log, 1, env);
        add_operator ("log10", &fn_log10, 1, env);
        add_operator ("exp", &fn_exp, 1, env);
        add_operator ("abs", &fn_abs, 1, env);
        add_operator ("min", &fn_min, 1, env);
        add_operator ("max", &fn_max, 1, env);
        add_operator ("sum", &fn_sum, 1, env);
        add_operator ("size", &fn_size, 1, env);
        add_operator ("neg", &fn_neg, 1, env);
        add_operator ("floor", &fn_floor, 1, env);
        add_operator ("puts", &fn_format<0>, 1, env);
        add_operator ("gets", &fn_read, 0, env);
        add_operator ("source", &fn_load, 1, env);
        add_operator ("save", &fn_format<2>, 2, env);
        add_operator ("tostr", &fn_format<1>, 1, env); 
        add_operator ("string", &fn_string, 2, env);
        add_operator ("exec", &fn_exec, 1, env);
        add_operator ("exit", &fn_exit, 0, env);
        add_operator ("import", &fn_import, 1, env);
        return env;    
    }

    // interface
    void repl (AtomPtr env, std::istream& in, std::ostream& out) {
        std::istream* current = &in;	
        while (true){
            out << ">> ";
            try {
                print (eval (read_line (*current), env), out);
                out << std::endl;
            } catch (f8::AtomPtr& e) {
                std::cout << RED << "error: "; f8::print (e, out) << RESET << std::endl;
            } catch (std::exception& e) {
                std::cout << RED << "exception: " << e.what () << RESET << std::endl;
            } catch (...) {
                std::cout << RED << "fatal unknown error" << RESET << std::endl;
            }
        } 
    }
}

#endif // CORE_H

// eof