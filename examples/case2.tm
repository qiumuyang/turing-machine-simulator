; Case 2: 1m x 1n = 1mn (m, n > 0).
; Input: a string of 1's, x and =

; 3 tapes:
;   1st: input tape     11x111=111111
;                       ^----->
;   2nd: tape for m     11
;                      <--^
;   3rd: tape for n     111
;                      <---^

; First, move 1's from tape1 to tape2 until reach 'x' (State m)
; Then,  move 1's from tape1 to tape3 until reach '=' (State n)
; Then,  compare tape1 with tape2,3:
;     - erase one 1 on tape1 and move right head1 when head3 reads one 1, move left head3 (State cmp)
;     - put head3 back to the right end when head3 reaches '_', then move left head2 (State rewind3)
;     thus it's an m*n loop
; Deal with other bad cases during process

#Q = {start, m, n, cmp, prep, rclear, rewind3, extra, at, ar, au, ae, rf, ra, rl, rs, re, accept, reject}

#F = {accept}

#q0 = start

#S = {1, x, =}

#G = {_, 1, x, =, t, r, u, e, f, a, l, s}

#B = _

#N = 3

; State start
start ___ ___ *** rf            ; empty input
start x__ x__ *** rclear
start =__ =__ *** rclear
start 1__ 1__ *** m

; State m : move first part of 1's from tape1 to tape2
m 1__ _1_ rr* m                 ; move m to tape2
m x__ ___ r** n                 ; m finish, next n
m =__ =__ *** rclear
m ___ ___ *** rf

; State n : move second part of 1's from tape1 to tape3
n 1__ __1 r*r n                 ; move n to tape3
n =__ ___ r** prep              ; n finish, start cmp after pre-process
n x__ x__ *** rclear
n ___ ___ *** rf

; State prep : prepare for cmp
;              head2, 3 reach '_', move them left (to '1')
prep 1__ 1__ *ll prep
prep 111 111 *** cmp            ; ready for cmp
prep 11_ 1__ *** rclear         ; m has at least one 1, but n has no 1's (n empty)
prep x__ x__ *** rclear         ; illegal symbols (similarly hereinafter)
prep x_1 x__ *** rclear
prep x1_ x__ *** rclear
prep x11 x__ *** rclear
prep =__ =__ *** rclear
prep =_1 =__ *** rclear
prep =1_ =__ *** rclear
prep =11 =__ *** rclear
prep ___ ___ *** rclear         ; no 1's after '=' (similarly hereinafter)
prep __1 ___ *** rclear
prep _1_ ___ *** rclear
prep _11 ___ *** rclear

; State cmp : tape2 has m 1's and tape3 n,
;             erase one 1 on tape1 for each m,n on tape2,3
cmp 111 _11 r*l cmp             ; loop: tape2 outer, tape3 inner
cmp _1_ ___ *l* extra           ; extra case
cmp 11_ 11_ *lr rewind3         ; inner loop ends, set head3 back
cmp ___ ___ *** at              ; mn == m*n, accept
cmp 1__ 1__ *** rclear          ; more mn than m*n (similarly hereinafter)
cmp 1_1 1__ *** rclear
cmp x11 x__ *** rclear          ; illegal symbol (similarly hereinafter)
cmp x1_ x__ *** rclear
cmp x_1 x__ *** rclear
cmp x__ x__ *** rclear
cmp =11 =__ *** rclear
cmp =1_ =__ *** rclear
cmp =_1 =__ *** rclear
cmp =__ =__ *** rclear
cmp _11 ___ *** rf              ; less mn than m*n (similarly hereinafter)
cmp __1 ___ *** rf

; State extra : input 1xn=n or mx1=m will reach '_1_', tape2 should be empty after this 1 removed
;               add this state to judge as extra
extra ___ ___ *** at
extra _1_ ___ *** rf

; State rewind3 : move head3 to rightmost
rewind3 111 111 **r rewind3     ; half way (similarly hereinafter)
rewind3 1_1 1_1 **r rewind3
rewind3 _11 _11 **r rewind3
rewind3 __1 __1 **r rewind3
rewind3 11_ 11_ **l cmp         ; reach right end (similarly hereinafter)
rewind3 1__ 1__ **l cmp
rewind3 _1_ _1_ **l cmp
rewind3 ___ ___ **l cmp

; State rclear : erase tape1 from current position to the right end, then reject
;                [tape2,3 current symbol should be '_']
rclear 1__ ___ r** rclear
rclear x__ ___ r** rclear
rclear =__ ___ r** rclear
rclear ___ ___ *** rf

; State rf, ra, rs, rl, re : write 'false'
rf ___ f__ r** ra
ra ___ a__ r** rl
rl ___ l__ r** rs
rs ___ s__ r** re
re ___ e__ r** reject

; State at, ar, au, ae : write 'true'
at ___ t__ r** ar
ar ___ r__ r** au
au ___ u__ r** ae
ae ___ e__ r** accept