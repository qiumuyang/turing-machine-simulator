; Case 1: ai bj ai bj (i, j > 0).
; Input: a string of a's and b's

; 2 tapes
;   1st: input tape
;   2nd: tape stores the first half of input

; First, move the former ab's from tape1 to tape2
;     - move a until reach b (State mv1)
;     - move b until reach a (State mv2)
; Then,  compare the later ab's on tape1 with tape2 (State cmp)
; Before comparison, put head2 back to the left end (State rewind2)
; Deal with other bad cases during process

#Q = {start, mv1, mv2, cmp, rewind2, rclear, at, ar, au, ae, rf, ra, rl, rs, re, accept, reject}

#F = {accept}

#q0 = start

#S = {a, b}

#G = {b, _, t, r, u, e, f, a, l, s}

#B = _

#N = 2

; State start
start __ __ ** rf           ; empty input
start a_ a_ ** mv1
start b_ b_ ** rclear       ; input started with b

; State mv1 : move a's from tape1 to tape2 until reads b
mv1 a_ _a rr mv1
mv1 b_ b_ ** mv2
mv1 __ __ ** rf

; State mv2 : move b's from tape1 to tape2 until reads a
mv2 b_ _b rr mv2
mv2 a_ a_ *l rewind2
mv2 __ __ ** rf

; State rewind2 : move head2 to the left end, then cmp
rewind2 aa aa *l rewind2
rewind2 ab ab *l rewind2
rewind2 a_ a_ *r cmp 

; State cmp : compare the rest of tape1 with tape2
cmp aa __ rr cmp
cmp bb __ rr cmp
cmp __ __ ** at
cmp ab a_ ** rclear
cmp a_ a_ ** rclear
cmp ba b_ ** rclear
cmp b_ b_ ** rclear
cmp _a __ ** rf
cmp _b __ ** rf

; State rclear : erase tape1 from current position to the right end, then reject
;                [tape2 current symbol should be '_']
rclear a_ __ r* rclear
rclear b_ __ r* rclear
rclear __ __ ** rf

; State rf, ra, rs, rl, re : write 'false'
rf __ f_ r* ra
ra __ a_ r* rl
rl __ l_ r* rs
rs __ s_ r* re
re __ e_ r* reject

; State at, ar, au, ae : write 'true'
at __ t_ rr ar
ar __ r_ rr au
au __ u_ rr ae
ae __ e_ rr accept