
.586
.model flat, stdcall
option casemap :none

include C:\masm32\include\masm32rt.inc
include C:\masm32\macros\macros.asm
include C:\masm32\include\msvcrt.inc
includelib C:\masm32\lib\kernel32
includelib C:\masm32\lib\msvcrt
includelib C:\masm32\lib\masm32

.data

stm  SYSTEMTIME<>
    org stm
wYear dw 0
wMonth dw 0
wToDay dw 0 ; Sunday 0 to Saturday 6
wDay    dw 0
wHour   dw 0
wMinute dw 0
wSecond dw 0
wKsecond dw 0
startTime dw 0
pow real8 1.00
zero real8 0.0
negone sdword -1
one dword 1
two dword 2
ten dword 10
rnd dword 0
rndCtr dword 0
d real8 0.0
output real8 0.0
result real8 0.0
weights real8 3 DUP(0.0)
inputs real8 0.0
real8 1.0
real8 1.0
real8 1.0
real8 0.0
real8 1.0

outputs real8 0.0
real8 1.0

.code

_random:
	invoke GetSystemTime, addr stm
	movzx ecx,wKsecond
	add ecx, rndCtr
	invoke nseed, ecx
	invoke nrandom, 10d
	mov rnd,0
	add rnd, eax
	add rndCtr, ecx
	ret
epow:
	fld output
	fstp pow
	fldl2e
	FMUL pow
	fst result
	fld st
	frndint
	fsub st(1),st
	fxch st(1)
	f2xm1
	fld1
	fadd
	fscale       
	fstp result
	fstp st(0)
	ret
delay:
	dec bp
	cmp bp, 0
	jne delay
	dec si
	nop
	jnz delay
	dec di
	cmp di,0 
	jnz delay
	ret
setupWeights:
	mov ebx, 4
	xor esi, esi
	newWeight:
		push esi
		mov bp, 8
		fldz
		digit:
			call _random
			FIADD rnd
			FIDIV ten
			FIMUL two
			FISUB one
			dec bp
			jnz digit
		pop esi
		fstp REAL8 PTR weights[esi]
		mov eax, -1
		add esi, 8
		dec ebx
		jnz newWeight
	ret
run:
	fldz
	fstp output

	mov ecx, 0
	inWeight:
		fld weights[ecx]
		fmul inputs[eax*8+ecx]
		fadd output
		fstp output
		add ecx,8
		cmp ecx,24
		jne inWeight

	call epow

	fld result
	fiadd one
	fst output
	fisub one
	fdiv output
	fstp output
	
	ret
d_sigmoid:
	FiMUL negone
	fiadd one
	fmul d
	ret
train:
	call run
	
	fld output
	fimul negone
	FADD outputs[ebx*8]
	fst d
	call d_sigmoid
	fstp d

	mov ecx, 0
	err:
		fld d
		fmul inputs[eax*8+ecx]
		fadd weights[ecx]
		fstp weights[ecx]
		add ecx,8
		cmp ecx,24
		jne err
		
	ret

_start:
	
	invoke GetSystemTime, addr stm
	mov di, wKsecond
	mov startTime, di
call setupWeights
mov eax,0
call run
printf("Result: %f\n", output)
mov eax,3
call run
printf("Result: %f\n", output)
mov edx, 100000
training:
mov eax, 0
mov ebx, 0
call train
mov eax, 3
mov ebx, 1
call train
dec edx
cmp edx, 0
jnz training
mov eax,0
call run
printf("Result: %f\n", output)
mov eax,3
call run
printf("Result: %f\n", output)
	invoke GetSystemTime, addr stm
	mov di, wKsecond
	sub di, startTime
	printf("run time: %i ms\n", di)
	exit
end _start