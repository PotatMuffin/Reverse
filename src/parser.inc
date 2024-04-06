parse: ; void parse(token** tokens, short EOF)
    push rbp
    mov rbp, rsp
    sub rsp, 26
    mov [rbp - 8], rdi
    mov [rbp - 26], si

    node_root
    mov [rbp - 16], rax

    parseloop1:
        parseloop2:
            mov rbx, [rbp - 8]
            mov rbx, [rbx]
            cmp word [rbx], TT_NEWLINE
            jne L5
            mov rbx, [rbp - 8]
            add qword [rbx], tok_len
            jmp parseloop2
        L5:

        mov rbx, [rbp - 8]
        mov rbx, [rbx]
        mov ax, word [rbp - 26]
        cmp word [rbx], ax
        je parseret
        
        mov rdi, qword [rbp - 8]
        call parse_stmt
        mov [rbp - 24], rax

        linked_list_item qword [rbp - 24]
        mov rbx, qword [rbp - 16]
        mov rcx, qword [rbx + 2]
        cmp rcx, 0
        je PL1
        mov qword [rax + 8], rcx
        mov qword [rcx + 16], rax
        PL1:
        mov qword [rbx + 2], rax
        inc qword [rbx + 10]

        mov rbx, [rbp - 8]
        mov rbx, [rbx]
        cmp word [rbx], TT_NEWLINE
        je parseloop1
        mov ax, word [rbp - 26]
        cmp word [rbx], ax
        je parseret

        mov rbx, [rbp - 8]
        mov rdi, [rbx]
        jmp syntax_error_token

    parseret:

    mov rax, [rbp - 16]
    leave 
    ret

parse_stmt: ; void* parse_stmt(token** Ptokens)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    cmp word [rbx], TT_BRACEL
    jne psL1

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    call parse_scope_expr
    leave
    ret

    psL1:

    mov rdi, [rbp - 8]
    call parse_expr

    leave
    ret

parse_expr: ; void* parse_expr(token** Ptokens)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    cmp word [rbx], TT_KEYWORD
    jne peL3
    mov rdi, [rbx + 2]
    mov rsi, asm_
    call strcmp
    cmp al, 0
    je peL3
    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    mov rcx, [rbx]
    cmp word [rcx], TT_PARENL
    je peL4
    mov rdi, rcx
    pushmany 0, _parenl
    mov rsi, rsp
    call syntax_error_expected
    peL4:
    add qword [rbx], tok_len
    mov rcx, [rbx]
    cmp word [rcx], TT_STRLIT
    je peL5

    mov rdi, rcx
    pushmany 0, _str
    mov rsi, rsp
    call syntax_error_expected
    peL5:

    mov [rbp - 16], rcx
    add qword [rbx], tok_len
    mov rcx, [rbx]
    cmp word [rcx], TT_PARENR
    je peL6

    mov rdi, rcx
    pushmany 0, _parenr
    mov rsi, rsp
    call syntax_error_expected
    peL6:

    add qword [rbx], tok_len
    node_asm [rbp - 16]
    leave
    ret
    peL3:
    
    mov rdi, [rbp - 8]
    mov sil, 0
    call parse_bin_expr
    mov [rbp - 16], rax

    mov rbx, [rbp - 8]
    mov rcx, [rbx]
    cmp word [rcx], TT_EQ
    jne peL2
    
    add qword [rbx], tok_len
    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
    call parse_var_expr

    peL2:

    leave
    ret

