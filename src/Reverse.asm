format ELF64 executable

segment readable executable
include "system.inc"
include "tokenizer.inc"
include "parser.inc"
include "symboltable.inc"
include "validator.inc"
include "generator.inc"

entry $
check_param_count:
    mov rax, [rsp]
    cmp rax, 2
    jge get_params

    printmany usage_suffix, [rsp + 8], usage
    exit_process 1

get_params:
    mov rbx, [rsp + 16]
    mov [source], rbx

    cmp rax, 3
    jl L7
    mov rbx, [rsp + 24]
    mov [output], rbx
    jmp main
    L7:
    mov [output], outfilename

main:
    mov rbp, rsp
    sub rsp, 40
    call init_memory
    
    open [source], O_RDONLY, 0
    mov [rbp - 8], rax
    
    cmp rax, 0
    jl file_error

    mov rdi, [rbp - 8] 
    call read_file
    mov [rbp - 16], rax

    close [rbp - 8]

    mov rdi, LOGINFO
    mov rsi, _tokenizing
    call log

    mov rdi, [rbp - 16]
    call tokenize
    mov [rbp - 24], rax

    mov rdi, LOGINFO
    mov rsi, _parsing
    call log

    lea rdi, [rbp - 24]
    mov si, TT_EOF
    call parse
    mov [rbp - 32], rax

    mov rdi, LOGINFO
    mov rsi, _validating
    call log

    symbol_table 10000h
    mov [rbp - 40], rax

    mov rdi, [rbp - 32]
    mov rsi, [rbp - 40]
    call validate_prog

    mov rdi, LOGINFO
    mov rsi, _generating
    call log

    mov rdi, [rbp - 32]
    mov rsi, [rbp - 40]
    call generate

    exit_process 0

segment readable writeable
include "variables.inc"
include "errors.inc"