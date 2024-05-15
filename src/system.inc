macro read fd, buf, count
{
    mov rax, 0
    mov rdi, fd
    mov rsi, buf
    mov rdx, count
    syscall
}

macro write fd, buf, count
{
    mov rax, 1
    mov rdi, fd
    mov rsi, buf
    mov rdx, count
    syscall
}

macro open fn, flags, mode
{
    mov rax, 2
    mov rdi, fn
    mov rsi, flags
    mov rdx, mode
    syscall
}

macro close fd
{
    mov rax, 3
    mov rdi, fd
    syscall
}

macro fstat fd, buf
{
    mov rax, 5
    mov rdi, fd
    mov rsi, buf
    syscall
}

macro brk new_breakpoint
{
    mov rax, 12
    mov rdi, new_breakpoint
    syscall
}

macro exit_process code
{
    mov rax, 60
    mov rdi, code
    syscall
}

macro print_tok_pos token
{
    mov rdi, brackl
    call print
    mov rbx, token
    movsxd rdi, dword [rbx + 10]
    call display_number
    mov rdi, brackr
    call print
    mov rdi, space
    call print
}

macro pushmany [arg]
{
    forward
    push arg
}

macro printmany [arg]
{
    forward
    mov rdi, arg
    call print
}

macro fprintmany fd, [arg]
{
    forward
    mov rdi, fd
    mov rsi, arg
    call fprint
}

macro check_memory required
{
    mov rax, [memory_pointer]
    add rax, required
    cmp rax, [memory_text_pointer]
    jg no_memory_error
}

macro check_memory_text required
{
    mov rax, [memory_text_pointer]
    sub rax, required
    cmp rax, [memory_pointer]
    jl no_memory_error
}

macro linked_list_item item
{
    check_memory 24
    mov rax, [memory_pointer]
    mov rbx, item
    mov qword [rax], rbx ; data
    mov qword [rax + 8], 0 ; next
    mov qword [rax + 16], 0 ; previous
    add [memory_pointer], 24
}

macro symbol_table table_size, parent
{
    mov rcx, table_size
    imul rcx, symbol_size
    add rcx, symbol_table_size
    check_memory rcx
    mov rax, [memory_pointer]
    mov rbx, table_size
    mov qword [rax], rbx
    mov dword [rax + 12], 1
    mov rbx, parent
    mov qword [rax + 16], rbx
    mov dword [rax + 24], 1
    add [memory_pointer], rcx
}

macro var type, pos
{
    check_memory var_size
    mov rax, [memory_pointer]
    mov word [rax], VAR
    mov rbx, type
    mov qword [rax + 2], rbx
    mov ebx, pos
    mov dword [rax + 10], ebx
    add [memory_pointer], var_size
}

macro struct fields, sizeof
{
    check_memory struct_size
    mov rax, [memory_pointer]
    mov word [rax], STRUCT
    mov rbx, fields
    mov qword [rax + 2], rbx
    mov ebx, sizeof
    mov dword [rax + 10], ebx
    mov dword [rax + 14], 0
    add [memory_pointer], struct_size
}

macro builtin_type size
{
    check_memory builtin_type_size
    mov rax, [memory_pointer]
    mov word [rax], BUILTIN_TYPE
    mov ebx, size
    mov dword [rax + 2], ebx
    add [memory_pointer], builtin_type_size
}

macro ptr ptr_type, type
{
    check_memory ptr_size
    mov rax, [memory_pointer]
    mov word [rax], _PTR
    mov dword [rax + 2], qword_
    mov rbx, ptr_type
    mov qword [rax + 6], rbx
    mov rbx, type
    mov qword [rax + 14], rbx
    add [memory_pointer], ptr_size
}

macro func arity, params, return, table
{
    check_memory func_size
    mov rax, [memory_pointer]
    mov word [rax], FUNC
    mov rbx, arity
    mov qword [rax + 2], rbx
    mov rbx, params
    mov qword [rax + 10], rbx
    mov rbx, return
    mov qword [rax + 18], rbx
    mov rbx, table
    mov qword [rax + 26], rbx
    mov dword [rax + 34], 0
    add [memory_pointer], func_size
}

