memory_start dq ?
memory_pointer dq ? ; memory_pointer grows up toward memory_end
memory_text_pointer dq ? ; memory_text pointer grows down towards memory_start
memory_end dq ?
memory_setting dq ? 
data_capacity dq ?
data_occupied dq ?

source   rq 1
output   rq 1
outputfd rq 1

if_label    dq 1
while_label dq 1
for_label   dq 1
str_label   dq 1
other_label dq 1

in_function db 0
pass db 0

builtin_types rq 5

stat rb 144
usage_suffix db "Usage: ", 0
usage db " <source> [output]", 10, 0
outfilename db "out.asm",0

LOGINFO  = 0
LOGNOTE  = 1
LOGWARN  = 2
LOGERROR = 3

log_message:
info db "[INFO] ",0,0
note db "[NOTE] ",0,0
WARN db "[WARN] ",0,0
ERROR db "[ERROR] ",0 

_tokenizing db "Tokenizing file",10,0
_parsing db "Parsing file",10,0
_validating db "Validating program",10,0
_generating db "Generating assembly",10,0

L db "L",0
_parenl db "'('",0
_parenr db "')'",0
_bracel db "'{'",0
_bracer db "'}'",0
parenl db "(",0
parenr db ")",0
brackl db "[",0
brackr db "]",0
_str db "string",0
_void db 'void',0
__byte db 'byte',0
_short db 'short',0
_int db 'int',0
_long db 'long',0
_ident db "identifier",0
_var db "'var'",0
_if db "'if'",0
_while db "'while'",0
_for db "'for'",0
_semi db "';'",0
newline db 10,0
space db ' ',0
comma db ', ',0
colon db ':',0
quote db "'",0
suffix db ".",10,0
_min db '-',0
_plus db '+',0
zero db '0',0
one db '1',0
_type db 'type',0
_func db 'func',0
_end db 'end',0
_struct db "'struct'",0

if_ db "if",0
else_ db "else",0
while_ db "while",0
for_ db "for",0
asm_ db "asm",0
return_ db "return",0
struct_ db "struct",0

keywords:
   dq if_, else_, while_, for_, asm_, return_, struct_, 0

types:
   dq _void
   dq __byte
   dq _short
   dq _int
   dq _long
   dq 0

default_memory_setting = 1024 * 1024 * 16
STDIN      = 0
STDOUT     = 1
STDERR     = 2

S_ISUID    = 4000o
S_ISGID    = 2000o
S_ISVTX    = 1000o
S_IRUSR    = 0400o
S_IWUSR    = 0200o
S_IXUSR    = 0100o
S_IRGRP    = 0040o
S_IWGRP    = 0020o
S_IXGRP    = 0010o
S_IROTH    = 0004o
S_IWOTH    = 0002o
S_IXOTH    = 0001o
default_open_mode = S_IRUSR + S_IWUSR + S_IRGRP + S_IWGRP + S_IROTH

O_RDONLY    = 0
O_WRONLY    = 1
O_RDWR      = 2
O_CREAT     = 64
O_NOCTTY    = 256
O_EXCL      = 128
O_TRUNC     = 512
O_APPEND    = 1024
O_NONBLOCK  = 2048
O_DSYNC     = 4096
O_ASYNC     = 8192
O_DIRECTORY = 16384
O_NOFOLLOW  = 32768
O_CLOEXEC   = 524288
O_SYNC      = 1052672
default_open_flags = O_WRONLY + O_CREAT + O_TRUNC

whitespaces db 9, 11, 12, 13, 32, 0
alpha db "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_",0
alphanumeric db "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789",0
digits db "0123456789",0

; struct token
;{
;   short type
;   long value
;   int line
;}
tok_len = 14

TT_EOF       = 0 
TT_PLUS      = 1 ; +
TT_MIN       = 2 ; -
TT_STAR      = 3 ; *
TT_FSLASH    = 4 ; /
TT_PERCENT   = 5 ; %
TT_ANGBRACKL = 6 ; <
TT_ANGBRACKR = 7 ; >
TT_GTE       = 8 ; >=
TT_LTE       = 9 ; =<
TT_IS        = 10 ; ==
TT_ISNT      = 11 ; !=
TT_LOGAND    = 12 ; &&
TT_LOGOR     = 13 ; ||
TT_AND       = 14 ; &
TT_OR        = 15 ; |
TT_XOR       = 16 ; ^
TT_SHL       = 17 ; <<
TT_SHR       = 18 ; >>
TT_EXCL      = 128 ; !
TT_NOT       = 129
TT_BSLASH    = 256 ; \
TT_PARENL    = 257 ; (
TT_PARENR    = 258 ; )
TT_BRACEL    = 259 ; {
TT_BRACER    = 260 ; }
TT_BRACKL    = 261 ; [
TT_BRACKR    = 262 ; ]
TT_EQ        = 263 ; =
TT_COMMA     = 264 ; ,
TT_INTLIT    = 512
TT_STRLIT    = 513
TT_IDENT     = 768
TT_KEYWORD   = 769
TT_NEWLINE   = 1024 ; \n
TT_NOEXIST   = 2048 

