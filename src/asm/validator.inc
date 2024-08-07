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

    valcontext [rbp - 16], 0, 0, 0
    mov [rbp - 24], rax

    mov rdi, [rbp - 8]
    mov rsi, [rbp - 24]
    call valvisit_node_root

    mov [pass], 1
    mov rdi, [rbp - 8]
    mov rsi, [rbp - 24]
    call valvisit_node_root

    leave
    ret

valvisit: ; void* valvisit(void* node, valcontext* context)
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
    
    cmp [pass], 1
    je vvL1
    cmp rcx, NODE_FUNC_DEF
    je vvL1
    cmp rcx, NODE_STRUCT
    je vvL1
    leave
    ret
    vvL1: 
    mov rdi, qword [rbp - 8]
    mov rsi, [rbp - 16]
    mov rax, qword [vallookuptable + rcx * 8]
    call rax

    leave
    ret

valvisit_node_root: ; void* valvisit_node_root(void* node, valcontext* context)
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

valvisit_node_binop: ; void* valvisit_node_binop(void* node, valcontext* context)
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

    mov rax, [rbp - 24]
    leave
    ret

valvisit_node_lit: ; void* valvisit_node_lit(void* node, valcontext* context)
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

valvisit_node_unop: ; void* valvisit_node_unop(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rcx, [rbx + 2]
    cmp word [rcx], TT_SIZEOF
    je vvnuL7

    mov rbx, [rbp - 8]
    mov rcx, [rbx + 10]
    mov rbx, [rbx + 2]
    cmp word [rbx], TT_AND
    jne vvnuL1
    cmp word [rcx], NODE_VAR
    je vvnuL1
    cmp word [rcx], NODE_FIELD_ACCESS
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

    vvnuL7:
    mov rdi, [rbx + 10]
    mov rbx, [rbp - 16]
    mov rsi, [rbx]
    call get_real_type
    cmp rax, -1
    jne vvnuL8

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 2]
    mov rdi, [rbx]
    call is_not_a_type

    vvnuL8:
    mov rbx, [rbp - 8]
    mov [rbx + 10], rax

    mov rax, [builtin_types + 32]
    leave
    ret
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

    mov rcx, [rbp - 24]
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

valvisit_node_vardef: ; void valvisit_node_vardef(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 18]
    mov rbx, [rbp - 16]
    mov rsi, [rbx]
    call get_real_type
    cmp rax, -1
    jne vvnvL1

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 18]
    mov rdi, [rbx]
    call is_not_a_type

    vvnvL1:
    cmp rax, [builtin_types]
    jne vvnvL2

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 18]
    mov rdi, [rbx]
    call cant_def_void

    vvnvL2:
    mov [rbp - 24], rax
    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov rbx, [rbp - 16]
    valcontext [rbx], [rbp - 24], 0, 0
    mov rsi, rax
    call valvisit

    mov rdi, rax
    mov rsi, [rbp - 24]
    call type_equals
    cmp al,0
    jne vvnvL4
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 18]
    mov rdi, [rbx]
    call type_mismatch

    vvnvL4:

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 10]
    mov rdx, [rbp - 24]
    call st_add_var
    
    leave
    ret

valvisit_node_varass: ; void valvisit_node_varass(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 10]
    mov dl, 1
    call st_get_symbol
    mov [rbp - 32], rax

    cmp rax, -1
    jne vvnv_L2

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    call name_error_var_undefined

    vvnv_L2:

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov r15, [rbp - 32]
    mov r15, [r15 + 8]
    mov rbx, [rbp - 16]
    valcontext [rbx], [r15 + 2], 0, 0
    mov rsi, rax
    call valvisit
    mov [rbp - 24], rax

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
    
valvisit_node_var: ; void* valvisit_node_var(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 2]
    mov dl, 1
    call st_get_symbol
    mov [rbp - 24], rax

    cmp rax, -1
    jne _vvnv_L1

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call name_error_var_undefined
    _vvnv_L1:

    mov rax, [rbp - 24]
    mov rax, [rax + 8]
    cmp word [rax], FUNC
    je _vvnv_L2
    mov rax, [rax + 2]
    _vvnv_L2:
    leave
    ret

