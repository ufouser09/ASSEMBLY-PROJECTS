; -----------------------------------------------------------------------
; Program: Maksimum 10 elemanlı bir tamsayı dizisinin en çok tekrar eden (mod) değerini hesaplar.
; ANA_PROGRAM      : Ana program
; SAYI_OKU         : Klavyeden girilen sayıyı AX yazmacına kaydeder.
; MOD_HESAPLA      : Dizinin mod değerini hesaplar.
; DIZI_YAZ         : Dizideki sayıları ekrana yazar.
; YAZI_YAZ         : Bir metni ekrana yazar.
; KARAKTER_YAZ     : Tek bir karakteri ekrana yazar.
; KARAKTER_OKU     : Klavyeden tek bir karakter okur.
; SAYI_YAZ         : Bir sayıyı ekrana yazar.
; -----------------------------------------------------------------------

PAGE 60,80
TITLE Dizi Mod Hesaplama

; Segment tanımlamaları
YIGIN_SEGMENTI   SEGMENT PARA STACK 'STACK'
        DW 32 DUP (?)
YIGIN_SEGMENTI   ENDS

VERI_SEGMENTI    SEGMENT PARA 'DATA'
CARIAGE_RETURN   EQU 13
LINE_FEED        EQU 10

; Veri tanımlamaları
DIZI_SAYILAR     DW 10 DUP(?)      ; Kullanıcıdan alınacak sayılar
ELEMAN_SAYISI    DW ?              ; Eleman sayısı
MOD_DEGERI       DW ?              ; Mod değeri
MAKS_FREKANS     DW ?              ; Maksimum frekans
GECICI_FREKANS   DW ?              ; Frekans sayacı

; Mesajlar
MESAJ_ELEMAN_SAY  DB 'Kaç eleman gireceksiniz (1-10): ', 0
MESAJ_SAYI_GIRIS  DB 'Bir sayi girin: ', 0
MESAJ_MOD_SONUC   DB CARIAGE_RETURN, LINE_FEED, 'Dizinin mod degeri: ', 0
MESAJ_DIZI_YAZ    DB CARIAGE_RETURN, LINE_FEED, 'Girilen sayilar: ', 0
MESAJ_HATA        DB CARIAGE_RETURN, LINE_FEED, 'Geçersiz giriş, lütfen tekrar deneyin.', 0

VERI_SEGMENTI    ENDS

KOD_SEGMENTI     SEGMENT PARA 'CODE'
        ASSUME CS:KOD_SEGMENTI, DS:VERI_SEGMENTI, SS:YIGIN_SEGMENTI

ANA_PROGRAM   PROC FAR
        PUSH DS
        XOR AX, AX
        PUSH AX
        MOV AX, VERI_SEGMENTI
        MOV DS, AX

    ; Kullanıcıdan eleman sayısını al
ELEMAN_SAYISI_GIRIS:                    ;Bu etiket, kullanıcının eleman sayısını girmesinin istendiği bölümün başlangıcını işaretler.
        MOV AX, OFFSET MESAJ_ELEMAN_SAY ;AX kaydına eleman sayısını soran mesajın adresini yükler.
        CALL YAZI_YAZ                   ;AX'deki adreste saklanan mesajı görüntülemek için bir prosedürü (muhtemelen) çağırır.
        CALL SAYI_OKU                   ;Kullanıcıdan bir tamsayı girdisini okumak için bir prosedürü çağırır. Giriş değeri AX'te döndürülür.
        MOV ELEMAN_SAYISI, AX           ;Girilen eleman sayısını değişkene kaydeder ELEMAN_SAYISI.

        ; Eleman sayısını kontrol et
        MOV AX, ELEMAN_SAYISI           ;Eleman sayısını kontrol için AX'e taşır.
        CMP AX, 1                       ;Eleman sayısını 1 ile karşılaştırır.
        JL HATA_GIRIS                   ;Öğe sayısı 1'den az ise, en üste atlar.HATA_GIRIShatayı işlemek için etiket.
        CMP AX, 10                      ;Eleman sayısını 10 ile karşılaştırır.
        JG HATA_GIRIS                   ;Eleman sayısı 10'dan fazla ise HATA_GIRIS hata ayıklama içine atlar.

        ; Elemanları al
        MOV CX, AX              ;Döngü için sayaç olarak kullanılacak olan CX kaydına eleman sayısını yükler.
        LEA SI, DIZI_SAYILAR    ;Dizinin etkin adresini DIZI_SAYILARSI kaydına yükler.

