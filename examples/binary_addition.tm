; This example program calculates the addition of two binary number
; Input:  two binary number concatenated by '#'
; Output: the addition of two binary number
; > 101#10
;   111
; > 1110#11
;   10001

#Q = {start, to_op2, move_op2, to_op1, add, add_c, move_res, halt, reject, reject2, reject3, reject4}

#S = {0, 1, #}

#G = {0, 1, #, _, e, r}

#B = _

#F = {halt}

#N = 3

#q0 = start

start    0__ 0__ *** to_op2  
start    1__ 1__ *** to_op2  
start    #__ #__ *** reject   ; the first operand is empty
start    ___ 0__ *** halt    

to_op2   0__ 0__ r** to_op2  
to_op2   1__ 1__ r** to_op2  
to_op2   #__ ___ r** move_op2 ; reach the beginning of the second operand
to_op2   ___ ___ l** reject  

reject   0__ ___ l** reject   ; clear the first tape and output 'err'
reject   1__ ___ l** reject  
reject   #__ ___ l** reject  
reject   ___ ___ l** reject2 
reject2  ___ e__ r** reject3 
reject3  ___ r__ r** reject4 
reject4  ___ r__ r** halt    

move_op2 0__ _0_ rr* move_op2
move_op2 1__ _1_ rr* move_op2
move_op2 #__ ___ l** reject  
move_op2 ___ ___ ll* to_op1  

to_op1   _0_ _0_ l** to_op1  
to_op1   _1_ _1_ l** to_op1  
to_op1   00_ 00_ *** add     
to_op1   01_ 01_ *** add     
to_op1   10_ 10_ *** add     
to_op1   11_ 11_ *** add     

add      00_ 000 lll add     
add      01_ 011 lll add     
add      10_ 101 lll add     
add      11_ 110 lll add_c   
add      _0_ _00 lll add     
add      _1_ _11 lll add     
add      0__ 0_0 lll add     
add      1__ 1_1 lll add     
add      ___ ___ r*r move_res

add_c    00_ 001 lll add     
add_c    01_ 010 lll add_c   
add_c    10_ 100 lll add_c   
add_c    11_ 111 lll add_c   
add_c    _0_ _01 lll add     
add_c    _1_ _10 lll add_c   
add_c    0__ 0_1 lll add     
add_c    1__ 1_0 lll add_c   
add_c    ___ __1 r** move_res

move_res 0_0 0__ r*r move_res ; move the third tape to the first tape
move_res 1_0 0__ r*r move_res
move_res __0 0__ r*r move_res
move_res 0_1 1__ r*r move_res
move_res 1_1 1__ r*r move_res
move_res __1 1__ r*r move_res
move_res 0__ ___ r*r move_res
move_res 1__ ___ r*r move_res
move_res ___ ___ *** halt    