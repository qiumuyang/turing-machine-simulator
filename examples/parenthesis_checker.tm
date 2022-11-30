; This example program checks if the input expression has balanced parentheses.
; Input: An expression with parentheses.
; > <([]+())>
;   true
; > ())
;   false

#Q = {start, read, check1, check2, check3, check_empty, accept, aclear, accept2, accept3, accept4, reject, rclear, reject2, reject3, reject4, reject5, halt}

#F = {halt}

#S = {(, ), [, ], <, >, +, -, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9}

#G = {(, ), [, ], <, >, +, -, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, _}

#q0 = start

#B = _

#N = 2

start       __ __ ** accept     
start       *_ *_ ** read       

; Read symbols
;    If symbol in {'(', '[', '<'} then push symbol to stack (tape 2)
;    If symbol in {')', ']', '>'} then goto check state for pop
;    If blank reached then check stack is empty
;    Else continue reading
read        __ __ *l check_empty
read        (_ (( rr read       
read        [_ [[ rr read       
read        <_ << rr read       
read        )_ )_ rl check1     
read        ]_ ]_ rl check2     
read        >_ >_ rl check3     
read        *_ *_ r* read       

; When the input ends, check if the stack is empty.
check_empty __ __ ** accept     
check_empty _( _( lr reject     
check_empty _[ _[ lr reject     
check_empty _< _< lr reject     

; Check if the corresponding opening parentheses is on the top of the stack.
check1      *( *_ ** read       
check1      *_ *_ ** reject     
check1      *[ *[ *r reject     
check1      *< *< *r reject     
check2      *[ *_ ** read       
check2      *_ *_ ** reject     
check2      *( *( *r reject     
check2      *< *< *r reject     
check3      *< *_ ** read       
check3      *_ *_ ** reject     
check3      *( *( *r reject     
check3      *[ *[ *r reject     

accept      __ __ l* aclear     
accept      *_ *_ r* accept     
aclear      __ t_ r* accept2    
aclear      *_ __ l* aclear     
accept2     __ r_ r* accept3    
accept3     __ u_ r* accept4    
accept4     __ e_ ** halt       

reject      __ __ l* rclear     
reject      *_ *_ r* reject     
rclear      __ f_ r* reject2    
rclear      *_ __ l* rclear     
reject2     __ a_ r* reject3    
reject3     __ l_ r* reject4    
reject4     __ s_ r* reject5    
reject5     __ e_ ** halt       