        ; -----------------------------------------------------------------------
        ; Okunan işaretli iki sayının toplamını hesaplayıp ekrana yazdırır.
        ; ANA 		: Ana yordam 
        ; PUT_STR 	: Ekrana sonu 0 ile belirlenmiş dizgeyi yazdırır. 
        ; PUTC 	: AL deki karakteri ekrana yazdırır. 
        ; GETC 	: Klavyeden basılan karakteri AL’ye alır.
        ; PUTN 	: AX’deki sayeyi ekrana yazdırır. 
        ; GETN 	: Klavyeden okunan sayeyi AX’e koyar
        ; -----------------------------------------------------------------------
SSEG 	SEGMENT PARA STACK 'STACK'
	DW 32 DUP (?)
SSEG 	ENDS

DSEG	SEGMENT PARA 'DATA'
CR	EQU 13
LF	EQU 10
HATA	DB CR, LF, 'Dikkat !!! Sayi vermediniz yeniden giris yapiniz.!!!  ', 0
MOST_FREQ_MSG DB CR, LF, 'En cok tekrar eden sayi: ', 0
ARRAYLENGTH DB CR, LF, 'Pozitif Dizi Uzunlugu:', 0
SAYILAR DW 10 DUP(?)
N       DW 0
modVal DW ?
maxCount DB 0

DSEG 	ENDS 

CSEG 	SEGMENT PARA 'CODE'
	ASSUME CS:CSEG, DS:DSEG, SS:SSEG

        GIRDI MACRO SAYILAR,n
        
        LOCAL L1
        ; Array uzunlugunu n'ye aktarma
        MOV AX, OFFSET ARRAYLENGTH 
        CALL PUT_STR
        CALL GETN
        MOV n, AX
        MOV CX, AX ; döngü sayısını atama
        XOR SI, SI ; döngü index'ini sıfırlama

L1:     CALL GETN       ; kullanıcıdan sayı alma
        MOV SAYILAR[SI], AX ; alınan sayıyı diziye kaydetme
        ADD SI, 2               ; dizinin sonraki elemanına geçme
        LOOP L1
                 
        ENDM

ANA 	PROC FAR
        PUSH DS
        XOR AX,AX
        PUSH AX
        MOV AX, DSEG 
        MOV DS, AX

        GIRDI SAYILAR, N
        ; Stack'e fonksiyon içinde kullanılacak değerleri pushla
        LEA BX, SAYILAR
        PUSH BX
        MOV CX, N
        PUSH CX

	CALL findMod
        
        
        MOV AX, OFFSET MOST_FREQ_MSG
        CALL PUT_STR
        MOV AX, [modVal] 
        CALL PUTN ; mod değerini yazdır

        MOV AX, 4C00h
        INT 21h
        RETF 
ANA 	ENDP

findMod PROC NEAR
    ; SAYILAR dizisindeki en çok tekrar eden değeri bul
    PUSH BX
    PUSH CX
    PUSH DI
    PUSH BP
    PUSH DX

    MOV BX, 0           
    MOV BP, SP     
    MOV SI, [BP+14] ; DIZI ve CX değerini stackten alma, direkt poplayamıyoruz çünkü fonksiyonunu geri dönüş offsetini kaybetmemeliyiz
    MOV CX, [BP+12]
    
    
    MOV [maxCount], 0       ; En yüksek tekrar sayısını sıfırla

outer_loop:                  ;
    MOV AX, [SI]      ; Dizinin SI indisindeki değeri al
    MOV DI, SI                ; İç döngü için dizinin başına index
    ADD DI, 2
    MOV BL, CL               ; BL: iç döngüde kaç kere dönülecek
    DEC BL;
    MOV BH, 0                ; count sayısını sıfırla

inner_loop:
    MOV DX, [DI]      ; Dizinin DI indisindeki değeri al
    CMP AX, DX               ; AX ile DX değerini karşılaştır
    JNE not_equal            ; Eşit değilse not_equal’e atla
    INC BH                  ; Eşitse sayacı artır

