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
        call parse_expr
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

parse_expr: ; void* parse_expr(token** Ptokens)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    cmp word [rbx], TT_BRACEL
    jne peL1

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    call parse_scope_expr
    leave
    ret

    peL1:
    
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
    sub rsp, 16
    mov [rbp - 8], rdi

    mov rdi, [rbp - 8]
    mov si, TT_BRACER
    call parse
    mov [rbp - 16], rax

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len

    mov rax, [rbp - 16]
    leave
    ret

parse_var_expr: ; void* parse_bin_expr(token** Ptokens, void* value)
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
    cmp word [rcx], TT_KEYWORD
    jne pveL2


    mov rdi, qword [rcx + 2]
    mov rsi, var
    call strcmp

    cmp al, 0
    je pveL2

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len
    node_vardef [rbp - 16], [rbp - 24]
    leave
    ret

    pveL2:
    mov rbx, [rbp - 8]
    mov rdi, [rbx]
    pushmany 0, _var
    mov rsi, rsp
    call syntax_error_expected

    pveL3:
    node_varass [rbp - 16], [rbp - 24]
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
        cmp word [rbx], TT_FSLASH
        jg pbeRet

        mov bx, word [rbx]
        and rbx, 0FFFFh
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
    jne paL1

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
    jne paL4

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

display_ast: ; void display_ast(void* node)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi

    mov rbx, qword [rbp - 8]
    cmp word [rbx], NODE_ROOT
    jne daL0

    cmp qword [rbx + 10], 0
    je daRet    

    mov rbx, qword [rbx + 2]
    ; go to the last item in the list
    daloop1:
    cmp qword [rbx + 8], 0
    je daloop2
    mov rbx, qword [rbx + 8]
    jmp daloop1
    
    daloop2:
    ; print current tree
    mov qword [rbp - 16], rbx
    mov rdi, [rbx]
    call display_ast
    mov rdi, suffix
    call print
    mov rbx, qword [rbp - 16]
    ; go back one item
    cmp qword [rbx + 16], 0
    je daRet
    mov rbx, qword [rbx + 16]
    jmp daloop2

    daL0:
    cmp word [rbx], NODE_LIT
    jne daL1

    mov rbx, qword [rbx + 2]
    mov rdi, qword [rbx + 2]
    call display_number
    jmp daRet
    
    daL1:
    cmp word [rbx], NODE_BINOP
    jne daL2

    mov rdi, _parenl
    call print
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call display_ast

    mov rdi, space
    call print
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    movsx rdi, word [rbx]
    call display_number
    mov rdi, space
    call print

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 18]
    call display_ast
    mov rdi, _parenr
    call print

    daL2:
    
    daRet:
    leave
    ret
