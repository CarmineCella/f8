# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Machine learning library
#
# (c) www.carminecella.com
#

proc (getcolumn data n) {
    set w ()
    set sz (llength data)
    set i 0
    while (< i sz) {
        lappend w (lindex (lindex data i) n)
        = i (+ i 1)
    }
    = w (array w)
}
proc (accuracy classes gt) {
    set total (llength classes)
    set i 0
    set ct 0
    while (< i total) {
        if (eq (lindex classes i) (llast (lindex gt i))) (= ct (+ ct 1))
        = i (+ i 1)
    }
    / ct total
}
proc (make-dataset data) {
    set Xy ()
    set i 0
    set samples (llength data)
    set features (- (llength (car data)) 1)
    while (< i samples) {
        set item (lindex data i)
        set f (array (lrange item 0 features))
        set l (car (lrange item features 1))
        lappend Xy (list f l)
        = i (+ i 1)
    }
    set Xy
}
proc (matsize m) {
	set sz (list (cols m) (rows m))
}
proc (mpdecomp sig t components db) {
    set decomp ()
    set c 0
    set residual sig
    while (< c components) {
        set max_i 0
        set max_abs 0
        set max_dot 0
        set n 0
        while (< n (llength db)) {
            set item (lindex db n);
            set atom (car item)
            set len (size atom)
            set d (dot (slice sig t len) atom)
            if (> (abs d) max_abs) {
                = max_abs (abs d)
                = max_dot d
                = max_i n
            }
        }
        set m (car (lindex db max_i))
        = m (* m max_dot)
        assign residual t (size m) (- (slice residual t (size m) m))
        lappend decomp (list t max_i max_dot)
    }
}
proc (mdrebuild decomp db) {
    set sig ()
}

# eof