valvisit_node_if: ; void valvisit_node_if(void* node, valcontext* context)
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

valvisit_node_while: ; void valvisit_node_while(void* node, valcontext* context)
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
    mov r15, [rbp - 16]
    valcontext [r15], 0, [r15 + 16], 1
    mov rsi, rax
    call valvisit

    leave
    ret

valvisit_node_for: ; void valisit_node_for(void* node, valcontext* context)
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
    mov r15, [rbp - 16]
    valcontext [r15], 0, [r15 + 16], 1
    mov rsi, rax
    call valvisit

    leave
    ret

valvisit_node_asm: ; void valvisit_node_asm(void* node, valcontext* context)
    nop
    ret

valvisit_node_func_def: ;void valvisit_node_func_def(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 56
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    cmp [pass], 0
    jne vvnfdL7
    
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 34]
    mov rbx, [rbp - 16]
    mov rsi, [rbx]
    call get_real_type
    cmp rax, -1
    jne vvnfdL8

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 34]
    mov rdi, [rbx]
    call is_not_a_type

    vvnfdL8:

    mov [rbp - 48], rax

    mov r15, [rbp - 16]
    symbol_table 256, [r15]
    mov [rbp - 24], rax

    mov rbx, [rbp - 8]
    mov r15, [rbx + 26]
    mov r14, [rbx + 18]
    func r15, r14, [rbp - 48], [rbp - 24]
    mov r15, rax

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 10]
    mov rdx, r15
    call st_add_func
    leave
    ret
    vvnfdL7:

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 34]
    mov rbx, [rbp - 16]
    mov rsi, [rbx]
    call get_real_type
    mov [rbp - 56], rax

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 10]
    mov dl, 0
    call st_get_symbol

    mov rbx, [rax + 8]
    mov rbx, [rbx + 26]
    mov [rbp - 24], rbx

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 26]
    mov qword [rbp - 32], rbx
    dec qword [rbp - 32]

    jmp vvnfdL2
    vvnfdL1:
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 18]
    mov rax, [rbp - 32]
    imul rax, 32
    lea rbx, [rbx + rax]
    mov [rbp - 40], rbx

    mov rdi, [rbx + 8]
    mov rbx, [rbp - 16]
    mov rsi, [rbx]
    call get_real_type
    cmp rax, -1
    jne vvnfdL5

    mov rbx, [rbp - 40]
    mov rbx, [rbx + 8]
    mov rdi, [rbx]
    call is_not_a_type

    vvnfdL5:
    cmp rax, [builtin_types]
    jne vvnfdL6

    mov rbx, [rbp - 40]
    mov rbx, [rbx + 8]
    mov rdi, [rbx]
    call cant_def_void

    vvnfdL6:

    mov rdx, rax
    mov rdi, [rbp - 24]
    mov rbx, [rbp - 40]
    mov rsi, [rbx]
    call st_add_var

    dec qword [rbp - 32]

    vvnfdL2:
    cmp qword [rbp - 32], 0
    jge vvnfdL1

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    valcontext [rbp - 24], 0, [rbp - 56], 0
    mov rsi, rax
    call valvisit

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 2]
    mov rbx, [rbx + 2]
    cmp rbx, 0
    je vvnfdL4

    mov rbx, [rbx]
    cmp word [rbx], NODE_CONTROL
    je vvnfdL3

    vvnfdL4:

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    call last_stmt_return

    vvnfdL3:
    leave
    ret

