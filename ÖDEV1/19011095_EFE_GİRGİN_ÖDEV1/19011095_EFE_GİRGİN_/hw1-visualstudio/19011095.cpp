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

    number_of_pixels = width * height;//G�R�NT�N�N TOPLAM P�XEL SAYISINI HESAPLARIZ BURDA
    __asm {
        mov ecx, number_of_pixels// Piksel say�s�n� ECX'e y�kle
        mov esi, image// Giri� resminin adresini ESI'ye y�kle
        mov edi, outputImage// ��k�� resminin adresini EDI'ye y�kle

        loop_start :
        cmp ecx, 0// T�m pikseller i�lendi mi ?
            je loop_end// Evetse d�ng�den ��k

            mov al, [esi]// AL = giri� piksel de�eri
            mov bl, 255// BL = 255
            sub bl, al// BL = 255 - AL(invers piksel de�eri)
            mov[edi], bl// ��k�� resmine invers piksel de�erini yaz

            inc esi// Giri� resminde sonraki piksele ge�
            inc edi// ��k�� resminde sonraki piksele ge�

            dec ecx// Piksel sayac�n� azalt
            jmp loop_start// D�ng�ye geri d�n

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
    //BU KOD BLOGUNDA KAYNAK G�R�NT�M�Z�N HER P�XEL DE�ER�N�N FREKANSINI HESAPLAYARAK H�STOGRAM OLU�TURUYORUZ
    __asm {
        mov ecx, number_of_pixels       //ECX REG�STERINA TOPLAM P�XEL SAYISI Y�KLEN�YOR
        mov esi, image                  //ESI REG�STERINA ��LENECEK OLAN G�R�NT�N�N BA�LANGI� ADRES� ATANIYOR
        mov edi, hist                   //EDI REG�STERINA H�STOGRAM D�Z�S�N�N BA�LANGI� ADRES� ATANIYOR 

        histogram_loop :
        cmp ecx, 0
            je histogram_end            //ECX 0 �SE D�NG� SONU ET�KET�NE ATANIR

            mov al, [esi]               //��LENECEK OLAN G�R�NT� D�Z�S�NDEK� MEVCUT P�XEL� ALIR VE AL REG�STERINA ATAR
            xor ebx, ebx                //EBX REG�STERI 0'LANIR 
            mov bl, al                  //AL REG�STERINDAK� P�XEL DEGER BL REG�STERINA ATANIR
            mov eax, [edi + ebx * 4]    //H�STOGRAM D�Z�S�NDEK� MEVCUT P�XEL DE�ER�N� OKUR 
            //EBX H�STOGRAM D�Z�S�N�N �NDEKS�N� TEMS�L EDER
            //DEGER EAX REG�STIRINA Y�KLEN�R
            inc eax                     //EAX DEGER�M�Z B�R ARTTIRILIR BU MEVCUT P�XEL DE�ER�N�N G�R�NT�DE B�R KEZ DAHA G�R�LD���N� BAHSEDER. 
            mov[edi + ebx * 4], eax     //G�NCELLENEN H�STOGRAM DE�ER� Y�NE AYNI KONUMUNA YAZILIR

            //Histogram dizisi unsigned int veya int tipinde oldu�u i�in her eleman 4 bayt yer kaplar.
            //Bu nedenle, piksel de�eri indeksini 4 ile �arparak do�ru bellek adresine eri�iyoruz

            inc esi                     //KAYNAK G�R�NT� D�Z�S�NDEK� B�R SONRAK� P�XELE GE�ER
            dec ecx                     //KALAN P�XEL SAYISINI G�NCELLER
            jmp histogram_loop

            histogram_end :
    }

    /* Q-2 (b) - Compute the Cumulative Distribution Function cdf
                    and save it to cdf array which is defined above. */

                    // CDF Calculation (cdf[i] = cdf[i-1] + hist[i])

    //Histogram dizisi olan hist kullan�larak CDF dizisi olan cdf hesaplan�r.
    //Her cdf[i] de�eri, hist[0] ile hist[i] aras�ndaki t�m de�erlerin toplam�d�r.
    //��lem, histogramdaki de�erleri birikimli olarak toplayarak yeni bir diziye kaydeder.

    __asm {
        mov esi, hist           //ESI REG�STERINA H�ST D�Z�S�N�N BA�LANGI� ADRES� Y�KLEN�R
        mov edi, cdf            //EDI REG�STERINA CDF D�Z�S�N�N BA�LANGI� ADRES� Y�KLEN�R
        xor eax, eax            //EAX REG�STERI SIFIRLANIR
        mov ecx, 256            //H�STOGRAMIN 256 FARKLI OLASI P�KSEL DEGER� VARDIR(0-255)
        xor ebx, ebx            //EBX REG�STERI 0'LANIR 
        //EBX H�STOGRAM VE CDF D�Z�LER�NDE �NDEKS OLARAK KULLANILACAKTIR 

        cdf_loop :
        cmp ecx, 0              //D�NG�N�N B�T�P B�TMED���N� KONTROL EDER
            je cdf_end          //ECX REG�STERI 0'A E��TSE D�NG� SONUNA ATLANIR

            mov edx, [esi + ebx * 4]    //H�STOGRAM MATR�S�NDEK� MEVCUT P�XEL EDX REG�STERINA Y�KLEN�R
            add eax, edx                //EDX'TEK� H�STOGRAM DEGER�N� EAX'TEK� B�R�K�ML� TOPLAMA EKLER
            //EAX REG�STERINDA SUANA KADARK� TOPLAMI TUTARIZ
            mov[edi + ebx * 4], eax     //B�R�K�ML� TOPLAM DEGER�N� CDF D�Z�S�NDEK� �LG�L� �NDEXE ATARIZ

            inc ebx                     //B�R SONRAK� �NDEXE GE�MEK ���N �NDEX�M�Z� B�R ARTTIRIRIZ
            dec ecx                     //DONGU SAYACINI G�NCELLER
            jmp cdf_loop

            cdf_end :
    }


    /* Q-2 (c) - Normalize the Cumulative Distribution Funtion
                    that you have just calculated on the same cdf array. */

                    // Normalized cdf[i] = ((cdf[i] - min_cdf) * 255) / range
                    // range = (number_of_pixels - min_cdf)

    __asm {

        mov esi, cdf            // ESI REGISTERINA CDF D�Z�S�N�N BA�LANGI� ADRES�N� Y�KLER�Z
        mov ecx, 256            // OLASI T�M P�KSEL DE�ERLER�N� ATARIZ
        xor ebx, ebx            // EBX REGISTERINI 0'LARIZ, EBX REGISTERINI �NDEX OLARAK KULLANACA�IZ
        mov eax, -1             // EAX'� -1 �LE BA�LATIRIZ (SIFIRDAN B�Y�K B�R M�N�MUM DE�ER� BULMAK ���N)

        find_min_cdf:
        cmp ecx, 0              // ECX SIFIRA E��T M� ONA BAKARIZ. E�ER SIFIRA E��TSE T�M D�Z�Y� TARAMI�IZ DEMEKT�R
            je min_cdf_found

            mov edx, [esi + ebx * 4] // CDF D�Z�S�N�N EBX'�NC� �NDEX�NDEK� DE�ER� EDX'E ATARIZ
            cmp edx, 0              // EDX'�N SIFIR OLUP OLMADI�I KONTROL ED�L�R
            jle skip_value          // E�ER SIFIR VEYA NEGAT�FSE BU DE�ER ATLANIR

            cmp eax, -1             // MEVCUT B�R M�N�MUM VAR MI KONTROL ED�L�R (-1 �SE YOKTUR)
            je update_min           // E�ER YOKSA G�NCELLEN�R

            cmp edx, eax            // E�ER VARDIYSA, MEVCUT M�N�MUMDAN K���K M� KONTROL ED�L�R
            jge skip_value          // K���K DE��LSE BU DE�ER ATLANIR

            update_min :
            mov eax, edx            // E�ER �ARTLAR SA�LANIYORSA, EDX M�N�MUM DE�ER OLARAK G�NCELLEN�R

            skip_value :
            inc ebx                 // D�Z�DEK� B�R SONRAK� �NDEKSE ATLARIZ 
            dec ecx                 // KALAN ELEMAN SAYISINI G�NCELLER�Z
            jmp find_min_cdf        // D�NG�N�N BA�INA D�NER�Z

            min_cdf_found :
        mov min_cdf, eax        // BULUNAN EN K���K DE�ER� M�N_CDF'E KAYDEDER�Z


        //BU A�AMADA RANGE DEGER� HESAPLANIR
        mov eax, number_of_pixels       //EAX REG�STERINA TOPLAM P�KSEL SAYISI Y�KLEN�R
            sub eax, min_cdf                //range = number_of_pixels - min_cdf

            mov range, eax                  //range de�erimizi kaydederiz.

            //BU A�AMADA CDF D�Z�S� NORMAL�ZE ED�L�R
            mov esi, cdf        //ESI REG�STERINA CDF D�Z�S�N�N BASLANGIC ADRES� Y�KLEN�R
            mov ecx, 256        //ECX REG�STERINA 256 DEGER� YUKLEN�R
            xor ebx, ebx        //�NDEX OLARAK KULLANILACAK EBX REG�STERI 0'LANIR 

            normalize_cdf_loop :
            cmp ecx, 0                  //ECX 0'A E��TSE DONGUNUN SONUNA ATLANIR 
            je normalize_cdf_end

            mov edx, [esi + ebx * 4]    //CDF D�Z�S�N�N MEVCUT �NDEX�NDEK� DEGER EDX REG�STERINA ATANIR
            sub edx, min_cdf            //EDX'TEN M�N_CDF �IKARTILIR CDF[�]-M�N_CDF

            // 64 - bit �arpma i�in edx de�erini geni�letiyoruz
            mov eax, edx                //EAX REG�STERINA EDX REG�STERI ATANIR
            mov edx, 0                  //EDX:EAX REG�STERI �ARPMA ��LEM� ���N HAZIRLANIR
            mov edi, 255
            mul edi               //EAX'� 255 �LE �ARPARIZ SONU� 64 B�TL�K EDX:EAX ��FT�NDE TUTULUR 
            div range                   // EDX:EAX �iftini range'e b�l�yoruz
            //EAX = ((cdf[i] - min_cdf) * 255) / range i�lemi ger�ekle�tirilmi� olur.
            cmp eax, 0                  //EAX'�n s�f�rdan k���k olup olmad���n� kontrol eder.

            jl zero_value
            cmp eax, 255                //EAX'�n 255'ten b�y�k olup olmad���n� kontrol eder.

            jg max_value
            jmp store_value             //De�er uygun aral�ktaysa, store_value etiketine atlar.


            zero_value :                //EAX s�f�rdan k���kse buraya atlan�r.

        mov eax, 0                      //EAX s�f�rlan�r.

            jmp store_value             //store_value etiketine atlan�r.


            max_value :                 //EAX 255'ten b�y�kse buraya atlan�r.

        mov eax, 255                    //EAX 255 olarak ayarlan�r.


            store_value :               //Normalle�tirilmi� EAX de�erini cdf dizisine kaydederiz.
            mov[esi + ebx * 4], eax     //Normalle�tirilmi� CDF de�eri dizideki yerine yaz�l�r.



            inc ebx                     //Bir sonraki indekse ge�eriz.

            dec ecx                     //D�ng� sayac�n� g�nceller.

            jmp normalize_cdf_loop      //D�ng�n�n ba��na geri d�ner.


            normalize_cdf_end :         //T�m CDF de�erleri normalle�tirildikten sonra buraya geliriz.

    }


    /* Q-2 (d) - Apply the histogram equalization on the image.
                    Write the new pixels to outputImage. */
                    //Girdi: Orijinal g�r�nt� pikselleri (image dizisi) ve normalize edilmi� K�m�latif Da��l�m Fonksiyonu (cdf dizisi).
                    //��kt�: Histogram e�itleme uygulanm�� yeni piksel de�erleri ile outputImage dizisi.

    __asm {
        mov ecx, number_of_pixels   //ECX register'�na toplam piksel say�s�n� y�kler.
        //D�ng� sayac� olarak kullan�lacakt�r.

        mov esi, image              //ESI register'�na image dizisinin ba�lang�� adresini y�kler.

        mov edi, outputImage        //EDI register'�na outputImage dizisinin ba�lang�� adresini y�kler.

        mov ebx, cdf                //EBX register'�na cdf dizisinin ba�lang�� adresini y�kler.


        equalization_loop :         //Histogram e�itleme i�lemini ger�ekle�tirecek d�ng�n�n ba�lang�� etiketi.

        cmp ecx, 0                  //D�ng� sayac� s�f�ra ula�t�ysa t�m pikseller i�lenmi�tir.

            je equalization_end     //ECX s�f�ra e�itse, d�ng�y� sonland�rmak i�in equalization_end etiketine atlar.


            mov al, [esi]           //ESI'nin i�aret etti�i bellek adresindeki piksel de�erini al�r ve AL register'�na y�kler.

            xor edx, edx            //EDX register'�n� s�f�rlar.

            mov dl, al              // EDX = piksel de�eri(indeks olarak kullanaca��z)
            mov eax, [ebx + edx * 4]        //EBX'in (CDF dizisinin ba�lang�c�) EDX ile adreslenmi� konumundan (piksel de�eri indeks olarak kullan�larak) de�eri okur.

            mov[edi], al                        //AL register'�n�n i�eri�ini EDI'nin i�aret etti�i bellek adresine yazar.
            //outputImage dizisine yazma i�lemi ger�ekle�tirir.


            inc esi                     //image dizisindeki bir sonraki piksele ge�ilir.

            inc edi                     //outputImage dizisindeki bir sonraki konuma ge�ilir.

            dec ecx                     //Kalan piksel say�s�n� g�nceller.

            jmp equalization_loop

            equalization_end :          //T�m pikseller i�lendi�inde d�ng� buraya atlar ve i�lem tamamlan�r.

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