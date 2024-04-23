generate: ; void generate(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov r15, [memory_setting]
    shr r15, 3 ; rbx \ 8
    check_memory_text r15
    sub [memory_text_pointer], r15
    mov byte [memory_text_pointer], 0

    context [rbp - 16], 0, 0, 0
    mov [rbp - 24], rax

    open [output], default_open_flags, default_open_mode
    mov [outputfd], rax

    fprintmany [outputfd], _format, _segment, _executable, _entry

    mov rdi, [rbp - 8]
    mov rsi, [rbp - 24]
    call visit_node_root

    mov rdi, [outputfd]
    mov rsi, _exit
    call fprint

    fprintmany [outputfd], _segment, _writeable
    fprintmany [outputfd], [memory_text_pointer]

    mov rbx, [rbp - 16]
    cmp dword [rbx + 8], 0
    je gL1
    movsxd rdi, dword [rbx + 8]
    neg rdi
    call itoa
    mov r15, rax

    fprintmany [outputfd], _mem, space, _rb, r15, newline

    gL1:

    close [outputfd]

    leave
    ret

visit: ; void* visit(void* node, context* context, bool lhs)
    push rbp
    mov rbp, rsp
    sub rsp, 17
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov [rbp - 17], dl

    mov rbx, [rbp - 8]
    movzx rcx, word [rbx]
    cmp rcx, node_count
    mov rdi, rcx
    jg unimplemented_node

    mov rdi, qword [rbp - 8]
    mov rsi, [rbp - 16]
    mov dl, [rbp - 17]
    mov rax, qword [genlookuptable + rcx * 8]
    call rax

    leave 
    ret

visit_node_root: ; void visit_node_root(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov qword [rbp - 8], rdi
    mov qword [rbp - 16], rsi

    mov rbx, qword [rbp - 8]
    cmp qword [rbx + 10], 0
    je vnrRet

    mov rbx, qword [rbx + 2]
    mov qword [rbp - 24], rbx

    vnrloop1:
    mov rbx, qword [rbp - 24]
    cmp qword [rbx + 8], 0
    je vnrloop2
    mov rbx, qword [rbx + 8]
    mov qword [rbp - 24], rbx
    jmp vnrloop1
    
    vnrloop2:
    nop
    mov rbx, [memory_pointer]
    mov [rbp - 32], rbx

    mov rbx, qword [rbp - 24]
    mov rdi, qword [rbx]
    mov rsi, [rbp - 16]
    mov dl, 1
    call visit

    mov rbx, [rbp - 32]
    mov [memory_pointer], rbx

    mov rbx, qword [rbp - 24]
    cmp qword [rbx + 16], 0
    je vnrRet
    mov rbx, qword [rbx + 16]
    mov qword [rbp - 24], rbx
    jmp vnrloop2
    
    vnrRet:
    leave
    ret

visit_node_binop: ; void* visit_node_binop(void* node, context* context, bool lhs)
    push rbp
    mov rbp, rsp
    sub rsp, 57
    mov [rbp - 8], rdi
    mov [rbp - 57], rsi
    mov [rbp - 25], dl

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    cmp word [rbx], TT_LOGAND
    je vnbL5
    cmp word [rbx], TT_LOGOR
    je vnbL5
    jmp vnbL6
    vnbL5:

    mov rdi, [rbp - 8]
    mov rsi, [rbp - 57]
    call logical_binop

    leave
    ret

    vnbL6:

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov r15, [rbp - 57]
    context [r15], 0, 0, 0
    mov rsi, rax
    mov dl, 1
    call visit
    mov [rbp - 16], rax

    cmp byte [rbp - 25], 0
    je vnbL2
    ; push the result of the left child 
    ; if that child is a unop or binop and the current node is on the left hand side of its parent
    mov rbx, [rbp - 8]
    mov rbx, qword [rbx + 2]
    cmp word [rbx], NODE_UNOP
    je vnbL10
    cmp word [rbx], NODE_BINOP
    je vnbL10
    cmp word [rbx], NODE_FUNC_CALL
    jne vnbL2
    vnbL10:
    mov rbx, [rbp - 8]
    mov rbx, qword [rbx + 18]
    cmp word [rbx], NODE_UNOP
    je vnbL9
    cmp word [rbx], NODE_BINOP
    je vnbL9
    cmp word [rbx], NODE_FUNC_CALL
    jne vnbL2

    vnbL9:
    mov qword [rbp - 33], 0
    mov rdi, [rbp - 16]
    call value_to_string
    mov r15, rax
    fprintmany [outputfd], _push, r15, newline 
    jmp vnbL1

    vnbL2:
    reg rax_, qword_
    mov qword [rbp - 33], rax
    vnbL1:

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 18]
    mov r15, [rbp - 57]
    context [r15], 0, 0, 0
    mov rsi, rax
    mov dl, 0
    call visit
    mov [rbp - 24], rax

    cmp byte [rbp - 25], 0
    je vnbL3

    reg rdi_, qword_
    mov qword [rbp - 41], rax
    jmp vnbL4

    vnbL3:
    reg rcx_, qword_
    mov qword [rbp - 41], rax
    vnbL4:

    ; move result of right child to rdi or rcx
    mov rdi, [rbp - 41]
    mov rsi, [rbp - 24]
    call mov_to_reg

    ; move result of left child into rax if it's not already pushed onto the stack
    cmp qword [rbp - 33], 0
    je vnbL8
    mov rdi, [rbp - 33]
    mov rsi, [rbp - 16]
    call mov_to_reg
    jmp vnbL7
    vnbL8:
    reg rax_, qword_
    mov [rbp - 33], rax
    mov rdi, rax
    call value_to_string
    mov r15, rax
    fprintmany [outputfd], _pop, r15, newline

    vnbL7:

    mov rdi, [rbp - 33]
    mov rsi, [rbp - 41]
    mov rbx, [rbp - 8]
    mov rdx, [rbx + 10]
    call reg_binop

    leave
    ret

visit_node_lit: ; void* visit_node_lit(void* node)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 2]
    cmp word [rbx], TT_INTLIT
    jne vnlL1
    mov rbx, [rbp - 8]
    immvalue [rbx + 2]
    leave
    ret
    vnlL1:
    mov rdi, [str_label]
    inc qword [str_label]
    call itoa
    mov r15, rax
    pushmany 0, r15, _str
    mov rdi, rsp
    push rsp
    call concat_str
    mov [rbp - 16], rax
    pop rsp 

    mov r15, [rbp - 8]
    mov r15, [r15 + 2]
    pushmany 0, _db, space, qword [rbp - 16]
    mov rdi, rsp
    push rsp
    call concat_str
    pop rsp
    mov rdi, rax
    mov rax, [data_occupied]
    mov rbx, [memory_text_pointer]
    lea rsi, [rax + rbx]
    call str_copy
    dec rax 
    add [data_occupied], rax
    mov qword [rbp - 24], 0

    vnlloop:
        cmp qword [rbp - 24], 0
        je vnlL2
        mov rdi, comma
        mov rax, [data_occupied]
        mov rbx, [memory_text_pointer]
        lea rsi, [rax + rbx]
        call str_copy
        dec rax
        add [data_occupied], rax
        vnlL2:

        mov rbx, [rbp - 8]
        mov rbx, [rbx + 2]
        mov rbx, [rbx + 2]
        mov rcx, [rbp - 24]
        movzx rdi, byte [rbx + rcx]
        call itoa
        mov rdi, rax
        mov rax, [data_occupied]
        mov rbx, [memory_text_pointer]
        lea rsi, [rax + rbx]
        call str_copy
        dec rax
        add [data_occupied], rax
        mov rbx, [rbp - 8]
        mov rbx, [rbx + 2]
        mov rbx, [rbx + 2]
        mov rcx, [rbp - 24]
        inc qword [rbp - 24]
        cmp byte [rbx + rcx],0
        jne vnlloop
    mov rdi, newline
    mov rax, [data_occupied]
    mov rbx, [memory_text_pointer]
    lea rsi, [rax + rbx]
    call str_copy
    dec rax
    add [data_occupied], rax
    label [rbp - 16]
    leave
    ret

