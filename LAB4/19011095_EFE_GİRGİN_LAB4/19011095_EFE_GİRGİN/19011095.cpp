#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>

// Function declarations
void print_matrix(unsigned* matrix, unsigned rows, unsigned cols, FILE* file);
void read_matrix(const char* filename, unsigned** matrix, unsigned* rows, unsigned* cols);
void read_kernel(const char* filename, unsigned** kernel, unsigned* k);
void write_output(const char* filename, unsigned* output, unsigned rows, unsigned cols);
void initialize_output(unsigned*, unsigned, unsigned);

int main() {

    unsigned n, m, k;  // n = rows of matrix, m = cols of matrix, k = kernel size
    unsigned* matrix = NULL;   // Input matrix
    unsigned* kernel = NULL;   // Kernel matrix
    unsigned* output = NULL;   // Output matrix

    char matrix_filename[30];
    char kernel_filename[30];

    // Read the file names
    printf("Enter matrix filename: ");
    scanf("%s", matrix_filename);
    printf("Enter kernel filename: ");
    scanf("%s", kernel_filename);

    // Read matrix and kernel from files
    read_matrix(matrix_filename, &matrix, &n, &m);  // Read matrix from file
    read_kernel(kernel_filename, &kernel, &k);      // Read kernel from file

    // For simplicity we say: padding = 0, stride = 1
    // With this setting we can calculate the output size
    unsigned output_rows = n - k + 1;
    unsigned output_cols = m - k + 1;
    output = (unsigned*)malloc(output_rows * output_cols * sizeof(unsigned));
    initialize_output(output, output_rows, output_cols);

    // Check for successful memory allocation
    if (!matrix || !kernel || !output) {
        printf("Memory allocation failed.\n");
        return -1;
    }

    // Print the input matrix and kernel
    printf("Input Matrix: ");
    print_matrix(matrix, n, m, stdout);

    printf("\nKernel: ");
    print_matrix(kernel, k, k, stdout);

    /******************* KODUN BU KISMINDAN SONRA DEĞİŞİKLİK YAPABİLİRSİNİZ - ÖNCEKİ KISIMLARI DEĞİŞTİRMEYİN *******************/

    // Gerekli değişkenleri tanımlıyoruz
    unsigned matrix_rows = n;         // Giriş matrisinin satır sayısı
    unsigned matrix_cols = m;         // Giriş matrisinin sütun sayısı
    unsigned kernel_size = k;         // Kernel boyutu

    // Assembly bloğunda kullanacağımız diğer değişkenler
    unsigned int i, j, ki, kj;
    unsigned int sum;
    unsigned int kernel_val, matrix_val;
    unsigned int k_idx, m_idx, out_idx;
    unsigned int rows_out, cols_out, cols_in, k_size;

    __asm {

        // BURADA MATRİX,OUTPUT VE KERNEL'İN BAŞLANGIC MATRİSLERİNİ TUTUYORUZ
        MOV ESI, matrix                                 // ESI < -matrix pointer
        MOV EDI, output                                 // EDI < -output pointer
        MOV EBX, kernel                                 // EBX < -kernel pointer

        //BOYUTLARI TANIMLIYORUZ BURADA
        //OUTPUT MATRİSİNİN SATIR SAYISINI ALDIK
        MOV EAX, output_rows
        MOV rows_out, EAX                           // rows_out = output_rows

        //OUTPUT MATRİSİNİN SUTUN SAYISINI ALDIK
        MOV EAX, output_cols
        MOV cols_out, EAX                                   // cols_out = output_cols

        //GİRİŞ MATRİSİNİN SUTUN SAYISINI ALDIK
        MOV EAX, matrix_cols
        MOV cols_in, EAX                                        // cols_in = matrix_cols

        //KERNEL MATRİSİNİN BOYUTU
        MOV EAX, kernel_size
        MOV k_size, EAX                                         // k_size = kernel_size

        // i = 0 İNDİSİMİZİ 0'LADIK VE İ'YE ATADIK
        XOR EAX, EAX
        MOV i, EAX

        OuterLoop_i :
        //SATIR SAYISININ SONUNA GELİRSEK
        MOV EAX, i
            CMP EAX, rows_out
            JAE EndOuterLoop_i                      // EĞER İ İNDİSİMİZ OUTPUT MATRİSİNİN SONUNA GELDİYSE ATLAMA YAPAR(TERMİNATİON)

            // j = 0 İNDİSİMİZİ 0'LADIK
            XOR EAX, EAX
            MOV j, EAX

            InnerLoop_j :
            //SUTUN SAYISININ SONUNA GELİRSEK
        MOV EAX, j
            CMP EAX, cols_out
            JAE EndInnerLoop_j                  // EĞER SUTUN SAYISI OUTPUT MATRİSİNİN SUTUN SAYISINI AŞARSA ATLAMA YAPAR

            //SUM = 0
            XOR EAX, EAX
            MOV sum, EAX

            //KERNEL MATRİSİNİN İNDİSİ'Nİ 0'LADIK
            XOR EAX, EAX
            MOV ki, EAX

            //KERNEL MATRİSİNİN İNDİSİ SATIR SAYISINI TAMAMLARSA
            Loop_ki :
        MOV EAX, ki
            CMP EAX, k_size
            JAE EndLoop_ki                  // KERNEL SATIR İNDİSİ KERNEL'IN BOYUTUNU AŞARSA ATLAMA YAPAR

            //KERNEL MATRİSİNİN İNDİSİNİ 0'LADIK
            XOR EAX, EAX
            MOV kj, EAX

            //KERNEL MATRİSİNİN İNDİSİ SUTUN SAYISINI TAMAMLARSA
            Loop_kj :
        MOV EAX, kj
            CMP EAX, k_size
            JAE EndLoop_kj                  // KERNEL SUTUN İNDİSİ KERNEL'IN BOYUTUNU AŞARSA ATLAMA YAPAR

            //SINIR KONTROLU SATIR İÇİN ANA MATRİSTE
            MOV EAX, i
            ADD EAX, ki
            CMP EAX, matrix_rows
            JAE SkipCalculation
            //SINIR KONTROLU SUTUN İÇİN ANA MATRİSTE
            MOV EAX, j
            ADD EAX, kj
            CMP EAX, matrix_cols
            JAE SkipCalculation

            // k_idx = ki * k_size + kj
            //MATRİSİN KAÇINCI ELEMANINDA OLDUĞUMUZU TESPİT EDİYORUZ
            MOV EAX, ki                                         // EAX = ki
            MUL k_size                                          // EDX:EAX = ki * k_size
            ADD EAX, kj                                         // EAX = ki * k_size + kj
            MOV k_idx, EAX

            // kernel_val = kernel[k_idx]
            //KERNEL POİNTERINDAN 8 BAYT ÖTEDEKİ YERİ KERNEL VALUE OLARAK GETİRİYORUZ
            //4 İLE ÇARPMAMIZIN SEBEBİ İNT DEĞERİ OLDUGU İÇİN 
            MOV EAX, k_idx                      // EAX = k_idx
            MOV EDX, [EBX + EAX * 4]            // EAX = kernel[k_idx]
            MOV kernel_val, EDX


            // m_idx = (i + ki) * cols_in + (j + kj)
            //MATRİXİN İNDEX DEĞERİNİ BÖYLE ELDE EDİYORUZ 
            MOV EAX, i
            ADD EAX, ki                                     // EAX = i + ki
            MUL cols_in                                     // EDX:EAX = (i + ki) * cols_in
            ADD EAX, j
            ADD EAX, kj                                     // EAX = (i + ki) * cols_in + j + kj
            MOV m_idx, EAX


            // matrix_val = matrix[m_idx]
            //O İNDEXTEKİ DEĞERİ BİR DEĞİŞKENE ALIYORUZ
            MOV EAX, m_idx                                  // EAX = m_idx
            MOV EDX, [ESI + EAX * 4]                        // EAX = matrix[m_idx]
            MOV matrix_val, EDX


            // sum += kernel_val * matrix_val
            MOV EAX, kernel_val
            MUL matrix_val                      // EDX:EAX = kernel_val * matrix_val
            ADD sum, EAX                        // sum += EAX



        SkipCalculation:
        // kj++
        MOV EAX, kj
            INC EAX
            MOV kj, EAX
            JMP Loop_kj



        EndLoop_kj:
        // ki++
        MOV EAX, ki
            INC EAX
            MOV ki, EAX
            JMP Loop_ki



        EndLoop_ki:
        // out_idx = i * cols_out + j
        MOV EAX, i
            MUL cols_out// EDX:EAX = i * cols_out
            ADD EAX, j// EAX = i * cols_out + j
            MOV out_idx, EAX



            // output[out_idx] = sum
            mov EDX,sum
            MOV EAX, out_idx
            MOV[EDI + eax * 4], EDX
            mov sum,eax


            // j++
            MOV EAX, j
            INC EAX
            MOV j, EAX
            JMP InnerLoop_j

            EndInnerLoop_j :
        // i++
        MOV EAX, i
            INC EAX
            MOV i, EAX
            JMP OuterLoop_i

            EndOuterLoop_i :
    }

    /******************* DEĞİŞİKLİK YAPABİLECEĞİNİZ KISIM *******************/

    // Write result to output file
    write_output("./output.txt", output, output_rows, output_cols);

    // Print result
    printf("\nOutput matrix after convolution: ");
    print_matrix(output, output_rows, output_cols, stdout);

    // Free allocated memory
    free(matrix);
    free(kernel);
    free(output);

    return 0;
}