parse_scope_expr: ; void* parse_scope_expr(token** Ptokens)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi

    mov rdi, [rbp - 8]
    mov si, TT_BRACER
    call parse
    mov [rbp - 16], rax

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len

    mov rcx, [rbx]
    cmp word [rcx], TT_KEYWORD
    jne pseL2
    mov rdi, [rcx + 2]
    mov rsi, else_
    call strcmp
    cmp al, 0
    je pseL2

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len

    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
    call parse_else_expr
    leave
    ret

    pseL2:
    mov rbx, [rbp - 8]
    mov rcx, [rbx]
    cmp word [rcx], TT_PARENL
    jne pseL1
    add qword [rbx], tok_len

    mov dword [rbp - 28], 0 
    mov dword [rbp - 32], 1

    pseloop:
    mov rbx, qword [rbp - 8]
    mov rbx, [rbx]
    mov eax, dword [rbp - 28]
    imul rax, tok_len
    lea rbx, [rbx + rax]

    cmp word [rbx], TT_EOF
    jne pseL5

    mov rdi, rbx
    pushmany 0, _parenr
    mov rsi, rsp
    call syntax_error_expected

    pseL5:
    cmp word [rbx], TT_PARENL
    sete al
    movzx eax, al
    add dword [rbp - 32], eax

    cmp word [rbx], TT_PARENR
    sete al
    movzx eax, al
    sub dword [rbp - 32], eax

    inc dword [rbp - 28]
    cmp dword [rbp - 32], 0
    jne pseloop

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    mov eax, dword [rbp - 28]
    imul rax, tok_len
    lea rbx, [rbx + rax]

    cmp word [rbx], TT_IDENT
    jne pseL6
    lea rcx, [rbx + tok_len]
    cmp word [rcx], TT_IDENT
    je pseL7

    mov rdi, rcx
    pushmany 0, _type
    mov rsi, rsp
    call syntax_error_expected

    pseL7:
    
    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
    call parse_func_expr
    leave
    ret

    pseL6:
    cmp word [rbx], TT_KEYWORD
    jne pse_expected

    mov rdi, [rbx + 2]
    mov rsi, for_
    call strcmp
    cmp al, 0
    je pseL4

    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
    call parse_for_expr
    leave
    ret

    pseL4:
    mov rdi, [rbp - 8]
    mov sil, 0
    call parse_expr
    mov [rbp - 24], rax

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    cmp word [rbx], TT_PARENR
    je pseL3

    mov rdi, rbx
    pushmany 0, _parenr
    mov rsi, rsp
    call syntax_error_expected

    pseL3:
    mov rbx, [rbp - 8]
    add qword [rbx], tok_len

    mov rbx, [rbx]
    mov rdi, [rbx + 2]
    mov rsi, if_
    call strcmp
    cmp al, 0
    jne pse_parse_if

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    mov rdi, [rbx + 2]
    mov rsi, while_
    call strcmp
    cmp al, 0
    jne pse_parse_while

    pse_expected:
    mov rbx, [rbp - 8]
    mov rdi, [rbx]
    pushmany 0, _if, _while, _for, _ident
    mov rsi, rsp
    call syntax_error_expected

    pse_parse_while:
    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    node_while [rbp - 24], [rbp - 16]
    leave 
    ret

    pse_parse_if:
    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    node_if [rbp - 24], [rbp - 16]
    leave
    ret
    pseL1:

    mov rax, [rbp - 16]
    leave
    ret

parse_for_expr: ; void* parse_for_expr(token** Ptokens, void* scope)
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov qword [rbp - 24], 0

    pfeloop:
    mov rdi, [rbp - 8]
    call parse_expr

    mov rbx, [rbp - 24]
    mov [rbp - 48 + rbx*8], rax

    cmp word [rax], NODE_IF
    je pfeL2
    cmp word [rax], NODE_WHILE
    je pfeL2
    cmp word [rax], NODE_FOR
    jne pfeL3
    pfeL2:

    mov rbx, [rbp - 8]
    mov rdi, [rbx]
    call iwf_not_allowed

    pfeL3:

    inc qword [rbp - 24]
    cmp qword [rbp - 24], 3
    jge pfeloopend

    mov rbx, [rbp - 8]
    mov rcx, [rbx]
    add qword [rbx], tok_len
    cmp word [rcx], TT_NEWLINE
    je pfeloop

    mov rbx, [rbp - 8]
    mov rdi, [rbx]
    pushmany 0, _semi
    mov rsi, rsp
    call syntax_error_expected

    pfeloopend:

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    cmp word [rbx], TT_PARENR
    je pfeL1

    mov rdi, rbx
    pushmany 0, _parenr
    mov rsi, rsp
    call syntax_error_expected

    pfeL1:

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len*2
    node_for [rbp - 32], [rbp - 40], [rbp - 48], [rbp - 16]

    leave
    ret

