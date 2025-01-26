#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>

void printMatrix(int** matrix, int row, int col);
int** convMatrix(int** matrix, int mRow, int mCol, int** kernel, int kRow, int kCol);
int** createMatrix(int rows, int cols);

int main(int argc, char* argv[]) {

    int** kernel = (int**)malloc(3 * sizeof(int*));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (int*)malloc(3 * sizeof(int));
    }

    // Değerleri yerleştirme
    int values[3][3] = { {0, 1, 0}, {-5, 3, 0}, {-7, 1, 0} };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = values[i][j];
        }
    }

    // 10x12 boyutunda bir matris tanımlama
    int** matrix = createMatrix(6, 8);

    // Matrisin elemanlarını sıralı sayılarla doldurma
    int count = 1;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            matrix[i][j] = count++;
        }
    }
    int** conv = convMatrix(matrix, 6, 8, kernel, 3, 3);
    printMatrix(conv, 4, 6);
    return 0;
}
int** createMatrix(int rows, int cols) {
    int** matrix = (int**)malloc(rows * sizeof(int*));
    if (matrix == NULL) {
        printf("Bellek ayrilamadi!\n");
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
        if (matrix[i] == NULL) {
            printf("Bellek ayrilamadi!\n");
            // Daha önce ayrılan belleği serbest bırak
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            exit(1);
        }
    }

    return matrix;
}
int** convMatrix(int** matrix, int mRow, int mCol, int** kernel, int kRow, int kCol)
{
    int i, j, k, l;
    int convRow = mRow - kRow + 1;
    int convCol = mCol - kCol + 1;
    int sum;
    int** conv = (int**)malloc(sizeof(int*) * convRow);
    for (i = 0; i < convRow; i++)
    {
        conv[i] = (int*)malloc(sizeof(int) * convCol);
    }
    for (i = 0; i < mRow - kRow + 1; i++)
    {
        for (j = 0; j < mCol - kCol + 1; j++)
        {
            sum = 0;
            for (k = 0; k < kRow; k++)
            {
                for (l = 0; l < kCol; l++)
                {
                    sum += matrix[i + k][j + l] * kernel[k][l];
                }
            }
            conv[i][j] = sum;
        }
    }
    return conv;
}
void printMatrix(int** matrix, int row, int col)
{
    int i, j;
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}