DIZI_GIRIS_DONGUSU:             ;Bu etiket, dizinin elemanlarını girmek için döngünün başlangıcını işaretler.
        MOV AX, OFFSET MESAJ_SAYI_GIRIS ;Her numara için gelen mesajın adresini AX'e yükler.
        CALL YAZI_YAZ                   ;Mesajı görüntüleyecek prosedürü çağırır.
        CALL SAYI_OKU                   ;Kullanıcıdan bir tamsayı girdisini okumak için prosedürü çağırır.
        MOV [SI], AX                    ;Girilen sayıyı SI ile işaret edilen adresteki diziye depolar.
        ADD SI, 2                       ;Dizi içindeki bir sonraki öğeye işaret etmek için SI kaydını 2 artırır
                                        ;(16 bitlik tam sayılar varsayılarak).
        LOOP DIZI_GIRIS_DONGUSU         ;DIZI_GIRIS_DONGUSU ​​: CX'i azaltır ve CX sıfır değilse geri döner ,
                                        ;böylece tüm elemanlar girilene kadar döngü tekrarlanır.

        ; Dizi adresini tekrar yükle
        LEA SI, DIZI_SAYILAR            ;Dizinin adresini daha ileri işleme için SI'ye yeniden yükler.

        ; Girilen diziyi ekrana yazdır
        MOV AX, OFFSET MESAJ_DIZI_YAZ       ;Dizinin yazdırılacağını belirten mesajın adresini yükler.
        CALL YAZI_YAZ                       ; Bu mesajın görüntülenmesini sağlayacak prosedürü çağırır.
        MOV AX, ELEMAN_SAYISI               ; Eleman sayısını AX'e taşır.
        PUSH AX                             ; Bir sonraki prosedür için parametre olarak yığına eleman sayısını iter.
        LEA AX, DIZI_SAYILAR                ;Dizinin adresini AX'e yükler.
        PUSH AX                             ; Dizinin adresini yığına koy
        CALL DIZI_YAZ                       ;Diziyi yazdırmak için prosedürü çağırır.
        ADD SP, 4                           ;Daha önce itilen iki parametreyi (toplam 4 bayt) kaldırarak yığını temizler.

        ; Mod hesapla
        LEA AX, DIZI_SAYILAR                ;Dizinin adresini AX'e yükler.
        PUSH AX                             ; Dizinin adresini yığına koy
        MOV AX, ELEMAN_SAYISI               ;Eleman sayısını AX'e taşır.
        PUSH AX                             ;Eleman sayısını yığına koy
        CALL MOD_HESAPLA                    ;Dizinin modunu hesaplama prosedürünü çağırır.

        ; Mod değerini yazdır
        MOV AX, OFFSET MESAJ_MOD_SONUC      ; Mod sonucunun yazdırılacağını belirten mesajın adresini yükler.
        CALL YAZI_YAZ                       ;Bu mesajın görüntülenmesini sağlayacak prosedürü çağırır.
        MOV AX, MOD_DEGERI                  ;Hesaplanan mod değerini AX'e taşır.
        CALL SAYI_YAZ                       ;Mod değerini yazdırmak için prosedürü çağırır.

        ; Program sonu
        MOV AH, 4Ch
        INT 21h

HATA_GIRIS:                                 ;Bu etiket hata işleme bölümünü işaretler.
        MOV AX, OFFSET MESAJ_HATA           ;Hata mesajının adresini AX'e yükler.
        CALL YAZI_YAZ                       ;Hata mesajını görüntüleyecek prosedürü çağırır.
        JMP ELEMAN_SAYISI_GIRIS             ;Kullanıcıya tekrar soru sormak için giriş bölümünün başına geri döner.