parse_func_expr: ;void* parse_func_expr(token** Ptokens, void* scope)
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov qword [rbp - 40],0
    mov qword [rbp - 48],0

    mov rbx, [rbp - 8]
    mov rcx, [rbx]
    cmp word [rcx], TT_IDENT
    jne pfe_L1

    mov r15, rcx
    add qword [rbx], tok_len
    mov rcx, [rbx]
    cmp word [rcx], TT_IDENT
    je pfe_L5

    mov rdi, rcx
    pushmany 0, _type
    mov rsi, rsp
    call syntax_error_expected
    pfe_L5:
    mov rdi, [rbp - 8]
    call parse_type
    mov r14, rax
    param r15, r14
    mov [rbp - 40], rax
    inc qword [rbp - 48]

    jmp pfe_L3
    pfe_L4:
    inc qword [rbp - 48]
    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    mov rcx, [rbx]
    cmp word [rcx], TT_IDENT
    je pfe_L6

    mov rdi, rcx
    pushmany 0, _ident
    mov rsi, rsp
    call syntax_error_expected
    pfe_L6:

    mov r15, rcx
    add qword [rbx], tok_len
    mov rcx, [rbx]
    cmp word [rcx], TT_IDENT
    je pfe_L7

    mov rdi, rcx
    pushmany 0, _type
    mov rsi, rsp
    call syntax_error_expected
    pfe_L7:

    mov rdi, [rbp - 8]
    call parse_type

    mov r14, rax
    param r15, r14

    pfe_L3:
    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    cmp word [rbx], TT_COMMA
    je pfe_L4

    pfe_L1:
    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    cmp word [rbx], TT_PARENR
    je pfe_L2

    mov rdi, rbx
    pushmany 0, _parenr
    mov rsi, rsp
    call syntax_error_expected

    pfe_L2:

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    mov rcx, [rbx]
    mov [rbp - 24], rcx
    add qword [rbx], tok_len
    mov rdi, [rbp - 8]
    call parse_type
    mov [rbp - 32], rax
    node_func_def [rbp - 16], [rbp - 24], [rbp - 40], [rbp - 48], [rbp - 32]

    leave
    ret

parse_else_expr: ; void* parse_else_expr(token** Ptokens, void* scope)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    peeloop:
        mov rbx, [rbp - 8]
        mov rcx, [rbx]
        cmp word [rcx], TT_NEWLINE
        jne peeloopend
        add qword [rbx], tok_len
        jmp peeloop
    peeloopend:

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    cmp word [rbx], TT_BRACEL
    je peeL1

    mov rdi, rbx
    pushmany 0, _bracel
    mov rsi, rsp
    call syntax_error_expected

    peeL1:

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len

    mov rdi, [rbp - 8]
    call parse_scope_expr
    mov [rbp - 24], rax

    mov rbx, [rbp - 24]
    cmp word [rbx], NODE_IF
    je peeL2

    mov rbx, [rbp - 8]
    mov rdi, [rbx]
    pushmany 0, _if
    mov rsi, rsp
    call syntax_error_expected

    peeL2:

    mov rax, [rbp - 24]
    mov rbx, [rbp - 16]
    mov [rax + 18], rbx
    leave
    ret

