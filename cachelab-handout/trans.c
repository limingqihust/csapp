/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */



// int* A=0x0010d080    0x0010e07c
// int* B=0x0014d080    0x0014e07c
// 直接相联
// set_index有5位 32个cache行
// block_off有5位 一个cache行有8个int型数据
//0x0010d080 0b10000110100[00100][00000]
//0x0010e07c 0b10000111000[00011][11100]
//0x0014d080 0b10100110100[00100][00000]
//0x0014e07c 0b10100111000[00011][11100]
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{

    if(M==32 && N==32) {
        for (int i = 0; i < N; i += 8) {
            for (int j = 0; j < M; j += 8) {
                if(i==j)
                {
                    for(int p=0;p<8;p++) {
                        int buf0, buf1, buf2, buf3, buf4, buf5, buf6, buf7;
                        buf0 = A[i + p][j + 0];
                        buf1 = A[i + p][j + 1];
                        buf2 = A[i + p][j + 2];
                        buf3 = A[i + p][j + 3];
                        buf4 = A[i + p][j + 4];
                        buf5 = A[i + p][j + 5];
                        buf6 = A[i + p][j + 6];
                        buf7 = A[i + p][j + 7];

                        B[i+p][j+0] = buf0;
                        B[i+p][j+1] = buf1;
                        B[i+p][j+2] = buf2;
                        B[i+p][j+3] = buf3;
                        B[i+p][j+4] = buf4;
                        B[i+p][j+5] = buf5;
                        B[i+p][j+6] = buf6;
                        B[i+p][j+7] = buf7;
                    }
                    for(int p=0;p<8;p++)
                    {
                        for(int q=p;q<8;q++)
                        {
                            if(p==q)
                                continue;
                            int tmp=B[i+p][j+q];
                            B[i+p][j+q]=B[j+q][i+p];
                            B[j+q][i+p]=tmp;
                        }
                    }
                }
                else
                {
                    for(int p=0;p<8;p++)
                    {
                        for(int q=0;q<8;q++)
                        {
                            B[j+q][i+p]=A[i+p][j+q];
                        }
                    }
                }
            }
        }
    }
    else if(M==64 && N==64)
    {
        for(int i=0;i<N;i+=8)
        {
            for(int j=0;j<M;j+=8)
            {

                int buf0,buf1,buf2,buf3,buf4,buf5,buf6,buf7;

                // A[1,1]->B[1,1] 转置
                // A[1,2]->B[1,2] 转置
                for(int p=0;p<4;p++) {
                    buf0 = A[i + p][j + 0];
                    buf1 = A[i + p][j + 1];
                    buf2 = A[i + p][j + 2];
                    buf3 = A[i + p][j + 3];
                    buf4 = A[i + p][j + 4];
                    buf5 = A[i + p][j + 5];
                    buf6 = A[i + p][j + 6];
                    buf7 = A[i + p][j + 7];
                    B[j + 0][i + p] = buf0;
                    B[j + 1][i + p] = buf1;
                    B[j + 2][i + p] = buf2;
                    B[j + 3][i + p] = buf3;
                    B[j + 0][i + 4 + p] = buf4;
                    B[j + 1][i + 4 + p] = buf5;
                    B[j + 2][i + 4 + p] = buf6;
                    B[j + 3][i + 4 + p] = buf7;
                }


                // B[1,2]->B[2,1] 不转置
                // A[2,1]->B[1,2] 转置
                for(int p=0;p<4;p++)
                {
                    buf0=A[i+4+0][j+p];
                    buf1=A[i+4+1][j+p];
                    buf2=A[i+4+2][j+p];
                    buf3=A[i+4+3][j+p];


                    buf4=B[j+p][i+4+0];
                    buf5=B[j+p][i+4+1];
                    buf6=B[j+p][i+4+2];
                    buf7=B[j+p][i+4+3];

                    B[j+p][i+4+0]=buf0;
                    B[j+p][i+4+1]=buf1;
                    B[j+p][i+4+2]=buf2;
                    B[j+p][i+4+3]=buf3;

                    B[j+4+p][i+0]=buf4;
                    B[j+4+p][i+1]=buf5;
                    B[j+4+p][i+2]=buf6;
                    B[j+4+p][i+3]=buf7;
                }

//                // A[2,2]->B[2,2] 转置
                for(int p=0;p<4;p++)
                {
                    buf4 = A[i + 4 + p][j + 4];
                    buf5 = A[i + 4 + p][j + 5];
                    buf6 = A[i + 4 + p][j + 6];
                    buf7 = A[i + 4 + p][j + 7];
                    B[j + 4][i + 4 + p] = buf4;
                    B[j + 5][i + 4 + p] = buf5;
                    B[j + 6][i + 4 + p] = buf6;
                    B[j + 7][i + 4 + p] = buf7;

                }
            }
        }

    }
    else
    {
#define T 17
        for(int i=0;i<N;i+=T)
       {
           for(int j=0;j<M;j+=T)
           {
               for(int p=0;p<T && p+i<N;p++)
               {
                   for(int q=0;q<T && q+j<M;q++)
                       B[j+q][i+p]=A[i+p][j+q];
               }

           }
       }

    }
}


/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);
    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

