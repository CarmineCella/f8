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

#define MAX_COL 12

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
	AtomPtr matrix2list (Matrix<Real>& m) {
		AtomPtr l = make_node ();
		for (std::size_t row = 0; row < m.rows (); ++row) {
			std::valarray<Real> rarray (m.cols ());
			for (std::size_t col = 0; col < m.cols (); ++col) {
				rarray[col] = m (row, col);
			}
			l->tail.push_back (make_node (rarray));
		}
		return l;
	}
	AtomPtr matrix2list (Real* m, int r, int c) {
		Matrix<Real> mm (m, r, c);
		return matrix2list (mm);
	}	
	Matrix<Real> list2matrix (AtomPtr l) {
        Matrix<Real> m (l->tail.size (), type_check (l->tail.at (0), NUMERIC)->val.size ());
		for (unsigned i = 0; i < l->tail.size (); ++i) {
			std::valarray<Real>& row = type_check (l->tail.at (i), NUMERIC)->val;
			for (unsigned j = 0; j < row.size (); ++j) {
				m (i, j) = row[j];
			}	
		}
		return m;
	}
	size_t number_of_digits(Real n) {
		std::ostringstream strs;
		strs << n;
		return strs.str().size();
	}	
	void display_matrix(Matrix<Real>& m) {
		std::cout << "[" << std::endl;
		std::vector<int> max_len_per_col;
		for (size_t j = 0; j < m.cols (); ++j) {
			size_t max_len {};

			for (size_t i = 0; i < m.rows (); ++i) {
				int num_length = number_of_digits(m (i, j));
				if (num_length > max_len) {
					max_len = num_length;
				}
					
			}
			max_len_per_col.push_back (max_len);
		}
		for (std::size_t row = 0; row < m.rows (); ++row) {
			for (std::size_t col = 0; col < m.cols (); ++col) {
				std::cout <<std::setw (max_len_per_col[col]) << m (row, col);
				if (col != m.cols () - 1) std::cout << ", ";
				if (col > MAX_COL) {
					std::cout << "...";
					break;
				}
			}
			if (row != m.rows () - 1) std::cout << ";\n";
			if (row > MAX_PRINT) {
				std::cout << "...";
				break;
			}
		}
		std::cout << "\n]\n";
	}
	AtomPtr fn_matdisp (AtomPtr node, AtomPtr env) {
		for (unsigned i = 0; i < node->tail.size (); ++i) {
			AtomPtr lmatrix = type_check (node->tail.at (i), LIST);
			int n = lmatrix->tail.size (); // rows
			if (n < 1) Context::error ("[matdisp] invalid matrix size", node);
			Matrix<Real> m = list2matrix (lmatrix);
			display_matrix (m);
			std::cout << std::endl;
		}
		return make_node ("");
	}
	AtomPtr fn_matmul (AtomPtr node, AtomPtr env) {
		Matrix<Real> a = list2matrix (type_check (node->tail.at (0), LIST));
		for (unsigned i = 1; i < node->tail.size (); ++i) {
			Matrix<Real> b = list2matrix (type_check (node->tail.at (i), LIST));
			if (a.cols () != b.rows ()) Context::error ("[matmul] nonconformant arguments", node);
			a = matmul<Real> (a, b);
		}
		return matrix2list (a);
	}
	AtomPtr fn_mattran (AtomPtr node, AtomPtr env) {
		Matrix<Real> a = list2matrix (type_check (node->tail.at (0), LIST));
		Matrix<Real> tr = ~a;
		return matrix2list (tr);
	}	
	AtomPtr add_learning (AtomPtr env) {
		add_operator ("median", fn_median, 2, env);
		add_operator ("knntrain", fn_knntrain, 2, env);
		add_operator ("knntest", fn_knntest, 2, env);
		add_operator ("linefit", fn_linefit, 2, env);
		add_operator ("matdisp", fn_matdisp, 1, env);
		add_operator ("matmul", fn_matmul, 2, env);
		add_operator ("mattran", fn_mattran, 1, env);
		return env;
	}
}

// eof
