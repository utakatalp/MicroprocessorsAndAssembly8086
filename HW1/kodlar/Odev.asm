my_ss       SEGMENT PARA STACK 'yigin'
            DW 20 DUP(?)
my_ss       ENDS

my_ds       SEGMENT PARA 'veri'

vize DW 77, 85, 64, 96
final DW 56, 63, 86, 74
obp           DW 4 DUP(?)         
obp_siralama  DW 4 DUP(?)         
              

my_ds ENDS

my_cs SEGMENT PARA 'kod'
            ASSUME CS:my_cs, DS:my_ds, SS:my_ss
YORDAM      PROC FAR
            PUSH DS
            XOR AX,AX
            PUSH AX
            MOV AX, my_ds
            MOV DS, AX

            push DS
            xor ax,ax
            push ax
            mov ax,my_ds
            mov ds, AX


            

        

             
            MOV CX, 4
            LEA SI, vize
            LEA DI, final
            LEA BX, obp

calculate:  MOV AX, [SI]
            MOV DX, 4h
            MUL DX
            PUSH AX                 ; vize notunun 4 ile çarpılmış hali saklanır

            MOV AX, [DI]
            MOV DX, 6h
            MUL DX

            POP DX                  ; 4 ile çarpılan vize notunu pop'la
            ADD AX, DX
            
            PUSH BX                 ; obp dizisi saklanır
            MOV BX, 10
            DIV BL                  ; Finalle vize toplanır 10'a bölünür 
            
            POP BX                  ; obp dizisini geri al
            
            CMP AH, 5               ; Kalan 5'ten büyükse yukarı yuvarlanır küçükse gerek yok
            JB devam
            INC AL                  ; Round

devam:      MOV AH, 00h
            MOV [BX], AX

            ADD SI,2
            ADD DI,2                ; Dizilerde gezinme
            ADD BX,2
            LOOP calculate

            ; Hesaplama ile işimiz bitti

            LEA SI, obp
            LEA DI, obp_siralama    ; 
            MOV CX, 4
copy_loop:
            MOV AX, [SI]              ; AX = obp[i]
            MOV [DI], AX              ; obp_siralama[i] = AX
            ADD SI, 2                 ; Bir sonraki obp elemanına geç
            ADD DI, 2                 ; Bir sonraki obp_siralama elemanına geç
            LOOP copy_loop            ; CX 0 olana kadar döngü

            ; Selection Sort (Büyükten Küçüğe)
            LEA SI, obp_siralama      ; SI = obp_siralama dizisinin başlangıç adr   
            MOV CX, 3                 ; Dış döngü için CX = 3

outerLoop:
            MOV DX, CX     
            ;DEC DX                    ; DX iç döngü sayacı
            MOV DI, SI                ; DI = İç döngüde karşılaştırma adresi
            ADD DI, 2                 ; İlk karşılaştırmayı atlamak için DI 1 word arttırılır, bir sonraki elemana geçilir
            MOV AX, [SI]              ; AX = Geçici maksimum (obp_siralama[i])
            MOV BX, SI                ; BX = geçici olarak Maksimum elemanın adresi

innerLoop:
            CMP AX, [DI]              ; AX ile obp_siralama[j] karşılaştır
            JAE traverseInner         ; Eğer AX >= obp_siralama[j] ise atla (unsigned)
            MOV AX, [DI]              ; Daha büyük eleman bulundu, AX'e ata
            MOV BX, DI                ; BX = Yeni maksimum elemanın adresi

traverseInner:
            ADD DI, 2                 ; Bir sonraki elemana geç
            DEC DX                    ; DX sayaç azalt
            JG innerLoop              ; DX sıfırdan büyükse döngüye devam et

            ; swap
            XCHG AX, [SI]             ; AX ve obp_siralama[i]'nin değerini değiştir
            XCHG AX, [BX]             ; AX ve obp_siralama[maks] değerini değiştir
            ;XCHG [SI], [BX]          ; çalışmadı

    
            ADD SI, 2                 ; SI'yi artırarak bir sonraki elemana geç
            LOOP outerLoop           ; CX sıfır değilse döngü devam etsin


    ; Programı sonlandır
           
           
            MOV AH, 09h
            MOV DX, OFFSET obp
            INT 21h

            MOV AH, 04Ch
            INT 21h

            RETF
YORDAM      ENDP
my_cs       ENDS
            END YORDAM