valvisit_node_control: ; void* valvisit_node_control(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 2]
    mov rdi, [rbx + 2]
    mov rsi, return_
    call strcmp
    cmp al, 0
    je vvnrL3

    mov rbx, [rbp - 16]
    cmp qword [rbx + 16], 0
    jne vvnrL2

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call cant_ret_outside_func

    vvnrL2:

    mov rbx, [rbp - 16]
    mov rcx, [builtin_types]
    cmp qword [rbx + 16], rcx
    jne vvnrL1
    mov rbx, [rbp - 8]
    cmp qword [rbx + 10], 0
    je vvnrL4

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call return_val_but_void

    vvnrL1:

    mov rbx, [rbp - 8]
    cmp qword [rbx + 10], 0
    jne vvnrL5

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call no_return_val

    vvnrL5:

    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call valvisit

    mov [rbp - 24], rax
    mov rdi, rax
    mov rbx, [rbp - 16]
    mov rsi, [rbx + 16]
    call type_equals
    cmp al, 0
    jne vvnrL6

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call type_mismatch

    vvnrL6:

    mov rax, [rbp - 24]
    leave 
    ret
    vvnrL4:
    mov rax, [builtin_types]
    leave
    ret
    vvnrL3:
    mov rbx, [rbp - 16]
    cmp byte [rbx + 24], 0
    jne vvnrL7

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call outside_loop

    vvnrL7:

    leave
    ret

valvisit_node_func_call: ; void* valvisit_node_func_call(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 40
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 2]
    mov dl, 1
    call st_get_symbol

    cmp rax, -1
    jne vvnfcL2

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call not_defined

    vvnfcL2:

    mov rbx, [rax + 8]
    cmp word [rbx], FUNC
    je vvnfcL1

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call isnotfunc
    
    vvnfcL1:
    mov [rbp - 24], rbx
    mov rcx, [rbp - 8]
    mov ecx, [rcx + 18]
    cmp qword [rbx + 2], rcx
    je vvnfcL3

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rbx, [rbp - 24]
    mov rsi, [rbx + 2]
    mov rbx, [rbp - 8]
    mov edx, [rbx + 18]
    call expect_n_params

    vvnfcL3:
    cmp qword [rbx + 2], 0
    je vvnfcL4

    mov rbx, [rbp - 24]
    mov rbx, [rbx + 2]
    mov [rbp - 32], rbx
    dec qword [rbp - 32]

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    mov [rbp - 40], rbx

    vvnfcloop:
    mov rbx, [rbp - 40]
    mov rdi, [rbx]
    mov rsi, [rbp - 16]
    call valvisit
    mov r14, rax

    mov rcx, [rbp - 24]
    mov rcx, [rcx + 10]
    mov rbx, [rbp - 32]
    imul rbx, 32
    lea rbx, [rcx + rbx]
    mov rdi, [rbx + 8]
    mov rbx, [rbp - 16]
    mov rsi, [rbx]
    call get_real_type

    mov rdi, rax
    mov rsi, r14
    call type_equals
    cmp al, 0
    jne vvnfcL5

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rcx, [rbp - 24]
    mov rcx, [rcx + 10]
    mov rbx, [rbp - 32]
    imul rbx, 32
    lea rbx, [rcx + rbx]
    mov rbx, [rbx]
    mov rsi, [rbx + 2]
    call type_mismatch_param

    vvnfcL5:

    dec qword [rbp - 32]
    mov rbx, qword [rbp - 40]
    cmp qword [rbx + 8], 0
    je vvnfcL4
    mov rbx, qword [rbx + 8]
    mov qword [rbp - 40], rbx
    jmp vvnfcloop
    vvnfcL4:

    mov rbx, [rbp - 24]
    mov rax, [rbx + 18]
    leave
    ret

valvisit_node_ptr_ass: ; void valvisit_node_ptr_ass(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call valvisit
    mov [rbp - 32], rax

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rbx, [rbp - 16]
    valcontext [rbx], [rbp - 32], 0, 0
    mov rsi, rax
    call valvisit
    mov [rbp - 24], rax

    mov rdi, [rbp - 24]
    mov rsi, [rbp - 32]
    call type_equals
    cmp al, 0
    jne vvnpaL1
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    mov rdi, [rbx + 2]
    call type_mismatch

    vvnpaL1:

    leave
    ret

valvisit_node_cast: ; void* valvisit_node_cast(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call valvisit
    mov [rbp - 24], rax

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rbx, [rbp - 16]
    mov rsi, [rbx]
    call get_real_type
    mov [rbp - 32], rax

    cmp rax, -1
    jne vvncL2

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 2]
    mov rdi, [rbx]
    call is_not_a_type
    
    vvncL2:

    mov rdi, [rbp - 24]
    mov rsi, [rbp - 32]
    call can_convert_expl
    cmp al, 0
    jne vvncL1
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 2]
    mov rdi, [rbx]
    call illegal_cast

    vvncL1:
    mov rax, [rbp - 32]
    leave
    ret