macro context table, logop_true, logop_false, logop_token, mem_pos, if_end
{
    check_memory context_size
    mov rax, [memory_pointer]
    mov rbx, table
    mov [rax], rbx
    mov rbx, logop_true
    mov [rax + 8], rbx
    mov rbx, logop_false
    mov [rax + 16], rbx
    mov rbx, logop_token
    mov [rax + 24], rbx
    mov rbx, mem_pos
    mov [rax + 32], rbx
    mov rbx, if_end
    mov [rax + 40], rbx
    add [memory_pointer], context_size
}

macro valcontext table, type
{
    check_memory valcontext_size
    mov rax, [memory_pointer]
    mov rbx, table
    mov qword [rax], rbx
    mov rbx, type
    mov qword [rax + 8], rbx
    add [memory_pointer], valcontext_size
}

macro node_root
{
    check_memory node_root_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_ROOT
    mov qword [rax + 2], 0
    mov qword [rax + 10], 0
    add [memory_pointer], node_root_size
}

macro node_binop left, op, right
{   
    check_memory node_binop_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_BINOP
    mov rbx, left
    mov qword [rax + 2], rbx
    mov rbx, op
    mov qword [rax + 10], rbx
    mov rbx, right
    mov qword [rax + 18], rbx
    add [memory_pointer], node_binop_size
}

macro node_lit token
{
    check_memory node_lit_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_LIT
    mov rbx, token
    mov qword [rax + 2], rbx
    add [memory_pointer], node_lit_size
}

macro node_unop op, node
{
    check_memory node_unop_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_UNOP
    mov rbx, op
    mov qword [rax + 2], rbx
    mov rbx, node
    mov qword [rax + 10], rbx
    add [memory_pointer], node_unop_size
}

macro node_vardef value, name, type
{
    check_memory node_vardef_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_VARDEF
    mov rbx, value
    mov qword [rax + 2], rbx
    mov rbx, name
    mov qword [rax + 10], rbx
    mov rbx, type
    mov qword [rax + 18], rbx
    add [memory_pointer], node_vardef_size
}

macro node_varass value, name
{
    check_memory node_varass_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_VARASS
    mov rbx, value
    mov qword [rax + 2], rbx
    mov rbx, name
    mov qword [rax + 10], rbx
    add [memory_pointer], node_varass_size
}

macro node_var name
{
    check_memory node_var_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_VAR
    mov rbx, name
    mov qword [rax + 2], rbx
    add [memory_pointer], node_var_size
}

macro node_if condition, scope
{
    check_memory node_if_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_IF
    mov rbx, condition
    mov qword [rax + 2], rbx
    mov rbx, scope
    mov qword [rax + 10], rbx
    mov qword [rax + 18], 0
    add [memory_pointer], node_if_size
}

macro node_while condition, scope
{
    check_memory node_while_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_WHILE
    mov rbx, condition
    mov qword [rax + 2], rbx
    mov rbx, scope
    mov qword [rax + 10], rbx
    mov qword [rax + 18], 0
    add [memory_pointer], node_while_size
}

macro node_for on_entry, condition, on_loop, scope
{
    check_memory node_for_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_FOR
    mov rbx, on_entry
    mov qword [rax + 2], rbx
    mov rbx, condition
    mov qword [rax + 10], rbx
    mov rbx, on_loop
    mov qword [rax  + 18], rbx
    mov rbx, scope
    mov qword [rax + 26], rbx
    add [memory_pointer], node_for_size 
}

macro node_asm asm
{
    check_memory node_asm_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_ASM
    mov rbx, asm
    mov [rax + 2], rbx
    add [memory_pointer], node_asm_size
}

macro type type_name, ptr
{
    check_memory type_size
    mov rax, [memory_pointer]
    mov rbx, type_name
    mov [rax], rbx
    mov rbx, ptr
    mov [rax + 8], rbx
    add [memory_pointer], type_size
}

macro param name, type
{
    check_memory param_size
    mov rax, [memory_pointer]
    mov rbx, name
    mov [rax], rbx
    mov rbx, type
    mov [rax + 8], rbx
    add [memory_pointer], param_size
}

