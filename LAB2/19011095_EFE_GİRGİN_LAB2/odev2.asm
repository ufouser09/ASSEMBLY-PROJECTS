;PAGE 60,80
;TITLE TRIANGLE SOLUTION 
CSSSEG SEGMENT PARA 'CODE'
ORG 100H 
ASSUME DS:CSSSEG,CS:CSSSEG,SS:CSSSEG 

BASLA: 
    JMP ANA 
    
    a dw 1
    b dw 1
    HIPO dw ?
    primeOddSum DW 15 dup(0)        ; Koşulları sağlayan hipotenüsler için dizi
    nonPrimeOrEvenSum dW 15 dup(0)  ; Koşulları sağlamayanlar için dizi
    primeIndex dW 0                 ; primeOddSum dizisinin indeksi
    nonPrimeIndex dW 0              ; nonPrimeOrEvenSum dizisinin indeksi
    ;maxEntries dW 15                ; Dizi kapasitesi maksimum 15


ANA PROC NEAR

mov cx,1                            ; KENAR A İÇİN DÖNGÜ BAŞLAR
outer_loop:
 mov DI,1                           ; KENAR B İÇİN DÖNGÜ BAŞLAR
inner_loop:
    mov ax,cx                       ; AX = A

    
    mul ax                          ; AX*AX--->DX:AX---->A^2(DX BURADA 0 OLARAK ÇIKAR)
    

    mov bx,ax                       ; BX = A^2

    mov ax,DI                       ; AX = B

    
    mul ax                           ; AX*AX--->DX:AX---->A^2(DX BURADA 0 OLARAK ÇIKAR)
    
   
    add bx,ax                       ; BX = A^2 + B^2 (C^2 değeri)


;--------------------------------------------------------------
; HIPOTENUS DEGERİNİN TAM SAYI OLUP OLMADIĞI KONTROL EDİLİYOR
;---------------------------------------------------------------   
    mov SI,1   ;AX=1
    while_sqrt:
        
        mov ax,SI

        mul ax              ;AX*AX-->DX:AX--->AX^2(DX BURADAN 0 DEĞERİ OLUR)
        
        cmp ax,BX           ;BULDUGUMUZ HIPOTENUS DEGERI HERHANGİ BİR SAYININ KARESİ Mİ KONTROLU
        JA sqrt_done    

        inc SI
        jmp while_sqrt


;--------------------------------------------------------------------
;İNDİSİN KARESİYLE C^2 İFADESİ ÖRTÜŞÜRSE YA DA O DEGERİ AŞARSA
;--------------------------------------------------------------------
    sqrt_done:
    dec SI  ; AX=AX-1---->AX'İN GEÇERLİ DEĞERİNİ ELDE ETMİŞ OLDUK
    MOV AX,SI

    mul ax  ; AX * AX-->DX:AX--->AX^2(DX BURADAN 0 OLARAK ÇIKAR)
    
    cmp ax,BX ;HIPOTENUS DEGERİNİN KARESİYLE İNDİSİMİZİN KARESİNİN KARŞILAŞTIRILMASI 

    jne NEXT_B     ;ŞARTIMIZI SAĞLAMAZSA EĞER DÖNGÜDEN ÇIKILIR YANİ TAM SAYI OLMADIĞINI ANLARIZ.
    mov HIPO,SI            ; HIPO DEĞİŞKENİNDE TAM SAYI İFADEMİZİ TUTARIZ ARTIK.
    cmp HIPO,50            ; BU TAM SAYI İFADENİN 50 SINIRINI AŞIP AŞMADIĞI KONTROL EDİLİR.
    ja NEXT_B               ;EĞER HİPOTENÜS DEĞERİMİZ 50 Yİ AŞMIŞSA BUNU ES GEÇİP DÖNGÜYE DEVAM EDİCEZ

    
    mov ax,cx              ;AX=A KENARI(A DEGERİNİ YAZMACA ATAR)
    add ax,DI              ;A VE B KENARLARINI TOPLAR
    TEST ax,1               ;A VE B KENARLARININ TOPLAMININ TEK OLUP OLMADIĞINI KONTROL EDER
    jz store_non_prime      ;ŞARTIMIZI SAĞLAMAZSA EĞER DÖNGÜ KIRILIR YANİ A VE B KENARLARININ TOPLAMI TEK DEĞİLSE



;-------------------------------------------------------------
;HİPOTENUSUN ASAL OLUP OLMADIĞININ KONTROLU
;-------------------------------------------------------------
    mov AX,HIPO    ;SI YAZMACINDA HIPOTENUS DEGERI VAR
    mov SI,2       ;AX=2
    while_prime:
        mov ax,HIPO
        cmp SI,AX
        jge prime_found     ;EĞER BU KOŞULU SAĞLIYORSA ASAL BİR SAYI OLDUGUNU KANITLAR ŞARTIMIZ SAĞLANIR
        
        XOR DX,DX           ;BOLME ONCESİ DX:AX/SI OLDUGUNDAN DX DEGERİNİN SIFIRLANMASI 
        
        DIV SI              ;DX:AX/SI---->AX---->DX
        CMP dx, 0           ;KALAN DEGER KONTROLU
        
        je store_non_prime  ;KALAN DEĞER YOKSA DEMEK Kİ TAM BOLUNMUŞ O ZAMAN ASAL DEĞİLDİR ŞARTIMIZ SAĞLANMAZ DÖNGÜ KIRILIR
        
        inc SI             ;BOLEN DEGER ARTTIRILIR
        jmp while_prime

    prime_found:
    MOV SI,primeIndex
    cmp SI,15
    jae NEXT_B              ;EĞER BİRİNCİ DİZİMİZ DOLUYSA BU İFADEMİZİ ES GEÇİP DÖNGÜYE DEVAM EDİCEZ
    mov aX,HIPO
    mov primeOddSum[SI],aX
    ADD primeIndex,2
    
    

next_b:
    
    inc DI
    cmp DI,50
    jle inner_LOOP
    MOV DI,1
    inc cx
    cmp cx, 50
    jle OUTER_LOOP

    JMP PROGRAM_END


store_non_prime:
    
    MOV SI,nonPrimeIndex
    cmp SI,15
    jae NEXT_B      ;EĞER İKİNCİ DİZİMİZ DOLUYSA BU DİZİYİ ES GEÇİP DÖNGÜYE DEVAM EDİCEZ
    mov aX,HIPO
    mov nonPrimeOrEvenSum[SI],aX
    ADD nonPrimeIndex,2
    
    JMP NEXT_B



    
    
    PROGRAM_END:
    mov AH,4Ch
    int 21h

    RET 

ANA endp
CSSSEG ENDS
end BASLA
