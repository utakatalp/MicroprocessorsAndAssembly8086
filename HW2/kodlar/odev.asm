codesg SEGMENT PARA 'kod'
                ORG 100h
                ASSUME CS:codesg, DS:codesg, SS:codesg
basla:          JMP ANA 
primeOddSum     DB 15 DUP(0) 
nonPrimeOrEvenSum DB 15 DUP(0)
hipo            DB ?
bound           DW 50

ANA             PROC NEAR
                
                
                LEA SI, primeOddSum
                LEA DI, nonPrimeOrEvenSum
                MOV CX, bound     

L1:             ;
                ; Outer Loop
                MOV AX, CX
                DEC AX
                CMP AX, 0
                JE bitir
                MOV DX, AX      ; Inner Loop iterator
    
L2:             MOV AX, CX ;i
                MUL AL; i^2
                MOV BX, AX
                MOV AX, DX
                MUL AL ; j^2
                ADD AX, BX; i^2 + j^2
                PUSH CX
                PUSH DX ; store iterators
                CALL ISPSQUARE  
                CMP AX,0        ; value is not psquare
                JE CONTINUE
                MOV hipo, AL    ; value is psquare, store it in hipo
                ;MOV AX, CX      
                CALL ISPRIME ; Parameter AX = c Output: BX=c
                CMP AX, 0 ; if ax == 0 then the number is not prime
                JE addNPOES
                POP DX
                POP CX
                PUSH CX
                PUSH DX
                CALL ODDSUM ; Parameter CX, DX 
                CMP AH, 0 ; a+b == 0 
                JE addNPOES

addPOS:         MOV BL, hipo 
                MOV [SI], BX
                INC SI
                
                JMP CONTINUE

addNPOES:       MOV BL, hipo
                MOV [DI], BX
                INC DI       

CONTINUE:       POP DX
                POP CX
                DEC DX        
                JNZ L2
                
                LOOP L1
            
        
                
bitir:          RET
ANA             ENDP




ODDSUM          PROC NEAR ; CX = a DX = b

                
                MOV AX, DX
                
                ADD AX, CX
                MOV BL, 2
                DIV BL ; AH= remainder if AH = 1 the sum is odd

                RET
ODDSUM          ENDP

ISPSQUARE       PROC NEAR; PARAMETER: AX=c^2 

                MOV BX, AX ; store c^2 in BX
                MOV AX, CX ; optimization: assume a > b , a^2 + b^2 = c^2 then c > a so initiate i to a AX:i
                INC AX     ; i++
LL1:            MOV CX, AX ; store i in CX to return value after leaving loop
                MUL AX     ; i = i^2
                CMP AX, BX ;i^2 >= c^2
                JE PSQUARE ; i^2 = c^2
                JA NOTPSQUARE ; i^2 > c^2 continue i is not in range and c^2 is not perfect square
                MOV AX, CX    ; i = AX
                INC AX        ; i++
                CMP AX, bound ; is i ranging in bound?
                JBE LL1       ; if true continue loop try iteratively i

NOTPSQUARE:     MOV AX, 0     ; not psquare assign 0 to AX
                RET

PSQUARE:        MOV AX, CX    ; psqaure assign sqrt value to ax 
                RET

ISPSQUARE       ENDP

ISPRIME         PROC NEAR ; BEFORE USING DX,BX MUST BE STORED IN STACK, IF THE NUMBER IS PRIME THEN CARRY FLAG WOULD BE SET
                      ; PARAMETER: AX 
                MOV AH, 0 ; CLEANING
                MOV DH, AL; The number is being stored in DH
                MOV BL, 2 ; T(N) => T(N/2)
                DIV BL
                CMP AH, 0 ; if remainder is equal to 0 then finish, number is not prime
                JE NOTPRIME
                MOV DL, AL;
                MOV BL, 3; start dividing by 2 until AL/2

PL1:            MOV AL, DH
                MOV AH,0
                DIV BL
                CMP AH, 0 ;  checking remainder 
                JE NOTPRIME ;     equal to 0 then finish, number is not prime
                INC BL  
                CMP DH, BL
                JA PL1

                
                MOV DL, DH;          prime
                MOV DH, 0
                MOV BX, DX
                RET
        
NOTPRIME:       MOV AX, 0
                MOV DL, DH
                MOV DH, 0
                RET

ISPRIME         ENDP

codesg          ENDS
                END basla