visit_node_unop: ; void* visit_node_unop(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, qword [rbp - 8]
    mov rdi, qword [rbx + 10]
    mov rsi, [rbp - 16]
    mov dl, 1
    call visit
    mov [rbp - 24], rax

    reg rax_, qword_
    mov [rbp - 32], rax

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 2]
    cmp word [rbx], TT_AND
    jne vnuL1

    mov rdi, [rbp - 32]
    call value_to_string
    mov r15, rax
    mov rdi, [rbp - 24]
    call value_to_string
    mov r14, rax
    fprintmany [outputfd], _lea, r15, comma, r14, newline
    mov rax, [rbp - 32]
    leave
    ret
    vnuL1:
    cmp word [rbx], TT_STAR
    jne vnuL2
    mov rdi, [rbp - 32]
    mov rsi, [rbp - 24]
    call mov_to_reg

    mov rdi, [rbp - 8]
    mov rbx, [rbp - 16]
    mov rsi, [rbx]
    call valvisit
    mov r15d, dword [rax + 2]
    mov rbx, [rbp - 32]
    mov r14d, dword [rbx + 2]
    mempos r14d, r15d, 0
    leave
    ret
    vnuL2:

    mov rdi, [rbp - 32]
    mov rsi, [rbp - 24]
    call mov_to_reg

    mov rdi, [rbp - 32]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 2]
    call reg_unop

    leave
    ret

