outofmemory db "Out of memory",0
filenotfound db "Source file not found",0
illegalchar db "Illegal Char '",0
syntaxerrorexpected db "Expected ", 0
syntaxerrortoken db "Token cannot appear after previous token",0
iwfnotallowed db "if, while, and for are not allowed here",0
unimplementednode db "Encountered unimplemented node type: ",0
unimplementedop db "Encountered unimplemented operator",0
varprefix db "Variable '",0
vardefined db "' is already defined",0
varnotdefined db "' is not defined",0
cantdefvoid db "Can't define a variable with type void",0
expectedvar db "Expected a variable",0
expectedptr db "Can only dereference a pointer",0
typemismatch db "Type mismatch",0
typemismatchparam db "type mismatch with parameter '",0
illegalcast db "Illegal cast",0
isnottype db "' is not a type",0
locnotreg db "Given location is not a register",0
locnotmem db "Given location is not a memory address",0
bugincom db "This is probably a bug in the compiler",0
missingtermchar db "Missing terminating character",0 
invalidstr db "Invalid string",0
laststmtreturn db "Last statement in a function must be return",0
cantretoutfun db "Cant return outside of a function",0
isnotfunc db "' is not a function",0
expects db "' expects ",0
parambutgot db " parameters but got ",0
cantinfertype db "Cant infer type of struct",0
invalidconstruct db "Invalid type for constructor",0
unreachable db "Unreachable",10,0