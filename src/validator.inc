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
    mov rdi, [rbx + 10]
    call cant_def_void
    vvnvL2:
    mov rbx, [builtin_types + rax * 8]
    mov [rbp - 24], rbx

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov rsi, [rbp - 16]
    call valvisit

    mov rdi, [rbp - 16]
    mov rsi, [rbp - 8]
    mov rdx, [rbp - 24]
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
    call st_get_symbol

    cmp rax, -1
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
    call st_get_symbol

    cmp rax, -1
    jne LRet2

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call name_error_var_undefined

    LRet2:
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