macro node_func_def scope, name, params, arity, return
{   
    check_memory node_func_def_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_FUNC_DEF
    mov rbx, scope
    mov qword [rax + 2], rbx
    mov rbx, name
    mov qword [rax + 10], rbx
    mov rbx, params
    mov qword [rax + 18], rbx
    mov rbx, arity 
    mov qword [rax + 26], rbx
    mov rbx, return
    mov qword [rax + 34], rbx
    add [memory_pointer], node_func_def_size
}

macro node_return tok, value
{
    check_memory node_return_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_RETURN
    mov rbx, tok
    mov qword [rax + 2], rbx
    mov rbx, value
    mov qword [rax + 10], rbx
    add [memory_pointer], node_return_size
}   

macro node_func_call func_name, params, count
{
    check_memory node_func_call_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_FUNC_CALL
    mov rbx, func_name
    mov qword [rax + 2], rbx
    mov rbx, params
    mov qword [rax + 10], rbx
    mov ebx, count
    mov dword [rax + 18], ebx
    add [memory_pointer], node_func_call_size
}

macro node_ptr_ass value, loc
{
    check_memory node_ptr_ass_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_PTR_ASS
    mov rbx, value
    mov qword [rax + 2], rbx
    mov rbx, loc
    mov qword [rax + 10], rbx
    add [memory_pointer], node_ptr_ass_size
}

macro node_cast cast_type, castee
{
    check_memory_text node_cast_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_CAST
    mov rbx, cast_type
    mov qword [rax + 2], rbx
    mov rbx, castee
    mov qword [rax + 10], rbx
    add [memory_pointer], node_cast_size
}

macro field name, type, pos
{
    check_memory field_size
    mov rax, [memory_pointer]
    mov rbx, name
    mov qword [rax], rbx
    mov rbx, type
    mov qword [rax + 8], rbx
    mov ebx, pos
    mov dword [rax + 16], ebx
    add [memory_pointer], field_size
}

macro node_struct name, fields, field_count
{
    check_memory node_struct_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_STRUCT
    mov rbx, name
    mov qword [rax + 2], rbx
    mov rbx, fields
    mov qword [rax + 10], rbx
    mov rbx, field_count
    mov qword [rax + 18], rbx
    add [memory_pointer], node_struct_size
}

macro node_struct_ini fields, tok
{
    check_memory node_struct_ini_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_STRUCT_INI
    mov rbx, fields
    mov qword [rax + 2], rbx
    mov rbx, tok
    mov qword [rax + 10], rbx
    mov qword [rax + 18], 0
    add [memory_pointer], node_struct_ini_size
}

macro node_field_access field_name, struct
{
    check_memory node_field_access_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_FIELD_ACCESS
    mov rbx, field_name
    mov qword [rax + 2], rbx
    mov rbx, struct
    mov qword [rax + 10], rbx
    mov qword [rax + 18], 0
    add [memory_pointer], node_field_access_size
}

macro node_field_assign value, field
{
    check_memory node_field_assign_size
    mov rax, [memory_pointer]
    mov word [rax], NODE_FIELD_ASSIGN
    mov rbx, value
    mov qword [rax + 2], rbx
    mov rbx, field
    mov qword [rax + 10], rbx
    add [memory_pointer], node_field_assign_size
}

macro immvalue value
{
    check_memory immvalue_size
    mov rax, [memory_pointer]
    mov word [rax], IMMVALUE
    mov rbx, value
    mov qword [rax + 2], rbx
    add [memory_pointer], immvalue_size
}

macro reg register, size
{
    check_memory reg_size
    mov rax, [memory_pointer]
    mov word [rax], REG
    mov ebx, register
    mov dword [rax + 2], ebx
    mov ebx, size
    mov dword [rax + 6], ebx
    add [memory_pointer], reg_size
}

macro mempos register, size, offset
{
    check_memory mempos_size
    mov rax, [memory_pointer]
    mov word [rax], MEMPOS
    mov ebx, register
    mov dword [rax + 2], ebx
    mov ebx, size
    mov dword [rax + 6], ebx
    mov ebx, offset
    mov dword [rax + 10], ebx
    add [memory_pointer], mempos_size
}

