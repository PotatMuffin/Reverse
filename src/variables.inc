memory_start dq ?
memory_pointer dq ? ; memory_pointer grows up toward memory_end
memory_text_pointer dq ? ; memory_text pointer grows down towards memory_start
memory_end dq ?
memory_setting dq ? 

source rq 1
output rq 1
outputfd rq 1

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
_generating db "Generating assembly",10,0

_parenl db "'('",0
_parenr db "')'",0
parenl db "(",0
parenr db ")",0
brackl db "[",0
brackr db "]",0
_int db 'int',0
_ident db "identifier",0
_var db "'var'",0
newline db 10,0
space db ' ',0
comma db ',',0
quote db "'",0
suffix db ".",10,0
_or db 'or',0
_min db '-',0

var db "var",0

keywords:
   dq var
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
alfa db "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_",0
alfanumeric db "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789",0
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
TT_ANGBRACKL = 5 ; <
TT_ANGBRACKR = 6 ; >
TT_BSLASH    = 256 ; \
TT_PARENL    = 257 ; (
TT_PARENR    = 258 ; )
TT_BRACEL    = 259 ; {
TT_BRACER    = 260 ; }
TT_BRACKL    = 261 ; [
TT_BRACKR    = 262 ; ]
TT_EQ        = 263 ; =
TT_INTLIT    = 512
TT_IDENT     = 768
TT_KEYWORD   = 769
TT_NEWLINE   = 1024 ; \n

op_prec db 0, 1, 1, 2, 2
   ;          +  -  *  /

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
; }
node_vardef_size = 18

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

node_count  = 7
NODE_ROOT   = 1
NODE_BINOP  = 2
NODE_LIT    = 3
NODE_UNOP   = 4
NODE_VARDEF = 5
NODE_VARASS = 6
NODE_VAR    = 7

; struct symbol
; {
;   char* name
;   int pos
; }

symbol_size = 12
symbol_table_size = 12

_rax db 'rax',0
_eax db 'eax',0
_ax db 'ax',0,0
_al db 'al',0,0

_rbx db 'rbx',0
_ebx db 'ebx',0
_bx db 'bx',0,0
_bl db 'bl',0,0

_rcx db 'rcx',0
_ecx db 'ecx',0
_cx db 'cx',0,0
_cl db 'cl',0,0

_rdx db 'rdx',0
_edx db 'edx',0
_dx db 'dx',0,0
_dl db 'dl',0,0

_rsi db 'rsi',0
_esi db 'esi',0
_si db 'si',0,0
_sil db 'sil',0

_rdi db 'rdi',0
_edi db 'edi',0
_di db 'di',0,0
_dil db 'dil',0

_rsp db 'rsp',0
_esp db 'esp',0
_sp db 'sp',0,0
_spl db 'spl',0

_rbp db 'rbp',0
_ebp db 'ebp',0
_bp db 'bp',0,0
_bpl db 'bpl',0

_mov db 'mov ',0
_add db 'add ',0
_sub db 'sub ',0
_imul db 'imul ',0
_cqo db 'cqo',0
_idiv db 'idiv ',0
_neg db 'neg ',0

_qword db "qword ",0
_dword db "dword ",0
_word db "word ",0
_byte db "byte ",0

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
      db 'syscall',0