ANA_PROGRAM   ENDP              ;Ana program prosedürünün sonunu işaretler.

; -----------------------------------------------------------------
; Prosedür: DIZI_YAZ
; Açıklama: Verilen tamsayı dizisini ekrana yazar.
; Giriş: Yığında - [BP+4]: Dizinin adresi, [BP+6]: Eleman sayısı
; -----------------------------------------------------------------
DIZI_YAZ PROC NEAR              ;Bu satır, adlı bir prosedürün başlangıcını tanımlar DIZI_YAZ.
                                ;NEAR Anahtar sözcük, bu prosedürün aynı segment içerisinden çağrılabileceğini belirtir.
    PUSH BP                     ;Mevcut temel işaretçiyi (BP) yığına kaydeder. Bu, önceki kare işaretçisini korumak için yapılır.
    MOV BP, SP                  ;Temel işaretçiyi (BP) geçerli yığın işaretçisine (SP) ayarlar ve bu prosedür için yeni bir yığın çerçevesi oluşturur.

    PUSH SI                     ;Bu satırlar, geçerli değerlerini kaydetmek için SI, CX ve AX kayıtlarını yığına iter.
                                ;Bu, prosedür bunları değiştirmeden önce bu kayıtların durumunu korumak için önemlidir.
    PUSH CX
    PUSH AX

    ; Parametreleri al
    MOV SI, [BP+4]      ; SI = Dizinin adresi;Dizinin adresini SI kaydına yükler.Bu adres, prosedüre ilk parametre olarak geçirilir.
    MOV CX, [BP+6]      ; CX = Eleman sayısı;Dizideki eleman sayısını CX kaydına yükler. Bu ikinci parametredir.

    ; Döngü ile elemanları yazdır
DIZI_YAZ_LOOP:                  ;Bu etiket, dizinin elemanlarını yazdırmak için döngünün başlangıcını işaretler.
    MOV AX, [SI]        ; AX = Dizideki sayı;Geçerli öğeyi diziden (SI ile işaret edilen) AX kaydına yükler.
    CALL SAYI_YAZ       ; Sayıyı ekrana yaz; AX'te kayıtlı sayıyı ekranda görüntülemek için bir prosedürü (muhtemelen) çağırır.
    
    ; Sayılar arasında boşluk ekleyelim
    MOV DL, ' '             ;DL kaydına bir boşluk karakteri yükler.
    MOV AH, 02h             ;Bir karakteri görüntülemek için DOS kesmesini çağırmaya hazırlanır.
    INT 21h                 ;DL'deki karakteri (boşluk olan) ekrana yazdırmak için DOS kesmesini çağırır ve yazdırılan sayılar arasında ayrım sağlar.

    ADD SI, 2           ; Bir sonraki elemana geç
    LOOP DIZI_YAZ_LOOP  ;DIZI_YAZ_LOOP : CX'i azaltır ve CX sıfır değilse geri döner,
                        ;böylece tüm elemanlar yazdırılana kadar döngü tekrarlanır.

    POP AX
    POP CX
    POP SI
    POP BP
    RET 4               ; Parametreleri yığından temizle
DIZI_YAZ ENDP           ;İşlemin sonunu işaretler DIZI_YAZ.