macro label lbl
{
    check_memory label_size
    mov rax, [memory_pointer]
    mov word [rax], _LABEL
    mov rbx, lbl
    mov qword [rax + 2], rbx
    add [memory_pointer], label_size
}

init_memory:
    brk 0
    mov [memory_start], rax
    mov [memory_pointer], rax

    mov rcx, [memory_setting]
    shl rcx, 10
    jnz allocate_memory
    mov rcx, default_memory_setting
    mov [memory_setting], rcx
    allocate_memory:
    lea rdi, [rax + rcx]
    brk rdi

    mov [memory_text_pointer], rax
    mov [memory_end], rax
    ret

read_file: ; char* read_file(int fd)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi

    fstat [rbp - 8], stat
    mov rbx, qword [stat + 48]
    inc rbx
    mov [rbp - 16], rbx
    mov rbx, [memory_pointer]
    mov [rbp - 24], rbx

    read [rbp - 8], [rbp - 24], [rbp - 16]

    mov rax, [rbp - 24]
    mov rbx, [rbp - 16]
    add qword [memory_pointer], rbx

    leave
    ret
fprint: ; void fprint(long fd, char* ptr)
    push rbp
    mov rbp, rsp
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    xor rdx, rdx

    fprintloopstart:
        mov al, byte [rsi + rdx]
        cmp al, 0 
        je fprintloopend

        inc rdx
        jmp fprintloopstart 

    fprintloopend:

    write [rbp - 8], [rbp - 16], rdx

    leave
    ret

log: ; void log(long level, char* message):
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    lea rdi, [log_message + rbx * 9]
    call print
    mov rdi, [rbp - 16]
    call print

    leave
    ret

print: ; void print(char* ptr)
    xor rdx, rdx
    mov rsi, rdi

    printloopstart:

    mov al, byte [rsi + rdx]
    cmp al, 0
    je printloopend

    inc rdx
    jmp printloopstart

    printloopend:

    write STDOUT, rsi, rdx
    ret

display_number: ; void display_number(long number)
    push rbp
    mov rbp, rsp
    sub rsp, 29

    mov [rbp - 28], rdi
    cmp qword [rbp - 28], 0
    setl byte [rbp - 29]

    xor rbx, rbx
    mov rcx, rbp

    display_loop:
        mov rax, [rbp - 28]
        cqo
        mov r10, 10
        idiv r10
        mov [rbp - 28], rax

        inc rbx
        dec rcx

        cmp dl, 0
        jg L4
        neg dl

        L4:
        add dl, '0'
        mov [rcx], dl

        cmp qword [rbp - 28], 0
        jnz display_loop

    cmp byte [rbp - 29], 0
    jz display_print

    dec rcx
    inc rbx
    mov [rcx], byte '-'

    display_print:
    write STDOUT, rcx, rbx

    leave
    ret

itoa: ; char* iota(long num)
    push rbp
    mov rbp, rsp
    sub rsp, 17
    mov [rbp - 8], rdi
    check_memory 24
    add [memory_pointer], 24
    mov rbx, [memory_pointer]
    mov qword [rbp - 16], rbx

    cmp qword [rbp - 8], 0
    jne itoaL1
    mov rax, zero
    leave
    ret
    itoaL1:

    cmp qword [rbp - 8], 0
    setl byte [rbp - 17]

    dec qword [rbp - 16]
    mov rbx, qword [rbp - 16]
    mov byte [rbx], 0

    itoaloop:
        mov rax, [rbp - 8]
        cqo
        mov r10, 10
        idiv r10
        mov [rbp - 8], rax

        cmp dl, 0
        jg L6
        neg dl
        L6:
        add dl, '0'
        dec qword [rbp - 16]
        mov rbx, [rbp - 16]
        mov byte [rbx], dl

        cmp qword [rbp - 8], 0
        jnz itoaloop

    cmp byte [rbp - 17], 0
    jz itoaRet

    dec qword [rbp - 16]
    mov rbx, [rbp - 16]
    mov byte [rbx], '-'

    itoaRet:
    mov rax, [rbp - 16]
    leave
    ret

