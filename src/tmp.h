AtomPtr read_block (std::istream& in);
AtomPtr read_line (std::istream& in) {
    AtomPtr l = make_node ();
    while (!in.eof ()) {
        std::string token = next (in);
        if (token == "\n") {
            if (l->tail.size ()) break;
            else continue;
        } else if (token == "(") {
            AtomPtr block = read_block (in);
            if (block->tail.size () > 1) l->tail.push_back (block);
            else {
                for (unsigned i = 0; i < block->tail.size (); ++i) {
                    if (!is_nil (block->tail.at (i))) l->tail.push_back (block->tail.at (i));
                }
            }
            return l;
        } else if (is_number (token)) {
            l->tail.push_back (make_node (atof (token.c_str ())));
        }  else {
            if (token.size ()) {
                l->tail.push_back  (make_node (token));
                if (token ==")") break;
            }
            else continue;
        }
    }
    return l;
}
AtomPtr read_block (std::istream& in) {
    AtomPtr l = make_node();
    while (!in.eof ()) {
        AtomPtr a = read_line (in);
        if (a->tail.size () == 0) continue;
        AtomPtr last = a->tail.at (a->tail.size () -1);
        if (last->lexeme == ")" && last->type != STRING) {
            a->tail.pop_back ();
            l->tail.push_back (a);
            break;
        }
        l->tail.push_back (a);
    }
    return l;
}

