; This example program checks if the length of input is a perfect square number.
; Input: a string of 1's
; > 1111
;   true
; > 1111111111111111
;   true
; > 11111111111111111
;   false
; Note: (n + 1) ^ 2 = n ^ 2 + 2 * n + 1, check it iteratively.

#Q = {start, halt, acc_clr, rej_clr, acc1, acc2, acc3, acc4, rej1, rej2, rej3, rej4, rej5, consume, consume_r, incr, incr_r}

#S = {1}

#G = {1, _, t, r, u, e, f, a, l, s}

#F = {halt}

#q0 = start

#B = _

#N = 2

start     __ __ ** acc_clr  
start     1_ 11 ** consume  

; consume 1's from left to right
consume   11 11 rr consume  
consume   1_ 11 *r incr      ; head 2 reaches the end, append 2 1's
incr      1_ 11 ** consume_r ; consume reversally
consume   _1 _1 l* rej_clr  
consume   __ __ l* acc_clr  

; consume 1's from right to left
consume_r 11 11 rl consume_r
consume_r 1_ 11 *l incr_r   
incr_r    1_ 11 ** consume  
consume_r _1 _1 l* rej_clr  
consume_r __ __ l* acc_clr  

acc_clr   1* _* l* acc_clr  
acc_clr   _* _* ** acc1     
rej_clr   1* _* l* rej_clr  
rej_clr   _* _* ** rej1     

acc1      ** t* r* acc2     
acc2      ** r* r* acc3     
acc3      ** u* r* acc4     
acc4      ** e* r* halt     

rej1      ** f* r* rej2     
rej2      ** a* r* rej3     
rej3      ** l* r* rej4     
rej4      ** s* r* rej5     
rej5      ** e* r* halt     