op_prec db 0, 4, 4, 5, 5, 5, 2, 2, 2, 2, 2, 2, 1, 1, 3, 3, 3, 2, 2
   ;          +  -  *  /  %  <  > >= =< == != && ||  &  |  ^ << >>

; struct node_root
; {
;   short type
;   void* children
;   long child_count
; }
node_root_size = 18

; struct node_binop
; {
;   short type
;   void* left
;   token* op
;   void* right
; }
node_binop_size = 26

; struct node_lit
; {
;   short type
;   token* literal
; }
node_lit_size = 10

; struct node_unop
; {
;   short type
;   token* op
;   void* node
; }
node_unop_size = 18

;struct node_vardef
; {
;   short type
;   void* value
;   token* name
;   type* vartype
; }
node_vardef_size = 26

; struct node_varass
; {
;   short type
;   void* value
;   token* name
; }
node_varass_size = 18

; struct node_var
; {
;   short type
;   token* name
; }
node_var_size = 10

; struct node_if
; {
;   short type
;   void* condition
;   void* scope
;   void* else_scope
; }
node_if_size = 26

; struct node_while
; {
;   short type
;   void* condition
;   void* scope
; }
node_while_size = 18

; struct node_for
; {
;   short type
;   void* on_entry
;   void* condition
;   void* on_loop
;   void* scope
; }
node_for_size = 34

; struct node_asm
; {
;   short type
;   token* asm
; }
node_asm_size = 10

; struct node_func_def
; {
;   short type
;   void* scope
;   token* name
;   void* params
;   long arity
;   type* return
; }
node_func_def_size = 42

; struct node_return
; {
;   short type
;   token* tok
;   void value*
; }
node_return_size = 18

; struct node_func_call
; {
;   short type
;   token* func_name
;   void params
;   int param_count
; }
node_func_call_size = 22

; struct node_ptr_ass
; {
;   short type
;   void* value
;   void* loc
; }
node_ptr_ass_size = 18

; struct node_cast
; {
;   short type
;   void* cast_type
;   void* castee
; }
node_cast_size = 18

; struct node_struct
; {
;   short type
;   token* name
;   void* fields
;   long field_count
; }
node_struct_size = 26

node_count     = 17
NODE_ROOT      = 1
NODE_BINOP     = 2
NODE_LIT       = 3
NODE_UNOP      = 4
NODE_VARDEF    = 5
NODE_VARASS    = 6
NODE_VAR       = 7
NODE_IF        = 8
NODE_WHILE     = 9
NODE_FOR       = 10
NODE_ASM       = 11
NODE_FUNC_DEF  = 12
NODE_RETURN    = 13
NODE_FUNC_CALL = 14
NODE_PTR_ASS   = 15
NODE_CAST      = 16
NODE_STRUCT    = 17

; struct type
; {
;   token* type_name
;   long ptr
; }
type_size = 16

; struct param
; {
;   token* name
;   type* type
; }
param_size = 16

; struct field
; {
;   token* name
;   type* type
;   int pos
; }
field_size = 20

; struct symbol
; {
;   char* name
;   void* data
; }
symbol_size = 16

; struct var
; {
;   short type
;   void* var_type
;   int pos
; }
var_size = 14

; struct builtin_type
; {
;   short type
;   int size
; }
builtin_type_size = 6

; struct ptr
; {
;   short type
;   int size
;   long ptr_type
;   void* _type
; }
ptr_size = 22

; struct func 
; {
;   short type
;   long arity
;   param* params
;   type* return
;   symbol_table* table
;   int num
; }
func_size = 38

; struct struct
; {
;   short type
;   void* fields
;   int sizeof
;   int num
; }
struct_size = 18

VAR          = 1
BUILTIN_TYPE = 2
_PTR         = 3
FUNC         = 4
STRUCT       = 5

symbol_table_size = 28

; struct context
; {
;   symbol_table* table
;   char* logop_true
;   char* logop_false
;   token* logop_token
;   mempos* mem_pos
; }
context_size = 40

; struct value
; {
;   short type
;   char* value
; }
immvalue_size = 10

; struct reg
; {
;   short type
;   int register
;   int size
; }
reg_size = 10

; struct mempos
; {
;   short type
;   int register
;   int size
;   int offset
; }
mempos_size = 14