contains_char: ; bool contains_char(char* source, char c)
    push rbp
    mov rbp, rsp
    sub rsp, 9
    mov [rbp - 8], rdi
    mov [rbp - 9], sil

    mov bl, [rbp - 9]
    xor rcx, rcx
    loopcontainsstart:
        mov rax, [rbp - 8]
        mov dl, [rax+rcx]

        cmp dl, 0
        je containsret

        cmp bl, dl
        sete al

        cmp al, 1
        je containsret

        inc rcx
        jmp loopcontainsstart

    containsret:
    leave
    ret

copy_chars: ; char* copy_chars(char* start, char* allowed)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov qword [rbp - 24], 0

    copyloop:
        mov rdi, [rbp - 16]
        mov rbx, [rbp - 8]
        mov rax, [rbp - 24]
        mov rsi, [rbx + rax]
        call contains_char

        inc qword [rbp - 24]

        cmp al, 1
        je copyloop

    check_memory_text [rbp - 24]
    mov rbx, [rbp - 24]
    sub [memory_text_pointer], rbx

    xor rcx, rcx
    copyloop2:
        mov rax, [rbp - 8]
        mov rbx, [memory_text_pointer]

        mov al, [rax + rcx]
        mov [rbx + rcx], al

        inc rcx

        cmp rcx, [rbp - 24]
        jl copyloop2

    mov rbx, [memory_text_pointer]
    mov byte [rbx + rcx - 1], 0

    mov rax, [memory_text_pointer]
    mov rbx, [rbp - 24]
    leave
    ret

fcopy_str: ; char* fcopy_str(char* str)
    push rbp
    mov rbp, rsp
    sub rsp, 24
    mov [rbp - 8], rdi
    mov qword [rbp - 16], 0 ; len
    
    fcopy_loop1:
        mov rbx, [rbp - 8]
        add rbx, [rbp - 16]
        cmp byte [rbx], '"'
        jne fcopy_L1
        cmp byte [rbx - 1], '\'
        jne fcopy_loop1end
        fcopy_L1:
        cmp byte [rbx], 10
        je fcopyfail
        cmp byte [rbx], 0
        je fcopyfail
        inc qword [rbp - 16]
        jmp fcopy_loop1
    fcopy_loop1end:
    inc qword [rbp - 16]


    check_memory_text [rbp - 16]
    mov rbx, [rbp - 16]
    sub [memory_text_pointer], rbx
    
    xor rcx, rcx
    xor rdx, rdx
    fcopy_loop2:    
        mov rax, [rbp - 8]
        mov rbx, [memory_text_pointer]
        cmp byte [rax + rdx], '\'
        jne fcopyL2
        cmp byte [rax + rdx + 1], 'n'
        jne fcopyL3
        mov byte [rbx + rcx], 10
        inc rdx
        jmp fcopyL1
        fcopyL3:
        inc rdx 
        fcopyL2:
        mov al, [rax + rdx]
        mov [rbx + rcx], al

        fcopyL1:
        inc rcx
        inc rdx
        cmp rdx, [rbp - 16]
        jl fcopy_loop2
    mov rbx, [memory_text_pointer]
    mov byte [rbx + rcx - 1], 0 
    mov rax, [memory_text_pointer]
    leave
    ret
    fcopyfail:
    mov rax, -1
    leave
    ret

concat_str: ; char* concat_str(char** in)
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov [rbp - 16], rdi
    mov rbx, [memory_pointer]
    mov [rbp - 8], rbx
    mov qword [rbp - 24], 0 ; length
    mov qword [rbp - 32], 0 ; strcount

    concatloop1:
        mov rbx, [rbp - 16]
        mov rax, [rbp - 32]
        mov rbx, [rbx + rax * 8]
        cmp rbx, 0
        je concatRet

        xor rcx, rcx
        concatloop2:
            check_memory 1

            mov dl, [rbx + rcx]
            cmp dl, 0
            je concatloop2end

            mov rax, [rbp - 8]
            add rax, [rbp - 24]
            mov [rax], dl
            inc rcx
            inc qword [rbp - 24]
            jmp concatloop2

        concatloop2end:
        inc qword [rbp - 32]
        jmp concatloop1

    concatRet:
    check_memory 1
    mov rbx, [rbp - 24]
    add rbx, [rbp - 8]
    mov byte [rbx], 0
    inc qword [rbp - 24]

    mov rbx, [rbp - 24]
    add [memory_pointer], rbx
    mov rax, [rbp - 8]
    leave
    ret

