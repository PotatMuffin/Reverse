generate: ; void generate(void* node)
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    open [output], default_open_flags, default_open_mode
    mov [outputfd], rax

    fprintmany [outputfd], _format, _segment, _executable, _entry

    mov rdi, [rbp - 8]
    call visit_node_root

    mov rdi, [outputfd]
    mov rsi, _exit
    call fprint

    close [outputfd]

    leave
    ret

visit: ; char* visit(void* node, bool lhs)
    push rbp
    mov rbp, rsp
    sub rsp, 9
    mov [rbp - 8], rdi
    mov [rbp - 9], sil

    mov rbx, [rbp - 8]
    movsx rcx, word [rbx]
    cmp rcx, node_count
    jg unimplemented_node

    mov rdi, qword [rbp - 8]
    mov sil, [rbp - 9]
    mov rax, qword [genlookuptable + rcx * 8]
    call rax

    leave 
    ret

visit_node_root: ; char* visit_node_root(void* node)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov qword [rbp - 8], rdi

    mov rbx, qword [rbp - 8]
    cmp qword [rbx + 10], 0
    je vnrRet

    mov rbx, qword [rbx + 2]
    mov qword [rbp - 16], rbx

    vnrloop1:
    mov rbx, qword [rbp - 16]
    cmp qword [rbx + 8], 0
    je vnrloop2
    mov rbx, qword [rbx + 8]
    mov qword [rbp - 16], rbx
    jmp vnrloop1
    
    vnrloop2:

    mov rbx, qword [rbp - 16]
    mov rdi, qword [rbx]
    mov sil, 1
    call visit

    mov rbx, qword [rbp - 16]
    cmp qword [rbx + 16], 0
    je vnrRet
    mov rbx, qword [rbx + 16]
    mov qword [rbp - 16], rbx
    jmp vnrloop2
    
    vnrRet:
    leave
    ret

visit_node_binop: ; char* visit_node_binop(void* node, bool lhs)
    push rbp
    mov rbp, rsp
    sub rsp, 49
    mov [rbp - 8], rdi
    mov [rbp - 25], sil

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 2]
    mov sil, 1
    call visit
    mov [rbp - 16], rax

    cmp byte [rbp - 25], 0
    je vnbL2
    ; move result of left child into ecx if that child is a unop or binop and the current node is on the left hand side
    ; of its parent
    mov rbx, [rbp - 8]
    mov rbx, qword [rbx + 2]
    cmp word [rbx], NODE_UNOP
    je vnbl9
    cmp word [rbx], NODE_BINOP
    je vnbl9
    jmp vnbL2

    vnbl9:
    mov qword [rbp - 33], _ecx
    fprintmany [outputfd], _mov, [rbp - 33], comma, [rbp - 16], newline
    jmp vnbL1

    vnbL2:

    mov qword [rbp - 33], _eax
    vnbL1:

    mov rbx, [rbp - 8]
    mov rdi, qword [rbx + 18]
    mov sil, 0
    call visit
    mov [rbp - 24], rax
    ; I think this code is unnecessary for it to work but I am unsure so it's still here as a comment
    ; cmp byte [rbp - 25], 0
    ; je vnbL3

    ; mov qword [rbp - 41], _ebx
    ; jmp vnbL4

    ; vnbL3:
    ; mov qword [rbp - 41], _ecx

    ; vnbL4:
    ; ; move result of right child to ebx or ecx
    ; fprintmany [outputfd], _mov, [rbp - 41], comma, [rbp - 24], newline

    ; move result of left child into eax if it's not already in ecx
    cmp qword [rbp - 33], _eax
    jne vnbL8
    fprintmany [outputfd], _mov, [rbp - 33], comma, [rbp - 16], newline
    vnbL8:
    ; select instruction depending on which operator is given
    mov rbx, [rbp - 8]
    mov rbx, [rbx + 10]
    cmp word [rbx], TT_PLUS
    jne vnbL5    
    mov qword [rbp - 49], _add
    jmp vnbL7
    vnbL5:
    cmp word [rbx], TT_MIN
    jne vnbL6
    mov qword [rbp - 49], _sub
    jmp vnbL7
    vnbL6:
    mov qword [rbp - 49], _imul
    vnbL7:
    
    fprintmany [outputfd], [rbp - 49], [rbp - 33], comma, [rbp - 24], newline

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

visit_node_unop: ; char* visit_node_unop(void* node)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi

    mov rbx, qword [rbp - 8]
    mov rdi, qword [rbx + 10]
    mov rsi, 1
    call visit
    mov [rbp - 16], rax

    fprintmany [outputfd], _mov, _eax, comma, [rbp - 16], newline
    fprintmany [outputfd], _neg, _eax, newline
    mov rax, _eax
    
    leave
    ret

genlookuptable:
    dq 0
    dq visit_node_root
    dq visit_node_binop
    dq visit_node_lit
    dq visit_node_unop