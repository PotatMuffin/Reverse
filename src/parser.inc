parse: ; void parse(token* tokens)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    parseloop1:
        parseloop2:
            mov rbx, [rbp - 8]
            cmp word [rbx], TT_NEWLINE
            jne L5
            add qword [rbp - 8], tok_len
            jmp parseloop2
        L5:

        mov rbx, [rbp - 8]
        cmp word [rbx], TT_EOF
        je parseret
        
        lea rdi, qword [rbp - 8]
        call parse_expr

        mov rdi, rax
        call display_number
        mov rdi, suffix
        call print

        mov rbx, [rbp - 8]
        cmp word [rbx], TT_NEWLINE
        je parseloop1
        cmp word [rbx], TT_EOF
        je parseret

        mov rdi, [rbp - 8]
        jmp syntax_error_token

    parseret:

    leave 
    ret

parse_expr: ; void* parse_expr(token** Ptokens)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    mov rdi, [rbp - 8]
    call parse_bin_expr

    leave
    ret

parse_bin_expr: ; void* parse_bin_expr(token** Ptokens)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    mov rdi, [rbp - 8]
    call parse_atom

    leave
    ret

parse_atom: ; void* parse_atom(token** Ptokens)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    cmp word [rbx], TT_INTLIT
    jne paL1

    node_lit rbx

    mov rbx, [rbp - 8]
    add qword [rbx], tok_len

    jmp paRet

    paL1:

    paRet:

    leave 
    ret