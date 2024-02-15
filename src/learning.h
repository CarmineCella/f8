// learning.h
//

#include "f8.h"
#include "learning/algorithms.h"
#include "learning/KNN.h"

#include <valarray>
#include <string>
#include <sstream>
#include <iomanip>

namespace f8 {
	AtomPtr fn_median (AtomPtr node, AtomPtr env) {
		std::valarray<Real>& v = type_check (node->tail.at (0), NUMERIC)->val;
		int order = (int) type_check (node->tail.at (1), NUMERIC)->val[0];

		if (order < 0 || order >= v.size ()) Context::error ("[median] invalid order", node);

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
	AtomPtr fn_knntrain (AtomPtr node, AtomPtr env) {
		AtomPtr data = type_check (node->tail.at (0), LIST);
		int k = type_check (node->tail.at (1), NUMERIC)->val[0];
		int obs = data->tail.size ();
		if (obs < 1) Context::error ("[knntrain] insufficient number of observations", node);
		if (k < 1 || k > obs) Context::error ("[knntrain] invalid K parameter", node);
		int features = data->tail.at (0)->tail.at (0)->val.size (); // take the first sample for dimensions
		if (features < 1) Context::error ("[knntrain] invalid number of features", node);

		KNN<Real>* knn = new KNN<Real>(k, features);
		for (unsigned i = 0; i < data->tail.size (); ++i) {
			Observation<Real>* o = new Observation<Real> ();
			AtomPtr item = type_check (data->tail.at (i), LIST);
			o->attributes = type_check (item->tail.at (0), NUMERIC)->val;
			std::stringstream s;
			puts (item->tail.at (1), s);
			o->classlabel = s.str ();
			knn->addObservation (o);
		} 
		return make_obj ("knntrain", (void*) knn, make_node ()); 
	}	
	AtomPtr fn_knntest (AtomPtr node, AtomPtr env) {
		AtomPtr p = type_check (node->tail.at(0), OBJECT);
        if (p->obj == 0 || p->lexeme != "knntrain") return  make_node(0);
        KNN<Real>* knn = static_cast<KNN<Real>*>(p->obj);
		AtomPtr classif = make_node ();
		AtomPtr data = type_check (node->tail.at(1), LIST);
		for (unsigned i = 0; i < data->tail.size (); ++i) {
			AtomPtr item =  type_check (data->tail.at(i), LIST);
			Observation<Real> o;
			o.attributes = item->tail.at (0)->val;
			classif->tail.push_back (make_node (knn->classify (o)));
		}
		return classif;
	}
	AtomPtr add_learning (AtomPtr env) {
		add_operator ("median", fn_median, 2, env);
		add_operator ("knntrain", fn_knntrain, 2, env);
		add_operator ("knntest", fn_knntest, 2, env);
		return env;
	}
}

// eof
