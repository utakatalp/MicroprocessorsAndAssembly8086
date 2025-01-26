#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>

// Function to print a matrix stored in a 1D array
void print_matrix(unsigned* matrix, unsigned rows, unsigned cols, FILE* file);
// Function to read matrix from a file
void read_matrix(const char* filename, unsigned** matrix, unsigned* rows, unsigned* cols);
// Function to read kernel from a file
void read_kernel(const char* filename, unsigned** kernel, unsigned* k);
// Function to write output matrix to a file
void write_output(const char* filename, unsigned* output, unsigned rows, unsigned cols);
// Initialize output as zeros.
void initialize_output(unsigned*, unsigned, unsigned);

int main() {

    unsigned n, m, k;  // n = rows of matrix, m = cols of matrix, k = kernel size
    // Dynamically allocate memory for matrix, kernel, and output
    unsigned* matrix = NULL;  // Input matrix
    unsigned* kernel = NULL;  // Kernel size 3x3
    unsigned* output = NULL;  // Max size of output matrix

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

    // Print the input matrix and kernel
    printf("Input Matrix: ");
    print_matrix(matrix, n, m, stdout);

    printf("\nKernel: ");
    print_matrix(kernel, k, k, stdout);

    /******************* KODUN BU KISMINDAN SONRASINDA DEĞİŞİKLİK YAPABİLİRSİNİZ - ÖNCEKİ KISIMLARI DEĞİŞTİRMEYİN *******************/


    // Assembly dilinde 2d konvolüsyon işlemini aşağıdaki blokta yazınız ----->
    // Assembly kod bloğu içinde kullanacağınız değişkenleri burada tanımlayabilirsiniz. ---------------------->
    // Aşağıdaki değişkenleri kullanmak zorunda değilsiniz. İsterseniz değişiklik yapabilirsiniz.

    unsigned matrix_value, kernel_value; // Konvolüsyon için gerekli 1 matrix ve 1 kernel değişkenleri saklanabilir.
    unsigned sum = 0;                       // Konvolüsyon toplamını saklayabilirsiniz.
    unsigned matrix_offset, kernel_offset, output_offset;  // Input matrisi üzerinde gezme işleminde sınırları ayarlamak için kullanılabilir.
    
    __asm {
                XOR ESI, ESI        // i = 0 matrix row

        mROW:
                CMP ESI, [output_rows]
                JAE finishMRow
                XOR EDI, EDI        // j = 0 matrix col
        mCOL:
                MOV EAX, [output_cols]
                CMP EDI, EAX
                JAE finishMCol
                MOV sum, 0          // Her Output indisi için sum'ı 0'la
                XOR EBX, EBX        // k = 0 kernel row
                
        kROW:
                MOV EAX, [k]
                CMP EBX, EAX
                JAE finishKRow
                
                XOR ECX, ECX        // l = 0 kernel col
        kCOL:
                MOV EAX, [k]
                CMP ECX, EAX
                JAE finishKCol
                

                
                // matrix offset
                MOV EAX, ESI        // EAX = i
                ADD EAX, EBX        // EAX = i + k
                MUL [m]   // EAX = (i+k)*m
                ADD EAX, EDI        // EAX = (i+k)*m+j
                ADD EAX, ECX        // EAX = (i+k)*m+(j+l)
                MOV matrix_offset, EAX
                
                // matrix index
                MOV EDX, [matrix]   // Matrix'in adresini çekme
                MOV EAX, matrix_offset
                SHL EAX, 2          // DWORD ile çalıştığımızdan 4 ile çarpıyoruz offseti
                ADD EDX, EAX        // İlgili elemanın adresine ulaşım
                PUSH EBX
                MOV EBX, EDX
                MOV EAX, [EBX]      // İlgili elemanı matrix value'ye aktarma
                MOV matrix_value, EAX
                POP EBX
                
                // kernel ofset: k * k + l
                MOV EAX, EBX        // EAX = k
                MUL [k]   // EAX = k*k, DWORD PTR kullanmaya gerek yok k zaten 32 bit
                ADD EAX, ECX        // EAX = k*k+l
                MOV kernel_offset, EAX
                
                // kernel index
                MOV EDX, [kernel]   // Kernel'in adresini çekme
                MOV EAX, kernel_offset
                SHL EAX, 2          // Offseti 4 ile çarp (DWORD)
                ADD EDX, EAX        // Offseti kernel'in adresine ekle
                PUSH EBX
                MOV EBX, EDX
                MOV EAX, [EBX]      // İlgili değeri kernel_value'ya at
                MOV kernel_value, EAX
                POP EBX
                
                // ilgili değerleri çarp ve sum'a ekle
                MOV EAX, matrix_value
                MUL kernel_value
                ADD sum, EAX
                
                INC ECX             // l++
                JMP kCOL

        finishKCol:
                INC EBX             // k++
                JMP kROW

        finishKRow:
                // Output index: i * output_cols + j
                MOV EAX, ESI
                MUL [output_cols]
                ADD EAX, EDI
                MOV output_offset, EAX
                
                // sum değerini output'un index'ine at
                MOV EDX, [output]   // output adresini yükle
                MOV EAX, output_offset
                SHL EAX, 2          // 4'le çarp offseti
                ADD EDX, EAX        // output'un adresine offseti ekle
                PUSH EBX
                MOV EBX, EDX
                MOV EAX, sum
                MOV[EBX], EAX      // sum'ı output'un indexine at
                POP EBX
                
                INC EDI             // j++
                JMP mCOL

        finishMCol:
                INC ESI             // i++
                JMP mROW

        finishMRow:

    }



    /******************* KODUN BU KISMINDAN ÖNCESİNDE DEĞİŞİKLİK YAPABİLİRSİNİZ - SONRAKİ KISIMLARI DEĞİŞTİRMEYİN *******************/


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

void print_matrix(unsigned* matrix, unsigned rows, unsigned cols, FILE* file) {
    if (file == stdout) {
        printf("(%ux%u)\n", rows, cols);
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
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
    *matrix = (unsigned*)malloc(((*rows) * (*cols)) * sizeof(unsigned));

    // Read matrix elements
    for (int i = 0; i < (*rows); i++) {
        for (int j = 0; j < (*cols); j++) {
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
    for (int i = 0; i < (*k); i++) {
        for (int j = 0; j < (*k); j++) {
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
    int i;
    for (i = 0; i < output_cols * output_rows; i++)
        output[i] = 0;

}

