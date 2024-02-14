// learning.h
//

#include "f8.h"
#include "learning/algorithms.h"

#include <valarray>
#include <iomanip>

namespace f8 {
	AtomPtr fn_median (AtomPtr node, AtomPtr env) {
		std::valarray<Real>& v = type_check (node->tail.at (0), NUMERIC)->val;
		int order = (int) type_check (node->tail.at (1), NUMERIC)->val[0];

		Real init = 0;
		std::valarray<Real> in (init, v.size () + order);
		in[std::slice(order / 2, v.size () + order / 2, 1)] = v;
		std::valarray<Real> out (init, v.size () + order);
		for (unsigned i = 0; i < v.size (); ++i) {
			std::valarray<Real> s = in[std::slice (i, i + order, 1)];
			out[i] = median<Real> (&s[0], order); // alignment is order / 2
		}
		std::valarray<Real> c = out[std::slice(0, v.size (), 1)];
		return make_node(c);
	}
	AtomPtr add_learning (AtomPtr env) {
		add_operator ("median", fn_median, 2, env);
		return env;
	}
}

// eof
