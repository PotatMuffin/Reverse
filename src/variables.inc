memory_start dq ?
memory_end dq ?
memory_pointer dq ?
memory_setting dq ? 

source rq 1
output rq 1

stat rb 144
usage_suffix db "Usage: ", 0
usage db " <source> [output]", 10, 0

_parenl db '(',0
_parenr db ')',0
_int db 'int',0
space db ' ',0
comma db ',',0
quote db "'",0
suffix db ".",10,0
lineprefix db "[",0
linesuffix db "] ",0
_or db 'or',0

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
TT_INTLIT    = 512
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

; struct node_bin_expr
; {
;   short type
;   void* left
;   token* op
;   void* right
; }
node_bin_expr_size = 26

; struct node_lit
; {
;   short type
;   token* literal
; }
node_lit_size = 10

NODE_ROOT    = 1
NODE_BINEXPR = 2
NODE_LIT     = 3