visit_node_vardef: ; void* visit_node_vardef(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov rsi, [rbp - 16]
    mov dl, 1
    call visit
    mov [rbp - 24], rax

    reg rax_, qword_
    mov rdi, rax
    mov rsi, [rbp - 24]
    mov dx, MEMPOS
    call mov_value_to_reg
    mov [rbp - 24], rax

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 10]
    call st_get_var_pos
    mov [rbp - 32], rax

    mov rdi, [rbp - 32]
    mov rsi, [rbp - 24]
    call mov_to_mempos

    mov rax, [rbp - 32]
    leave
    ret

visit_node_var: ; void* visit_node_var(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 2]
    call st_get_var_pos

    leave
    ret

visit_node_if: ; void visit_node_if(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 40
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rdi, [if_label]
    call itoa
    mov [rbp - 24], rax
    inc qword [if_label]

    mov rbx, [rbp - 8]
    cmp qword [rbx + 18], 0
    je vniL1

    mov rdi, [if_label]
    call itoa
    mov [rbp - 40], rax
    inc qword [if_label]
    vniL1:

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rsi, [rbp - 16]
    call visit
    mov [rbp - 32], rax

    reg rax_, qword_
    mov rdi, rax
    mov rsi, [rbp - 32]
    mov dx, IMMVALUE
    call mov_value_to_reg

    mov rdi, rax
    call value_to_string
    mov r15, rax

    fprintmany [outputfd], _cmp, r15, comma, zero, newline
    fprintmany [outputfd], _je, if_, [rbp - 24], newline

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call visit

    mov rbx, [rbp - 8]
    cmp qword [rbx + 18], 0
    je vniL2

    fprintmany [outputfd], _jmp, if_, [rbp - 40], newline

    vniL2:

    fprintmany [outputfd], if_, [rbp - 24], colon, newline

    mov rbx, [rbp - 8]
    cmp qword [rbx + 18], 0
    je vniL3

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 18]
    mov rsi, [rbp - 16]
    call visit

    fprintmany [outputfd], if_, [rbp - 40], colon, newline

    vniL3:

    leave
    ret

visit_node_while: ; void visit_node_while(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rdi, [while_label]
    call itoa
    mov [rbp - 24], rax
    inc qword [while_label]

    mov rdi, [while_label]
    call itoa
    mov [rbp - 32], rax
    inc qword [while_label]

    mov rdi, [while_label]
    call itoa
    mov [rbp - 40], rax
    inc qword [while_label]

    fprintmany [outputfd], _jmp, while_, [rbp - 32], newline

    fprintmany [outputfd], while_, [rbp - 24], colon, newline

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call visit

    fprintmany [outputfd], while_, [rbp - 32], colon, newline

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rsi, [rbp - 16]
    call visit
    mov [rbp - 48], rax

    reg rax_, qword_
    mov rdi, rax
    mov rsi, [rbp - 48]
    mov dx, IMMVALUE
    call mov_value_to_reg

    mov rdi, rax
    call value_to_string
    mov r15, rax

    fprintmany [outputfd], _cmp, r15, comma, zero, newline
    fprintmany [outputfd], _jne, while_, [rbp - 24], newline
    fprintmany [outputfd], while_, [rbp - 40], colon, newline

    leave
    ret

visit_node_for: ; void visit_node_for(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 56
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    
    mov rdi, [for_label]
    call itoa
    mov [rbp - 24], rax
    inc qword [for_label]

    mov rdi, [for_label]
    call itoa
    mov [rbp - 32], rax
    inc qword [for_label]

    mov rdi, [for_label]
    call itoa
    mov [rbp - 40], rax
    inc qword [for_label]

    mov rdi, [for_label]
    call itoa
    mov [rbp - 48], rax
    inc qword [for_label]

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rsi, [rbp - 16]
    call visit

    fprintmany [outputfd], _jmp, for_, [rbp - 40], newline
    fprintmany [outputfd], for_, [rbp - 24], colon, newline

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 26]
    mov rsi, [rbp - 16]
    call visit

    fprintmany [outputfd], for_, [rbp - 32], colon, newline

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 18]
    mov rsi, [rbp - 16]
    call visit

    fprintmany [outputfd], for_, [rbp - 40], colon, newline

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call visit
    mov [rbp - 56], rax

    reg rax_, qword_
    mov rdi, rax
    mov rsi, [rbp - 56]
    mov dx, IMMVALUE
    call mov_value_to_reg

    mov rdi, rax
    call value_to_string
    mov [rbp - 56], rax

    fprintmany [outputfd], _cmp, [rbp - 56], comma, zero, newline
    fprintmany [outputfd], _jne, for_, [rbp - 24], newline

    fprintmany [outputfd], for_, [rbp - 48], colon, newline

    leave
    ret