// Diğer fonksiyonlar...
void print_matrix(unsigned* matrix, unsigned rows, unsigned cols, FILE* file) {
    if (file == stdout) {
        printf("(%ux%u)\n", rows, cols);
    }
    for (unsigned i = 0; i < rows; i++) {
        for (unsigned j = 0; j < cols; j++) {
            fprintf(file, "%u ", matrix[i * cols + j]);
        }
        fprintf(file, "\n");
    }
}

void read_matrix(const char* filename, unsigned** matrix, unsigned* rows, unsigned* cols) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    // Read dimensions
    fscanf(file, "%u %u", rows, cols);
    *matrix = (unsigned*)malloc((*rows) * (*cols) * sizeof(unsigned));

    // Read matrix elements
    for (unsigned i = 0; i < (*rows); i++) {
        for (unsigned j = 0; j < (*cols); j++) {
            fscanf(file, "%u", &(*matrix)[i * (*cols) + j]);
        }
    }

    fclose(file);
}

void read_kernel(const char* filename, unsigned** kernel, unsigned* k) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    // Read kernel size
    fscanf(file, "%u", k);
    *kernel = (unsigned*)malloc((*k) * (*k) * sizeof(unsigned));

    // Read kernel elements
    for (unsigned i = 0; i < (*k); i++) {
        for (unsigned j = 0; j < (*k); j++) {
            fscanf(file, "%u", &(*kernel)[i * (*k) + j]);
        }
    }

    fclose(file);
}

void write_output(const char* filename, unsigned* output, unsigned rows, unsigned cols) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    // Write dimensions of the output matrix
    fprintf(file, "%u %u\n", rows, cols);

    // Write output matrix elements
    print_matrix(output, rows, cols, file);

    fclose(file);
}

void initialize_output(unsigned* output, unsigned output_rows, unsigned output_cols) {
    for (unsigned i = 0; i < output_rows * output_cols; i++) {
        output[i] = 0;
    }
}