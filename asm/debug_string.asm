.ORIG x3000
    LD R7, STACK            ; init stack
    LEA R6, STRING          ; init string

LOOP_PUSH
    LDR R5, R6, #0          ; 'h' 'e' 'l' 'l' 'o' '\0' <- here we want to jump to DONE
    BRz DONE
    
    STR R5, R7, #-1
    ADD R7, R7, #-1
    ADD R6, R6, #1          ; advance through the word, pointing to the next address
    BR LOOP_PUSH

DONE
    LEA R5, STRING          ; Load the string again
    LD R4, STACK
    NOT R4, R4
    ADD R4, R4, #1

LOOP_POP
    ADD R3, R7, R4
    BRz END

    AND R1, R1, #0
    LDR R1, R7, #0          ; get the current value from the stack
    STR R1, R5, #0          ; store the current address of the string
    ADD R7, R7, #1
    ADD R5, R5, #1
    BR LOOP_POP

END
    LEA R0, STRING
    PUTS
    
    HALT

STACK   .FILL       x4000
STRING  .STRINGZ    "hello, from assembler"
.END
