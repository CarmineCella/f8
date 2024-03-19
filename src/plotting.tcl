# --------------------------------
# The f8 (fate) scripting language
# --------------------------------
#
# Machine learning library
#
# (c) www.carminecella.com
#

set Aqua (array '(0 255 255))
set Black (array '(0 0 0))
set Blue (array '(0 0 255))
set Brown (array '(165 42 42))
set Cyan (array '(0 255 255))
set Fuchsia (array '(255 0 255))
set Green (array '(0 128 0))
set Lime (array '(0 255 0))
set Magenta (array '(255 0 255))
set Orange (array '(255 165 0))
set Purple (array '(128 0 128))
set Red (array '(255 0 0))
set Silver (array '(192 192 192))
set White (array '(255 255 255))
set Yellow (array '(255 255 0))
set Gray (array '(127 127 127))
proc (plot name x) {
	set a (opensvg name 512 512)
	polyline a x Blue
	closesvg a
}
proc (scatplot name x y) {
	set a (opensvg name 512 512)
	scatter a x y Blue
	closesvg a
}

# eof

