.ORIG x3000
    LEA R0, MSG
    PUTS

    GETC
    OUT

    LD R6, MAX_UPPER
    NOT R6, R6
    ADD R6, R6, #1

    ADD R3, R0, R6

    BRp IS_LOWER

    LEA R0, UPPER
    BRnzp PRINT

IS_LOWER
    LEA R0, LOWER

PRINT
    PUTS
    HALT

MAX_UPPER .FILL x5A
MSG .STRINGZ "Enter a character\n"
UPPER .STRINGZ " is upper"
LOWER .STRINGZ " is lower"
.END
