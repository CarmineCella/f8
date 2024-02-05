// plotting.cpp -- sound library for f8
//

#include "f8.h"
#include "plotting/svg_tools.h"

#include <iomanip>

namespace f8 {
	AtomPtr fn_opensvg (AtomPtr node, AtomPtr env) {
		std::string name = type_check (node->tail.at (0), STRING)->lexeme;
		int width = (int) type_check (node->tail.at (1), NUMBER)->val;
		int height = (int) type_check (node->tail.at (2), NUMBER)->val;

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
		if (p->obj == 0) return make_node();
		if (p->lexeme == "svgdoc") {
			Document* doc = (Document*) p->obj;
			Real width = doc->get_layout ().dimensions.width - 60;
			Real height = doc->get_layout ().dimensions.height - 160;
			
			AtomPtr pl = node->tail.at (1);
			if (!pl->tail.size ()) return make_node(0);
			AtomPtr color = node->tail.at (2);
			if (color->tail.size () != 3) return make_node(0);
			
			Real hstep = ((Real) width / pl->tail.size ());

			LineChart chart (30.);
			Polyline polyline (Stroke (.5, Color (color->tail.at (0)->val,
				color->tail.at (1)->val, color->tail.at (2)->val)));
			Real min = pl->tail.at (0)->val;
			Real max = pl->tail.at (0)->val;
			for (auto m : pl->tail) {
				if (m->val > max) max = m->val;
				if (m->val < min) min = m->val;
			}
			Real delta = fabs (max - min);
			for (unsigned i = 0; i < pl->tail.size (); ++i) {
				polyline << Point ((hstep * i), 
					((pl->tail.at (i)->val - min) * height) / delta);
			}
			chart << polyline;
			*doc << chart;
		}

		return make_node(1);
	}
	AtomPtr fn_scatter (AtomPtr node, AtomPtr env) {
		AtomPtr p = node->tail.at(0);
		if (p->obj == 0) return make_node();
		if (p->lexeme == "svgdoc") {
			Document* doc = (Document*) p->obj;
			Real width = doc->get_layout ().dimensions.width - 60;
			Real height = doc->get_layout ().dimensions.height - 160;
				
			AtomPtr x = node->tail.at (1);
			if (!x->tail.size ()) return make_node(0);
			AtomPtr y = node->tail.at (2);
			if (!y->tail.size ()) return make_node(0);	
			if (x->tail.size () != y->tail.size ()) return make_node (0);	
			AtomPtr color = node->tail.at (3);
			if (color->tail.size () != 3) return make_node(0);

			Real minx = x->tail.at (0)->val;
			Real maxx = x->tail.at (0)->val;
			Real miny = y->tail.at (0)->val;
			Real maxy = y->tail.at (0)->val;		
			for (unsigned i = 0; i < x->tail.size (); ++i) {
				Real xval = x->tail.at(i)->val;
				if (xval > maxx) maxx = xval;
				if (xval < minx) minx = xval;
				Real yval = y->tail.at(i)->val;
				if (yval > maxy) maxy = yval;
				if (yval < miny) miny = yval;
			}
			Real deltax = fabs (maxx - minx);
			Real deltay = fabs (maxy - miny);

			for (unsigned i = 0; i < x->tail.size (); ++i) {
				// polyline << Point ((x->tail.at (i)->value - minx) * width / deltax, 
				// 	(y->tail.at (i)->value - miny) * height / deltay);
				*doc << Circle(Point (
					((x->tail.at (i)->val - minx) * width / deltax) + 30, 
					((y->tail.at (i)->val - miny) * height / deltay) + 30), 
					2, Fill(Color(color->tail.at (0)->val, color->tail.at (1)->val, 
						color->tail.at (2)->val)), Stroke(1, Color(color->tail.at (0)->val, 
						color->tail.at (1)->val, color->tail.at (2)->val)));
			}
		}
		return make_node(1);
	}
	AtomPtr fn_grid (AtomPtr node, AtomPtr env) {
		AtomPtr p = node->tail.at(0);
		if (p->obj == 0) return make_node(0);
		if (p->lexeme == "svgdoc") {
			Document* doc = (Document*) p->obj;
			Real width = doc->get_layout ().dimensions.width - 60;
			Real height = doc->get_layout ().dimensions.height - 160;

			std::stringstream v;
			v << std::defaultfloat << node->tail.at (1)->val; 
			*doc << Text(Point(30, 20), v.str (), Color::Black, Font(6, "Verdana"));
			std::stringstream vh;
			vh << std::defaultfloat << (node->tail.at (2)->val - node->tail.at (1)->val) / 2;  
			*doc << Text(Point(width / 2 + 30, 20), vh.str(), Color::Black, Font(6, "Verdana"));
			std::stringstream ve;
			ve << std::defaultfloat << node->tail.at (2)->val;
			*doc << Text(Point(width, 20), ve.str(), Color::Black, Font(6, "Verdana"));
			Line l1 (Point(width / 2 + 30, 30), Point(width / 2 + 30, height + 30), Stroke (1, Color::Black));
			*doc << l1;	

			std::stringstream v1;
			v1 << std::defaultfloat << std::defaultfloat << node->tail.at (3)->val; ;
			*doc << Text(Point(2, 35), v1.str(), Color::Black, Font(6, "Verdana"));
			std::stringstream v2;
			v2 << std::defaultfloat << node->tail.at (4)->val; ;
			*doc << Text(Point(2, height + 30), v2.str(), Color::Black, Font(6, "Verdana"));		
			Line l2 (Point(30, height / 2 + 30), Point(30 + width, height / 2 + 30), Stroke (1, Color::Black));
			*doc << l2;	
		}
		return  make_node(1);
	}
	AtomPtr fn_title (AtomPtr node, AtomPtr env) {
		AtomPtr p = node->tail.at(0);
		if (p->obj == 0) return make_node(0);
		if (p->lexeme == "svgdoc") {
			Document* doc = (Document*) p->obj;
			Real height = doc->get_layout ().dimensions.height - 160;
			std::string tit = node->tail.at (1)->lexeme;

			*doc << Text(Point(30, height + 100), tit,
				Color::Black, Font(24, "Verdana"));
		}
		return make_node(1);
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
				AtomPtr item = node->tail.at(i * 2);
				AtomPtr color = node->tail.at (i * 2 + 1);			
				int x = width - 30; int y =  height + 100 - (10 * i);
				if (i >= 5) {
					x = 3 * width / 4; y = height + 100 - (10 * (i - 5));
				}
				if (i >= 10) return make_node(0);

				if (color->tail.size () != 3) return make_node(0);
				*doc << Text (Point (x, y), 
					item->lexeme, 
					Color (color->tail.at (0)->val,
					color->tail.at (1)->val, color->tail.at (2)->val), 
					Font (8, "Verdana"));
			}

		} else return make_node(0);

		return make_node(1);
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
		} else return make_node(0);
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