not_equal:
    ADD DI, 2                ; Bir sonraki elemana geç (word olduğu için 2 artırıyoruz)
    DEC BL
    JNZ inner_loop          ; İç döngüyü tamamla

    ; Tekrar sayısını en yüksek count'la karşılaştır
    MOV DL, [maxCount]      ; En yüksek tekrar sayısını DL'e yükle, maxCount değeri byte'da depolanır bu sayede bh ve bl'yi değişken olarak kullandım
    CMP BH, DL               ; Mevcut tekrar sayısını karşılaştır
    JBE skip_update          ; Eğer BH(count) <= maxCount ise, güncelleme yapma
    MOV [maxCount], BH      ; Yeni en yüksek count sayısını güncelle
    MOV [modVal], AX      ; mod'u güncelle

skip_update:
    ADD SI, 2                ; Bir sonraki elemana geç
    LOOP outer_loop           ; Eleman kalmışsa dış döngüye devam et

    POP BX
    POP CX
    POP DI
    POP BP
    POP DX

    RET 4
findMod ENDP

GETC	PROC NEAR
        ;------------------------------------------------------------------------
        ; Klavyeden basılan karakteri AL yazmacına alır ve ekranda gösterir. 
        ; işlem sonucunda sadece AL etkilenir. 
        ;------------------------------------------------------------------------
        MOV AH, 1h
        INT 21H
        RET 
GETC	ENDP 

PUTC	PROC NEAR
        ;------------------------------------------------------------------------
        ; AL yazmacındaki değeri ekranda gösterir. DL ve AH değişiyor. AX ve DX 
        ; yazmaçlarının değerleri korumak için PUSH/POP yapılır. 
        ;------------------------------------------------------------------------
        PUSH AX
        PUSH DX
        MOV DL, AL
        MOV AH,2
        INT 21H
        POP DX
        POP AX
        RET 
PUTC 	ENDP 

GETN 	PROC NEAR
        ;------------------------------------------------------------------------
        ; Klavyeden basılan sayiyi okur, sonucu AX yazmacı üzerinden dondurur. 
        ; DX: sayının işaretli olup/olmadığını belirler. 1 (+), -1 (-) demek 
        ; BL: hane bilgisini tutar 
        ; CX: okunan sayının islenmesi sırasındaki ara değeri tutar. 
        ; AL: klavyeden okunan karakteri tutar (ASCII)
        ; AX zaten dönüş değeri olarak değişmek durumundadır. Ancak diğer 
        ; yazmaçların önceki değerleri korunmalıdır. 
        ;------------------------------------------------------------------------
        PUSH BX
        PUSH CX
        PUSH DX
GETN_START:
        MOV DX, 1	                        ; sayının şimdilik + olduğunu varsayalım 
        XOR BX, BX 	                        ; okuma yapmadı Hane 0 olur. 
        XOR CX,CX	                        ; ara toplam değeri de 0’dır. 
NEW:
        CALL GETC	                        ; klavyeden ilk değeri AL’ye oku. 
        CMP AL,CR 
        JE FIN_READ	                        ; Enter tuşuna basilmiş ise okuma biter
        CMP  AL, '-'	                        ; AL ,'-' mi geldi ? 
        JNE  CTRL_NUM	                        ; gelen 0-9 arasında bir sayı mı?
NEGATIVE:
        MOV DX, -1	                        ; - basıldı ise sayı negatif, DX=-1 olur
        JMP NEW		                        ; yeni haneyi al
CTRL_NUM:
        CMP AL, '0'	                        ; sayının 0-9 arasında olduğunu kontrol et.
        JB error 
        CMP AL, '9'
        JA error		                ; değil ise HATA mesajı verilecek
        SUB AL,'0'	                        ; rakam alındı, haneyi toplama dâhil et 
        MOV BL, AL	                        ; BL’ye okunan haneyi koy 
        MOV AX, 10 	                        ; Haneyi eklerken *10 yapılacak 
        PUSH DX		                        ; MUL komutu DX’i bozar işaret için saklanmalı
        MUL CX		                        ; DX:AX = AX * CX
        POP DX		                        ; işareti geri al 
        MOV CX, AX	                        ; CX deki ara değer *10 yapıldı 
        ADD CX, BX 	                        ; okunan haneyi ara değere ekle 
        JMP NEW 		                ; klavyeden yeni basılan değeri al 
