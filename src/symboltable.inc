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

st_add_func: ; void st_add_func(symbol_table* table, token*, func* func)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov [rbp - 24], rdx

    mov rbx, [rbp - 16]
    mov rdi, [rbx + 2]
    call hash

    mov rbx, [rbp - 8]
    mov rbx, qword [rbx]
    xor rdx, rdx
    idiv rbx
    mov [rbp - 32], rdx

    tryaddfunc:
    mov rbx, [rbp - 8]
    mov rcx, [rbp - 32]
    imul rcx, symbol_size
    mov rax, [rbx + rcx + symbol_table_size]
    cmp rax, 0
    je addfunc

    mov rdi, rax
    mov rbx, [rbp - 16]
    mov rsi, [rbx + 2]
    call strcmp
    movzx rax, al

    add qword [rbp - 32], 1
    sub qword [rbp - 32], rax
    
    cmp al, 0
    je tryaddfunc

    mov rdi, [rbp - 16]
    call name_error_var_defined

    addfunc:

    mov rbx, [rbp - 8]
    mov rcx, [rbp - 32]
    imul rcx, symbol_size

    mov rax, [rbp - 16]
    mov rax, qword [rax + 2]
    mov qword [rbx + rcx + symbol_table_size], rax

    mov rbx, [rbp - 8]
    mov rcx, [rbp - 32]
    imul rcx, symbol_size
    mov edx, [rbx + 12]
    inc dword [rbx + 12]
    mov rax, [rbp - 24]
    mov [rax + 34], edx 
    mov qword [rbx + rcx + 8 + symbol_table_size], rax

    leave
    ret

st_add_var: ; void st_add_var(symbol_table* table, token* name, void* type)
    push rbp
    mov rbp, rsp
    sub rsp, 40
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov [rbp - 40], rdx

    mov rbx, [rbp - 16]
    mov rdi, qword [rbx + 2]
    call hash

    mov rbx, [rbp - 8]
    mov rbx, qword [rbx]
    xor rdx, rdx
    idiv rbx
    mov [rbp - 24], rdx ; index
    ; TODO: this will become an infinite loop if the entire table is full
    tryaddvar:
    mov rbx, [rbp - 8]
    mov rcx, [rbp - 24]
    imul rcx, symbol_size
    mov rax, [rbx + rcx + symbol_table_size]
    cmp rax, 0
    je addvar

    mov rdi, rax
    mov rbx, [rbp - 16]
    mov rsi, [rbx + 2]
    call strcmp
    movzx rax, al

    add qword [rbp - 24], 1
    sub qword [rbp - 24], rax
    
    cmp al, 0
    je tryaddvar

    mov rdi, [rbp - 16]
    call name_error_var_defined

    addvar:
    mov rbx, [rbp - 8]
    mov rcx, [rbp - 24]
    imul rcx, symbol_size

    mov rax, [rbp - 16]
    mov rax, qword [rax + 2]
    mov qword [rbx + rcx + symbol_table_size], rax

    mov rcx, [rbp - 40]
    mov ecx, [rcx + 2]
    movzx rcx, byte [sizes + rcx]

    sub dword [rbx + 8], ecx
    mov r15d, dword [rbx + 8]
    var [rbp - 40], r15d

    mov rbx, [rbp - 8]
    mov rcx, [rbp - 24]
    imul rcx, symbol_size
    mov qword [rbx + rcx + 8 + symbol_table_size], rax

    leave
    ret

st_get_symbol: ; symbol* st_get_symbol(symbol_table* table, token* token)
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
    xor rdx, rdx
    idiv rbx
    mov [rbp - 24], rdx
    ; TODO: this will become an infinite loop if the entire table is full
    trygetsymbol:
    mov rbx, [rbp - 8]
    mov rcx, [rbp - 24]
    imul rcx, symbol_size

    mov rax, [rbx + rcx + symbol_table_size]
    cmp rax, 0
    je getsymbolfail

    mov rdi, rax
    mov rbx, [rbp - 16]
    mov rsi, [rbx + 2]
    call strcmp
    movzx rax, al
    
    add qword [rbp - 24], 1
    sub qword [rbp - 24], rax

    cmp al, 0
    je trygetsymbol

    mov rbx, [rbp - 8]
    mov rcx, [rbp - 24]
    imul rcx, symbol_size
    lea rax, [rbx + rcx + symbol_table_size]
    leave
    ret

    getsymbolfail:
    mov rax, -1
    leave
    ret

st_get_var_pos: ; mempos* st_get_var_pos(symbol_table* table, token* token)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    
    mov rdi, [rbp - 8]
    mov rsi, [rbp - 16]
    call st_get_symbol
    mov [rbp - 24], rax

    mov rbx, [rbp - 24]
    mov rbx, qword [rbx + 8]
    mov r15d, dword [rbx + 10]
    mov rbx, [rbx + 2]
    mov r14d, [rbx + 2]
    mempos rbp_, r14d, r15d

    getvarRet:
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

    pushmany 0, brackr, qword [rbp - 24], _min, _rbp, brackl, _qword
    mov rdi, rsp
    push rsp
    call concat_str
    pop rsp

    leave
    ret
