.text
    .global os_exit
    .global os_putchar

os_exit:
    movq $2,  %rdi   
    movq $60, %rax
    syscall

os_putchar:
    movb    %dil, -4(%rsp)          # store my_char into the red-zone
    movl    $1, %edi
    leaq    -4(%rsp), %rsi
    movl    %edi, %edx              # optimize because fd = len
    movl    $1,%eax                 # system call number 
    syscall                         # dummy arg picked -4(%rsp)
    ret

.data                   

msg:
