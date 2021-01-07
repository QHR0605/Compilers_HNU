* C-MINUS Compilation to TM Code
* File: test.tm
* -> init zero, gp, esp
  0:    LDC  0,0(0) 	reg[zero]=0 (init zero)
  1:     LD  5,0(0) 	gp=dMem[0] (dMem[0]==maxaddress)
  2:     ST  0,0(0) 	dMem[0]=0 (clear location 0)
  3:    LDA  6,1(5) 	esp=gp-global_var_num+1
* <- init zero, gp, esp
* skip 5 instr: call main, waiting for addr of main
  9:   HALT  0,0,0 	stop program
* -> pre-defined function: input
* -> function head
 10:    LDA  6,-1(6) 	PUSH EBP
 11:     ST  4,0(6) 	
 12:    LDA  4,0(6) 	MOV EBP, ESP
 13:    LDA  6,0(6) 	ESP -= # var
* <- function head
 14:     IN  1,0,0 	input ac
* -> function tail
 15:    LDA  6,0(4) 	MOV ESP, EBP
 16:    LDA  6,1(6) 	POP EBP
 17:     LD  4,-1(6) 	
 18:    LDA  6,1(6) 	Return addr; POP PC
 19:     LD  7,-1(6) 	
* <- function tail
* <- pre-defined function: input
* -> pre-defined function: output
* -> function head
 20:    LDA  6,-1(6) 	PUSH EBP
 21:     ST  4,0(6) 	
 22:    LDA  4,0(6) 	MOV EBP, ESP
 23:    LDA  6,0(6) 	ESP -= # var
* <- function head
 24:     LD  1,2(4) 	ac=dMem[ebp+2] (param 0)
 25:    OUT  1,0,0 	output ac
* -> function tail
 26:    LDA  6,0(4) 	MOV ESP, EBP
 27:    LDA  6,1(6) 	POP EBP
 28:     LD  4,-1(6) 	
 29:    LDA  6,1(6) 	Return addr; POP PC
 30:     LD  7,-1(6) 	
* <- function tail
* <- pre-defined function: output
* -> function declaration
* gcd
* -> function head
 31:    LDA  6,-1(6) 	PUSH EBP
 32:     ST  4,0(6) 	
 33:    LDA  4,0(6) 	MOV EBP, ESP
 34:    LDA  6,0(6) 	ESP -= # var
* <- function head
* -> compound
* -> if
* -> Op
* -> Id
* -> get variable
 35:    LDA  1,3(4) 	ac = ebp+offset+2
* <- get variable
 36:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
 37:    LDA  6,-1(6) 	PUSH AC
 38:     ST  1,0(6) 	
* -> Const
 39:    LDC  1,0(0) 	ac=const value
