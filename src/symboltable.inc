hash: ; ulong hash(char* str)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov qword [rbp - 16], 5381

    hashloop:
        mov rax, [rbp - 8]
        movzx rbx, byte [rax]
        cmp bl, 0
        je hashloopend

        inc qword [rbp - 8]

        mov rax, [rbp - 16]
        sal rax, 5
        add rax, [rbp - 16]
        add rax, rbx
        mov [rbp - 16], rax
        jmp hashloop
    hashloopend:

    mov rax, [rbp - 16]
    leave
    ret

st_add_var: ; void st_add_var(symbol_table* table, void* node):
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 16]
    mov rbx, qword [rbx + 10]
    mov rdi, qword [rbx + 2]
    call hash

    mov rbx, [rbp - 8]
    mov rbx, qword [rbx]
    cqo
    idiv rbx
    mov [rbp - 24], rdx ; index

    mov rbx, [rbp - 8]
    mov rcx, [rbp - 24]
    imul rcx, symbol_size

    mov rax, [rbp - 16]
    mov rax, qword [rax + 10]
    mov rax, qword [rax + 2]
    mov qword [rbx + rcx + symbol_table_size], rax

    add dword [rbx + 8], 8
    mov eax, dword [rbx + 8]
    mov dword [rbx + rcx + symbol_table_size + 8], eax

    leave
    ret

st_get_var_pos: ; long st_get_var_pos(symbol_table* table, token* node)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    

    mov rbx, [rbp - 16]
    mov rdi, qword [rbx + 2]
    call hash

    mov rbx, [rbp - 8]
    mov rbx, qword [rbx]
    cqo
    idiv rbx
    mov [rbp - 24], rdx ; index

    mov rbx, [rbp - 8]
    mov rcx, [rbp - 24]
    imul rcx, symbol_size

    mov eax, dword [rbx + rcx + symbol_table_size + 8]

    leave
    ret

st_get_var_pos_str: ; char* st_get_var_pos_str(symbol_table* table, token* token)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
    call st_get_var_pos

    mov rdi, rax
    call itoa
    mov [rbp - 24], rax

    mov rdi, [memory_pointer]
    mov [rbp - 32], rdi
    pushmany 0, brackr, qword [rbp - 24], _min, _rbp, brackl, _qword
    mov rsi, rsp
    push rsp
    call concat_str
    pop rsp

    mov rax, [rbp - 32]
    leave
    ret

create_symbol_table: ; symbol_table* create_symbol_table(node_root* root)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    symbol_table 10000h
    mov [rbp - 16], rax

    mov rbx, qword [rbp - 8]
    cmp qword [rbx + 10], 0
    je cstRet

    mov rbx, qword [rbx + 2]
    mov qword [rbp - 24], rbx

    cstloop1:
    mov rbx, qword [rbp - 24]
    cmp qword [rbx + 8], 0
    je cstloop2
    mov rbx, qword [rbx + 8]
    mov qword [rbp - 24], rbx
    jmp cstloop1
    
    cstloop2:

    mov rbx, qword [rbp - 24]
    mov rdi, qword [rbx]
    mov rsi, [rbp - 16]
    call symvisit

    mov rbx, qword [rbp - 24]
    cmp qword [rbx + 16], 0
    je cstRet
    mov rbx, qword [rbx + 16]
    mov qword [rbp - 24], rbx
    jmp cstloop2

    cstRet:
    mov rax, [rbp - 16]
    leave 
    ret

symvisit: ; void symvisit(void* node, symbol_table* table)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, qword [rbp - 8]
    cmp word [rbx], NODE_VARDEF
    jne svRet
    
    mov rdi, [rbp - 16]
    mov rsi, [rbp - 8]
    call st_add_var

    svRet:
    leave
    ret