valvisit_node_struct: ; void valvisit_node_struct(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 36
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    cmp [pass], 0
    je vvnsL8
    leave
    ret
    vvnsL8:

    mov dword [rbp - 36], 0
    mov qword [rbp - 32], 0
    mov qword [rbp - 24], 0
    jmp vvnsL2
    vvnsL1:
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    mov rcx, [rbp - 24]
    imul rcx, 32
    lea r15, [rbx + rcx]
    push r15
    mov rdi, [r15 + 8]
    mov rbx, [rbp - 16]
    mov rsi, [rbx]
    call get_real_type
    cmp rax, -1
    jne vvnsL3

    mov rbx, [r15 + 8]
    mov rdi, [rbx]
    call is_not_a_type

    vvnsL3:
    cmp rax, [builtin_types]
    jne vvnsL4

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 18]
    mov rdi, [rbx]
    call cant_def_void

    vvnsL4:
    pop r15

    mov r12d, [rbp - 36]

    cmp word [rax], BUILTIN_TYPE
    je vvnsL7
    cmp word [rax], _PTR
    jne vvnsL5

    vvnsL7:
    mov ebx, [rax + 2]
    movzx ebx, byte [sizes + ebx]
    add dword [rbp - 36], ebx
    jmp vvnsL6
    vvnsL5:

    mov ebx, [rax + 10]
    add dword [rbp - 36], ebx
    vvnsL6:

    mov r14, [r15]
    mov r13, rax
    field r14, r13, r12d
    mov r14, rax

    linked_list_item r14
    mov rcx, [rbp - 32]
    mov [rax + 8], rcx
    mov [rbp - 32], rax

    inc qword [rbp - 24]

    vvnsL2:
    mov rbx, [rbp - 24]
    mov rcx, [rbp - 8]
    cmp rbx, [rcx + 18]
    jl vvnsL1

    struct [rbp - 32], dword [rbp - 36]
    mov [rbp - 32], rax

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 2]
    mov rdx, [rbp - 32]
    call st_add_struct

    leave
    ret

valvisit_node_struct_ini: ; void* valvisit_node_struct_ini(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 40
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 16]
    cmp qword [rbx + 8], 0
    jne vvnsiL1
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    call cant_infer_type
    vvnsiL1:

    mov rbx, [rbx + 8]
    cmp word [rbx], STRUCT
    je vvnsiL2
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    call invalid_construct
    vvnsiL2:

    mov rcx, [rbp - 8]
    mov [rcx + 18], rbx

    mov rbx, [rbp - 8]
    mov rcx, [rbx + 18]
    mov rbx, [rbx + 2]
    mov rcx, [rcx + 2]
    mov [rbp - 24], rbx
    mov [rbp - 32], rcx

    vvnsiL3:
    mov rbx, [rbp - 24]
    mov rdi, [rbx]
    mov r15, [rbp - 16]
    mov r14, [rbp - 32]
    mov r14, [r14]
    valcontext [r15], [r14 + 8], 0, 0
    mov rsi, rax
    call valvisit
    mov [rbp - 40], rax

    mov rdi, [rbp - 40]
    mov rbx, [rbp - 32]
    mov rbx, [rbx]
    mov rsi, [rbx + 8]
    call type_equals
    cmp al, 0
    jne vvnsiL5

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    call type_mismatch

    vvnsiL5:
    
    vvnsiL4:
    mov rbx, [rbp - 24]
    mov rbx, [rbx + 8]
    mov [rbp - 24], rbx
    cmp rbx, 0
    je vvnsiL6
    mov rbx, [rbp - 32]
    mov rbx, [rbx + 8]
    mov [rbp - 32], rbx
    cmp rbx, 0
    jne vvnsiL3

    vvnsiL6:

    mov rbx, [rbp - 16]
    mov rax, [rbx + 8]
    leave
    ret

