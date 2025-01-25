#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>

#define pixel_max(a) ((a) <= 255 ? (a) : 255)
#define pixel_min(a) ((a) >= 0 ? (a) : 0)

// Function to read an image in grayscale
unsigned char* readImage(const char* filename, int& width, int& height, int& channels) {
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 1); // Load as grayscale
    if (!image) {
        std::cerr << "Failed to load image: " << stbi_failure_reason() << std::endl;
        return nullptr;
    }
    std::cout << "Image loaded successfully!" << std::endl;
    std::cout << "Width: " << width << ", Height: " << height << ", Channels: " << channels << std::endl;
    return image;
}

// Function to write an image to a PNG file
bool writeImage(const char* filename, unsigned char* image, int width, int height) {
    if (!image) {
        std::cerr << "Image data is null before writing!" << std::endl;
        return false;
    }
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid image dimensions: width = " << width << ", height = " << height << std::endl;
        return false;
    }
    // For grayscale images, stride is the same as the width
    int stride = width;
    if (stbi_write_png(filename, width, height, 1, image, stride) == 0) {
        std::cerr << "Failed to write the image to file: " << filename << std::endl;
        return false;
    }
    std::cout << "Image written successfully to: " << filename << std::endl;
    return true;
}

int main() {
    // Input and output file paths
    const char* inputFilename = "input_image4.png";
    const char* outputFilename1 = "output_image1.png";
    const char* outputFilename2 = "output_image2.png";

    // Image data variables
    int width, height, channels; // channels = 1 (grayscale)
    unsigned int number_of_pixels;

    // Read the input image
    unsigned char* image = readImage(inputFilename, width, height, channels);
    if (!image)
        return -1; // Exit if the image failed to load

    // Allocate memory for the output image
    unsigned char* outputImage = new unsigned char[width * height];
    if (!outputImage) {
        std::cerr << "Failed to allocate memory for output image!" << std::endl;
        stbi_image_free(image);
        return -1;
    }

    // image is 1d array 
    // with length = width * height
    // pixels can be used as image[i] 
    // pixels can be updated as image[i] = 100, etc.
    // a pixel is defined as unsigned char
    // so a pixel can be 255 at max, and 0 at min.

    /* -------------------------------------------------------- QUESTION-1 -------------------------------------------------------- */

    /* Q-1 Inverse the colors of image.
    Inverse -> pixel_color = 255 - pixel_color */

    number_of_pixels = width * height;//GÖRÜNTÜNÜN TOPLAM PÝXEL SAYISINI HESAPLARIZ BURDA
    __asm {
        mov ecx, number_of_pixels// Piksel sayýsýný ECX'e yükle
        mov esi, image// Giriþ resminin adresini ESI'ye yükle
        mov edi, outputImage// Çýkýþ resminin adresini EDI'ye yükle

        loop_start :
        cmp ecx, 0// Tüm pikseller iþlendi mi ?
            je loop_end// Evetse döngüden çýk

            mov al, [esi]// AL = giriþ piksel deðeri
            mov bl, 255// BL = 255
            sub bl, al// BL = 255 - AL(invers piksel deðeri)
            mov[edi], bl// Çýkýþ resmine invers piksel deðerini yaz

            inc esi// Giriþ resminde sonraki piksele geç
            inc edi// Çýkýþ resminde sonraki piksele geç

            dec ecx// Piksel sayacýný azalt
            jmp loop_start// Döngüye geri dön

            loop_end :
    }

    // Write the modified image as output_image1.png
    if (!writeImage(outputFilename1, outputImage, width, height)) {
        stbi_image_free(image);
        return -1;
    }
    stbi_image_free(outputImage); // Clear the outputImage.

    /* -------------------------------------------------------- QUESTION-2 -------------------------------------------------------- */
    /* Histogram Equalization */

    outputImage = new unsigned char[width * height];
    if (!outputImage) {
        std::cerr << "Failed to allocate memory for output image!" << std::endl;
        stbi_image_free(image);
        return -1;
    }

    unsigned int* hist = (unsigned int*)malloc(sizeof(unsigned int) * 256);
    unsigned int* cdf = (unsigned int*)malloc(sizeof(unsigned int) * 256);

    // Check if memory allocation succeeded
    if (hist == NULL) {
        std::cerr << "Memory allocation for hist failed!" << std::endl;
        return -1;
    }
    if (cdf == NULL) {
        std::cerr << "Memory allocation for cdf failed!" << std::endl;
        free(hist);
        return -1;
    }

    // Both hist and cdf are initialized as zeros.
    for (int i = 0; i < 256; i++) {
        hist[i] = 0;
        cdf[i] = 0;
    }

    // You can define new variables here... As a hint some variables are already defined.
    unsigned int min_cdf, range;
    number_of_pixels = width * height;

    // Q-2 (a) - Compute the histogram of the input image.
    //BU KOD BLOGUNDA KAYNAK GÖRÜNTÜMÜZÜN HER PÝXEL DEÐERÝNÝN FREKANSINI HESAPLAYARAK HÝSTOGRAM OLUÞTURUYORUZ
    __asm {
        mov ecx, number_of_pixels       //ECX REGÝSTERINA TOPLAM PÝXEL SAYISI YÜKLENÝYOR
        mov esi, image                  //ESI REGÝSTERINA ÝÞLENECEK OLAN GÖRÜNTÜNÜN BAÞLANGIÇ ADRESÝ ATANIYOR
        mov edi, hist                   //EDI REGÝSTERINA HÝSTOGRAM DÝZÝSÝNÝN BAÞLANGIÇ ADRESÝ ATANIYOR 

        histogram_loop :
        cmp ecx, 0
            je histogram_end            //ECX 0 ÝSE DÖNGÜ SONU ETÝKETÝNE ATANIR

            mov al, [esi]               //ÝÞLENECEK OLAN GÖRÜNTÜ DÝZÝSÝNDEKÝ MEVCUT PÝXELÝ ALIR VE AL REGÝSTERINA ATAR
            xor ebx, ebx                //EBX REGÝSTERI 0'LANIR 
            mov bl, al                  //AL REGÝSTERINDAKÝ PÝXEL DEGER BL REGÝSTERINA ATANIR
            mov eax, [edi + ebx * 4]    //HÝSTOGRAM DÝZÝSÝNDEKÝ MEVCUT PÝXEL DEÐERÝNÝ OKUR 
            //EBX HÝSTOGRAM DÝZÝSÝNÝN ÝNDEKSÝNÝ TEMSÝL EDER
            //DEGER EAX REGÝSTIRINA YÜKLENÝR
            inc eax                     //EAX DEGERÝMÝZ BÝR ARTTIRILIR BU MEVCUT PÝXEL DEÐERÝNÝN GÖRÜNTÜDE BÝR KEZ DAHA GÖRÜLDÜÐÜNÜ BAHSEDER. 
            mov[edi + ebx * 4], eax     //GÜNCELLENEN HÝSTOGRAM DEÐERÝ YÝNE AYNI KONUMUNA YAZILIR

            //Histogram dizisi unsigned int veya int tipinde olduðu için her eleman 4 bayt yer kaplar.
            //Bu nedenle, piksel deðeri indeksini 4 ile çarparak doðru bellek adresine eriþiyoruz

            inc esi                     //KAYNAK GÖRÜNTÜ DÝZÝSÝNDEKÝ BÝR SONRAKÝ PÝXELE GEÇER
            dec ecx                     //KALAN PÝXEL SAYISINI GÜNCELLER
            jmp histogram_loop

            histogram_end :
    }

    /* Q-2 (b) - Compute the Cumulative Distribution Function cdf
                    and save it to cdf array which is defined above. */

                    // CDF Calculation (cdf[i] = cdf[i-1] + hist[i])

    //Histogram dizisi olan hist kullanýlarak CDF dizisi olan cdf hesaplanýr.
    //Her cdf[i] deðeri, hist[0] ile hist[i] arasýndaki tüm deðerlerin toplamýdýr.
    //Ýþlem, histogramdaki deðerleri birikimli olarak toplayarak yeni bir diziye kaydeder.

    __asm {
        mov esi, hist           //ESI REGÝSTERINA HÝST DÝZÝSÝNÝN BAÞLANGIÇ ADRESÝ YÜKLENÝR
        mov edi, cdf            //EDI REGÝSTERINA CDF DÝZÝSÝNÝN BAÞLANGIÇ ADRESÝ YÜKLENÝR
        xor eax, eax            //EAX REGÝSTERI SIFIRLANIR
        mov ecx, 256            //HÝSTOGRAMIN 256 FARKLI OLASI PÝKSEL DEGERÝ VARDIR(0-255)
        xor ebx, ebx            //EBX REGÝSTERI 0'LANIR 
        //EBX HÝSTOGRAM VE CDF DÝZÝLERÝNDE ÝNDEKS OLARAK KULLANILACAKTIR 

        cdf_loop :
        cmp ecx, 0              //DÖNGÜNÜN BÝTÝP BÝTMEDÝÐÝNÝ KONTROL EDER
            je cdf_end          //ECX REGÝSTERI 0'A EÞÝTSE DÖNGÜ SONUNA ATLANIR

            mov edx, [esi + ebx * 4]    //HÝSTOGRAM MATRÝSÝNDEKÝ MEVCUT PÝXEL EDX REGÝSTERINA YÜKLENÝR
            add eax, edx                //EDX'TEKÝ HÝSTOGRAM DEGERÝNÝ EAX'TEKÝ BÝRÝKÝMLÝ TOPLAMA EKLER
            //EAX REGÝSTERINDA SUANA KADARKÝ TOPLAMI TUTARIZ
            mov[edi + ebx * 4], eax     //BÝRÝKÝMLÝ TOPLAM DEGERÝNÝ CDF DÝZÝSÝNDEKÝ ÝLGÝLÝ ÝNDEXE ATARIZ

            inc ebx                     //BÝR SONRAKÝ ÝNDEXE GEÇMEK ÝÇÝN ÝNDEXÝMÝZÝ BÝR ARTTIRIRIZ
            dec ecx                     //DONGU SAYACINI GÜNCELLER
            jmp cdf_loop

            cdf_end :
    }


    /* Q-2 (c) - Normalize the Cumulative Distribution Funtion
                    that you have just calculated on the same cdf array. */

                    // Normalized cdf[i] = ((cdf[i] - min_cdf) * 255) / range
                    // range = (number_of_pixels - min_cdf)

    __asm {

        mov esi, cdf            // ESI REGISTERINA CDF DÝZÝSÝNÝN BAÞLANGIÇ ADRESÝNÝ YÜKLERÝZ
        mov ecx, 256            // OLASI TÜM PÝKSEL DEÐERLERÝNÝ ATARIZ
        xor ebx, ebx            // EBX REGISTERINI 0'LARIZ, EBX REGISTERINI ÝNDEX OLARAK KULLANACAÐIZ
        mov eax, -1             // EAX'Ý -1 ÝLE BAÞLATIRIZ (SIFIRDAN BÜYÜK BÝR MÝNÝMUM DEÐERÝ BULMAK ÝÇÝN)

        find_min_cdf:
        cmp ecx, 0              // ECX SIFIRA EÞÝT MÝ ONA BAKARIZ. EÐER SIFIRA EÞÝTSE TÜM DÝZÝYÝ TARAMIÞIZ DEMEKTÝR
            je min_cdf_found

            mov edx, [esi + ebx * 4] // CDF DÝZÝSÝNÝN EBX'ÝNCÝ ÝNDEXÝNDEKÝ DEÐERÝ EDX'E ATARIZ
            cmp edx, 0              // EDX'ÝN SIFIR OLUP OLMADIÐI KONTROL EDÝLÝR
            jle skip_value          // EÐER SIFIR VEYA NEGATÝFSE BU DEÐER ATLANIR

            cmp eax, -1             // MEVCUT BÝR MÝNÝMUM VAR MI KONTROL EDÝLÝR (-1 ÝSE YOKTUR)
            je update_min           // EÐER YOKSA GÜNCELLENÝR

            cmp edx, eax            // EÐER VARDIYSA, MEVCUT MÝNÝMUMDAN KÜÇÜK MÜ KONTROL EDÝLÝR
            jge skip_value          // KÜÇÜK DEÐÝLSE BU DEÐER ATLANIR

            update_min :
            mov eax, edx            // EÐER ÞARTLAR SAÐLANIYORSA, EDX MÝNÝMUM DEÐER OLARAK GÜNCELLENÝR

            skip_value :
            inc ebx                 // DÝZÝDEKÝ BÝR SONRAKÝ ÝNDEKSE ATLARIZ 
            dec ecx                 // KALAN ELEMAN SAYISINI GÜNCELLERÝZ
            jmp find_min_cdf        // DÖNGÜNÜN BAÞINA DÖNERÝZ

            min_cdf_found :
        mov min_cdf, eax        // BULUNAN EN KÜÇÜK DEÐERÝ MÝN_CDF'E KAYDEDERÝZ


        //BU AÞAMADA RANGE DEGERÝ HESAPLANIR
        mov eax, number_of_pixels       //EAX REGÝSTERINA TOPLAM PÝKSEL SAYISI YÜKLENÝR
            sub eax, min_cdf                //range = number_of_pixels - min_cdf

            mov range, eax                  //range deðerimizi kaydederiz.

            //BU AÞAMADA CDF DÝZÝSÝ NORMALÝZE EDÝLÝR
            mov esi, cdf        //ESI REGÝSTERINA CDF DÝZÝSÝNÝN BASLANGIC ADRESÝ YÜKLENÝR
            mov ecx, 256        //ECX REGÝSTERINA 256 DEGERÝ YUKLENÝR
            xor ebx, ebx        //ÝNDEX OLARAK KULLANILACAK EBX REGÝSTERI 0'LANIR 

            normalize_cdf_loop :
            cmp ecx, 0                  //ECX 0'A EÞÝTSE DONGUNUN SONUNA ATLANIR 
            je normalize_cdf_end

            mov edx, [esi + ebx * 4]    //CDF DÝZÝSÝNÝN MEVCUT ÝNDEXÝNDEKÝ DEGER EDX REGÝSTERINA ATANIR
            sub edx, min_cdf            //EDX'TEN MÝN_CDF ÇIKARTILIR CDF[Ý]-MÝN_CDF

            // 64 - bit çarpma için edx deðerini geniþletiyoruz
            mov eax, edx                //EAX REGÝSTERINA EDX REGÝSTERI ATANIR
            mov edx, 0                  //EDX:EAX REGÝSTERI ÇARPMA ÝÞLEMÝ ÝÇÝN HAZIRLANIR
            mov edi, 255
            mul edi               //EAX'Ý 255 ÝLE ÇARPARIZ SONUÇ 64 BÝTLÝK EDX:EAX ÇÝFTÝNDE TUTULUR 
            div range                   // EDX:EAX çiftini range'e bölüyoruz
            //EAX = ((cdf[i] - min_cdf) * 255) / range iþlemi gerçekleþtirilmiþ olur.
            cmp eax, 0                  //EAX'ýn sýfýrdan küçük olup olmadýðýný kontrol eder.

            jl zero_value
            cmp eax, 255                //EAX'ýn 255'ten büyük olup olmadýðýný kontrol eder.

            jg max_value
            jmp store_value             //Deðer uygun aralýktaysa, store_value etiketine atlar.


            zero_value :                //EAX sýfýrdan küçükse buraya atlanýr.

        mov eax, 0                      //EAX sýfýrlanýr.

            jmp store_value             //store_value etiketine atlanýr.


            max_value :                 //EAX 255'ten büyükse buraya atlanýr.

        mov eax, 255                    //EAX 255 olarak ayarlanýr.


            store_value :               //Normalleþtirilmiþ EAX deðerini cdf dizisine kaydederiz.
            mov[esi + ebx * 4], eax     //Normalleþtirilmiþ CDF deðeri dizideki yerine yazýlýr.



            inc ebx                     //Bir sonraki indekse geçeriz.

            dec ecx                     //Döngü sayacýný günceller.

            jmp normalize_cdf_loop      //Döngünün baþýna geri döner.


            normalize_cdf_end :         //Tüm CDF deðerleri normalleþtirildikten sonra buraya geliriz.

    }


    /* Q-2 (d) - Apply the histogram equalization on the image.
                    Write the new pixels to outputImage. */
                    //Girdi: Orijinal görüntü pikselleri (image dizisi) ve normalize edilmiþ Kümülatif Daðýlým Fonksiyonu (cdf dizisi).
                    //Çýktý: Histogram eþitleme uygulanmýþ yeni piksel deðerleri ile outputImage dizisi.

    __asm {
        mov ecx, number_of_pixels   //ECX register'ýna toplam piksel sayýsýný yükler.
        //Döngü sayacý olarak kullanýlacaktýr.

        mov esi, image              //ESI register'ýna image dizisinin baþlangýç adresini yükler.

        mov edi, outputImage        //EDI register'ýna outputImage dizisinin baþlangýç adresini yükler.

        mov ebx, cdf                //EBX register'ýna cdf dizisinin baþlangýç adresini yükler.


        equalization_loop :         //Histogram eþitleme iþlemini gerçekleþtirecek döngünün baþlangýç etiketi.

        cmp ecx, 0                  //Döngü sayacý sýfýra ulaþtýysa tüm pikseller iþlenmiþtir.

            je equalization_end     //ECX sýfýra eþitse, döngüyü sonlandýrmak için equalization_end etiketine atlar.


            mov al, [esi]           //ESI'nin iþaret ettiði bellek adresindeki piksel deðerini alýr ve AL register'ýna yükler.

            xor edx, edx            //EDX register'ýný sýfýrlar.

            mov dl, al              // EDX = piksel deðeri(indeks olarak kullanacaðýz)
            mov eax, [ebx + edx * 4]        //EBX'in (CDF dizisinin baþlangýcý) EDX ile adreslenmiþ konumundan (piksel deðeri indeks olarak kullanýlarak) deðeri okur.

            mov[edi], al                        //AL register'ýnýn içeriðini EDI'nin iþaret ettiði bellek adresine yazar.
            //outputImage dizisine yazma iþlemi gerçekleþtirir.


            inc esi                     //image dizisindeki bir sonraki piksele geçilir.

            inc edi                     //outputImage dizisindeki bir sonraki konuma geçilir.

            dec ecx                     //Kalan piksel sayýsýný günceller.

            jmp equalization_loop

            equalization_end :          //Tüm pikseller iþlendiðinde döngü buraya atlar ve iþlem tamamlanýr.

    }
    // Write the modified image
    if (!writeImage(outputFilename2, outputImage, width, height)) {
        stbi_image_free(image);
        return -1;
    }

    // Free the image memory
    stbi_image_free(image);
    stbi_image_free(outputImage);

    return 0;
}