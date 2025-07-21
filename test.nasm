
section .text
global _start

hello:
	push rbp
	mov rbp, rsp
	mov rbx, 200
	mov rax, rbx
	pop rbp
	ret

main:
	push rbp
	mov rbp, rsp
	mov rbx, 100
	mov qword [rbp+-8], rbx
	mov rbx, [rbp+-8]
	mov rax, rbx
	pop rbp
	ret

_start:
	push rbp
	mov rbp, rsp
	call main
	mov rdi, rax
	mov rax, 60
	syscall