str_copy: ; long str_copy(char* src, char* dest)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    xor rcx, rcx
    str_copyloop:
        mov rbx, [rbp - 8]
        mov bl, [rbx + rcx]
        mov rax, [rbp - 16]
        mov [rax + rcx], bl
        inc rcx
        cmp bl, 0
        jne str_copyloop 
    mov rax, rcx
    leave
    ret

strcmp: ; bool strcmp(char* a, char* b)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    xor rcx, rcx
    strcmploop:
        mov rbx, [rbp - 8]
        mov bl, [rbx + rcx]
        mov rdx, [rbp - 16]
        mov dl, [rdx + rcx]
        cmp bl, dl
        sete al
        
        cmp bl, 0
        je strcmpRet

        cmp al, 0
        je strcmpRet

        inc rcx
        jmp strcmploop

    strcmpRet:

    leave
    ret

contains_str: ; bool contains_str(char** charArr, char* str)
    push rbp
    mov rbp, rsp
    sub rsp, 25
    mov [rbp - 25], rdi
    mov [rbp - 8], rsi
    mov qword [rbp - 16], 0 ; i
    mov byte [rbp - 17], 0 ; found

    containsstrloop:
        mov rbx, [rbp - 25]
        mov rax, [rbp - 16]
        mov rbx, qword [rbx + rax * 8]

        cmp rbx, 0
        je containsstrloopend

        mov rdi, [rbp - 8]
        mov rsi, rbx
        call strcmp

        mov [rbp - 17], al
        cmp byte [rbp - 17], 1
        je containsstrloopend

        inc qword [rbp - 16]
        jmp containsstrloop

    containsstrloopend:
    mov al, [rbp - 17]
    leave
    ret

index_of: ; bool index_of(char** charArr, char* str)
    push rbp
    mov rbp, rsp
    sub rsp, 25
    mov [rbp - 25], rdi
    mov [rbp - 8], rsi
    mov qword [rbp - 16], 0 ; i
    mov byte [rbp - 17], 0 ; found

    indexofloop:
        mov rbx, [rbp - 25]
        mov rax, [rbp - 16]
        mov rbx, qword [rbx + rax * 8]

        cmp rbx, 0
        je indexofloopend

        mov rdi, [rbp - 8]
        mov rsi, rbx
        call strcmp

        mov [rbp - 17], al
        cmp byte [rbp - 17], 1
        je indexofloopend

        inc qword [rbp - 16]
        jmp indexofloop

    indexofloopend:
    mov rax, [rbp - 16]
    leave
    ret

value_to_string: ; char* value_to_string(void* value)
    push rbp
    mov rbp, rsp
    sub rsp, 40
    mov [rbp - 8], rdi

    mov rbx, [rbp - 8]
    cmp word [rbx], IMMVALUE
    jne vtsL1

    mov rbx, [rbx + 2]
    mov rdi, [rbx + 2]
    call itoa
    leave
    ret

    vtsL1:
    cmp word [rbx], REG
    jne vtsL2

    mov eax, [rbx + 2]
    imul rax, 20
    lea rax, [_rax + rax]
    mov ecx, [rbx + 6]
    imul rcx, 5
    lea rax, [rax + rcx]
    leave
    ret
    vtsL2:
    cmp word [rbx], MEMPOS
    jne vtsL3

    mov eax, dword [rbx + 2]
    imul rax, 20
    lea rax, [_rax + rax]
    mov [rbp - 16], rax

    mov eax, dword [rbx + 6]
    imul rax, 7
    lea rax, [_qword + rax]
    mov qword [rbp - 32], rax

    movsxd rdi, dword [rbx + 10]
    cmp rdi, 0
    jne vtsL6

    pushmany 0, brackr, qword [rbp - 16], brackl, qword [rbp - 32]
    mov rdi, rsp
    push rsp
    call concat_str
    pop rsp

    leave
    ret

    vtsL6:
    cmp rdi, 0
    jg vtsL4
    neg rdi
    mov qword [rbp - 40], _min
    jmp vtsL5
    vtsL4:
    mov qword [rbp - 40], _plus
    vtsL5:

    call itoa
    mov [rbp - 24], rax

    pushmany 0, brackr, qword [rbp - 24], qword [rbp - 40], qword [rbp - 16], brackl, qword [rbp - 32]
    mov rdi, rsp
    push rsp
    call concat_str
    pop rsp

    leave
    ret
    vtsL3:
    mov rax, [rbx + 2]
    leave
    ret

