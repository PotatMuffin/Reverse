macro read fd, buf, count
{
    mov rax, 0
    mov rdi, fd
    mov rsi, buf
    mov rdx, count
    syscall
}

macro write fd, buf, count
{
    mov rax, 1
    mov rdi, fd
    mov rsi, buf
    mov rdx, count
    syscall
}

macro open fn, flags, mode
{
    mov rax, 2
    mov rdi, fn
    mov rsi, flags
    mov rdx, mode
    syscall
}

macro close fd
{
    mov rax, 3
    mov rdi, fd
    syscall
}

macro fstat fd, buf
{
    mov rax, 5
    mov rdi, fd
    mov rsi, buf
    syscall
}

macro brk new_breakpoint
{
    mov rax, 12
    mov rdi, new_breakpoint
    syscall
}

macro exit_process code
{
    mov rax, 60
    mov rdi, code
    syscall
}

macro print_tok_pos token
{
    mov rdi, brackl
    call print
    mov rbx, token
    movsxd rdi, dword [rbx + 10]
    call display_number
    mov rdi, brackr
    call print
    mov rdi, space
    call print
}

macro pushmany [arg]
{
    forward
    push arg
}

macro printmany [arg]
{
    forward
    mov rdi, arg
    call print
}

macro fprintmany fd, [arg]
{
    forward
    mov rdi, fd
    mov rsi, arg
    call fprint
}

macro check_memory required
{
    mov rax, [memory_pointer]
    add rax, required
    cmp rax, [memory_text_pointer]
    jg no_memory_error
}

macro check_memory_text required
{
    mov rax, [memory_text_pointer]
    sub rax, required
    cmp rax, [memory_pointer]
    jl no_memory_error
}

macro linked_list_item item
{
    check_memory 24
    mov rax, [memory_pointer]
    mov rbx, item
    mov qword [rax], rbx ; data
    mov qword [rax + 8], 0 ; next
    mov qword [rax + 16], 0 ; previous
    add [memory_pointer], 24
}

macro symbol_table table_size
{
    mov rbx, table_size
    imul rbx, symbol_size
    add rbx, symbol_table_size
    check_memory rbx
    mov rax, [memory_pointer]
    mov qword [rax], table_size
    add [memory_pointer], rbx
}

macro node_root
{
    check_memory node_root_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_ROOT
    mov qword [rax + 2], 0
    mov qword [rax + 10], 0
    add [memory_pointer], node_root_size
}

macro node_binop left, op, right
{   
    check_memory node_binop_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_BINOP
    mov rbx, left
    mov qword [rax + 2], rbx
    mov rbx, op
    mov qword [rax + 10], rbx
    mov rbx, right
    mov qword [rax + 18], rbx
    add [memory_pointer], node_binop_size
}

macro node_lit token
{
    check_memory node_lit_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_LIT
    mov rbx, token
    mov qword [rax + 2], rbx
    add [memory_pointer], node_lit_size
}

macro node_unop op, node
{
    check_memory node_unop_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_UNOP
    mov rbx, op
    mov qword [rax + 2], rbx
    mov rbx, node
    mov qword [rax + 10], rbx
    add [memory_pointer], node_unop_size
}

macro node_vardef value, name
{
    check_memory node_vardef_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_VARDEF
    mov rbx, value
    mov qword [rax + 2], rbx
    mov rbx, name
    mov qword [rax + 10], rbx
    add [memory_pointer], node_vardef_size
}

macro node_varass value, name
{
    check_memory node_varass_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_VARASS
    mov rbx, value
    mov qword [rax + 2], rbx
    mov rbx, name
    mov qword [rax + 10], rbx
    add [memory_pointer], node_varass_size
}

macro node_var name
{
    check_memory node_var_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_VAR
    mov rbx, name
    mov qword [rax + 2], rbx
    add [memory_pointer], node_var_size
}

init_memory:
    brk 0
    mov [memory_start], rax
    mov [memory_pointer], rax

    mov rcx, [memory_setting]
    shl rcx, 10
    jnz allocate_memory
    mov rcx, default_memory_setting

    allocate_memory:
    lea rdi, [rax + rcx]
    brk rdi

    mov [memory_text_pointer], rax
    mov [memory_end], rax
    ret

