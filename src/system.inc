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

macro printmany [arg]
{
    mov rdi, arg
    call print
}

macro check_memory required
{
    mov rax, [memory_pointer]
    add rax, required
    cmp rax, [memory_end]
    jg no_memory_error
}

macro node_root children
{
    mov rax, [memory_pointer]
    mov word [rax], NODE_ROOT
    mov rbx, children
    mov qword [rax + 2] rbx
    add [memory_pointer], node_root_size
}

macro node_bin_expr left, op, right
{
    mov rax, [memory_pointer]
    mov word [rax], NODE_BINEXPR
    mov rbx, left
    mov qword [rax + 2], rbx
    mov rbx, op
    mov qword [rax + 10], rbx
    mov rbx, right
    mov qword [rax + 18], rbx
    add [memory_pointer], node_bin_expr_size
}

macro node_lit token
{
    mov rax, [memory_pointer]
    mov word [rax], NODE_LIT
    mov rbx, token
    mov qword [rax + 2], rbx
    add [memory_pointer], node_lit_size
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

print: ; void print(char* ptr)
    xor rdx, rdx
    mov rsi, rdi

    printloopstart:

    mov al, [rsi + rdx]
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

        cmp rax, 0
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

contains_char: ; bool contains_char(char** source, char c)
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

file_error:
    printmany _error, filenotfound, suffix
    exit_process 1
no_memory_error:
    printmany _error, outofmemory, suffix
    exit_process 1
syntax_error_token: ; syntax_error_token(token* token)
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    mov rdi, lineprefix
    call print
    mov rbx, [rbp - 8]
    xor rdi, rdi
    mov edi, dword [rbx + 10]
    call display_number
    mov rdi, linesuffix
    call print
    printmany syntaxerrortoken, suffix
    exit_process 1