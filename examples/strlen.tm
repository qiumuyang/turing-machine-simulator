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

add    *0 *1 *r return
add    *_ *1 *r return
add    *1 *0 *l add   

; State return : move the head to the right for next addition
return *_ *_ ** read  
return ** ** *r return

write  __ __ ** halt  
write  *0 00 ll write 
write  *1 11 ll write 
write  *_ __ l* write 