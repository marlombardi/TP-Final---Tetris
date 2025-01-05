#include "pieces.h"
#include <stdio.h>
#include <stdlib.h> //TEMP, JUST A STUB
#include <time.h> //TEMP, JUST A STUB

#define BOARD_HEIGHT 20
#define BOARD_WIDTH 10

void nextPiece(void); 

void rotateCW_3by3(int[][3]);
void rotateCW_4by4(int[][4]);

void printMatrix_3by3(int[][3]);
void printMatrix_4by4(int[][4]);
void printBoard(int[][10]);




int main() {
    srand(time(NULL)); //set random seed

    int board[20][10];

    //initialize the board, 20*10 size
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = 0;
        }
    }

    printBoard(board);
    nextPiece();
    nextPiece();
    nextPiece();
    nextPiece();
/*
    printMatrix_4by4(I);
    printMatrix_3by3(L);
    rotateCW_3by3(L);
    rotateCW_4by4(I);
    rotateCW_4by4(I);
    printMatrix_3by3(L);
    printMatrix_4by4(I);
*/

}

void nextPiece(void) {
    int nextPieceID = (rand() % 2);
    void* pieces[] = {(void*)L, (void*)I};
    if(nextPieceID == 0) {
        printMatrix_3by3(pieces[nextPieceID]);
    }
    
    if(nextPieceID == 1) {
        printMatrix_4by4(pieces[nextPieceID]);
    }
}


void rotateCW_3by3(int matrix[][3]) { //all 3's in this code are because we're working with a fixed size of 3.
    int temporaryMatrix[3][3];
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temporaryMatrix[i][j] = matrix[i][j]; 
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = temporaryMatrix[(2 - j)][i]; //(2-j) remaps the j-coord from (0 -> 2) to (2 -> 0)
        }
    }
}


void rotateCW_4by4(int matrix[][4]) { //all 4's in this code are because we're working with a fixed size of 4.
    int temporaryMatrix[4][4];
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temporaryMatrix[i][j] = matrix[i][j]; 
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[i][j] = temporaryMatrix[(3 - j)][i]; //(3-j) remaps the j-coord from (0 -> 3) to (3 -> 0)
        }
    }
}

void printMatrix_3by3(int matrix[][3]) { //all 3's in this code are because we're working with a fixed size of 4.
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
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

void printBoard(int board[][BOARD_WIDTH]){
    for(int i = 0; i < BOARD_HEIGHT; i++) {
        for(int j = 0; j < BOARD_WIDTH; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}