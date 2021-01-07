* C-MINUS Compilation to TM Code
* File: test/test.tm
* Standard prelude:
  0:     LD  5,0(0) 	gp=dMem[0] (dMem[0]==maxaddress)
  1:     ST  0,0(0) 	dMem[0]=0 (clear location 0)
  2:    LDA  6,1(5) 	esp=gp-global_var_num+1
* End of standard prelude.
* skip 5 instr: call main, waiting for addr of main
  8:   HALT  0,0,0 	stop program
* -> pre-defined function: input
* -> function head
  9:    LDA  6,-1(6) 	PUSH EBP
 10:     ST  4,0(6) 	
 11:    LDA  4,0(6) 	MOV EBP, ESP
 12:    LDA  6,0(6) 	ESP -= # var
* <- function head
 13:     IN  0,0,0 	input ac
* -> function tail
 14:    LDA  6,0(4) 	MOV ESP, EBP
 15:    LDA  6,1(6) 	POP EBP
 16:     LD  4,-1(6) 	
 17:    LDA  6,1(6) 	Return addr; POP PC
 18:     LD  7,-1(6) 	
* <- function tail
* <- pre-defined function: input
* -> pre-defined function: output
* -> function head
 19:    LDA  6,-1(6) 	PUSH EBP
 20:     ST  4,0(6) 	
 21:    LDA  4,0(6) 	MOV EBP, ESP
 22:    LDA  6,0(6) 	ESP -= # var
* <- function head
 23:     LD  0,2(4) 	ac=dMem[ebp+2] (param 0)
 24:    OUT  0,0,0 	output ac
* -> function tail
 25:    LDA  6,0(4) 	MOV ESP, EBP
 26:    LDA  6,1(6) 	POP EBP
 27:     LD  4,-1(6) 	
 28:    LDA  6,1(6) 	Return addr; POP PC
 29:     LD  7,-1(6) 	
* <- function tail
* <- pre-defined function: output
* -> function declaration
* gcd
* -> function head
 30:    LDA  6,-1(6) 	PUSH EBP
 31:     ST  4,0(6) 	
 32:    LDA  4,0(6) 	MOV EBP, ESP
 33:    LDA  6,0(6) 	ESP -= # var
* <- function head
* -> compound
* -> if
* -> Op
* -> Id
* -> get variable
 34:    LDA  0,3(4) 	ac = ebp+offset+2
* <- get variable
 35:     LD  0,0(0) 	ac=dMem[ac]
* <- Id
 36:    LDA  6,-1(6) 	PUSH AC
 37:     ST  0,0(6) 	
* -> Const
 38:    LDC  0,0(0) 	ac=const value
