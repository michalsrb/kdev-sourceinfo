This is experimental plugin for KDevelop that shows information about source code inline with the code. It is very hacky at the moment because not all information that it needs are yet available in the KDevelop DUchain, so it parses on its own in dirty ways.

Required KDevelop version: Current git master - will be probably released as 5.2.5. Required patches:

 * https://cgit.kde.org/kdevelop.git/commit/?id=6b146066a4916a7703a55476945cf39137aebd77
 * https://cgit.kde.org/kdevelop.git/commit/?id=c0e3d5248aa6983d16469ae563813d38378d1023

Required KDE Frameworks version: Current git master - will be released as 5.50.0. Required patches:
 
 * https://cgit.kde.org/ktexteditor.git/commit/?id=4ea5fee0afe5c76bbee07563c23ede808aa059de

Features supported at the moment:

 * Showing function argument names at call site.
 * Showing default values of function arguments in function definition and at call site.
 * Showing the actual type of `auto` variables.
 * Showing visualisation of struct field sizes and paddings.
 * Showing value of enum constants.
