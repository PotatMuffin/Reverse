validate_prog: ; void validate_prog(node_root* prog, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    builtin_type none_
    mov [builtin_types], rax
    builtin_type byte_
    mov [builtin_types + 8], rax
    builtin_type word_
    mov [builtin_types + 16], rax
    builtin_type dword_
    mov [builtin_types + 24], rax
    builtin_type qword_
    mov [builtin_types + 32], rax

    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
    call valvisit_node_root

    leave
    ret

valvisit: ; void* valvisit(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    movzx rcx, word [rbx]
    cmp rcx, node_count
    mov rdi, rcx
    jg unimplemented_node
    
    mov rdi, qword [rbp - 8]
    mov rsi, [rbp - 16]
    mov rax, qword [vallookuptable + rcx * 8]
    call rax

    leave
    ret

valvisit_node_root: ; void* valvisit_node_root(void* node symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, qword [rbp - 8]
    cmp qword [rbx + 10], 0
    je vvnRet

    mov rbx, qword [rbx + 2]
    mov qword [rbp - 24], rbx

    vvnloop1:
    mov rbx, qword [rbp - 24]
    cmp qword [rbx + 8], 0
    je vvnloop2
    mov rbx, qword [rbx + 8]
    mov qword [rbp - 24], rbx
    jmp vvnloop1
    
    vvnloop2:
    mov rbx, qword [rbp - 24]
    mov rdi, qword [rbx]
    mov rsi, [rbp - 16]
    call valvisit

    mov rbx, qword [rbp - 24]
    cmp qword [rbx + 16], 0
    je vvnRet
    mov rbx, qword [rbx + 16]
    mov qword [rbp - 24], rbx
    jmp vvnloop2
    
    vvnRet:
    leave
    ret

valvisit_node_binop: ; void* valvisit_node_binop(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov rax, [builtin_types + 32]
    mov [rbp - 24], rax

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov rsi, [rbp - 16]
    call valvisit
    cmp word [rax], _PTR
    jne vvnbL1
    mov [rbp - 24], rax
    vvnbL1:

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 18]
    mov rsi, [rbp - 16]
    call valvisit
    cmp word [rax], _PTR
    jne vvnbL2
    mov [rbp - 24], rax
    vvnbL2:

    mov rax, [rbp - 24]
    leave
    ret

valvisit_node_lit: ; void* valvisit_node_lit(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 2]
    cmp word [rbx], TT_INTLIT
    jne vvnlL1
    mov rax, [builtin_types + 32]
    leave
    ret
    vvnlL1:
    ptr 1, [builtin_types + 8]
    leave
    ret

valvisit_node_unop: ; void* valvisit_node_unop(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rcx, [rbx + 10]
    mov rbx, [rbx + 2]
    cmp word [rbx], TT_AND
    jne vvnuL1
    cmp word [rcx], NODE_VAR
    je vvnuL1

    mov rdi, rbx
    call expected_var

    vvnuL1:

    mov rbx, qword [rbp - 8]
    mov rdi, qword [rbx + 10]
    mov rsi, [rbp - 16]
    call valvisit
    mov [rbp - 24], rax

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 2]
    cmp word [rbx], TT_STAR
    jne vvnuL4
    mov rbx, [rbp - 24]
    cmp word [rbx], _PTR
    je vvnuL5
    printmany ERROR
    mov r15, [rbp - 8]
    mov r15, [r15 + 2]
    print_tok_pos r15
    printmany expectedptr, suffix
    exit_process 1

    vvnuL5:

    cmp qword [rbx + 6], 1
    je vvnuL6

    mov r15, [rbx + 6]
    dec r15
    mov r14, [rbx + 14]
    ptr r15, r14
    mov [rbp - 24], rax
    jmp vvnuL2
    vvnuL6:

    mov rbx, [rbx + 14]
    mov [rbp - 24], rbx

    vvnuL4:

    mov rbx, [rbp - 8]
    mov rcx, [rbx + 10]
    mov rbx, [rbx + 2]
    cmp word [rbx], TT_AND
    jne vvnuL2

    mov rdi, [rbp - 16]
    mov rsi, [rcx + 2]
    call st_get_symbol

    mov rbx, [rax + 8]
    mov rcx, [rbx + 2]
    cmp word [rcx], _PTR
    jne vvnuL3
    mov r15, [rcx + 6]
    inc r15
    mov r14, [rcx + 14]
    ptr r15, r14
    mov [rbp - 24], rax
    jmp vvnuL2

    vvnuL3:
    ptr 1, rcx
    mov [rbp - 24], rax
    vvnuL2:

    mov rax, [rbp - 24]
    leave
    ret

valvisit_node_vardef: ; void valvisit_node_vardef(void* node, symbol_table table)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rdi, types
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 18]
    mov rsi, [rbx + 2]
    call contains_str

    cmp al, 0
    jne vvnvL1

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 18]
    call is_not_a_type

    vvnvL1:

    
    mov rdi, types
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 18]
    mov rsi, [rbx + 2]
    call index_of
    cmp rax, 0
    jne vvnvL2
    mov rbx, [rbp - 8]
    cmp qword [rbx + 26], 0
    jne vvnvL2
    mov rdi, [rbx + 10]
    call cant_def_void
    vvnvL2:
    mov rbx, [builtin_types + rax * 8]
    mov [rbp - 24], rbx

    mov rbx, [rbp - 8]
    cmp qword [rbx + 26], 0
    je vvnvL3

    mov r15, [rbp - 8]
    ptr [r15 + 26], [rbp - 24]
    mov [rbp - 24], rax
    vvnvL3:

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov rsi, [rbp - 16]
    call valvisit

    mov rdi, rax
    mov rsi, [rbp - 24]
    call type_equals
    cmp al,0
    jne vvnvL4
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 18]
    call type_mismatch

    vvnvL4:

    mov rdi, [rbp - 16]
    mov rsi, [rbp - 8]
    mov rdx, [rbp - 24]
    call st_add_var
    
    leave
    ret