; struct label
; {
;   short type
;   char* label
; }
label_size = 10

IMMVALUE = 1
REG      = 2
MEMPOS   = 3
_LABEL   = 4

qword_  = 0
dword_  = 1
word_   = 2
byte_   = 3
none_   = 4
custom_ = 5
sizes db 8, 4, 2, 1, 0 

rax_ = 0
_rax db 'rax',0,0
_eax db 'eax',0,0
_ax db 'ax',0,0,0
_al db 'al',0,0,0

rbx_ = 1
_rbx db 'rbx',0,0
_ebx db 'ebx',0,0
_bx db 'bx',0,0,0
_bl db 'bl',0,0,0

rcx_ = 2
_rcx db 'rcx',0,0
_ecx db 'ecx',0,0
_cx db 'cx',0,0,0
_cl db 'cl',0,0,0

rdx_ = 3
_rdx db 'rdx',0,0
_edx db 'edx',0,0
_dx db 'dx',0,0,0
_dl db 'dl',0,0,0

rsi_ = 4
_rsi db 'rsi',0,0
_esi db 'esi',0,0
_si db 'si',0,0,0
_sil db 'sil',0,0

rdi_ = 5
_rdi db 'rdi',0,0
_edi db 'edi',0,0
_di db 'di',0,0,0
_dil db 'dil',0,0

rsp_ = 6
_rsp db 'rsp',0,0
_esp db 'esp',0,0
_sp db 'sp',0,0,0
_spl db 'spl',0,0

rbp_ = 7
_rbp db 'rbp',0,0
_ebp db 'ebp',0,0
_bp db 'bp',0,0,0
_bpl db 'bpl',0,0

r8_ = 8
_r8 db 'r8',0,0,0
_r8d db 'r8d',0,0
_r8w db 'r8w',0,0
_r8b db 'r8b',0,0

r9_ = 9
_r9 db 'r9',0,0,0
_r9d db 'r9d',0,0
_r9w db 'r9w',0,0
_r9b db 'r9b',0,0

r10_ = 10
_r10 db 'r10',0,0
_r10d db 'r10d',0
_r10w db 'r10w',0
_r10b db 'r10b',0

r11_ = 11
_r11 db 'r11',0,0
_r11d db 'r11d',0
_r11w db 'r11w',0
_r11b db 'r11b',0

r12_ = 12
_r12 db 'r12',0,0
_r12d db 'r12d',0
_r12w db 'r12w',0
_r12b db 'r12b',0

r13_ = 13
_r13 db 'r13',0,0
_r13d db 'r13d',0
_r13w db 'r13w',0
_r13b db 'r13b',0

r14_ = 14
_r14 db 'r14',0,0
_r14d db 'r14d',0
_r14w db 'r14w',0
_r14b db 'r14b',0

r15_ = 15 
_r15 db 'r15',0,0
_r15d db 'r15d',0
_r15w db 'r15w',0
_r15b db 'r15b',0

mem_ = 16
_memend db 'memend',0
_mem db 'mem',0

param_regs dd rdi_, rsi_, rdx_, rcx_, r8_, r9_
param_save_regs dd r15_, r14_, r13_, r12_, rbx_

_mov db 'mov ',0
_movzx db 'movzx ',0
_movsx db 'movsx ',0
_movsxd db 'movsxd ',0
_push db 'push ',0
_pop db 'pop ',0
_add db 'add ',0
_sub db 'sub ',0
_and db 'and ',0
_or db 'or ',0
_xor db 'xor ',0
_not db 'not ',0
_shl db 'shl ',0
_shr db 'shr ',0
_sar db 'sar ',0
_imul db 'imul ',0
_cqo db 'cqo',0
_idiv db 'idiv ',0
_neg db 'neg ',0
_cmp db 'cmp ',0
_jmp db 'jmp ',0
_je db 'je ',0
_jne db 'jne ',0
_setg db 'setg ',0
_setge db 'setge ',0
_setl db 'setl ',0
_setle db 'setle ',0
_sete db 'sete ',0
_setne db 'setne ',0
_lea db 'lea ',0
_db db 'db ',0
_rb db 'rb ',0
_call db 'call ',0

_qword db "qword ",0
_dword db "dword ",0
_word db "word ",0,0
_byte db "byte ",0,0

_prologue db 'push rbp',10
          db 'mov rbp, rsp',10,0

_epilogue db 'leave',10
          db 'ret',10,0

_format db 'format ELF64 executable',10,0
_segment db 'segment ',0
_executable db 'readable executable',10,0
_writeable db 'readable writeable',10,0
_entry db 'entry $',10,0
_exit db 'mov rdi,0', 10
      db 'mov rax,60',10
      db 'syscall',10,0