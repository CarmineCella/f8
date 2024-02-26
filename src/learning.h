// learning.h
//

#include "f8.h"
#include "learning/algorithms.h"
#include "learning/KNN.h"
#include "learning/linalg.h"

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
			if (item->tail.size () != 2) Context::error ("[knntrain] malformed item", node);
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
	AtomPtr fn_linefit (AtomPtr node, AtomPtr env) {
		std::valarray<Real>& x = type_check (node->tail.at(0), NUMERIC)->val;
		std::valarray<Real>& y = type_check (node->tail.at(1), NUMERIC)->val;
		if (x.size () != y.size ()) Context::error ("[linefit] x and y must have the same size", node);
		LineFit<Real> line;
		if (!line.fit (x, y)) Context::error ("[linefit] cannot fit a vertical line", node);
		Real slope = 0, intercept = 0;
		line.get_params (slope, intercept);
		std::valarray<Real> l ({slope, intercept});
		return make_node (l);
	}	
	AtomPtr matrix2list (Real* matrix, int r, int c) {
		AtomPtr m = make_node ();
		for (std::size_t row = 0; row < r; row++) {
			std::valarray<Real> rarray (c);
			for (std::size_t col = 0; col < c; col++) {
				rarray[col] = matrix[row * c + col];
			}
			m->tail.push_back (make_node (rarray));
		}
		return m;
	}
	void list2matrix (AtomPtr l, std::vector<Real>& m) {
		for (unsigned i = 0; i < l->tail.size (); ++i) {
			std::valarray<Real>& row = type_check (l->tail.at (i), NUMERIC)->val;
			for (unsigned j = 0; j < row.size (); ++j) {
				m.push_back (row[j]);
			}	
		}
	}
	void display_matrix(Real* matrix, int r, int c) {
		std::cout << "[";
		for (std::size_t row = 0; row < r; row++) {
			for (std::size_t col = 0; col < c; col++) {
				std::cout << matrix[row * c + col];
				if (col != c - 1) std::cout << ", ";
			}
			if (row != r - 1) std::cout << ";\n";
		}
		std::cout << "]\n";
	}
	AtomPtr fn_dispmat (AtomPtr node, AtomPtr env) {
		AtomPtr lmatrix = type_check (node->tail.at (0), LIST);
		int n = lmatrix->tail.size (); // rows
		if (n < 1) Context::error ("[dispmat] invalid matrix size", node);
		int m = type_check (lmatrix->tail.at (0), NUMERIC)->val.size (); // cols
		std::vector<Real> M;
		list2matrix (lmatrix, M);
		display_matrix (&M[0], n, m);
		return make_node ("");
	}
	AtomPtr fn_svd (AtomPtr node, AtomPtr env) {
		AtomPtr lmatrix = type_check (node->tail.at (0), LIST);
		int n = lmatrix->tail.size (); // rows
		if (n < 1) Context::error ("[svd] invalid matrix size", node);
		int m = type_check (lmatrix->tail.at (0), NUMERIC)->val.size (); // cols
		std::vector<Real> M;
		list2matrix (lmatrix, M);

		int k = (m<n?m:n);

		Real* tU =new Real[m*k];
		Real* tS =new Real[k];
		Real* tVT=new Real[k*n];
		
		// Compute SVD
		int INFO=0;
		char JOBU  = 'S';
		char JOBVT = 'S';
		int wssize = 3*(m<n?m:n)+(m>n?m:n);
		int wssize1 = 5*(m<n?m:n);
		wssize = (wssize>wssize1?wssize:wssize1);
		Real* wsbuf = new Real[wssize];
		svd(&JOBU, &JOBVT, &m, &n, &M[0], &m, &tS[0], &tU[0], &m, &tVT[0], &k, wsbuf, &wssize, &INFO);
		delete[] wsbuf;

		AtomPtr u = matrix2list (tU, m, k);
		std::valarray<Real> sarray (tS, k);
		AtomPtr s = make_node (sarray);
		AtomPtr vt = matrix2list (tVT, k, n);	
		
		AtomPtr res = make_node ();
		res->tail.push_back (u);
		res->tail.push_back (s);
		res->tail.push_back (vt);

		delete[] tU;
		delete[] tS;
		delete[] tVT;	
		return res;
	}		
	AtomPtr add_learning (AtomPtr env) {
		add_operator ("median", fn_median, 2, env);
		add_operator ("knntrain", fn_knntrain, 2, env);
		add_operator ("knntest", fn_knntest, 2, env);
		add_operator ("linefit", fn_linefit, 2, env);
		add_operator ("dispmat", fn_dispmat, 1, env);
		add_operator ("svd", fn_svd, 1, env);
		return env;
	}
}

// eof
