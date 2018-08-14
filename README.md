This is experimental plugin for KDevelop that shows information about source code inline with the code.

It currently needs patched KDevelop and patched ktexteditor. It is very hacky at the moment because not all information that it needs are yet available in the KDevelop DUchain, so it parses on its own in dirty ways.

Required KDevelop patches:

 * https://phabricator.kde.org/R32:248e34d1ece07b61298d95682de282f60d4ea11d
 * https://phabricator.kde.org/D14838

Required ktexteditor patches:
 
 * https://phabricator.kde.org/D12662 This will be probably replaced by alternative version.

Features supported at the moment:

 * Showing function argument names at call site.
 * Showing default values of function arguments in function definition and at call site.
 * Showing the actual type of `auto` variables.
 * Showing visualisation of struct field sizes and paddings.
 * Showing value of enum constants.