visit_node_asm: ; void visit_node_asm(void* node)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi
    mov rbx, [rbp - 8]
    mov r15, [rbx + 2]

    fprintmany [outputfd], qword [r15 + 2], newline

    leave
    ret

visit_node_func_def: ; void visit_node_func_def(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 10]
    mov dl, 0
    call st_get_symbol
    mov rbx, [rax + 8]
    mov rcx, [rbx + 26]
    mov [rbp - 48], rcx

    mov edi, dword [rbx + 34]  
    call itoa
    mov [rbp - 24], rax

    fprintmany [outputfd], _jmp, _func, [rbp - 24], _end, newline
    fprintmany [outputfd], _func, [rbp - 24], colon, newline
    fprintmany [outputfd], _prologue
    mov rbx, [rbp - 48]
    cmp dword [rbx + 8], 0
    je vnfdL1
    movsxd rdi, dword [rbx + 8]
    neg rdi
    call itoa
    mov r15, rax
    fprintmany [outputfd], _sub, _rsp, comma, r15, newline

    mov qword [rbp - 32], 0

    jmp vnfdL3
    vnfdL2:
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 18]
    mov rax, [rbp - 32]
    imul rax, 32
    lea rbx, [rbx + rax]
    mov [rbp - 40], rbx

    mov rdi, [rbp - 48]
    mov rbx, [rbp - 40]
    mov rsi, [rbx]
    call st_get_var_pos
    mov r15, rax
    
    mov rbx, [rbp - 32]
    cmp rbx, 6
    jge vnfdL4
    reg dword [param_regs + rbx * 4], qword_
    mov rdi, r15
    mov rsi, rax
    call mov_to_mempos
    jmp vnfdL5
    vnfdL4:

    reg rax_, qword_
    mov r14, rax

    mov r13, 16
    mov rbx, [rbp - 32]
    sub rbx, 6
    imul rbx, 8
    add r13, rbx

    mempos rbp_, qword_, r13d

    mov rdi, r14
    mov rsi, rax
    call mov_to_reg

    mov rdi, r15
    mov rsi, rax
    call mov_to_mempos

    vnfdL5:
    inc qword [rbp - 32]

    vnfdL3:
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 26]
    cmp qword [rbp - 32], rbx
    jl vnfdL2
    vnfdL1:

    context [rbp - 48], 0, 0, 0
    mov r15, rax
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rsi, r15
    call visit

    fprintmany [outputfd], _func, [rbp - 24], _end, colon, newline

    leave
    ret

