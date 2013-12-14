# void printint(int x) : prints a single int
# void printchar(char c) : prints a single char
# void printdouble(double d) : print a single double
# int toint(double d) : floor of number, then convert to int
# double todouble(int i) : convert int to a double
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
