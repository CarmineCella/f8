 # The f8 (fate) reference manual<br>
 ## Overview
 This manual is a of the *f8* runtime system. It is intended to be a reference document for programmers. It does not describe how to run *f8* or how to interact with it.<
 This chapter summarizes the semantics of *f8*, and explains the syntactic and lexical conventions of the language. Subsequent chapters describe special forms, built-in operators and the standard library.<br>
 
 These are the significant semantic characteristics of the *f8* language:
 - Types are latent: *f8* associates types with values (or objects) rather than with variables.
 - Proper tail recursion: *f8* is properly tail-recursive, which means that iterative computation can occur in constant space, even if the iterative computation is described by a syntactically recursive procedure; with a tail-recursive implementation, you can express iteration using the ordinary procedure-call mechanics; special iteration expressions are provided only for syntactic convenience.
 - Procedures are primary objects: *f8* procedures are objects, which means that you can create them dynamically, store them in data structures, return them as the results of other procedures, and so on.
 - Procedures can be both statically or dynamically scoped: with static (lexical) scoping, the structure of the program source code determines what variables you are referring to; with dynamic scoping, the runtime state of the program stack determines what variable you are referring to. 
 - Arguments are passed by value: the argument expressions are evaluated before a procedure gains control, whether or not the procedure needs the result of the evaluations (an exception to this rule is given by the *special forms*).<br>
 
 The syntax of *f8* is a mix between Scheme and TCL and is very simple. An important consequence of this simplicity is the susceptibility of *f8* programs and data to uniform treatment by other *f8* programs (homoiconicity). As with other Scheme dialects, the read primitive parses its input; that is, it performs syntactic as well as lexical decomposition of what it reads.<br>
 
 ## Basic concepts
 ### Variable bindings
 Any identifier may be used as a variable. A variable may name a location where a value can be stored. 
 A variable that does so is said to be bound to the location. The value stored in the location to which a variable is bound is called the variable’s value.
 Referencing an unassigned variable is an erro of type `unbound identifier`. The set of all currently bounded variables is called an *enviroment*. 
 The current enviroment can be visualized using the expression `info 'vars`.
 
 ### Static vs dynamic scope
 Procedures in *f8* can be both statically and dynamically scoped. In statically-scoped procedures the environment that is extended (and becomes current) 
 when a procedure is called is the environment in which the procedure was created. On the other hand, dynamically-scoped procedures the environment 
 that is extended is the one in which the procedure’s defining lambda expression was evaluated.

 ### True and false
 In *f8*, the boolean values true and false are denoted by `true` and `false`. However, any value can be treated as a boolean for the purpose of a conditional test. In conditional tests, all values count as true except for `false`, which counts as false.
 ## Types
 The valid types are: number, symbol, string, list, operator, lambda, object
 ## Built-in operators
 `quote` *`expr`* <br>
 The quote operator indicates literal data; it suppresses evaluation.