; -----------------------------------------------------------------
; Prosedür: MOD_HESAPLA
; Açıklama: Dizinin mod değerini hesaplar
; Giriş: Yığında - [BP+4]: Dizinin adresi, [BP+6]: Eleman sayısı
; Çıkış: MOD_DEGERI değişkenine hesaplanan mod değeri kaydedilir
; -----------------------------------------------------------------
MOD_HESAPLA PROC NEAR   ;Bu satır, adında bir prosedürün başlangıcını tanımlar.
                        ;MOD_HESAPLA.NEARanahtar sözcüğü bu prosedürün aynı segment içerisinden çağrılabileceğini belirtir.
    PUSH BP             ;Mevcut temel işaretçiyi (BP) yığına kaydeder. 
                        ;Bu, önceki kare işaretçisini korumak için yapılır.
    MOV BP, SP          ;Temel işaretçiyi (BP) geçerli yığın işaretçisine (SP) ayarlar ve 
                        ;bu prosedür için yeni bir yığın çerçevesi oluşturur.

    PUSH SI             ;Bu satırlar, SI, DI, CX, DX ve AX kayıtlarını geçerli değerlerini kaydetmek için yığına iter. 
                        ;Bu, prosedür bunları değiştirmeden önce bu kayıtların durumunu korumak için önemlidir.
    PUSH DI
    PUSH CX
    PUSH DX
    PUSH AX

    ; Parametreleri al
    MOV SI, [BP+4]      ; SI = DIZI_SAYILAR dizisinin adresi;Dizinin adresini yükler 
                        ;DIZI_SAYILAR(sayı dizisi) SI kaydına. Adres, prosedüre ilk parametre olarak geçirilir.
    MOV CX, [BP+6]      ; Dizideki eleman sayısını CX kaydına yükler. Bu ikinci parametredir.

    CMP CX, 0           ; Eleman sayısını (CX) 0 ile karşılaştırır.
    JE MOD_SIFIRLA      ;Eğer CX 0'a eşitse (yani hiçbir eleman yoksa), MOD_SIFIRLAbu durumu ele almak için etikete atlar.
    jmp ssxx

    MOD_SIFIRLA:        ;Bu etiket, hiçbir öğe olmadığında durumu işleyen kod bölümünü işaretler.
    MOV MOD_DEGERI,0    

    ssxx:
    ; Değişkenleri sıfırla
    MOV MAKS_FREKANS, 0     ; Maksimum frekansı 0'a ayarla
    MOV MOD_DEGERI, 0       ;;Mod değerinin de 0 olarak başlatılmasını sağlar.

    XOR BX, BX      ; BX = 0, dış döngü sayacı (i)

MOD_DIS_DONGUSU:    ;Bu etiket dış döngünün başlangıcını işaretler.
    CMP BX, CX      ;Dış döngü sayacını (BX) eleman sayısıyla (CX) karşılaştırır.
    JG MOD_BITIR    ;Eğer BX, CX'ten büyükse, MOD_BITIRtüm elemanların işlendiğini belirten 'e atlar.

    ; Mevcut sayıyı al
    MOV AX, BX              ; AX = BX (dış döngü sayacı)
    push bx                 ; BX'i değerini korumak için yığına iter.
    ADD BX,BX               ;Diziye erişim için bayt ofsetini almak amacıyla BX değerini iki katına çıkarır (BX = BX * 2).
    MOV DX, [SI + bX]       ; DX = DIZI_SAYILAR[BX];Hesaplanan ofseti kullanarak dizideki geçerli sayıyı DX'e yükler.
    pop bx                  ;Yığından BX'in orijinal değerini geri yükler.
    ADD AX,AX               ; AX = BX * 2;AX'i iki katına çıkarır (AX = BX * 2), bu daha sonraki hesaplamalarda kullanılır.
    

    ; Frekans sayacını sıfırla
    MOV GECICI_FREKANS, 0       ;Geçici bir frekans sayacını ( GECICI_FREKANS) 0'a başlatır.

    ; İç döngü
    XOR DI, DI      ; DI = 0, iç döngü sayacı (j)

