Compiler written for CSC 453: Compilers and Systems Software

```
$ git clone https://github.com/cjlarose/clike.git
$ git submodule init
$ git submdoule update
$ make clike
```

`integer_test.c`

```c
void printint(int);

int add_five(x)
int x;
{
  int n;
  n = x + 5;
  printint(n);
  return n;
}
```

```gas
$ clike < integer_test.c
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
.data


.text

add_five:
    subu    $sp, $sp, 24
    sw      $ra, 4($sp)
    sw      $fp, 0($sp)
    addiu   $fp, $sp, 20
    addi    $t0, $zero, 5
    sw      $t0, 4($fp) # temp0 = $t0
    lw      $t0, 12($fp) # $t0 = x
    lw      $t1, 4($fp) # $t1 = temp0
    add     $t2, $t0, $t1
    sw      $t2, 0($fp) # temp1 = $t2
    lw      $t4, 0($fp) # $t4 = temp1
    sw      $t4, 8($fp) # n = $t4
    lw      $t0, 8($fp) # $t0 = n
    move    $a0, $t0
    sw      $t0, 0($sp)
    jal     printint
    lw      $v0, 8($fp) # $v0 = n
    j       add_five_epilogue
    add_five_epilogue:
    lw      $ra, 4($sp)
    lw      $fp, 0($sp)
    addiu   $sp, $sp, 24
    jr      $ra
```
