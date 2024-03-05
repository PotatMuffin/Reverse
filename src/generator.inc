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

visit: ; char* visit(void* node, symbol_table* table, bool lhs)
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

visit_node_root: ; char* visit_node_root(void* node, symbol_table* table)
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

visit_node_binop: ; char* visit_node_binop(void* node, symbol_table* table, bool lhs)
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
    mov qword [rbp - 33], _rsi
    fprintmany [outputfd], _mov, [rbp - 33], comma, [rbp - 16], newline
    jmp vnbL1

    vnbL2:

    mov qword [rbp - 33], _rax
    vnbL1:

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 18]
    mov rsi, [rbp - 57]
    mov dl, 0
    call visit
    mov [rbp - 24], rax

    cmp byte [rbp - 25], 0
    je vnbL3

    mov qword [rbp - 41], _rdi
    jmp vnbL4

    vnbL3:
    mov qword [rbp - 41], _rcx
    vnbL4:

    ; move result of right child to rdi or rcx
    fprintmany [outputfd], _mov, [rbp - 41], comma, [rbp - 24], newline

    ; move result of left child into rax if it's not already in rsi
    cmp qword [rbp - 33], _rax
    jne vnbL8
    fprintmany [outputfd], _mov, [rbp - 33], comma, [rbp - 16], newline
    vnbL8:

    ; select instruction depending on which operator is given
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    cmp word [rbx], TT_PLUS
    jne vnbL5    
    mov qword [rbp - 49], _add
    jmp vnbL10
    vnbL5:
    cmp word [rbx], TT_MIN
    jne vnbL6
    mov qword [rbp - 49], _sub
    jmp vnbL10
    vnbL6:
    cmp word [rbx], TT_STAR
    jne vnbL7
    mov qword [rbp - 49], _imul
    jmp vnbL10
    vnbL7:
    cmp word [rbx], TT_FSLASH
    jne unimplemented_op

    cmp qword [rbp - 33], _rax
    je vnbL11

    fprintmany [outputfd], _mov, _rax, comma, [rbp - 33], newline 
    vnbL11:

    fprintmany [outputfd], _cqo,newline
    fprintmany [outputfd], _idiv, [rbp - 41], newline
    mov rax, _rax
    leave 
    ret

    vnbL10:
    
    fprintmany [outputfd], [rbp - 49], [rbp - 33], comma, [rbp - 41], newline

    mov rax, [rbp - 33]

    leave
    ret

visit_node_lit: ; char* visit_node_lit(void* node)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    mov rbx, qword [rbp - 8]
    mov rbx, qword [rbx + 2]
    mov rdi, [rbx + 2]
    call itoa

    leave
    ret

visit_node_unop: ; char* visit_node_unop(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, qword [rbp - 8]
    mov rdi, qword [rbx + 10]
    mov rsi, [rbp - 16]
    mov dl, 1
    call visit
    mov [rbp - 24], rax

    fprintmany [outputfd], _mov, _rax, comma, [rbp - 24], newline
    fprintmany [outputfd], _neg, _rax, newline
    mov rax, _rax
    
    leave
    ret

visit_node_vardef: ; char* visit_node_vardef(void* node, symbol_table* table)
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
    call st_get_var_pos_str
    mov [rbp - 32], rax

    fprintmany [outputfd], _mov, [rbp - 32], comma, [rbp - 24], newline

    leave
    ret

visit_node_var: ; char* visit_node_var(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rdi, [rbp - 16]
    mov rbx, [rbp - 8]
    mov rsi, [rbx + 2]
    call st_get_var_pos_str

    leave
    ret

visit_node_if: ; char* visit_node_if(void* node, symbol_table* table)
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

    fprintmany [outputfd], _cmp, [rbp - 32], comma, zero, newline
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