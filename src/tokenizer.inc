tokenize: ; void* tokenize(char* buf)
    push rbp 
    mov rbp, rsp
    sub rsp, 29
    mov [rbp - 8], rdi
    mov [rbp - 12], dword 1
    mov rax, [memory_pointer]
    mov qword [rbp - 20], rax
    mov qword [rbp - 28], 0

    tokloopstart1:
    check_memory tok_len
    mov r13, [memory_pointer]

    xor rax, rax
    mov r11, [rbp - 8]
    mov r12, [rbp - 28]
    mov al, [r11 + r12]
    mov [rbp - 29], al
    cmp [rbp - 29], byte 0
    je tokloopend1

    mov rdi, whitespaces
    mov sil, byte [rbp - 29]
    call contains_char
    cmp al, 1
    jne tokif0
    inc qword [rbp - 28]
    jmp tokloopstart1

    tokif0:
    cmp [rbp - 29], byte '+'
    jne tokif1

    mov [r13], word TT_PLUS
    jmp tokloopcontinue

    tokif1:
    cmp [rbp - 29], byte '-'
    jne tokif2

    mov [r13], word TT_MIN
    jmp tokloopcontinue

    tokif2:
    cmp [rbp - 29], byte '*'
    jne tokif3

    mov [r13], word TT_STAR
    jmp tokloopcontinue

    tokif3:
    cmp [rbp - 29], byte '/'
    jne tokif4

    mov [r13], word TT_FSLASH
    jmp tokloopcontinue

    tokif4:
    cmp [rbp - 29], byte '\'
    jne tokif5

    mov [r13], word TT_BSLASH
    jmp tokloopcontinue

    tokif5:
    cmp [rbp - 29], byte '<'
    jne tokif6

    mov [r13], word TT_ANGBRACKL
    jmp tokloopcontinue

    tokif6:
    cmp [rbp - 29], byte '>'
    jne tokif7

    mov [r13], word TT_ANGBRACKR
    jmp tokloopcontinue

    tokif7:
    cmp [rbp - 29], byte '('
    jne tokif8

    mov [r13], word TT_PARENL
    jmp tokloopcontinue

    tokif8:
    cmp [rbp - 29], byte ')'
    jne tokif9

    mov [r13], word TT_PARENR
    jmp tokloopcontinue

    tokif9:
    cmp [rbp - 29], byte '{'
    jne tokif10

    mov [r13], word TT_BRACEL
    jmp tokloopcontinue

    tokif10:
    cmp [rbp - 29], byte '}'
    jne tokif11

    mov [r13], word TT_BRACER
    jmp tokloopcontinue

    tokif11:
    cmp [rbp - 29], byte '['
    jne tokif12

    mov [r13], word TT_BRACKL
    jmp tokloopcontinue

    tokif12:
    cmp [rbp - 29], byte ']'
    jne tokif13

    mov [r13], word TT_BRACKR
    jmp tokloopcontinue

    tokif13:
    cmp [rbp - 29], byte '='
    jne tokif14

    mov [r13], word TT_EQ
    jmp tokloopcontinue

    tokif14:
    cmp [rbp - 29], byte 10
    jne tokif15

    mov [r13], word TT_NEWLINE
    inc dword [rbp - 12]
    jmp tokloopcontinue

    tokif15:
    cmp [rbp - 29], byte ';'
    jne tokif16

    mov [r13], word TT_NEWLINE
    jmp tokloopcontinue

    tokif16:
    cmp [rbp - 29], byte '0'
    jl tokif17
    cmp [rbp - 29], byte '9'
    jg tokif17
    mov [r13], word TT_INTLIT
    xor rax, rax
    xor rbx, rbx
    L1:
        mov bl, [rbp - 29]
        imul rax, 10
        add rax, rbx
        sub rax, '0'

        mov r11, [rbp - 8]
        mov r12, [rbp - 28]
        mov bl, byte [r11 + r12 + 1]
        mov [rbp - 29], bl
        cmp [rbp - 29], byte '0'
        jl L2
        cmp [rbp - 29], byte '9'
        jg L2
        inc qword [rbp - 28]
        jmp L1
    L2:
    
    mov qword [r13 + 2], rax
    jmp tokloopcontinue

    tokif17:
    mov rdi, alfa
    mov sil, byte [rbp - 29]
    call contains_char
    cmp al, 0
    je tokif18

    mov r11, [rbp - 8]
    mov r12, [rbp - 28]
    lea rdi, [r11 + r12]
    mov rsi, alfanumeric 
    call copy_chars
    dec rbx
    add qword [rbp - 28], rbx

    mov word [r13], TT_IDENT
    mov qword [r13 + 2], rax

    jmp tokloopcontinue

    tokif18:

    mov rdi, lineprefix
    call print
    movsxd rdi, dword [rbp - 12]
    call display_number
    mov rdi, linesuffix
    call print

    mov r11, [rbp - 8]
    mov r12, [rbp - 28]
    lea r14, [r11 + r12]
    printmany _error, illegalchar
    write STDOUT, r14, 1
    printmany quote, suffix
    exit_process 1

    tokloopcontinue:
        mov eax, dword [rbp - 12]
        mov [r13 + 10], eax
        inc qword [rbp - 28] 
        add [memory_pointer], tok_len
        jmp tokloopstart1

    tokloopend1:

    mov rax, [rbp - 20]
    mov [r13], word TT_EOF
    mov ebx, dword [rbp - 12]
    mov dword [r13 + 10], ebx
    add [memory_pointer], tok_len

    leave 
    ret