MOD_IC_DONGUSU:
    CMP DI, CX      ;İç döngü sayacını (DI) eleman sayısıyla (CX) karşılaştırır.
    JG FREKANS_KONTROL  ; Eğer DI, CX'ten büyükse FREKANS_KONTROLfrekansı kontrol etmeye geçer.

    ; Elemanı al
    MOV AX, DI              ; AX = DI (iç döngü sayacı)
    ADD AX,AX               ; AX = DI * 2
    MOV BX, AX              ; BX = DI * 2
    MOV AX, [SI + BX]       ; AX = DIZI_SAYILAR[DI]

    CMP AX, DX              ;Geçerli öğeyi (AX) sayılan sayıyla (DX) karşılaştırır.
    JNE MOD_ATLA            ;Eğer eşit değillerse, MOD_ATLAfrekansı artırma işlemini atlamaya geçer.

    ; Frekans sayacını artır
    INC GECICI_FREKANS      ; Bir eşleşme bulunduğundan beri geçici frekans sayacını artırır.

MOD_ATLA:                   ;Mevcut öğe eşleşmezse bu etikete ulaşılır.
    INC DI                  ;İç döngü sayacını (DI) artırır.
    JMP MOD_IC_DONGUSU      ;İç döngünün başlangıcına geri döner.

FREKANS_KONTROL:            ;Bu etiket frekansın kontrol edildiği noktayı işaretler.
    MOV AX, GECICI_FREKANS  ;Karşılaştırma için geçici frekansı AX'e taşır.
    CMP AX, MAKS_FREKANS    ;Mevcut frekansı maksimum frekansla karşılaştırır.
    JG MOD_GUNCELLE         ;Mevcut frekans daha büyükse, 'e atlar MOD_GUNCELLE.
    ; Eşitse ilk bulunan değeri koruyoruz
    JMP MOD_SIRADAKI        ;Eğer mevcut frekans daha büyük değilse, değerine atlar MOD_SIRADAKI, 
                            ;yani ilk bulunan mod değerini korur.

MOD_GUNCELLE:               ;Yeni bir maksimum frekans bulunduğunda bu etikete ulaşılır.
    MOV MAKS_FREKANS, AX    ;Maksimum frekansı geçerli frekansla günceller.
    MOV MOD_DEGERI, DX      ;Sayılmakta olan geçerli sayı ile mod değerini günceller.


MOD_SIRADAKI:
    INC BX                  ; Dış döngü sayacını artır
    JMP MOD_DIS_DONGUSU     ;Dış döngünün başlangıcına geri döner.

MOD_BITIR:                  ;Bu etiket dış döngünün sonunu işaretler.
    JMP MOD_CIKIS



MOD_CIKIS:
    POP AX
    POP DX
    POP CX
    POP DI
    POP SI
    POP BP
    RET 4           ; Parametreleri yığından temizle
MOD_HESAPLA ENDP
; -----------------------------------------------------------------
; Prosedür: SAYI_OKU
; Açıklama: Kullanıcıdan sayı girişi alır ve AX yazmacına kaydeder
; -----------------------------------------------------------------
SAYI_OKU    PROC NEAR
        PUSH SI            ; SI yazmacını koru
        PUSH DI            ; DI yazmacını yığına kaydet
        PUSH BX
        PUSH CX
        PUSH DX

        MOV DI, 1          ; DI yazmacı sayının işaretini tutar (+1 varsayıyoruz)
        XOR CX, CX         ; CX yazmacı toplamı tutar (toplam = 0)

SAYI_OKU_OKU:
        CALL KARAKTER_OKU
        CMP AL, CARIAGE_RETURN    ; Enter tuşu mu?
        JE SAYI_OKU_SONU          ; Evetse, okuma biter
        CMP AL, '-'
        JNE SAYI_OKU_KONTROL      ; Değilse, rakam kontrolüne geç
        MOV DI, -1                ; Sayının işaretini -1 yap
        JMP SAYI_OKU_OKU

SAYI_OKU_KONTROL:
        CMP AL, '0'
        JB SAYI_OKU_HATA
        CMP AL, '9'
        JA SAYI_OKU_HATA
        SUB AL, '0'               ; ASCII'den rakama çevir
        MOV AH, 0                 ; AL'ı 16 bit yapmak için AH'ı sıfırla

        ; Toplamı hesapla
        MOV BX, CX                ; BX = mevcut toplam
        MOV CX, AX                ; CX = yeni rakam
        MOV AX, BX                ; AX = mevcut toplam
        MOV BX, 10
        MUL BX                    ; AX = AX * 10
        ADD AX, CX                ; AX = (mevcut toplam * 10) + yeni rakam
        MOV CX, AX                ; CX = yeni toplam

        JMP SAYI_OKU_OKU

