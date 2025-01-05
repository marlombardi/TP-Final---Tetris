#include "pieces.h"
#include <stdio.h>

void rotateCCW_4by4(int[][4]);

void printMatrix_4by4(int[][4]);

int main() {
    printMatrix_4by4(I);
    rotateCCW_4by4(I);
    printMatrix_4by4(I);
}

void rotateCCW_4by4(int matrix[][4]) { //all 4's in this code are because we're working with a fixed size of 4.
    int temporaryMatrix[4][4];
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temporaryMatrix[i][j] = matrix[i][j]; 
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[(3 - j)][i] = temporaryMatrix[i][j]; //(3-j) remaps the j-coord from (0 -> 3) to (3 -> 0)
        }
    }
}


void printMatrix_4by4(int matrix[][4]) { //all 4's in this code are because we're working with a fixed size of 4.
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}