* <- Const
 39:    LDA  6,1(6) 	POP AC1
 40:     LD  1,-1(6) 	
 41:    SUB  0,1,0 	reg[ac]=reg[ac1]-reg[ac]
 42:    JEQ  0,2(7) 	conditional jmp: if true
 43:    LDC  0,0(0) 	ac=0 (false case
 44:    LDA  7,1(7) 	unconditional jmp
 45:    LDC  0,1(0) 	ac=1 (true case
* <- Op
* if: jump to else belongs here
* -> return
* -> Id
* -> get variable
 47:    LDA  0,2(4) 	ac = ebp+offset+2
* <- get variable
 48:     LD  0,0(0) 	ac=dMem[ac]
* <- Id
* -> function tail
 49:    LDA  6,0(4) 	MOV ESP, EBP
 50:    LDA  6,1(6) 	POP EBP
 51:     LD  4,-1(6) 	
 52:    LDA  6,1(6) 	Return addr; POP PC
 53:     LD  7,-1(6) 	
* <- function tail
* <- return
* if: jump to end belongs here
 46:    JEQ  0,8(7) 	if: jmp to else
* -> return
* -> call
* gcd
* -> Op
* -> Id
* -> get variable
 55:    LDA  0,2(4) 	ac = ebp+offset+2
* <- get variable
 56:     LD  0,0(0) 	ac=dMem[ac]
* <- Id
 57:    LDA  6,-1(6) 	PUSH AC
 58:     ST  0,0(6) 	
* -> Op
* -> Op
* -> Id
* -> get variable
 59:    LDA  0,2(4) 	ac = ebp+offset+2
* <- get variable
 60:     LD  0,0(0) 	ac=dMem[ac]
* <- Id
 61:    LDA  6,-1(6) 	PUSH AC
 62:     ST  0,0(6) 	
* -> Id
* -> get variable
 63:    LDA  0,3(4) 	ac = ebp+offset+2
* <- get variable
 64:     LD  0,0(0) 	ac=dMem[ac]
* <- Id
 65:    LDA  6,1(6) 	POP AC1
 66:     LD  1,-1(6) 	
 67:    DIV  0,1,0 	reg[ac]=reg[ac1] / reg[ac]
* <- Op
 68:    LDA  6,-1(6) 	PUSH AC
 69:     ST  0,0(6) 	
* -> Id
* -> get variable
 70:    LDA  0,3(4) 	ac = ebp+offset+2
* <- get variable
 71:     LD  0,0(0) 	ac=dMem[ac]
* <- Id
 72:    LDA  6,1(6) 	POP AC1
 73:     LD  1,-1(6) 	
 74:    MUL  0,1,0 	reg[ac]=reg[ac1] * reg[ac]
* <- Op
 75:    LDA  6,1(6) 	POP AC1
 76:     LD  1,-1(6) 	
 77:    SUB  0,1,0 	reg[ac]=reg[ac1] - reg[ac]
* <- Op
 78:    LDA  6,-1(6) 	PUSH AC (for argument)
 79:     ST  0,0(6) 	
* -> Id
* -> get variable
 80:    LDA  0,3(4) 	ac = ebp+offset+2
* <- get variable
 81:     LD  0,0(0) 	ac=dMem[ac]
* <- Id
 82:    LDA  6,-1(6) 	PUSH AC (for argument)
 83:     ST  0,0(6) 	
* -> call
* gcd
 84:    LDA  0,3(7) 	ac = pc + 3 (next pc)
 85:    LDA  6,-1(6) 	PUSH AC (return address)
 86:     ST  0,0(6) 	
 87:    LDC  7,30(0) 	pc = address (jmp to called function)
 88:    LDA  6,2(6) 	esp = esp + arg_num
* <- call
* <- call
* -> function tail
 89:    LDA  6,0(4) 	MOV ESP, EBP
 90:    LDA  6,1(6) 	POP EBP
 91:     LD  4,-1(6) 	
 92:    LDA  6,1(6) 	Return addr; POP PC
 93:     LD  7,-1(6) 	
* <- function tail
* <- return
 54:    LDA  7,39(7) 	jmp to end
* <- if
* <- compound
* -> function tail
 94:    LDA  6,0(4) 	MOV ESP, EBP
 95:    LDA  6,1(6) 	POP EBP
 96:     LD  4,-1(6) 	
 97:    LDA  6,1(6) 	Return addr; POP PC
 98:     LD  7,-1(6) 	
* <- function tail
* -> function declaration
* -> function declaration
* main
* -> function head
 99:    LDA  6,-1(6) 	PUSH EBP
100:     ST  4,0(6) 	
101:    LDA  4,0(6) 	MOV EBP, ESP
102:    LDA  6,-2(6) 	ESP -= # var
* <- function head
* -> compound
* -> assign
* -> Id
* -> get variable
103:    LDA  0,-1(4) 	ac = ebp-offset-1
* <- get variable
* <- Id
104:    LDA  6,-1(6) 	push ac
105:     ST  0,0(6) 	
* -> call
* input
* -> call
* input
106:    LDA  0,3(7) 	ac = pc + 3 (next pc)
107:    LDA  6,-1(6) 	PUSH AC (return address)
108:     ST  0,0(6) 	
109:    LDC  7,9(0) 	pc = address (jmp to called function)
110:    LDA  6,0(6) 	esp = esp + arg_num
* <- call
* <- call
111:    LDA  6,1(6) 	pop ac1
112:     LD  1,-1(6) 	
113:     ST  0,0(1) 	dMem[ac1]=ac
* <- assign
* -> assign
* -> Id
* -> get variable
114:    LDA  0,-2(4) 	ac = ebp-offset-1
* <- get variable
* <- Id
115:    LDA  6,-1(6) 	push ac
116:     ST  0,0(6) 	
* -> call
* input
* -> call
* input
117:    LDA  0,3(7) 	ac = pc + 3 (next pc)
118:    LDA  6,-1(6) 	PUSH AC (return address)
119:     ST  0,0(6) 	
120:    LDC  7,9(0) 	pc = address (jmp to called function)
121:    LDA  6,0(6) 	esp = esp + arg_num
* <- call
* <- call
122:    LDA  6,1(6) 	pop ac1
123:     LD  1,-1(6) 	
124:     ST  0,0(1) 	dMem[ac1]=ac
* <- assign
* -> call
* output
* -> call
* gcd
* -> Id
* -> get variable
125:    LDA  0,-2(4) 	ac = ebp-offset-1
* <- get variable
126:     LD  0,0(0) 	ac=dMem[ac]
* <- Id
127:    LDA  6,-1(6) 	PUSH AC (for argument)
128:     ST  0,0(6) 	
* -> Id
* -> get variable
129:    LDA  0,-1(4) 	ac = ebp-offset-1
* <- get variable
130:     LD  0,0(0) 	ac=dMem[ac]
* <- Id
131:    LDA  6,-1(6) 	PUSH AC (for argument)
132:     ST  0,0(6) 	
* -> call
* gcd
133:    LDA  0,3(7) 	ac = pc + 3 (next pc)
134:    LDA  6,-1(6) 	PUSH AC (return address)
135:     ST  0,0(6) 	
136:    LDC  7,30(0) 	pc = address (jmp to called function)
137:    LDA  6,2(6) 	esp = esp + arg_num
* <- call
* <- call
138:    LDA  6,-1(6) 	PUSH AC (for argument)
139:     ST  0,0(6) 	
* -> call
* output
140:    LDA  0,3(7) 	ac = pc + 3 (next pc)
141:    LDA  6,-1(6) 	PUSH AC (return address)
142:     ST  0,0(6) 	
143:    LDC  7,19(0) 	pc = address (jmp to called function)
144:    LDA  6,1(6) 	esp = esp + arg_num
* <- call
* <- call
* <- compound
* -> function tail
145:    LDA  6,0(4) 	MOV ESP, EBP
146:    LDA  6,1(6) 	POP EBP
147:     LD  4,-1(6) 	
148:    LDA  6,1(6) 	Return addr; POP PC
149:     LD  7,-1(6) 	
* <- function tail
* -> function declaration
* -> call
* main
  3:    LDA  0,3(7) 	ac = pc + 3 (next pc)
  4:    LDA  6,-1(6) 	PUSH AC (return address)
  5:     ST  0,0(6) 	
  6:    LDC  7,99(0) 	pc = address (jmp to called function)
  7:    LDA  6,0(6) 	esp = esp + arg_num
* <- call
* END OF C-MINUS Compilation to TM Code
