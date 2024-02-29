#include <stdio.h>
#include "matrix/matrix_comparison.h"

int main() {
    puts("Multi-threaded (GPU): ");
    matrixComparison(500, 50, 500*500, 0);

    puts("\nSingle-threaded (GPU): ");
    matrixComparison(500, 1, 1 ,1);

    // matrixComparison(800, 50);
    // matrixComparison(2300, 50);

    return 0;
}