visit_node_return: ; void visit_node_return(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    cmp qword [rbx + 10], 0
    je vnrL1

    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call visit
    mov r15, rax
    reg rax_, qword_
    mov rdi, rax
    mov rsi, r15
    call mov_to_reg
    vnrL1:

    fprintmany [outputfd], _epilogue

    leave
    ret

visit_node_func_call: ; void* visit_node_func_call(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 37
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    mov qword [rbp - 24], rbx
    mov qword [rbp - 32], 0

    mov byte [rbp - 33], 0 ; stored first param in r15
    mov byte [rbp - 34], 0 ; stored second param in r14
    mov byte [rbp - 35], 0 ; stored third param in r13
    mov byte [rbp - 36], 0 ; stored fourth param in r12
    mov byte [rbp - 37], 0 ; stored fifth param in rbx

    mov rbx, [rbp - 8]
    cmp dword [rbx + 18], 0
    je vnfcL1
    
    ; ===================== first loop for putting the first six parameters in correct registers =================
    vnfcloop:
    mov rbx, qword [rbp - 24]
    mov rdi, qword [rbx]
    mov rsi, [rbp - 16]
    call visit
    mov r14, rax

    mov r15, [rbp - 32]
    cmp r15, 6
    jge vnfcL3

    mov rdi, [rbp - 8]
    mov rsi, r15
    call should_store_param
    cmp al, 0
    je vnfcL4

    mov rcx, rbp
    sub rcx, r15
    sub rcx, 33

    mov byte [rcx], 1

    reg [param_save_regs + r15 * 4], qword_
    mov rdi, rax
    mov rsi, r14
    call mov_to_reg
    jmp vnfcl5
    vnfcL4:

    reg [param_regs + r15 * 4], qword_

    mov rdi, rax
    mov rsi, r14
    call mov_to_reg

    vnfcl5:
    inc qword [rbp - 32]

    mov rbx, qword [rbp - 24]
    cmp qword [rbx + 8], 0
    je vnfcL1
    mov rbx, qword [rbx + 8]
    mov qword [rbp - 24], rbx
    jmp vnfcloop

    vnfcL3:
    mov rbx, [rbp - 8]
    mov ebx, dword [rbx + 18]
    mov [rbp - 32], rbx

    ; =================== second loop for pushing the parameters in reverse ====================
    vnfcloop1:
    mov rbx, qword [rbp - 24]
    cmp qword [rbx + 8], 0
    je vnfcloop2
    mov rbx, qword [rbx + 8]
    mov qword [rbp - 24], rbx
    jmp vnfcloop1

    vnfcloop2:
    cmp qword [rbp - 32], 6
    jle vnfcL1

    mov rbx, qword [rbp - 24]
    mov rdi, qword [rbx]
    mov rsi, [rbp - 16]
    call visit
    mov r14, rax

    cmp word [r14], MEMPOS
    jne vnfcL2
    cmp dword [r14 + 6], qword_
    je vnfcL2

    reg rax_, qword_
    mov rdi, rax
    mov rsi, r14
    call mov_to_reg
    mov r14, rax
    
    vnfcL2:

    mov rdi, r14
    call value_to_string
    mov r15, rax

    fprintmany [outputfd], _push, r15, newline

    dec qword [rbp - 32]

    mov rbx, qword [rbp - 24]
    cmp qword [rbx + 16], 0
    je vnfcL1
    mov rbx, qword [rbx + 16]
    mov qword [rbp - 24], rbx
    jmp vnfcloop2

    ; ==================== end of loops ==================
    vnfcL1: 
    mov qword [rbp - 32], 0
    jmp vnfcL7

    vnfcL6:
    mov rcx, rbp
    sub rcx, [rbp - 32]
    sub rcx, 33
    cmp byte [rcx], 0
    je vnfcL8

    mov rcx, [rbp - 32]
    reg [param_regs + rcx * 4], qword_
    mov rdi, rax
    reg [param_save_regs + rcx * 4], qword_
    mov rsi, rax
    call mov_to_reg
    vnfcL8:

    inc qword [rbp - 32]
    vnfcL7:
    cmp qword [rbp - 32], 6
    jl vnfcL6

    mov rbx, [rbp - 16]
    mov rdi, [rbx]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 2]
    mov dl, 1
    call st_get_symbol
    
    mov rbx, [rax + 8]
    mov edi, [rbx + 34]
    call itoa
    mov r15, rax

    fprintmany [outputfd], _call, _func, r15, newline

    reg rax_, qword_
    leave
    ret

visit_node_ptr_ass: ; void* visit_node_ptr_ass(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov rsi, [rbp - 16]
    call visit
    mov [rbp - 24], rax

    cmp word [rax], REG
    jne vnpaL1

    mov rdi, rax
    call value_to_string
    mov r15, rax
    fprintmany [outputfd], _push, r15, newline
    vnpaL1:

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 10]
    mov rsi, [rbp - 16]
    call visit
    mov [rbp - 32], rax

    mov rbx, [rbp - 24]
    cmp word [rbx], REG
    jne vnpaL2

    reg rcx_, qword_
    mov [rbp - 24], rax
    mov rdi, rax
    call value_to_string
    mov r15, rax
    fprintmany [outputfd], _pop, r15, newline
    vnpaL2:

    reg rdi_, qword_
    mov rdi, rax
    mov rsi, [rbp - 24]
    mov dx, MEMPOS
    call mov_value_to_reg

    mov rdi, [rbp - 32]
    mov rsi, rax
    call mov_to_mempos

    leave
    ret

genlookuptable:
    dq 0
    dq visit_node_root
    dq visit_node_binop
    dq visit_node_lit
    dq visit_node_unop
    dq visit_node_vardef
    dq visit_node_vardef
    dq visit_node_var
    dq visit_node_if
    dq visit_node_while
    dq visit_node_for
    dq visit_node_asm
    dq visit_node_func_def
    dq visit_node_return
    dq visit_node_func_call
    dq visit_node_ptr_ass

mov_to_reg: ; void* mov_to_reg(void* reg, void* value)
    push rbp
    mov rbp, rsp
    sub rsp, 40
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov qword [rbp - 40], _mov

    mov rbx, [rbp - 8]
    cmp word [rbx], REG
    jne loc_not_reg

    mov rbx, [rbp - 16]
    cmp word [rbx], REG
    jne mtrL1
    mov rcx, [rbp - 8]
    mov ecx, [rcx + 2]
    cmp dword [rbx + 2], ecx
    jne mtrL1
    mov rax, [rbp - 8]
    leave
    ret
    mtrL1:
    cmp word [rbx], MEMPOS
    jne mtrL2
    mov rcx, [rbp - 8]
    mov ecx, [rcx + 6]
    cmp dword [rbx + 6], ecx
    je mtrL2
    cmp ecx, qword_
    je mtrL3
    mov rdi, LOGERROR
    mov rsi, unreachable
    call log
    exit_process 1
    mtrL3:
    mov rbx, [rbp - 16]
    cmp dword [rbx + 6], dword_
    jne mtrL4
    mov qword [rbp - 40], _mov
    mov rcx, [rbp - 8]
    reg dword [rcx + 2], dword_
    mov rdi, rax
    call value_to_string
    mov [rbp - 24], rax
    jmp mtrL5
    mtrL4: 
    mov qword [rbp - 40], _movzx
    mtrL2:

    mov rdi, [rbp - 8]
    call value_to_string
    mov [rbp - 24], rax
    mtrL5:

    mov rdi, [rbp - 16]
    call value_to_string
    mov [rbp - 32], rax

    fprintmany [outputfd], [rbp - 40], [rbp - 24], comma, [rbp - 32], newline
    mov rax, [rbp - 8]
    leave
    ret

mov_to_mempos: ; void* mov_to_mempos(void* mempos, void* value)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    cmp word [rbx], MEMPOS
    jne loc_not_mem

    mov rbx, [rbp - 16]
    cmp word [rbx], IMMVALUE
    jne mtmL2

    mov rbx, [rbx + 2]
    cmp qword [rbx + 2], 7FFFFFFFh
    jle mtmL2

    mov rdi, [rbp - 16]
    call value_to_string
    mov r14, rax
    reg rax_, qword_
    mov [rbp - 16], rax
    mov rdi, rax
    call value_to_string
    mov r15, rax
    fprintmany [outputfd], _mov, r15, comma, r14, newline

    mtmL2:

    mov rbx, [rbp - 16]
    cmp word [rbx], REG
    jne mtmL1

    mov rcx, [rbp - 8]
    reg [rbx + 2], [rcx + 6]
    mov [rbp - 16], rax
    mtmL1:

    mov rdi, [rbp - 8]
    call value_to_string
    mov [rbp - 24], rax

    mov rdi, [rbp - 16]
    call value_to_string
    mov [rbp - 32], rax

    fprintmany [outputfd], _mov, [rbp - 24], comma, [rbp - 32], newline
    mov rax, [rbp - 8]
    leave
    ret

mov_value_to_reg: ; void* mov_value_to_reg(void* reg, void* value, short type)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov [rbp - 24], dx

    mov rbx, [rbp - 16]
    mov ax, word [rbp - 24] 
    cmp word [rbx], ax
    je mitrL1
    mov rax, [rbp - 16]
    leave
    ret
    mitrL1:

    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
    call mov_to_reg
    leave
    ret

reg_binop: ; void* reg_binop(void* reg1, void* reg2, token* op)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov [rbp - 24], rdx

    mov rbx, [rbp - 24]
    cmp word [rbx], TT_PLUS
    jne rbL1  
    mov qword [rbp - 32], _add
    jmp rbL4
    rbL1:
    cmp word [rbx], TT_MIN
    jne rbL2
    mov qword [rbp - 32], _sub
    jmp rbL4
    rbL2:
    cmp word [rbx], TT_STAR
    jne rbL3
    mov qword [rbp - 32], _imul
    jmp rbL4
    rbL3:
    cmp word [rbx], TT_AND
    jne rbL14
    mov qword [rbp - 32], _and
    jmp rbL4
    rbL14:
    cmp word [rbx], TT_OR
    jne rbL15
    mov qword [rbp - 32], _or
    jmp rbL4
    rbL15:
    cmp word [rbx], TT_XOR
    jne rbL16
    mov qword [rbp - 32], _xor
    jmp rbL4
    rbL16:
    cmp word [rbx], TT_SHL
    jne rbL17
    mov qword [rbp - 32], _shl
    jmp rbL19
    rbL17:
    cmp word [rbx], TT_SHR
    jne rbL18
    mov qword [rbp - 32], _sar
    jmp rbL19
    rbL18:
    cmp word [rbx], TT_FSLASH
    je rbL6
    cmp word [rbx], TT_PERCENT
    je rbL6
    cmp word [rbx], TT_ANGBRACKL
    jne rbL9
    mov qword [rbp - 32], _setl
    jmp rbL8
    rbL9:
    cmp word [rbx], TT_ANGBRACKR
    jne rbL10
    mov qword [rbp - 32], _setg
    jmp rbL8
    rbL10:
    cmp word [rbx], TT_GTE
    jne rbL11
    mov qword [rbp - 32], _setge
    jmp rbL8
    rbL11:
    cmp word [rbx], TT_LTE
    jne rbL12
    mov qword [rbp - 32], _setle
    jmp rbL8
    rbL12:
    cmp word [rbx], TT_IS
    jne rbL13
    mov qword [rbp - 32], _sete
    jmp rbL8
    rbL13:
    cmp word [rbx], TT_ISNT
    jne unimplemented_op
    mov qword [rbp - 32], _setne
    jmp rbL8
    rbL6:

    mov rbx, [rbp - 8]
    cmp dword [rbx + 2], _rax
    je rbL5

    reg rax_, qword_

    mov rdi, rax
    mov rsi, [rbp - 8]
    call mov_to_reg 
    rbL5:

    fprintmany [outputfd], _cqo,newline
    mov rdi, [rbp - 16]
    call value_to_string
    mov r15, rax
    fprintmany [outputfd], _idiv, r15, newline

    mov rbx, [rbp - 24]
    cmp word [rbx], TT_PERCENT
    je  rbL7
    reg rax_, qword_
    leave
    ret
    rbL7:
    reg rdx_, qword_
    leave
    ret
    rbL19:

    reg rcx_, qword_
    mov rdi, rax
    mov rsi, [rbp - 16]
    call mov_to_reg
    reg rcx_, byte_
    mov [rbp - 16], rax

    rbL4:

    mov rdi, [rbp - 8]
    call value_to_string
    mov r15, rax
    mov rdi, [rbp - 16]
    call value_to_string
    mov r14, rax

    fprintmany [outputfd], [rbp - 32], r15, comma, r14, newline
    mov rax, [rbp - 8]

    leave
    ret
    rbL8:

    mov rdi, [rbp - 8]
    call value_to_string
    mov r15, rax
    mov rdi, [rbp - 16]
    call value_to_string
    mov r14, rax
    mov rcx, [rbp - 8]
    reg [rcx + 2], byte_
    mov rdi, rax
    call value_to_string
    mov r13, rax

    fprintmany [outputfd], _cmp, r15, comma, r14, newline
    fprintmany [outputfd], [rbp - 32], r13, newline
    fprintmany [outputfd], _movzx, r15, comma, r13, newline 
    mov rax, [rbp - 8]

    leave 
    ret

reg_unop: ; void* reg_unop(void* reg, token* op)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rdi, [rbp - 8]
    call value_to_string
    mov [rbp - 24], rax

    mov rbx, [rbp - 16]
    cmp word [rbx], TT_MIN
    jne reL2
    mov qword [rbp - 32], _neg
    jmp reL3
    reL2:
    cmp word [rbx], TT_NOT
    jne reL1
    mov qword [rbp - 32], _not
    reL3:

    fprintmany [outputfd], [rbp - 32], [rbp - 24], newline 
    mov rax, [rbp - 8]
    leave
    ret

    reL1:
    cmp word [rbx], TT_EXCL
    jne unimplemented_op

    mov rcx, [rbp - 8]
    reg [rcx + 2], byte_
    mov rdi, rax
    call value_to_string
    mov r15, rax

    fprintmany [outputfd], _cmp, [rbp - 24], comma, zero, newline
    fprintmany [outputfd], _sete, r15, newline
    fprintmany [outputfd], _movzx, [rbp - 24], comma, r15, newline
    mov rax, [rbp - 8]

    leave
    ret

logical_binop: ; void* logical_binop(void* node, context* context)
    push rbp
    mov rbp, rsp
    sub rsp, 80
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 16]
    cmp qword [rbx + 24], 0
    je laL1
    mov rax, [rbx + 24]
    mov ax, [rax]
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    cmp word [rbx], ax
    jne laL1

    laL12:
    mov rax, [rbx + 8]
    mov [rbp - 40], rax
    mov rax, [rbx + 16]
    mov [rbp - 48], rax
    mov rax, [rbx + 24]
    mov [rbp - 56], rax
    jmp laL2
    laL1:

    mov rdi, [other_label]
    call itoa
    mov [rbp - 40], rax
    inc qword [other_label]

    mov rdi, [other_label]
    call itoa
    mov [rbp - 48], rax
    inc qword [other_label]

    mov rdi, [other_label]
    call itoa
    mov [rbp - 56], rax
    inc qword [other_label]
    laL2:

    reg rax_, qword_
    mov [rbp - 64], rax

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    cmp word [rbx], TT_LOGOR
    je laL9
    mov qword [rbp - 72], _je
    mov rbx, [rbp - 48]
    mov [rbp - 80], rbx
    jmp laL10
    laL9:
    mov qword [rbp - 72], _jne
    mov rbx, [rbp - 40]
    mov [rbp - 80], rbx
    laL10:

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    mov r14, [rbx + 10]
    mov r15, [rbp - 16]
    context [r15], [rbp - 40], [rbp - 48], r14
    mov rsi, rax
    mov dl, 1 
    call visit
    mov [rbp - 24], rax

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 2]
    cmp word [rbx], NODE_BINOP
    jne laL3
    mov rbx, [rbp - 8]
    mov rax, [rbx + 18]
    cmp word [rax], NODE_BINOP
    jne laL3
    mov rax, [rax + 10]
    mov ax, [rax]
    mov rbx, [rbx + 10]
    cmp word [rbx], ax
    je laL4
    laL3:
    mov rdi, [rbp - 64]
    mov rsi, [rbp - 24]
    mov dx, IMMVALUE
    call mov_value_to_reg
    mov [rbp - 24], rax

    mov rdi, rax
    call value_to_string
    mov r15, rax

    fprintmany [outputfd], _cmp, r15, comma, zero, newline
    fprintmany [outputfd], [rbp - 72], L, [rbp - 80], newline
    
    laL4:

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 18]
    mov r14, [rbx + 10]
    mov r15, [rbp - 16]
    context [r15], [rbp - 40], [rbp - 48], r14
    mov rsi, rax
    mov dl, 0
    call visit
    mov [rbp - 32], rax

    mov rbx, [rbp - 8]
    mov rbx, [rbx + 18]
    cmp word [rbx], NODE_BINOP
    jne laL5
    mov rbx, [rbp - 8]
    mov rax, [rbx + 18]
    cmp word [rax], NODE_BINOP
    jne laL5
    mov rax, [rax + 10]
    mov ax, [rax]
    mov rbx, [rbx + 10]
    cmp word [rbx], ax
    je laL6
    laL5:

    mov rdi, [rbp - 64]
    mov rsi, [rbp - 32]
    mov dx, IMMVALUE
    call mov_value_to_reg
    mov [rbp - 32], rax

    mov rdi, rax
    call value_to_string
    mov r15, rax

    fprintmany [outputfd], _cmp, r15, comma, zero, newline
    fprintmany [outputfd], [rbp - 72], L, [rbp - 80], newline
    laL6:

    mov rbx, [rbp - 16]
    cmp qword [rbx + 24], 0
    je laL7
    mov rax, [rbx + 24]
    mov ax, [rax]
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    cmp word [rbx], ax
    je laL8
    laL7:
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    cmp word [rbx], TT_LOGOR
    jne laL11
    fprintmany [outputfd], _jmp, L, [rbp - 48], newline
    laL11:
    fprintmany [outputfd], L, [rbp - 40], colon, newline
    fprintmany [outputfd], _mov, _rax, comma, one, newline 
    fprintmany [outputfd], _jmp, L, [rbp - 56], newline
    fprintmany [outputfd], L, [rbp - 48], colon, newline
    fprintmany [outputfd], _mov, _rax, comma, zero, newline
    fprintmany [outputfd], L, [rbp - 56], colon, newline 

    laL8:

    mov rax, [rbp - 64]
    leave
    ret

should_store_param: ; bool should_store_param(void* node, long index)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    mov [rbp - 24], rbx
    mov qword [rbp - 32], 0

    stploop:
    mov rbx, [rbp - 32]
    cmp rbx, [rbp - 16]
    jle stpL1

    stploop2:
    mov rbx, [rbp - 24]
    mov rbx, [rbx]
    cmp word [rbx], NODE_BINOP
    je stpL3
    cmp word [rbx], NODE_UNOP
    je stpL3
    cmp word [rbx], NODE_FUNC_CALL
    jne stpL2
    stpL3:
    mov al, 1
    leave
    ret
    stpL2:

    mov rbx, [rbp - 24]
    cmp qword [rbx + 8], 0
    je stpRet

    mov rbx, [rbx + 8]
    mov [rbp - 24], rbx
    jmp stploop2

    stpL1:
    inc qword [rbp - 32]
    mov rbx, [rbp - 24]
    cmp qword [rbx + 8], 0
    je stpRet

    mov rbx, [rbx + 8]
    mov [rbp - 24], rbx
    jmp stploop

    stpRet:
    mov al, 0
    leave
    ret