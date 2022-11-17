; This example program divides the input binary number by 2.
; Input:  binary number X with no leading zeros
; Output: ceiling( X / 2 )
; > 1111 
;   1000
; > 10010
;   1001

#Q = {
    start,
    move_to_lsbit,
    div,
    add_one,
    halt,
    clear_rest
}

#S = {0, 1}

#G = {0, 1, _}

#B = _

#F = {halt}

#q0 = start

#N = 1

start         0 0 r clear_rest    ; input starts with 0, treated as 0, clear rest of tape and halt
start         1 1 r move_to_lsbit ; input starts with 1, move to least significant bit
start         _ _ * halt          ; input is empty, halt

move_to_lsbit 0 0 r move_to_lsbit
move_to_lsbit 1 1 r move_to_lsbit
move_to_lsbit _ _ l div           ; reach end of input, move to left and start division

div           0 _ l halt          ; input is even, set the lsb to 0 and halt
div           1 0 l add_one       ; input is odd, add X with 1 and re-perform division

add_one       0 1 r move_to_lsbit ; no more carry
add_one       1 0 l add_one       ; carry
add_one       _ 1 r move_to_lsbit ; carry to the most significant bit

clear_rest    0 _ r clear_rest    ; clear rest of tape
clear_rest    1 _ r clear_rest   
clear_rest    _ _ * halt         