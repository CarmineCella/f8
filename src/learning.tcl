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
        set max_n 0
        set max_abs 0
        set max_dot 0
        set max_atom 0        
        set n 0
        while (< n (llength db)) {
            set item (car (lindex db n))
            set atom (normal (/ (-> + item) (llength item)))
            set len (size atom)
            if (> len (+ t (size residual))) { 
                = len (- len (+ t (size residual))) 
                = atom (slice atom 0 len)
            }
            set d (/ (dot (slice residual t len) atom) len)
            if (> (abs d) max_abs) {
                = max_abs (abs d)
                = max_dot d
                = max_n n
                = max_atom atom
            }
            = n (+ 1 n)
        }         
        set m (* max_atom max_dot)
        set tmp residual
        assign residual (- (slice residual t (size m)) m) t (size m)
        lappend decomp (list t  max_n max_dot)
        = c (+ 1 c)
    }
    list decomp residual
}
proc (mprebuild decomp db) {
    set sig ()
    set maxchans 0
    set i 0
    while (< i (llength db)) {
        set item (car (lindex db i))
        if (< maxchans (llength item)) {
            = maxchans (llength item)
        }
        = i (+ 1 i)
    }

    = i 0
    set out (dup maxchans (zeros 1))
    while (< i (llength decomp)) {
        set d (lindex decomp i)
        set start (car d)
        set index (second d)
        set amp (third d)
        set sig (car (lindex db index))
        set j 0
        if (< (llength sig) maxchans) {
            = sig (dup maxchans (/ (-> + sig) maxchans))
        }
        while (< j maxchans) {
            set ch (lindex sig j)
            lset out (mix 0 (lindex out j) start (* ch amp)) j
            = j (+ 1 j)
        }
        = i (+ 1 i)
    }
    set out
}


# eof

