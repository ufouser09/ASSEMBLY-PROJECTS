PAGE 60,80
TITLE OBP HESAPLAMA 
SSEG SEGMENT PARA STACK 'STACK'
    DW 12 DUP(?)
SSEG ENDS    

DSEG SEGMENT PARA 'DATA'  
VIZE DB 77,85,64,96 
FINAL DB 56,63,86,74 
SONUC DB 4 DUP(?)
DSEG ENDS   

CSEG SEGMENT PARA 'CODE'
TC PROC FAR   
    ASSUME CS:CSEG,SS:SSEG,DS:DSEG 
;-------------------------------------------------------
;DÖNÜŞ İÇİN GEREKLİ OLAN DEĞERLER YIĞINDA SAKLANIYOR.
;-------------------------------------------------------
PUSH DS 
XOR AX,AX  
PUSH AX     
;--------------------------------------------------------------
;DSEG İSMİYLE TANIMLI KESİM ALANINA ERİŞEBİLMEK İÇİN GEREKLİ TANIMLAR
;--------------------------------------------------------------
MOV AX,DSEG
MOV DS,AX 
;---------------------------------------------------------------
;KONTROL İŞLEMİNİN GERÇEKLEŞTİRİLDİĞİ KOD BLOĞU 
;---------------------------------------------------------------
        MOV CX,4
        MOV SI,0
        MOV DI,0

CEVRIM: MOV AL,VIZE[SI]  ;DİZİ'Yİ SIRA SIRA AL'NİN İÇİNE ATIYORUZ
        MOV AH,0         
        MOV BL,4 
        MOV BH,0       
        MUL BL          ;AL*BL(2)--->AX
        MOV DX,AX
        
        

        MOV AL,FINAL[SI]
        MOV AH,0
        MOV BL,6
        MOV BH,0
        MUL BL          ;AL*BL(3)----->AX
        ADD AX,DX

        ADD AX,5
        MOV BX,10
        MOV DX,0
        DIV BX          ;DX:AX/BX---->AX---->DX
        
       

        MOV SONUC[DI],AL

        INC SI    
        INC DI
        LOOP CEVRIM
    ;---------------------------------------------------
    ;SIRALAMA
    ;---------------------------------------------------
    MOV CX, 4                       
    DEC CX                      

SIRALAMA:
            MOV SI, 0                 

        ICDONGU:
            MOV AL, SONUC[SI]           
            MOV BL, SONUC[SI+1]         
            CMP AL, BL                
            JAE DEGISTIRME

            ; SWAP İŞLEMİ
            MOV SONUC[SI], BL            
            MOV SONUC[SI+1], AL          

        DEGISTIRME:
            INC SI                    

            LOOP ICDONGU            

            DEC CX  

            JNZ SIRALAMA


            MOV SI,0
            MOV CX,4
        CEVRIM2:MOV AL, SONUC[SI] 
                INC SI
                LOOP CEVRIM2
        RET   
    TC  ENDP  
    CSEG ENDS    
    END TC