ERROR:
        MOV AX, OFFSET HATA 
        CALL PUT_STR	                        ; HATA mesajını göster 
        JMP GETN_START                          ; o ana kadar okunanları unut yeniden sayı almaya başla 
FIN_READ:
        MOV AX, CX	                        ; sonuç AX üzerinden dönecek 
        CMP DX, 1	                        ; İşarete göre sayıyı ayarlamak lazım 
        JE FIN_GETN
        NEG AX		                        ; AX = -AX
FIN_GETN:
        POP DX
        POP CX
        POP DX
        RET 
GETN 	ENDP 

PUTN 	PROC NEAR
        ;------------------------------------------------------------------------
        ; AX de bulunan sayiyi onluk tabanda hane hane yazdırır. 
        ; CX: haneleri 10’a bölerek bulacağız, CX=10 olacak
        ; DX: 32 bölmede işleme dâhil olacak. Soncu etkilemesin diye 0 olmalı 
        ;------------------------------------------------------------------------
        PUSH CX
        PUSH DX 	
        XOR DX,	DX 	                        ; DX 32 bit bölmede soncu etkilemesin diye 0 olmalı 
        PUSH DX		                        ; haneleri ASCII karakter olarak yığında saklayacağız.
                                                ; Kaç haneyi alacağımızı bilmediğimiz için yığına 0 
                                                ; değeri koyup onu alana kadar devam edelim.
        MOV CX, 10	                        ; CX = 10
        CMP AX, 0
        JGE CALC_DIGITS	
        NEG AX 		                        ; sayı negatif ise AX pozitif yapılır. 
        PUSH AX		                        ; AX sakla 
        MOV AL, '-'	                        ; işareti ekrana yazdır. 
        CALL PUTC
        POP AX		                        ; AX’i geri al 
        
CALC_DIGITS:
        DIV CX  		                ; DX:AX = AX/CX  AX = bölüm DX = kalan 
        ADD DX, '0'	                        ; kalan değerini ASCII olarak bul 
        PUSH DX		                        ; yığına sakla 
        XOR DX,DX	                        ; DX = 0
        CMP AX, 0	                        ; bölen 0 kaldı ise sayının işlenmesi bitti demek
        JNE CALC_DIGITS	                        ; işlemi tekrarla 
        
DISP_LOOP:
                                                ; yazılacak tüm haneler yığında. En anlamlı hane üstte 
                                                ; en az anlamlı hane en alta ve onu altında da 
                                                ; sona vardığımızı anlamak için konan 0 değeri var. 
        POP AX		                        ; sırayla değerleri yığından alalım
        CMP AX, 0 	                        ; AX=0 olursa sona geldik demek 
        JE END_DISP_LOOP 
        CALL PUTC 	                        ; AL deki ASCII değeri yaz
        JMP DISP_LOOP                           ; işleme devam
        
END_DISP_LOOP:
        POP DX 
        POP CX
        RET
PUTN 	ENDP 

PUT_STR	PROC NEAR
        ;------------------------------------------------------------------------
        ; AX de adresi verilen sonunda 0 olan dizgeyi karakter karakter yazdırır.
        ; BX dizgeye indis olarak kullanılır. Önceki değeri saklanmalıdır. 
        ;------------------------------------------------------------------------
	PUSH BX 
        MOV BX,	AX			        ; Adresi BX’e al 
        MOV AL, BYTE PTR [BX]	                ; AL’de ilk karakter var 
PUT_LOOP:   
        CMP AL,0		
        JE  PUT_FIN 			        ; 0 geldi ise dizge sona erdi demek
        CALL PUTC 			        ; AL’deki karakteri ekrana yazar
        INC BX 				        ; bir sonraki karaktere geç
        MOV AL, BYTE PTR [BX]
        JMP PUT_LOOP			        ; yazdırmaya devam 
PUT_FIN:
	POP BX
	RET 
PUT_STR	ENDP

CSEG 	ENDS 
	END ANA
