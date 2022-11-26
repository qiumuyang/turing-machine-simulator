; This example program counts the length of the input string
; > aabbb
;   101
; > b
;   1

#Q = {start, read, add, return, write, halt}

#S = {a, b}

#G = {a, b, 0, 1, _}

#B = _

#q0 = start

#F = {halt}

#N = 2

start  __ 0_ ** halt  
start  a_ a0 *r read  
start  b_ b0 *r read  

read   a_ a_ rl add   
read   b_ b_ rl add   
read   __ __ ll write 

; add *0 *1 *r return
; add *_ *1 *r return
; add *1 *0 *l add

; State return : move the head to the right for next addition
; return *_ *_ ** read
; return ** ** *r return

write  __ __ ** halt  
; write *0 00 ll write
; write *1 11 ll write
; write *_ __ l* write

; program expanded transitions
add    a0 a1 *r return
add    b0 b1 *r return
add    00 01 *r return
add    10 11 *r return
add    _0 _1 *r return
add    a_ a1 *r return
add    b_ b1 *r return
add    0_ 01 *r return
add    1_ 11 *r return
add    __ _1 *r return
add    a1 a0 *l add   
add    b1 b0 *l add   
add    01 00 *l add   
add    11 10 *l add   
add    _1 _0 *l add   
return a_ a_ ** read  
return b_ b_ ** read  
return 0_ 0_ ** read  
return 1_ 1_ ** read  
return __ __ ** read  
return aa aa *r return
return ab ab *r return
return a0 a0 *r return
return a1 a1 *r return
return ba ba *r return
return bb bb *r return
return b0 b0 *r return
return b1 b1 *r return
return 0a 0a *r return
return 0b 0b *r return
return 00 00 *r return
return 01 01 *r return
return 1a 1a *r return
return 1b 1b *r return
return 10 10 *r return
return 11 11 *r return
return _a _a *r return
return _b _b *r return
return _0 _0 *r return
return _1 _1 *r return
write  a0 00 ll write 
write  b0 00 ll write 
write  00 00 ll write 
write  10 00 ll write 
write  _0 00 ll write 
write  a1 11 ll write 
write  b1 11 ll write 
write  01 11 ll write 
write  11 11 ll write 
write  _1 11 ll write 
write  a_ __ l* write 
write  b_ __ l* write 
write  0_ __ l* write 
write  1_ __ l* write 