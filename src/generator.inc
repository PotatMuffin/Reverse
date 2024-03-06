generate: ; void generate(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    open [output], default_open_flags, default_open_mode
    mov [outputfd], rax

    fprintmany [outputfd], _format, _segment, _executable, _entry
    fprintmany [outputfd], _prologue

    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
    call visit_node_root

    mov rdi, [outputfd]
    mov rsi, _exit
    call fprint

    close [outputfd]

    leave
    ret

visit: ; void* visit(void* node, symbol_table* table, bool lhs)
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

visit_node_root: ; void visit_node_root(void* node, symbol_table* table)
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

visit_node_binop: ; void* visit_node_binop(void* node, symbol_table* table, bool lhs)
    push rbp
    mov rbp, rsp
    sub rsp, 57
    mov [rbp - 8], rdi
    mov [rbp - 57], rsi
    mov [rbp - 25], dl

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov rsi, [rbp - 57]
    mov dl, 1
    call visit
    mov [rbp - 16], rax

    cmp byte [rbp - 25], 0
    je vnbL2
    ; move result of left child into rsi if that child is a unop or binop and the current node is on the left hand side
    ; of its parent
    mov rbx, [rbp - 8]
    mov rbx, qword [rbx + 2]
    cmp word [rbx], NODE_UNOP
    je vnbl9
    cmp word [rbx], NODE_BINOP
    je vnbl9
    jmp vnbL2

    vnbl9:
    reg rsi_, qword_
    mov qword [rbp - 33], rax
    mov rdi, [rbp - 33]
    mov rsi, [rbp - 16]
    call mov_to_reg
    jmp vnbL1

    vnbL2:
    reg rax_, qword_
    mov qword [rbp - 33], rax
    vnbL1:

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 18]
    mov rsi, [rbp - 57]
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

    ; move result of left child into rax if it's not already in rsi
    mov rbx, [rbp - 33]
    cmp dword [rbx + 2], rax_
    jne vnbL8
    mov rdi, [rbp - 33]
    mov rsi, [rbp - 16]
    call mov_to_reg
    vnbL8:

    mov rdi, [rbp - 33]
    mov rsi, [rbp - 41]
    mov rbx, [rbp - 8]
    mov rdx, [rbx + 10]
    call reg_binop

    mov rax, [rbp - 33]

    leave
    ret

visit_node_lit: ; void* visit_node_lit(void* node)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    mov rbx, qword [rbp - 8]
    mov rbx, qword [rbx + 2]
    mov rdi, [rbx + 2]
    call itoa
    mov rdi, rax

    immvalue rdi

    leave
    ret

visit_node_unop: ; void* visit_node_unop(void* node, symbol_table* table)
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
    mov rdi, rax
    mov rsi, [rbp - 24]
    call mov_to_reg

    fprintmany [outputfd], _neg, _rax, newline
    mov rax, [rbp - 32]
    
    leave
    ret

visit_node_vardef: ; void* visit_node_vardef(void* node, symbol_table* table)
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

    mov rdi, [rbp - 16]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 10]
    call st_get_var_pos
    mov [rbp - 32], rax

    mempos rbp_, [rbp - 32]

    mov rdi, rax
    mov rsi, [rbp - 24]
    call mov_to_mempos

    leave
    ret

visit_node_var: ; void* visit_node_var(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rdi, [rbp - 16]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 2]
    call st_get_var_pos
    mov r15, rax
    mempos rbp_, r15d

    leave
    ret

visit_node_if: ; void* visit_node_if(void* node, symbol_table* table)
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
    call mov_imm_to_reg

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

    fprintmany [outputfd], if_, [rbp - 24], semi, newline

    mov rbx, [rbp - 8]
    cmp qword [rbx + 18], 0
    je vniL3

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 18]
    mov rsi, [rbp - 16]
    call visit

    fprintmany [outputfd], if_, [rbp - 40], semi, newline

    vniL3:

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