* <- Const
 40:    LDA  6,1(6) 	POP AC1
 41:     LD  2,-1(6) 	
 42:    SUB  1,2,1 	reg[ac]=reg[ac1]-reg[ac]
 43:    JEQ  1,2(7) 	conditional jmp: if true
 44:    LDC  1,0(1) 	ac=0 (false case
 45:    LDA  7,1(7) 	unconditional jmp
 46:    LDC  1,1(1) 	ac=1 (true case
* <- Op
* if: jump to else belongs here
* -> return
* -> Id
* -> get variable
 48:    LDA  1,2(4) 	ac = ebp+offset+2
* <- get variable
 49:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
* -> function tail
 50:    LDA  6,0(4) 	MOV ESP, EBP
 51:    LDA  6,1(6) 	POP EBP
 52:     LD  4,-1(6) 	
 53:    LDA  6,1(6) 	Return addr; POP PC
 54:     LD  7,-1(6) 	
* <- function tail
* <- return
* if: jump to end belongs here
 47:    JEQ  1,8(7) 	if: jmp to else
* -> return
* -> call
* gcd
* -> Op
* -> Id
* -> get variable
 56:    LDA  1,2(4) 	ac = ebp+offset+2
* <- get variable
 57:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
 58:    LDA  6,-1(6) 	PUSH AC
 59:     ST  1,0(6) 	
* -> Op
* -> Op
* -> Id
* -> get variable
 60:    LDA  1,2(4) 	ac = ebp+offset+2
* <- get variable
 61:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
 62:    LDA  6,-1(6) 	PUSH AC
 63:     ST  1,0(6) 	
* -> Id
* -> get variable
 64:    LDA  1,3(4) 	ac = ebp+offset+2
* <- get variable
 65:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
 66:    LDA  6,1(6) 	POP AC1
 67:     LD  2,-1(6) 	
 68:    DIV  1,2,1 	reg[ac]=reg[ac1] / reg[ac]
* <- Op
 69:    LDA  6,-1(6) 	PUSH AC
 70:     ST  1,0(6) 	
* -> Id
* -> get variable
 71:    LDA  1,3(4) 	ac = ebp+offset+2
* <- get variable
 72:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
 73:    LDA  6,1(6) 	POP AC1
 74:     LD  2,-1(6) 	
 75:    MUL  1,2,1 	reg[ac]=reg[ac1] * reg[ac]
* <- Op
 76:    LDA  6,1(6) 	POP AC1
 77:     LD  2,-1(6) 	
 78:    SUB  1,2,1 	reg[ac]=reg[ac1] - reg[ac]
* <- Op
 79:    LDA  6,-1(6) 	PUSH AC (for argument)
 80:     ST  1,0(6) 	
* -> Id
* -> get variable
 81:    LDA  1,3(4) 	ac = ebp+offset+2
* <- get variable
 82:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
 83:    LDA  6,-1(6) 	PUSH AC (for argument)
 84:     ST  1,0(6) 	
* -> call
* gcd
 85:    LDA  1,3(7) 	ac = pc + 3 (next pc)
 86:    LDA  6,-1(6) 	PUSH AC (return address)
 87:     ST  1,0(6) 	
 88:    LDC  7,31(0) 	pc = address (jmp to called function)
 89:    LDA  6,2(6) 	esp = esp + arg_num
* <- call
* <- call
* -> function tail
 90:    LDA  6,0(4) 	MOV ESP, EBP
 91:    LDA  6,1(6) 	POP EBP
 92:     LD  4,-1(6) 	
 93:    LDA  6,1(6) 	Return addr; POP PC
 94:     LD  7,-1(6) 	
* <- function tail
* <- return
 55:    LDA  7,39(7) 	jmp to end
* <- if
* <- compound
* -> function tail
 95:    LDA  6,0(4) 	MOV ESP, EBP
 96:    LDA  6,1(6) 	POP EBP
 97:     LD  4,-1(6) 	
 98:    LDA  6,1(6) 	Return addr; POP PC
 99:     LD  7,-1(6) 	
* <- function tail
* -> function declaration
* -> function declaration
* main
* -> function head
100:    LDA  6,-1(6) 	PUSH EBP
101:     ST  4,0(6) 	
102:    LDA  4,0(6) 	MOV EBP, ESP
103:    LDA  6,-7(6) 	ESP -= # var
* <- function head
* -> compound
* -> assign
* -> Id
* -> get variable
104:    LDA  1,-1(4) 	ac = ebp-offset-1
* <- get variable
* <- Id
105:    LDA  6,-1(6) 	push ac
106:     ST  1,0(6) 	
* -> call
* input
* -> call
* input
107:    LDA  1,3(7) 	ac = pc + 3 (next pc)
108:    LDA  6,-1(6) 	PUSH AC (return address)
109:     ST  1,0(6) 	
110:    LDC  7,10(0) 	pc = address (jmp to called function)
111:    LDA  6,0(6) 	esp = esp + arg_num
* <- call
* <- call
112:    LDA  6,1(6) 	pop ac1
113:     LD  2,-1(6) 	
114:     ST  1,0(2) 	dMem[ac1]=ac
* <- assign
* -> assign
* -> Id
* -> get variable
115:    LDA  1,-2(4) 	ac = ebp-offset-1
* <- get variable
* <- Id
116:    LDA  6,-1(6) 	push ac
117:     ST  1,0(6) 	
* -> call
* input
* -> call
* input
118:    LDA  1,3(7) 	ac = pc + 3 (next pc)
119:    LDA  6,-1(6) 	PUSH AC (return address)
120:     ST  1,0(6) 	
121:    LDC  7,10(0) 	pc = address (jmp to called function)
122:    LDA  6,0(6) 	esp = esp + arg_num
* <- call
* <- call
123:    LDA  6,1(6) 	pop ac1
124:     LD  2,-1(6) 	
125:     ST  1,0(2) 	dMem[ac1]=ac
* <- assign
* -> assign
* -> Array Id
* -> get variable
126:    LDA  2,-6(4) 	ac1 = ebp-offset-1
* <- get variable
* -> Const
127:    LDC  1,1(0) 	ac=const value
* <- Const
128:    SUB  1,2,1 	ac=ac1-ac (array offset)
* <- Array Id
129:    LDA  6,-1(6) 	push ac
130:     ST  1,0(6) 	
* -> Const
131:    LDC  1,3(0) 	ac=const value
* <- Const
132:    LDA  6,1(6) 	pop ac1
133:     LD  2,-1(6) 	
134:     ST  1,0(2) 	dMem[ac1]=ac
* <- assign
* -> assign
* -> Array Id
* -> get variable
135:    LDA  2,-8(4) 	ac1 = ebp-offset-1
* <- get variable
* -> Const
136:    LDC  1,0(0) 	ac=const value
* <- Const
137:    SUB  1,2,1 	ac=ac1-ac (array offset)
* <- Array Id
138:    LDA  6,-1(6) 	push ac
139:     ST  1,0(6) 	
* -> Const
140:    LDC  1,1(0) 	ac=const value
* <- Const
141:    LDA  6,1(6) 	pop ac1
142:     LD  2,-1(6) 	
143:     ST  1,0(2) 	dMem[ac1]=ac
* <- assign
* -> assign
* -> Array Id
* -> get variable
144:    LDA  2,-8(4) 	ac1 = ebp-offset-1
* <- get variable
* -> Const
145:    LDC  1,0(0) 	ac=const value
* <- Const
146:    SUB  1,2,1 	ac=ac1-ac (array offset)
* <- Array Id
147:    LDA  6,-1(6) 	push ac
148:     ST  1,0(6) 	
* -> Array Id
* -> get variable
149:    LDA  2,-6(4) 	ac1 = ebp-offset-1
* <- get variable
* -> Const
150:    LDC  1,1(0) 	ac=const value
* <- Const
151:    SUB  1,2,1 	ac=ac1-ac (array offset)
152:     LD  1,0(1) 	ac=dMem[ac]
* <- Array Id
153:    LDA  6,1(6) 	pop ac1
154:     LD  2,-1(6) 	
155:     ST  1,0(2) 	dMem[ac1]=ac
* <- assign
* -> call
* output
* -> Array Id
* -> get variable
156:    LDA  2,-8(4) 	ac1 = ebp-offset-1
* <- get variable
* -> Const
157:    LDC  1,0(0) 	ac=const value
* <- Const
158:    SUB  1,2,1 	ac=ac1-ac (array offset)
159:     LD  1,0(1) 	ac=dMem[ac]
* <- Array Id
160:    LDA  6,-1(6) 	PUSH AC (for argument)
161:     ST  1,0(6) 	
* -> call
* output
162:    LDA  1,3(7) 	ac = pc + 3 (next pc)
163:    LDA  6,-1(6) 	PUSH AC (return address)
164:     ST  1,0(6) 	
165:    LDC  7,20(0) 	pc = address (jmp to called function)
166:    LDA  6,1(6) 	esp = esp + arg_num
* <- call
* <- call
* -> assign
* -> Id
* -> get variable
167:    LDA  1,-2(4) 	ac = ebp-offset-1
* <- get variable
* <- Id
168:    LDA  6,-1(6) 	push ac
169:     ST  1,0(6) 	
* -> Id
* -> get variable
170:    LDA  1,-1(4) 	ac = ebp-offset-1
* <- get variable
171:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
172:    LDA  6,1(6) 	pop ac1
173:     LD  2,-1(6) 	
174:     ST  1,0(2) 	dMem[ac1]=ac
* <- assign
* -> call
* output
* -> Id
* -> get variable
175:    LDA  1,-2(4) 	ac = ebp-offset-1
* <- get variable
176:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
177:    LDA  6,-1(6) 	PUSH AC (for argument)
178:     ST  1,0(6) 	
* -> call
* output
179:    LDA  1,3(7) 	ac = pc + 3 (next pc)
180:    LDA  6,-1(6) 	PUSH AC (return address)
181:     ST  1,0(6) 	
182:    LDC  7,20(0) 	pc = address (jmp to called function)
183:    LDA  6,1(6) 	esp = esp + arg_num
* <- call
* <- call
* -> call
* output
* -> call
* gcd
* -> Id
* -> get variable
184:    LDA  1,-2(4) 	ac = ebp-offset-1
* <- get variable
185:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
186:    LDA  6,-1(6) 	PUSH AC (for argument)
187:     ST  1,0(6) 	
* -> Id
* -> get variable
188:    LDA  1,-1(4) 	ac = ebp-offset-1
* <- get variable
189:     LD  1,0(1) 	ac=dMem[ac]
* <- Id
190:    LDA  6,-1(6) 	PUSH AC (for argument)
191:     ST  1,0(6) 	
* -> call
* gcd
192:    LDA  1,3(7) 	ac = pc + 3 (next pc)
193:    LDA  6,-1(6) 	PUSH AC (return address)
194:     ST  1,0(6) 	
195:    LDC  7,31(0) 	pc = address (jmp to called function)
196:    LDA  6,2(6) 	esp = esp + arg_num
* <- call
* <- call
197:    LDA  6,-1(6) 	PUSH AC (for argument)
198:     ST  1,0(6) 	
* -> call
* output
199:    LDA  1,3(7) 	ac = pc + 3 (next pc)
200:    LDA  6,-1(6) 	PUSH AC (return address)
201:     ST  1,0(6) 	
202:    LDC  7,20(0) 	pc = address (jmp to called function)
203:    LDA  6,1(6) 	esp = esp + arg_num
* <- call
* <- call
* <- compound
* -> function tail
204:    LDA  6,0(4) 	MOV ESP, EBP
205:    LDA  6,1(6) 	POP EBP
206:     LD  4,-1(6) 	
207:    LDA  6,1(6) 	Return addr; POP PC
208:     LD  7,-1(6) 	
* <- function tail
* -> function declaration
* -> call
* main
  4:    LDA  1,3(7) 	ac = pc + 3 (next pc)
  5:    LDA  6,-1(6) 	PUSH AC (return address)
  6:     ST  1,0(6) 	
  7:    LDC  7,100(0) 	pc = address (jmp to called function)
  8:    LDA  6,0(6) 	esp = esp + arg_num
* <- call
* END OF C-MINUS Compilation to TM Code