parse_var_expr: ; void* parse_var_expr(token** Ptokens, void* value)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rcx, [rbx]
    mov [rbp - 24], rcx
    cmp word [rcx], TT_IDENT
    je pveL1

    mov rdi, rcx
    pushmany 0, _ident
    mov rsi, rsp
    call syntax_error_expected

    pveL1:
    add qword [rbx], tok_len
    mov rcx, [rbx]
    cmp word [rcx], TT_NEWLINE
    je pveL3
    cmp word [rcx], TT_EOF
    je pveL3
    cmp word [rcx], TT_PARENR
    je pveL3
    cmp word [rcx], TT_BRACER
    je pveL3
    cmp word [rcx], TT_IDENT
    jne pveL2

    mov rdi, [rbp - 8]
    call parse_type
    mov r15, rax
    node_vardef [rbp - 16], [rbp - 24], r15
    leave
    ret

    pveL2:
    mov rbx, [rbp - 8]
    mov rdi, [rbx]
    pushmany 0, _type
    mov rsi, rsp
    call syntax_error_expected

    pveL3:
    node_varass [rbp - 16], [rbp - 24]
    leave
    ret

parse_type: ; void* parse_type(token** Ptokens)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi

    mov rbx, [rbp - 8]
    mov rcx, [rbx]
    mov [rbp - 16], rcx
    add qword [rbx], tok_len
    mov qword [rbp - 24], 0

    ptloop:
        mov rbx, [rbp - 8]
        mov rcx, [rbx]
        cmp word [rcx], TT_STAR
        jne ptloopend
        add qword [rbx], tok_len
        inc qword [rbp - 24]
        jmp ptloop
    ptloopend:

    type [rbp - 16], [rbp - 24]
    leave
    ret

parse_bin_expr: ; void* parse_bin_expr(token** Ptokens, byte min_prec)
    push rbp
    mov rbp, rsp
    sub rsp, 34
    mov [rbp - 8], rdi ; Ptokens
    mov [rbp - 9], sil ; min_prec

    mov rdi, [rbp - 8]
    call parse_atom
    mov [rbp - 17], rax ; left

    pbeLoop1:
        mov rbx, [rbp - 8]
        mov rbx, [rbx]
        mov [rbp - 33], rbx ; op

        cmp word [rbx], TT_PLUS
        jl pbeRet
        cmp word [rbx], TT_SHR
        jg pbeRet

        movzx rbx, word [rbx]
        mov al, [op_prec + rbx]
        mov [rbp - 34], al ; prec

        cmp al, byte [rbp - 9]
        jl pbeRet

        mov rbx, [rbp - 8]
        add qword [rbx], tok_len

        mov rdi, [rbp - 8]
        mov sil, byte [rbp - 34]
        inc sil
        call parse_bin_expr
        mov [rbp - 25], rax ; right

        node_binop [rbp - 17], [rbp - 33], [rbp - 25]
        mov [rbp - 17], rax

        jmp pbeLoop1

    pbeRet:
    mov rax, [rbp - 17]
    leave
    ret

parse_atom: ; void* parse_atom(token** Ptokens)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    mov [rbp - 16], rbx
    cmp word [rbx], TT_INTLIT
    je paL7
    cmp word [rbx], TT_STRLIT
    jne paL1
    paL7:
    node_lit rbx

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    leave
    ret

    paL1:

    cmp word [rbx], TT_PARENL
    jne paL2

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len

    mov rdi, [rbp - 8]
    mov sil, 0
    call parse_bin_expr

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    cmp word [rbx], TT_PARENR
    je paL3

    mov rdi, rbx
    pushmany 0, _parenr
    mov rsi, rsp
    call syntax_error_expected

    paL3:
    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    leave 
    ret

    paL2:
    cmp word [rbx], TT_MIN
    je paL6
    cmp word [rbx], TT_EXCL
    je paL6
    cmp word [rbx], TT_NOT
    je paL6
    cmp word [rbx], TT_AND
    je paL6
    cmp word [rbx], TT_STAR
    jne paL4
    paL6:
    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    mov rdi, rbx
    call parse_atom
    mov [rbp - 24], rax

    node_unop [rbp - 16], [rbp - 24]
    leave
    ret

    paL4:
    cmp word [rbx], TT_IDENT
    jne paL5

    node_var rbx
    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    leave
    ret

    paL5:
    mov rdi, rbx
    pushmany 0, _parenl, _ident, _int
    mov rsi, rsp
    call syntax_error_expected

    leave 
    ret