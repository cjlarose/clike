# void printint(int x) : prints a single int
# void printchar(char c) : prints a single char
# void printdouble(double d) : print a single double
# int toint(double d) : floor of number, then convert to int
# double todouble(int i) : convert int to a double
.data
str:
    .asciiz "the answer = "
fav_float:
    .double 12.345678
.text
printint:
    li $v0, 1             # system call code for print_int
    syscall               # print it
    jr    $ra             # return to caller's code

printchar:
    li $v0, 11             # system call code for print_char
    syscall               # print it
    jr    $ra             # return to caller's code

printdouble:
    l.d $f12, 0($a0)      # copy words at a0 and a0+4 to $f12 and $f13
    li $v0, 3             # system call code for print_double
    syscall               # print it
    jr    $ra             # return to caller's code

toint:
    l.d $f12, 0($a0)      # copy words at a0 and a0+4 to $f12 and $f13
    floor.w.d $f12, $f12  # compute the floor (as a word), store in $f12
    mfc1 $v0, $f12        # move f12 in fp coproc to v0
    jr    $ra             # return to caller's code

todouble:
    mtc1 $a0, $f14        # move the int in a0 to f12
    cvt.d.w $f14, $f14    # convert f12 from int to double
    mfc1 $v0, $f14
    mfc1 $v1, $f15
    jr $ra

main:
    # Function prologue -- even main has one
    addiu $sp, $sp, -24   # allocate stack space -- default of 24 here
    sw    $fp, 0($sp)     # save caller's frame pointer
    sw    $ra, 4($sp)     # save return address
    addiu $fp, $sp, 20    # setup main's frame pointer

    li $v0, 4             # system call code for print_str
    la $a0, str           # address of string to print 
    syscall               # print the string

    li $a0 5
    jal printint

    li $a0 17
    jal printint

    li $a0 10
    jal printchar

    li $a0 72
    jal printchar
    li $a0 69
    jal printchar
    li $a0 76
    jal printchar
    li $a0 76
    jal printchar
    li $a0 79
    jal printchar

    li $a0 10
    jal printchar

    la $a0, fav_float
    jal printdouble

    li $a0 10
    jal printchar

    la $a0, fav_float
    jal toint
    move $a0, $v0
    jal printint

    li $a0 10
    jal printchar

    li $a0, 56
    jal todouble
    # now v0 and v1 store the new double
    # now overwrite fav_float
    la $a0, fav_float
    #sw $v0, 0($a0)
    #sw $v1, 4($a0)

    #jal printdouble 

    #li $v0, 1             # system call code for print_int
    #li $a0, 5             # integer to print 
    #syscall               # print it

    # Epilogue for main -- restore stack & frame pointers and return
    lw    $ra, 4($sp)     # get return address from stack
    lw    $fp, 0($sp)     # restore the caller's frame pointer
    addiu $sp, $sp, 24    # restore the caller's stack pointer
    jr    $ra             # return to caller's code