type_equals: ; bool type_equals(void* type1, void* type2)
    push rbp
    mov rbp, rsp
    sub rsp, 16
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi

    mov rbx, [rbp - 8]
    mov rcx, [rbp - 16]
    mov cx, word [rcx]
    cmp word [rbx], cx
    jne teL1

    cmp word [rbx], _PTR
    jne teL2
    mov rbx, [rbp - 8]
    mov rcx, [rbp - 16]
    mov rax, [rcx + 6]
    cmp qword [rbx + 6], rax
    jne teL1

    mov rbx, [rbp - 8]
    mov rdi, [rbx + 14]
    mov rcx, [rbp - 16]
    mov rsi, [rcx + 14]
    call type_equals
    leave
    ret

    teL2:
    cmp word [rbx], STRUCT
    jne teL3

    mov rbx, [rbp - 8]
    mov rcx, [rbp - 16]

    mov ecx, [rcx + 10]
    cmp dword [rbx + 10], ecx
    jne teL1

    teL3:

    mov al, 1
    leave
    ret
    teL1:
    mov al, 0
    leave
    ret

file_error:
    printmany ERROR, filenotfound, suffix
    exit_process 1
no_memory_error:
    printmany ERROR, outofmemory, suffix
    exit_process 1
name_error_var_defined: ; void name_error_var_defined(token* token)
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    mov rdi, ERROR
    call print
    print_tok_pos [rbp - 8]

    printmany varprefix
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call print
    printmany vardefined, suffix
    exit_process 1
name_error_var_undefined:
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi

    mov rdi, ERROR
    call print
    print_tok_pos [rbp - 8]

    printmany varprefix
    mov rbx, [rbp - 8]
    mov rdi, [rbx + 2]
    call print
    printmany varnotdefined, suffix
    exit_process 1
syntax_error_token: ; syntax_error_token(token* token)
    mov rbp, rsp
    sub rsp, 8
    mov qword [rbp - 8], rdi

    printmany ERROR
    print_tok_pos [rbp - 8]
    printmany syntaxerrortoken, suffix
    exit_process 1
syntax_error_expected: ; void syntax_error_expected(token* token, char** argv)
    mov rbp, rsp
    sub rsp, 24
    mov qword [rbp - 8], rdi
    mov qword [rbp - 16], rsi
    mov qword [rbp - 24], 0

    printmany ERROR
    print_tok_pos [rbp - 8]
    printmany syntaxerrorexpected

    seeloop:
        mov rbx, qword [rbp - 16]
        mov rcx, qword [rbp - 24]
        imul rcx, 8
        add rbx, rcx
        
        cmp qword [rbx + 8], 0
        jne seeL2
        cmp rcx, 0
        je seeL1
        printmany _or
        seeL1:
        printmany qword [rbx], suffix
        jmp seeRet

        seeL2: 
        printmany qword [rbx], comma
        inc qword [rbp - 24]
        jmp seeloop

    seeRet:
    exit_process 1
unimplemented_node:
    mov rbp, rsp
    sub rsp, 8
    mov [rbp - 8], rdi
    printmany ERROR, unimplementednode
    mov rdi, [rbp - 8]
    call display_number
    mov rdi, suffix
    call print
    exit_process 1
unimplemented_op:
    printmany ERROR, unimplementedop, suffix
    exit_process 1
loc_not_reg:
    mov rdi, LOGERROR
    mov rsi, locnotreg
    call log
    mov rdi, suffix
    call print
    mov rdi, LOGNOTE
    mov rsi, bugincom
    call log
    mov rdi, suffix
    call print
    exit_process 1
