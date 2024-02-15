# f8 (fate)


### Introduction

The *f8 (fate)* scripting language is a hybrid dialect of Lisp, Scheme and TCL; for more info, please see the [genealogy](docs/f8_anchestors.png) and the [reference manual (in progress)](docs/reference.md).

It has been designed to be small, easy to expand and easy to embed in host applications. 
The core of the language is made of a single [C++ header](src/core.h) (~ 1000 loc in total) and a  [small library](src/stdlib.tcl) written using *f8* itself.

*f8* includes several neat features, among which:

* homoiconicity and introspection
* tail recursion
* static and dynamic scoping
* partial evaluation
* lambda functions with closures

It can [bootstrap](examples/repl.tcl) itself. For an overview on the language, please see [this example](examples/overview.tcl); for an example on how to integrate the language in your application, please check the [main file](src/f8.cpp).


### Processing libraries

*f8* comes with several libraries, dedicated to signal processing and machine learning. Among the functionalities there are:

* [fft/ifft](examples/cross_synth.tcl)
* [convolution](examples/reverb.tcl)
* sound decomposition
* clustering and [classification](examples/knn_class.tcl)
* basic neural networks
* [plotting](examples/plot1.tcl)

It also comes with a system library that provides several functionalities, including [threading and sockets](examples/threads_sockets.tcl), and it can also send [OSC messages](examples/osc_like.tcl).

### Building

To compile, from *root* folder type:

`mkdir build`

`cd build`

`cmake ..`

`make`


### Licensing

The f8 language is released under the [BSD 2-Clause license](LICENSE.md).

(c) 2024 by Carmine-Emanuele Cella