valvisit_node_varass: ; void valvisit_node_varass(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov rsi, [rbp - 16]
    call valvisit
    mov [rbp - 24], rax

    mov rdi, [rbp - 16]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 10]
    call st_get_symbol
    mov [rbp - 32], rax

    cmp rax, -1
    jne vvnv_L2

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    call name_error_var_undefined

    vvnv_L2:

    mov rbx, [rbp - 32]
    mov rbx, [rbx + 8]
    mov rdi, [rbx + 2]
    mov rsi, [rbp - 24]
    call type_equals
    cmp al, 0
    jne vvnv_L1
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    call type_mismatch

    vvnv_L1:
    
    leave
    ret
    
valvisit_node_var: ; void* valvisit_node_var(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rdi, [rbp - 16]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 2]
    call st_get_symbol
    mov [rbp - 24], rax

    cmp rax, -1
    jne _vvnv_L1

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call name_error_var_undefined
    _vvnv_L1:

    mov rbx, [rbp - 24]
    mov rbx, [rbx + 8]
    mov rax, [rbx + 2]
    leave
    ret

valvisit_node_if: ; void valvisit_node_if(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rsi, [rbp - 16]
    call valvisit

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call valvisit

    mov rbx, [rbp - 8]
    cmp qword [rbx + 18],0
    je vvniL1
    
    mov rdi, [rbx + 18]
    mov rsi, [rbp - 16]
    call valvisit

    vvniL1:

    leave
    ret

valvisit_node_while: ; void valvisit_node_while(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rsi, [rbp - 16]
    call valvisit

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call valvisit

    leave
    ret

valvisit_node_for: ; void valisit_node_for(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rsi, [rbp - 16]
    call valvisit

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call valvisit

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 18]
    mov rsi, [rbp - 16]
    call valvisit

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 26]
    mov rsi, [rbp - 16]
    call valvisit

    leave
    ret

valvisit_node_asm: ; void valvisit_node_asm(void* node, symbol_table* table)
    nop
    ret

vallookuptable:
    dq 0
    dq valvisit_node_root
    dq valvisit_node_binop
    dq valvisit_node_lit
    dq valvisit_node_unop
    dq valvisit_node_vardef
    dq valvisit_node_varass
    dq valvisit_node_var
    dq valvisit_node_if
    dq valvisit_node_while
    dq valvisit_node_for
    dq valvisit_node_asm