loc_not_mem:
    mov rdi, LOGERROR
    mov rsi, locnotmem
    call log
    mov rdi, suffix
    call print
    mov rdi, LOGNOTE
    mov rsi, bugincom
    call log
    mov rdi, suffix
    call print
    exit_process 1
iwf_not_allowed:
    mov [rbp - 8], rdi
    mov rdi, ERROR
    call print
    print_tok_pos [rbp - 8]
    mov rdi, iwfnotallowed
    call print
    mov rdi, suffix
    call print
    exit_process 1
is_not_a_type:
    mov [rbp - 8], rdi
    mov rdi, ERROR
    call print
    print_tok_pos [rbp - 8]
    mov r15, [rbp - 8]
    printmany quote, [r15 + 2], isnottype, suffix
    exit_process 1
cant_def_void:
    mov [rbp - 8], rdi
    mov rdi, ERROR
    call print
    print_tok_pos [rbp - 8]
    printmany cantdefvoid, suffix
    exit_process 1
expected_var:
    mov [rbp - 8], rdi
    mov rdi, ERROR
    call print
    print_tok_pos [rbp - 8]
    printmany expectedvar, suffix
    exit_process 1
type_mismatch:
    mov [rbp - 8], rdi
    mov rdi, ERROR
    call print
    print_tok_pos [rbp - 8]
    printmany typemismatch, suffix
    exit_process 1
last_stmt_return:
    mov [rbp - 8], rdi
    mov rdi, ERROR
    call print
    print_tok_pos [rbp - 8]
    printmany laststmtreturn, suffix
    exit_process 1
cant_ret_outside_func:
    mov [rbp - 8], rdi
    printmany ERROR
    print_tok_pos [rbp - 8]
    printmany cantretoutfun, suffix
    exit_process 1
is_not_func:
    mov [rbp - 8], rdi
    printmany ERROR
    print_tok_pos [rbp - 8]
    mov r15, [rbp - 8]
    printmany quote, [r15 + 2], isnotfunc, suffix
    exit_process 1
not_defined:
    mov [rbp - 8], rdi
    printmany ERROR
    print_tok_pos [rbp - 8]
    mov r15, [rbp - 8]
    printmany quote, [r15 + 2], varnotdefined, suffix
    exit_process 1
already_defined:
    mov [rbp - 8], rdi
    printmany ERROR
    print_tok_pos [rbp - 8]
    mov r15, [rbp - 8]
    printmany quote, [r15 + 2], vardefined, suffix
    exit_process 1
expect_n_params:
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    mov [rbp - 24], rdx
    printmany ERROR
    print_tok_pos [rbp - 8]
    mov r15, [rbp - 8]
    printmany quote, [r15 + 2], expects
    mov rdi, [rbp - 16]
    call display_number
    printmany parambutgot
    mov rdi, [rbp - 24]
    call display_number
    printmany suffix
    exit_process 1
type_mismatch_param:
    mov [rbp - 8], rdi
    mov [rbp - 16], rsi
    printmany ERROR
    print_tok_pos [rbp - 8]
    printmany typemismatchparam, [rbp - 16], quote, suffix
    exit_process 1
illegal_cast:
    mov [rbp - 8], rdi
    printmany ERROR
    print_tok_pos [rbp - 8]
    printmany illegalcast, suffix
    exit_process 1
cant_infer_type:
    mov [rbp - 8], rdi
    printmany ERROR
    print_tok_pos [rbp  - 8]
    printmany cantinfertype, suffix
    exit_process 1
invalid_construct:
    mov [rbp - 8], rdi
    printmany ERROR
    print_tok_pos [rbp - 8]
    printmany invalidconstruct, suffix
    exit_process 1
builtin_no_field:
    mov [rbp - 8], rdi
    printmany ERROR
    print_tok_pos [rbp - 8]
    printmany builtinnofield, suffix
    exit_process 1
invalid_field:
    mov [rbp - 8], rdi
    printmany ERROR
    print_tok_pos [rbp - 8]
    printmany invalidfield, suffix
    exit_process 1