valvisit_node_field_access: ; void* valvisit_node_field_access(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 36
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call valvisit
    mov [rbp - 24], rax

    cmp word [rax], STRUCT
    je vvnfaL1
    cmp word [rax], _PTR
    jne vvnfaL2
    cmp word [rax + 6], 1
    jg vvnfaL2

    mov rbx, [rax + 14]
    mov [rbp - 24], rbx
    cmp word [rbx], STRUCT
    mov rbx, [rbp - 8]
    mov byte [rbx + 26], 1
    je vvnfaL1

    vvnfaL2:
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call builtin_no_field

    vvnfaL1:

    mov rbx, [rbp - 24]
    mov rbx, [rbx + 2]
    mov [rbp - 32], rbx

    vvnfaL3:
    mov rbx, [rbp - 32]
    mov rbx, [rbx]
    mov rax, [rbp - 8]
    mov [rax + 18], rbx

    mov rbx, [rbx]
    mov rdi, [rbx + 2]
    mov rcx, [rbp - 8]
    mov rcx, [rcx + 2]
    mov rsi, [rcx + 2]
    call strcmp
    cmp al, 0
    jne vvnfaL5

    vvnfaL4:
    mov rbx, [rbp - 32]
    mov rbx, [rbx + 8]
    mov [rbp - 32], rbx
    cmp rbx, 0
    jne vvnfaL3

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call invalid_field

    vvnfaL5:

    mov rbx, [rbp - 32]
    mov rbx, [rbx]
    mov rax, [rbx + 8]
    leave
    ret

valvisit_node_field_assign: ; void* valvisit_node_field_assign(void* node, valcontext* context)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call valvisit
    mov [rbp - 32], rax

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rbx, [rbp - 16]
    valcontext [rbx], [rbp - 32], 0, 0
    mov rsi, rax
    call valvisit
    mov [rbp - 24], rax

    mov rdi, [rbp - 32]
    mov rsi, [rbp - 24]
    call type_equals
    cmp al, 0
    jne vvnfa_L1

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    mov rdi, [rbx + 2]
    call type_mismatch

    vvnfa_L1:
    mov rax, [rbp - 32]
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
    dq valvisit_node_asm
    dq valvisit_node_func_def
    dq valvisit_node_control
    dq valvisit_node_func_call
    dq valvisit_node_ptr_ass
    dq valvisit_node_cast
    dq valvisit_node_struct
    dq valvisit_node_struct_ini
    dq valvisit_node_field_access
    dq valvisit_node_field_assign

; returns a boolean value depeding on if you can convert type a to type b
can_convert_expl: ; bool can_convert_expl(void* a, void* b)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rcx, [rbp - 16]
    cmp word [rbx], _PTR
    je cceL1

    cmp word [rbx], STRUCT
    je cceL2
    cmp word [rcx], STRUCT
    je cceL2
    jmp cceL3

    cceL1:
    cmp word [rcx], _PTR
    je cceL3
    cmp word [rcx], BUILTIN_TYPE
    jne cceL2

    cceL3:
    mov al, 1
    leave
    ret
    cceL2:
    mov al, 0
    leave
    ret

get_real_type: ; void* get_real_type(type* type, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 24], rsi

    mov rdi, types
    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    mov rsi, [rbx + 2]
    call contains_str

    cmp al, 0
    je grtL1

    mov rdi, types
    mov rbx, [rbp - 8]
    mov rbx, [rbx]
    mov rsi, [rbx + 2]
    call index_of

    mov rbx, [builtin_types + rax * 8]
    mov [rbp - 16], rbx
    jmp grtL3

    grtL1:
    mov rdi, [rbp - 24]
    mov rbx, [rbp - 8]
    mov rsi, [rbx]
    call st_get_symbol
    cmp rax, -1 
    je grtL4
    mov rax, [rax + 8]
    cmp word [rax], STRUCT
    mov [rbp - 16], rax
    grtL3:

    mov rbx, [rbp - 8]
    mov r15, [rbx + 8]

    cmp r15, 0
    je grtL2

    ptr r15, [rbp - 16]
    mov [rbp - 16], rax

    grtL2:
    mov rax, [rbp - 16]
    leave
    ret

    grtL4:
    mov rax, -1
    leave
    ret