; This example program does a right circular shift to the input binary number
; > 11101001
;   11110100
; > 00000000
;   00000000

#Q = {start, halt, copy, head, head0, head1}

#S = {0, 1}

#G = {0, 1, _}

#B = _

#q0 = start

#F = {halt}

#N = 2

start __ __ ** halt 
; start *_ *_ *r copy

copy  0_ 00 rr copy 
copy  1_ 11 rr copy 
copy  __ __ ll head 

; head *0 *_ *l head0
; head *1 *_ *l head1
; head0 *_ 0_ ** halt
; head0 *0 00 ll head0
; head0 *1 11 ll head0
; head1 *_ 1_ ** halt
; head1 *0 00 ll head1
; head1 *1 11 ll head1

; program expanded transitions
start 0_ 0_ *r copy 
start 1_ 1_ *r copy 
head  00 0_ *l head0
head  10 1_ *l head0
head  _0 __ *l head0
head  01 0_ *l head1
head  11 1_ *l head1
head  _1 __ *l head1
head0 0_ 0_ ** halt 
head0 1_ 0_ ** halt 
head0 __ 0_ ** halt 
head0 00 00 ll head0
head0 10 00 ll head0
head0 _0 00 ll head0
head0 01 11 ll head0
head0 11 11 ll head0
head0 _1 11 ll head0
head1 0_ 1_ ** halt 
head1 1_ 1_ ** halt 
head1 __ 1_ ** halt 
head1 00 00 ll head1
head1 10 00 ll head1
head1 _0 00 ll head1
head1 01 11 ll head1
head1 11 11 ll head1
head1 _1 11 ll head1