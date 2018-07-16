This is experimental plugin for KDevelop that shows information about source code inline with the code.

It currently needs patched KDevelop and patched ktexteditor. It is very hacky at the moment because not all information that it needs are yet available in the KDevelop DUchain, so it parses on its own in dirty ways.

Features supported at the moment:

 * Showing function argument names at call site.
 * Showing default values of function arguments in function definition and at call site.
 * Showing the actual type of `auto` variables.
 * Showing visualisation of struct field sizes and paddings.
 * Showing value of enum constants.