read_file: ; char* read_file(int fd)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi

    fstat [rbp - 8], stat
    mov rbx, qword [stat + 48]
    inc rbx
    mov [rbp - 16], rbx
    mov rbx, [memory_pointer]
    mov [rbp - 24], rbx

    read [rbp - 8], [rbp - 24], [rbp - 16]

    mov rax, [rbp - 24]
    mov rbx, [rbp - 16]
    add qword [memory_pointer], rbx

    leave
    ret
fprint: ; void fprint(long fd, char* ptr)
    push rbp
    mov rbp, rsp
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    xor rdx, rdx

    fprintloopstart:
        mov al, byte [rsi + rdx]
        cmp al, 0 
        je fprintloopend

        inc rdx
        jmp fprintloopstart 

    fprintloopend:

    write [rbp - 8], [rbp - 16], rdx

    leave
    ret

log: ; void log(long level, char* message):
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    imul rbx, 10
    lea rdi, [log_message + rbx]
    call print
    mov rdi, [rbp - 16]
    call print

    leave
    ret

print: ; void print(char* ptr)
    xor rdx, rdx
    mov rsi, rdi

    printloopstart:

    mov al, byte [rsi + rdx]
    cmp al, 0
    je printloopend

    inc rdx
    jmp printloopstart

    printloopend:

    write STDOUT, rsi, rdx
    ret

display_number: ; void display_number(long number)
    push rbp
    mov rbp, rsp
    sub rsp, 29

    mov [rbp - 28], rdi
    cmp qword [rbp - 28], 0
    setl byte [rbp - 29]

    xor rbx, rbx
    mov rcx, rbp

    display_loop:
        mov rax, [rbp - 28]
        cqo
        mov r10, 10
        idiv r10
        mov [rbp - 28], rax

        inc rbx
        dec rcx

        cmp dl, 0
        jg L4
        neg dl

        L4:
        add dl, '0'
        mov [rcx], dl

        cmp qword [rbp - 28], 0
        jnz display_loop

    cmp byte [rbp - 29], 0
    jz display_print

    dec rcx
    inc rbx
    mov [rcx], byte '-'

    display_print:
    write STDOUT, rcx, rbx

    leave
    ret

itoa: ; char* iota(long num)
    push rbp
    mov rbp, rsp
    sub rsp, 17
    mov [rbp - 8], rdi
    check_memory 24
    add [memory_pointer], 24
    mov rbx, [memory_pointer]
    mov qword [rbp - 16], rbx

    cmp qword [rbp - 8], 0
    setl byte [rbp - 17]

    dec qword [rbp - 16]
    mov rbx, qword [rbp - 16]
    mov byte [rbx], 0

    itoaloop:
        mov rax, [rbp - 8]
        cqo
        mov r10, 10
        idiv r10
        mov [rbp - 8], rax

        cmp dl, 0
        jg L6
        neg dl
        L6:
        add dl, '0'
        dec qword [rbp - 16]
        mov rbx, [rbp - 16]
        mov byte [rbx], dl

        cmp qword [rbp - 8], 0
        jnz itoaloop

    cmp byte [rbp - 17], 0
    jz itoaRet

    dec qword [rbp - 16]
    mov rbx, [rbp - 16]
    mov byte [rbx], '-'

    itoaRet:
    mov rax, [rbp - 16]
    leave
    ret

contains_char: ; bool contains_char(char* source, char c)
    push rbp
    mov rbp, rsp
    sub rsp, 9
    mov [rbp - 8], rdi
    mov [rbp - 9], sil

    mov bl, [rbp - 9]
    xor rcx, rcx
    loopcontainsstart:
        mov rax, [rbp - 8]
        mov dl, [rax+rcx]

        cmp dl, 0
        je containsret

        cmp bl, dl
        sete al

        cmp al, 1
        je containsret

        inc rcx
        jmp loopcontainsstart

    containsret:
    leave
    ret

copy_chars: ; char* copy_chars(char* start, char* allowed)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov qword [rbp - 24], 0

    copyloop:
        mov rdi, [rbp - 16]
        mov rbx, [rbp - 8]
        mov rax, [rbp - 24]
        mov rsi, [rbx + rax]
        call contains_char

        inc qword [rbp - 24]

        cmp al, 1
        je copyloop

    check_memory_text [rbp - 24]
    mov rbx, [rbp - 24]
    sub [memory_text_pointer], rbx

    xor rcx, rcx
    copyloop2:
        mov rax, [rbp - 8]
        mov rbx, [memory_text_pointer]

        mov al, [rax + rcx]
        mov [rbx + rcx], al

        inc rcx

        cmp rcx, [rbp - 24]
        jl copyloop2

    mov rbx, [memory_text_pointer]
    mov byte [rbx + rcx - 1], 0

    mov rax, [memory_text_pointer]
    mov rbx, [rbp - 24]
    leave
    ret

