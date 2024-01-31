format ELF64

section '.text' executable
public _start
public read_file
public tokenize

include "system.inc"
include "tokenizer.inc"
include "parser.inc"
_start:

check_param_count:
    mov rax, [rsp]
    cmp rax, 2
    jge get_params

    printmany usage_suffix, [rsp + 8], usage
    exit_process 1

get_params:
    mov rbx, [rsp + 16]
    mov [source], rbx

main:
    mov rbp, rsp
    sub rsp, 24

    call init_memory
    
    open [source], 0, 0
    mov [rbp - 8], rax
    
    cmp rax, 0
    jl file_error

    mov rdi, [rbp - 8] 
    call read_file
    mov [rbp - 16], rax

    close [rbp - 8]

    mov rdi, [rbp - 16]
    call tokenize
    mov [rbp - 24], rax
    
    mov rdi, [rbp - 24]
    call parse

    exit_process 0

section '.data' writeable
include "variables.inc"
include "errors.inc"