SAYI_OKU_HATA:
        MOV AX, OFFSET MESAJ_HATA
        CALL YAZI_YAZ
        JMP SAYI_OKU_TEKRAR

SAYI_OKU_SONU:
        MOV AX, CX                ; AX = toplam
        CMP DI, 1                 ; Sayının işareti +1 mi?
        JE SAYI_OKU_ISARET_OK
        NEG AX                    ; Değilse, AX = -AX

SAYI_OKU_ISARET_OK:
        POP DX
        POP CX
        POP BX
        POP DI                    ; DI yazmacını geri yükle
        POP SI                    ; SI yazmacını geri yükle
        RET

SAYI_OKU_TEKRAR:
        ; Toplam ve işareti sıfırla
        MOV DI, 1                 ; DI yazmacı sayının işaretini tutar (+1 varsayıyoruz)
        XOR CX, CX                ; CX yazmacı toplamı tutar (toplam = 0)
        JMP SAYI_OKU_OKU

SAYI_OKU    ENDP


; -----------------------------------------------------------------
; Prosedür: YAZI_YAZ
; Açıklama: AX'de adresi verilen sonu 0 ile biten metni ekrana yazar
; -----------------------------------------------------------------
YAZI_YAZ PROC NEAR
    PUSH AX
    PUSH BX
    MOV BX, AX
YAZI_YAZ_LOOP:
    MOV AL, [BX]
    CMP AL, 0
    JE YAZI_YAZ_BITIR
    CALL KARAKTER_YAZ
    INC BX
    JMP YAZI_YAZ_LOOP
YAZI_YAZ_BITIR:
    POP BX
    POP AX
    RET
YAZI_YAZ ENDP

; -----------------------------------------------------------------
; Prosedür: KARAKTER_YAZ
; Açıklama: AL yazmacındaki karakteri ekrana yazar
; -----------------------------------------------------------------
KARAKTER_YAZ PROC NEAR
    PUSH DX
    MOV DL, AL
    MOV AH, 02h
    INT 21h
    POP DX
    RET
KARAKTER_YAZ ENDP

; -----------------------------------------------------------------
; Prosedür: KARAKTER_OKU
; Açıklama: Klavyeden bir karakter okur ve AL yazmacına kaydeder
; -----------------------------------------------------------------
KARAKTER_OKU PROC NEAR
    MOV AH, 01h
    INT 21h
    RET
KARAKTER_OKU ENDP

; -----------------------------------------------------------------
; Prosedür: SAYI_YAZ
; Açıklama: AX yazmacındaki sayıyı ekrana yazar
; -----------------------------------------------------------------
SAYI_YAZ PROC NEAR
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI

    MOV SI, AX          ; SI = Yazılacak sayı
    XOR CX, CX          ; Hane sayacı

    CMP SI, 0
    JGE SAYI_YAZ_POZITIF
    NEG SI
    MOV DL, '-'
    MOV AH, 02h
    INT 21h
SAYI_YAZ_POZITIF:
    MOV BX, 10

SAYI_YAZ_LOOP:
    XOR DX, DX
    MOV AX, SI
    DIV BX
    PUSH DX             ; Kalanı yığına koy
    INC CX              ; Hane sayacını artır
    MOV SI, AX          ; Bölüm yeni sayı olur
    CMP SI, 0
    JNE SAYI_YAZ_LOOP

SAYI_YAZ_YAZDIR:
    POP DX
    ADD DL, '0'
    MOV AH, 02h
    INT 21h
    LOOP SAYI_YAZ_YAZDIR

    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
SAYI_YAZ ENDP

KOD_SEGMENTI     ENDS
        END ANA_PROGRAM