concat_str: ; void concat_str(char* str, char** in)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov qword [rbp - 24], 0 ; length
    mov qword [rbp - 32], 0 ; strcount

    concatloop1:
        mov rbx, [rbp - 16]
        mov rax, [rbp - 32]
        mov rbx, [rbx + rax * 8]
        cmp rbx, 0
        je concatRet

        xor rcx, rcx
        concatloop2:
            check_memory 1

            mov dl, [rbx + rcx]
            cmp dl, 0
            je concatloop2end

            mov rax, [rbp - 8]
            add rax, [rbp - 24]
            mov [rax], dl
            inc rcx
            inc qword [rbp - 24]
            jmp concatloop2

        concatloop2end:
        inc qword [rbp - 32]
        jmp concatloop1

    concatRet:
    check_memory 1
    mov rbx, [rbp - 24]
    add rbx, [rbp - 8]
    mov byte [rbx], 0
    inc qword [rbp - 24]

    mov rbx, [rbp - 24]
    add [memory_pointer], rbx
    leave
    ret

strcmp: ; bool strcmp(char* a, char* b)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    xor rcx, rcx
    strcmploop:
        mov rbx, [rbp - 8]
        mov bl, [rbx + rcx]
        mov rdx, [rbp - 16]
        mov dl, [rdx + rcx]
        cmp bl, dl
        sete al
        
        cmp bl, 0
        je strcmpRet

        cmp al, 0
        je strcmpRet

        inc rcx
        jmp strcmploop

    strcmpRet:

    leave
    ret

is_keyword: ; bool is_keyword(char* str)
    push rbp
    mov rbp, rsp
    sub rsp, 17
    mov [rbp - 8], rdi
    mov qword [rbp - 16], 0 ; i
    mov byte [rbp - 17], 0 ; is_keyword

    keywordloop:
        mov rbx, keywords
        mov rax, [rbp - 16]
        mov rbx, qword [rbx + rax * 8]

        cmp rbx, 0
        je keywordloopend

        mov rdi, [rbp - 8]
        mov rsi, rbx
        call strcmp

        mov [rbp - 17], al
        cmp byte [rbp - 17], 1
        je keywordloopend

        inc qword [rbp - 16]
        jmp keywordloop

    keywordloopend:
    mov al, [rbp - 17]
    leave
    ret

file_error:
    printmany ERROR, filenotfound, suffix
    exit_process 1
no_memory_error:
    printmany ERROR, outofmemory, suffix
    exit_process 1
syntax_error_token: ; syntax_error_token(token* token)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov qword [rbp - 8], rdi

    printmany ERROR
    print_tok_pos [rbp - 8]
    printmany syntaxerrorsuffix, syntaxerrortoken, suffix
    exit_process 1
syntax_error_expected: ; void syntax_error_expected(token* token, char** argv)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov qword [rbp - 8], rdi
    mov qword [rbp - 16], rsi
    mov qword [rbp - 24], 0

    printmany ERROR
    print_tok_pos [rbp - 8]
    printmany syntaxerrorsuffix, syntaxerrorexpected

    seeloop:
        mov rbx, qword [rbp - 16]
        mov rcx, qword [rbp - 24]
        imul rcx, 8
        add rbx, rcx
        
        cmp qword [rbx + 8], 0
        jne seeL2
        cmp rcx, 0
        je seeL1
        printmany _or, space
        seeL1:
        printmany qword [rbx], suffix
        jmp seeRet

        seeL2: 
        printmany qword [rbx], comma, space
        inc qword [rbp - 24]
        jmp seeloop

    seeRet:
    exit_process 1
unimplemented_node:
    mov [rbp - 8], rdi
    printmany ERROR, unimplementednode
    mov rdi, [rbp - 8]
    call display_number
    mov rdi, suffix
    call print
    exit_process 1
unimplemented_op:
    printmany ERROR, unimplementedop, suffix
    exit_process 1