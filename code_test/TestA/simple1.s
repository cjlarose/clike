#####################################################
printint:
    li   $v0, 1  # escape code for printint
    syscall    # printit
    jr $ra 
#####################################################
printchar:
    li   $v0, 11 # escape code for printchar
    syscall    # printit
    jr $ra 
#####################################################

main:
# START PROLOGUE ###########################################
    subu  $sp, $sp, 24   # stack frame is 24 bytes
    sw    $ra, 8($sp)   # Save return Address
    sw    $fp, 0($sp)   # Save old frame pointer
    addiu $fp, $sp, 20   # set up frame pointer
# END PROLOGUE ##############################################
    li  $t0, 1   # constant
    sw  $t0, -4($fp)   # var tmp1 int
    lw  $t0, -4($fp)   # var:tmp1 int
    sw  $t0, 0($fp)   # var x int
    lw  $t0, 0($fp)   # var:x int
    move  $a0, $t0    # Move register into parameter
    jal printint  # call subroutine printint with 1 args
# Start EPILOGUE ##############################################
    lw    $ra, 8($sp)    # restore return address
    lw    $fp, 0($sp)    # restore frame pointer
    addiu $sp, $sp, 24    # Pop stack frame
    jr    $ra             # Return to caller
# END EPILOGUE #################################################
