HOW TO GENERATE THE LEXER AND THE PARSER

1. To generate the lexer, move the file
"lexer.l" from the source/packages/aklabeth
folder into source/packages/ariadne/compiler/grammars
and execute makescan.bat. Then copy the generated
"aklabeth_lexer.cpp" back to source/packages/aklabeth,
as well as "lexer.l"

To make "aklabeth_lexer.cpp"compile, remove the
"include <unistd.h>" and insert these three lines
at the top of the file:

#include "code.h"
BEGIN_CODE_NAMESPACE
#define YY_SKIP_YYWRAP

2. To generate the parser, execute makegram.bat