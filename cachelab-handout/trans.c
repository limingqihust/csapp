/* 
 * trans.c - 矩阵转置B=A^T
 *每个转置函数都必须具有以下形式的原型：
 *void trans（int M，int N，int a[N][M]，int B[M][N]）；
 *通过计算，块大小为32字节的1KB直接映射缓存上的未命中数来计算转置函数。
 */ 
#include <stdio.h>
#include "cachelab.h"
int is_transpose(int M, int N, int A[N][M], int B[M][N]);
char transpose_submit_desc[] = "Transpose submission";  //请不要修改“Transpose_submission”


void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{

//                          请在此处添加代码
//*************************************Begin********************************************************
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
#define T 16
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
 * 我们在下面定义了一个简单的方法来帮助您开始，您可以根据下面的例子把上面值置补充完整。
 */ 

/* 
 * 简单的基线转置功能，未针对缓存进行优化。
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

void registerFunctions()
{
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    registerTransFunction(trans, trans_desc); 

}

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

