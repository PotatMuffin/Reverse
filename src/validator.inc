validate_prog: ; void validate_prog(node_root* prog, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
    call valvisit_node_root

    leave
    ret

valvisit: ; void valvisit(void* node, symbol_table* table)
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

valvisit_node_root: ; void valvisit_node_root(void* node symbol_table* table)
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

valvisit_node_binop: ; void valvisit_node_binop(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov rsi, [rbp - 16]
    call valvisit

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 18]
    mov rsi, [rbp - 16]
    call valvisit

    leave
    ret

valvisit_node_lit: ; void valvisit_node_lit(void* node, symbol_table* table)
    nop
    ret

valvisit_node_unop: ; void valvisit_node_unop(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, qword [rbp - 8]
    mov rdi, qword [rbx + 10]
    mov rsi, [rbp - 16]
    call valvisit

    leave
    ret

valvisit_node_vardef: ; valvisit_node_vardef(void* node, symbol_table table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov rsi, [rbp - 16]
    call valvisit

    mov rdi, [rbp - 16]
    mov rsi, [rbp - 8]
    call st_add_var
    
    leave
    ret

valvisit_node_varass: ; void valvisit_node_varass(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov rsi, [rbp - 16]
    call valvisit

    mov rdi, [rbp - 16]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 10]
    call st_get_var_pos

    cmp rax, 0
    jne LRet1

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    call name_error_var_undefined

    LRet1:
    leave
    ret
    
valvisit_node_var: ; void valvisit_node_var(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rdi, [rbp - 16]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 2]
    call st_get_var_pos

    cmp rax, 0
    jne LRet2

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call name_error_var_undefined

    LRet2:
    leave
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