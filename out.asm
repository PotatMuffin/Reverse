format ELF64 executable
segment readable executable
entry $
mov rbp, rsp
mov rax,10
add rax,10
mov rdi,0
mov rax,60
syscall