// plotting.h
//

#include "f8.h"
#include "plotting/svg_tools.h"

#include <iomanip>

namespace f8 {
	AtomPtr fn_opensvg (AtomPtr node, AtomPtr env) {
		std::string name = type_check (node->tail.at (0), STRING)->lexeme;
		int width = (int) type_check (node->tail.at (1), NUMERIC)->val[0];
		int height = (int) type_check (node->tail.at (2), NUMERIC)->val[0];

		if (width < 200 || height < 200) return make_node ();
		AtomPtr ll =  make_node ();
		ll->tail.push_back (make_node((std::string) "\"" + name));

		Dimensions dim (width, height);
		Document* doc = new Document (name, Layout (dim, Layout::BottomLeft));
		if (doc) {
			return make_obj ("svgdoc", (void*) doc, ll);
		} else return make_node(0);
	}
	AtomPtr fn_polyline (AtomPtr node, AtomPtr env) {
		AtomPtr p = type_check (node->tail.at(0), OBJECT);
		if (p->obj == 0) return make_node(0);
		if (p->lexeme == "svgdoc") {
			Document* doc = (Document*) p->obj;
			Real width = doc->get_layout ().dimensions.width - 60;
			Real height = doc->get_layout ().dimensions.height - 160;
			
			AtomPtr pl = type_check (node->tail.at (1), NUMERIC);
			if (!pl->val.size ()) return make_node(0);
			AtomPtr color = type_check (node->tail.at (2), NUMERIC);
			if (color->val.size () != 3) return make_node(0);
			
			Real hstep = ((Real) width / pl->val.size ());

			LineChart chart (30.);
			Polyline polyline (Stroke (.5, Color (color->val[0],
				color->val[1], color->val[2])));
			Real min = pl->val[0];
			Real max = pl->val[0];
			for (unsigned i = 0; i < pl->val.size (); ++i) {
				if (pl->val[i] > max) max = pl->val[i];
				if (pl->val[i] < min) min = pl->val[i];
			}
			Real delta = fabs (max - min);
			for (unsigned i = 0; i < pl->val.size (); ++i) {
				polyline << Point ((hstep * i), 
					((pl->val[i] - min) * height) / delta);
			}
			chart << polyline;
			*doc << chart;
			return make_node(1);
		}
		return make_node(0);
	}
	AtomPtr fn_scatter (AtomPtr node, AtomPtr env) {
		AtomPtr p = type_check (node->tail.at(0), OBJECT);
		if (p->obj == 0) return make_node(0);
		if (p->lexeme == "svgdoc") {
			Document* doc = (Document*) p->obj;
			Real width = doc->get_layout ().dimensions.width - 60;
			Real height = doc->get_layout ().dimensions.height - 160;
				
			AtomPtr x = type_check (node->tail.at (1), NUMERIC);
			if (!x->val.size ()) return make_node(0);
			AtomPtr y = type_check (node->tail.at (2), NUMERIC);
			if (!y->val.size ()) return make_node(0);	
			if (x->val.size () != y->val.size ()) return make_node (0);	

			AtomPtr colors = make_node ();
			AtomPtr tmp = node->tail.at (3);
			if (tmp->type == NUMERIC) {
				if (tmp->val.size () != 3) return make_node(0);
				for (unsigned i = 0; i < x->val.size (); ++i) {
					colors->tail.push_back (tmp);
				}
			} else if (tmp->type == LIST) {
				if (tmp->tail.size () != x->val.size ()) return make_node (0);
				colors = tmp;
			} else Context::error ("[scatter] invalid list of colors", node);

			Real minx = x->val[0];
			Real maxx = x->val[0];
			Real miny = y->val[0];
			Real maxy = y->val[0];		
			for (unsigned i = 0; i < x->val.size (); ++i) {
				Real xval = x->val[i];
				if (xval > maxx) maxx = xval;
				if (xval < minx) minx = xval;
				Real yval = y->val[i];
				if (yval > maxy) maxy = yval;
				if (yval < miny) miny = yval;
			}
			Real deltax = fabs (maxx - minx);
			Real deltay = fabs (maxy - miny);

			for (unsigned i = 0; i < x->val.size (); ++i) {
				// polyline << Point ((x->tail.at (i)->value - minx) * width / deltax, 
				// 	(y->tail.at (i)->value - miny) * height / deltay);
				AtomPtr color = type_check (colors->tail.at (i), NUMERIC);
				if (color->val.size () != 3) return make_node(0);
				*doc << Circle(Point (
					((x->val[i] - minx) * width / deltax) + 30, 
					((y->val[i] - miny) * height / deltay) + 30), 
					2, Fill(Color(color->val[0], color->val[1], 
						color->val[2])), Stroke(1, Color(color->val[0], 
						color->val[1], color->val[2])));
			}
			return make_node (1);
		}
		return make_node(0);
	}
	AtomPtr fn_grid (AtomPtr node, AtomPtr env) {
		AtomPtr p = node->tail.at(0);
		if (p->obj == 0) return make_node(0);
		if (p->lexeme == "svgdoc") {
			Document* doc = (Document*) p->obj;
			Real width = doc->get_layout ().dimensions.width - 60;
			Real height = doc->get_layout ().dimensions.height - 160;

			if (node->tail.size () == 5) { 
				AtomPtr i1 = type_check (node->tail.at (1), NUMERIC);
				AtomPtr i2 = type_check (node->tail.at (2), NUMERIC);
				AtomPtr i3 = type_check (node->tail.at (3), NUMERIC);
				AtomPtr i4 = type_check (node->tail.at (4), NUMERIC);
				if (i1->val.size () != 1 || i2->val.size () != 1 ||
					i3->val.size () != 1 || i4->val.size () != 1) return make_node (0);

				std::stringstream v;
				v << std::defaultfloat << i1->val[0]; 
				*doc << Text(Point(30, 20), v.str (), Color::Black, Font(6, "Verdana"));
				std::stringstream vh;
				vh << std::defaultfloat << (i2->val[0] - i1->val[0]) / 2;  
				*doc << Text(Point(width / 2 + 30, 20), vh.str(), Color::Black, Font(6, "Verdana"));
				std::stringstream ve;
				ve << std::defaultfloat << i2->val[0];
				*doc << Text(Point(width, 20), ve.str(), Color::Black, Font(6, "Verdana"));

				std::stringstream v1;
				v1 << std::defaultfloat << std::defaultfloat << i3->val[0]; ;
				*doc << Text(Point(2, 35), v1.str(), Color::Black, Font(6, "Verdana"));
				std::stringstream v2;
				v2 << std::defaultfloat << i4->val[0]; ;
				*doc << Text(Point(2, height + 30), v2.str(), Color::Black, Font(6, "Verdana"));		
			}
			Line l1 (Point(width / 2 + 30, 30), Point(width / 2 + 30, height + 30), Stroke (1, Color::Black));
			*doc << l1;			
			Line l2 (Point(30, height / 2 + 30), Point(30 + width, height / 2 + 30), Stroke (1, Color::Black));
			*doc << l2;	

			return  make_node(1);
		}
		return  make_node(0);
	}
	AtomPtr fn_title (AtomPtr node, AtomPtr env) {
		AtomPtr p = node->tail.at(0);
		if (p->obj == 0) return make_node(0);
		if (p->lexeme == "svgdoc") {
			Document* doc = (Document*) p->obj;
			Real height = doc->get_layout ().dimensions.height - 160;
			std::string tit = type_check (node->tail.at (1), STRING)->lexeme;

			*doc << Text(Point(30, height + 100), tit,
				Color::Black, Font(24, "Verdana"));
			return make_node(1);				
		}
		return make_node(0);		
	}
	AtomPtr fn_legend (AtomPtr node, AtomPtr env) {
		AtomPtr p = node->tail.at(0);
		if (p->obj == 0) return make_node(0);
		if (p->lexeme == "svgdoc") {
			Document* doc = (Document*) p->obj;
			Real width = doc->get_layout ().dimensions.width - 60;
			Real height = doc->get_layout ().dimensions.height - 160;
			node->tail.pop_front();
			for (unsigned i = 0; i < node->tail.size () / 2; ++i) {
				AtomPtr item = type_check (node->tail.at(i * 2), STRING);
				AtomPtr color = type_check (node->tail.at (i * 2 + 1), NUMERIC);			
				int x = width - 30; int y =  height + 100 - (10 * i);
				if (i >= 5) {
					x = 3 * width / 4; y = height + 100 - (10 * (i - 5));
				}
				if (i >= 10) return make_node(0);

				if (color->val.size () != 3) return make_node(0);
				*doc << Text (Point (x, y), 
					item->lexeme, 
					Color (color->val[0],
					color->val[1], color->val[2]), 
					Font (8, "Verdana"));
			}
			return make_node(1);
		} 
		return make_node(0);
	}
	AtomPtr fn_closesvg (AtomPtr node, AtomPtr env) {
		AtomPtr p = node->tail.at(0);
		if (p->obj == 0) return make_node(0);
		if (p->lexeme == "svgdoc") {
			Document* doc = (Document*) p->obj;
			doc->save ();
			delete doc;
			p->obj = 0;
			return make_node(1);
		} 
		return make_node(0);
	}
	AtomPtr add_plotting (AtomPtr env) {
		add_operator ("opensvg", fn_opensvg, 3, env);
		add_operator ("polyline", fn_polyline, 3, env);
		add_operator ("scatter", fn_scatter, 4, env);
		add_operator ("closesvg", fn_closesvg, 1, env);
		add_operator ("grid", fn_grid, 1, env);
		add_operator ("title", fn_title, 2, env);
		add_operator ("legend", fn_legend, 2, env);
		return env;
	}
}

// eof
