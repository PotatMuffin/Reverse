format ELF64 executable
segment readable executable
entry $
mov rdi, 35
mov rax, 34
add rax, rdi
mov dword [memend-4], eax
mov rdi, 1
mov eax, dword [memend-4]
cmp rax, rdi
sete al
movzx rax, al
cmp rax, 0
je if1
jmp if2
if1:
mov rdi, 420
mov eax, dword [memend-4]
cmp rax, rdi
sete al
movzx rax, al
cmp rax, 0
je if3
jmp if4
if3:
mov rdi, 69
mov eax, dword [memend-4]
cmp rax, rdi
sete al
movzx rax, al
cmp rax, 0
je if5
jmp if6
if5:
mov rdi, 2
mov eax, dword [memend-4]
cmp rax, rdi
sete al
movzx rax, al
cmp rax, 0
je if7
if7:
if6:
if4:
if2:
mov rdi,0
mov rax,60
syscall
segment readable writeable
mem rb 4
memend:
