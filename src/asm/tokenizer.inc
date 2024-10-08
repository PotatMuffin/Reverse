tokenize: ; void* tokenize(char* buf)
    push rbp 
    mov rbp, rsp
    sub rsp, 21
    mov [rbp - 8], rdi
    mov [rbp - 12], dword 1
    mov rax, [memory_pointer]
    mov qword [rbp - 20], rax

    tokloopstart1:
    check_memory tok_len
    mov r13, [memory_pointer]
    xor rax, rax
    mov rbx, [rbp - 8]
    mov al, [rbx]
    mov [rbp - 21], al
    cmp [rbp - 21], byte 0
    je tokloopend1

    mov rdi, whitespaces
    mov sil, byte [rbp - 21]
    call contains_char
    cmp al, 1
    jne tokif0
    inc qword [rbp - 8]
    jmp tokloopstart1

    tokif0:
    cmp [rbp - 21], byte '#'
    jne tokif1

    tokif_11:
    inc qword [rbp - 8]
    mov rbx, [rbp - 8]
    mov bl, [rbx]
    mov byte [rbp - 21], bl
    cmp byte [rbp - 21], 10
    jne tokif_11
    jmp tokloopstart1

    tokif1:
    cmp [rbp - 21], byte '+'
    jne tokif2

    mov [r13], word TT_PLUS
    jmp tokloopcontinue

    tokif2:
    cmp [rbp - 21], byte '-'
    jne tokif3

    mov [r13], word TT_MIN
    jmp tokloopcontinue

    tokif3:
    cmp [rbp - 21], byte '*'
    jne tokif4

    mov [r13], word TT_STAR
    jmp tokloopcontinue

    tokif4:
    cmp [rbp - 21], byte '/'
    jne tokif5

    mov [r13], word TT_FSLASH
    jmp tokloopcontinue

    tokif5:
    cmp [rbp - 21], byte '\'
    jne tokif6

    mov [r13], word TT_BSLASH
    jmp tokloopcontinue

    tokif6:
    cmp [rbp - 21], byte '%'
    jne tokif7

    mov [r13], word TT_PERCENT
    jmp tokloopcontinue

    tokif7:
    cmp [rbp - 21], byte '<'
    jne tokif8

    mov rbx, [rbp - 8]
    cmp [rbx + 1], byte '='
    jne tokif_2

    mov [r13], word TT_LTE
    inc qword [rbp - 8]
    jmp tokloopcontinue
    tokif_2:

    mov rbx, [rbp - 8]
    cmp [rbx + 1], byte '<'
    jne tokif_7

    mov [r13], word TT_SHL
    inc qword [rbp - 8]
    jmp tokloopcontinue
    tokif_7:

    mov [r13], word TT_ANGBRACKL
    jmp tokloopcontinue

    tokif8:
    cmp [rbp - 21], byte '>'
    jne tokif9

    mov rbx, [rbp - 8]
    cmp [rbx + 1], byte '='
    jne tokif_1

    mov [r13], word TT_GTE
    inc qword [rbp - 8]
    jmp tokloopcontinue

    tokif_1:
    cmp [rbx + 1], byte '>'
    jne tokif_8

    mov [r13], word TT_SHR
    inc qword [rbp - 8]
    jmp tokloopcontinue
    tokif_8:

    mov [r13], word TT_ANGBRACKR
    jmp tokloopcontinue

    tokif9:
    cmp [rbp - 21], byte '('
    jne tokif10

    mov [r13], word TT_PARENL
    jmp tokloopcontinue

    tokif10:
    cmp [rbp - 21], byte ')'
    jne tokif11

    mov [r13], word TT_PARENR
    jmp tokloopcontinue

    tokif11:
    cmp [rbp - 21], byte '{'
    jne tokif12

    mov [r13], word TT_BRACEL
    jmp tokloopcontinue

    tokif12:
    cmp [rbp - 21], byte '}'
    jne tokif13

    mov [r13], word TT_BRACER
    jmp tokloopcontinue

    tokif13:
    cmp [rbp - 21], byte '['
    jne tokif14

    mov [r13], word TT_BRACKL
    jmp tokloopcontinue

    tokif14:
    cmp [rbp - 21], byte ']'
    jne tokif15

    mov [r13], word TT_BRACKR
    jmp tokloopcontinue

    tokif15:
    cmp [rbp - 21], byte '='
    jne tokif16

    mov rbx, [rbp - 8]
    cmp [rbx + 1], byte '='
    jne tokif_3

    mov [r13], word TT_IS
    inc qword [rbp - 8]
    jmp tokloopcontinue
    tokif_3:

    mov [r13], word TT_EQ
    jmp tokloopcontinue

    tokif16:
    cmp [rbp - 21], byte '!'
    jne tokif17

    mov rbx, [rbp - 8]
    cmp [rbx + 1], byte '='
    jne tokif_4

    mov [r13], word TT_ISNT
    inc qword [rbp - 8]
    jmp tokloopcontinue
    tokif_4:
    mov [r13], word TT_EXCL
    jmp tokloopcontinue

    tokif17:
    cmp [rbp - 21], byte '&'
    jne tokif18

    mov rbx, [rbp - 8]
    cmp [rbx + 1], byte '&'
    jne tokif_5

    mov [r13], word TT_LOGAND
    inc qword [rbp - 8]
    jmp tokloopcontinue
    tokif_5:
    mov [r13], word TT_AND
    jmp tokloopcontinue

    tokif18:
    cmp [rbp - 21], byte '|'
    jne tokif19

    mov rbx, [rbp - 8]
    cmp [rbx + 1], byte '|'
    jne tokif_6
    mov [r13], word TT_LOGOR
    inc qword [rbp - 8]
    jmp tokloopcontinue
    tokif_6:

    mov [r13], word TT_OR
    jmp tokloopcontinue
    tokif19:
    cmp [rbp - 21], byte ':'
    jne tokif20

    mov rbx, [rbp - 8]
    cmp [rbx + 1], byte ':'
    jne tokif_12
    mov [r13], word TT_FIELD
    inc qword [rbp - 8]
    jmp tokloopcontinue
    tokif_12:

    mov [r13], word TT_NOEXIST
    jmp tokloopcontinue

    tokif20:
    cmp [rbp - 21], byte '^'
    jne tokif21

    mov [r13], word TT_XOR
    jmp tokloopcontinue

    tokif21:
    cmp [rbp - 21], byte '~'
    jne tokif22

    mov [r13], word TT_NOT
    jmp tokloopcontinue

    tokif22:
    cmp [rbp - 21], byte '@'
    jne tokif23

    mov [r13], word TT_SIZEOF
    jmp tokloopcontinue

    tokif23:
    cmp [rbp - 21], byte 10
    jne tokif24

    mov [r13], word TT_NEWLINE
    inc dword [rbp - 12]
    jmp tokloopcontinue

    tokif24:
    cmp [rbp - 21], byte ';'
    jne tokif25

    mov [r13], word TT_NEWLINE
    jmp tokloopcontinue

    tokif25:
    cmp [rbp - 21], byte ','
    jne tokif26

    mov [r13], word TT_COMMA
    jmp tokloopcontinue

    tokif26:
    cmp [rbp - 21], byte '0'
    jl tokif27
    cmp [rbp - 21], byte '9'
    jg tokif27
    mov [r13], word TT_INTLIT
    xor rax, rax
    L1:
        xor rbx, rbx
        mov bl, [rbp - 21]
        imul rax, 10
        add rax, rbx
        sub rax, '0'

        mov rbx, [rbp - 8]
        mov bl, [rbx + 1]
        mov [rbp - 21], bl
        cmp [rbp - 21], byte '0'
        jl L2
        cmp [rbp - 21], byte '9'
        jg L2
        inc qword [rbp - 8]
        jmp L1
    L2:
    
    mov qword [r13 + 2], rax
    jmp tokloopcontinue

    tokif27:
    cmp [rbp - 21], byte "'"
    jne tokif28
    mov [r13], word TT_INTLIT
    mov rbx, [rbp - 8]
    movzx rax, byte [rbx + 1]
    cmp [rbx + 2], byte "'"
    je tokif_9

    printmany ERROR, brackl
    movsxd rdi, dword [rbp - 12]
    call display_number
    printmany brackr, space, missingtermchar, suffix
    exit_process 1
    tokif_9:
    add qword [rbp - 8], 2
    mov qword [r13 + 2], rax
    jmp tokloopcontinue

    tokif28:
    cmp [rbp - 21], byte '"'
    jne tokif29
    mov word [r13], TT_STRLIT
    mov r15, [memory_text_pointer]
    mov rbx, [rbp - 8]
    lea rdi, [rbx + 1]
    call fcopy_str
    cmp rax, -1
    jne tokif_10
    printmany ERROR, brackl
    movsxd rdi, dword [rbp - 12]
    call display_number
    printmany brackr, space, invalidstr, suffix
    exit_process 1
    tokif_10:

    mov rbx, [memory_text_pointer]
    sub r15, rbx
    add qword [rbp - 8], r15

    mov qword [r13 + 2], rax
    jmp tokloopcontinue

    tokif29:
    mov rdi, alpha
    mov sil, byte [rbp - 21]
    call contains_char
    cmp al, 0
    je tokif30

    mov rdi, [rbp - 8]
    mov rsi, alphanumeric 
    call copy_chars
    sub rbx, 2
    add qword [rbp - 8], rbx

    mov word [r13], TT_IDENT
    mov qword [r13 + 2], rax

    mov rdi, keywords
    mov rsi, [r13 + 2]
    call contains_str
    movzx ax, al
    add word [r13], ax

    jmp tokloopcontinue

    tokif30:

    mov rdi, brackl
    call print
    movsxd rdi, dword [rbp - 12]
    call display_number
    mov rdi, brackr
    call print
    mov rdi, space
    call print

    mov r14, [rbp - 8]
    printmany ERROR, illegalchar
    write STDOUT, r14, 1
    printmany quote, suffix
    exit_process 1

    tokloopcontinue:
        mov eax, dword [rbp - 12]
        mov [r13 + 10], eax
        inc qword [rbp - 8] 
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