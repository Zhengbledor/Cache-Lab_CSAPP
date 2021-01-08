/*
 *      Name:郑志豪
 *      Time:2021/1/4  
 *      StudentId:U201814781
 */

/*
 *      32*32 : m<300
 *      64*64 : m<1300
 *      61*67 : m<2000
 * 
 *      s=5 2^5=32 sets
 *      E=1 1 line in one set
 *      b=5 2^5=32 bytes in one block, so one line can contian 8 int
 */

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

char transpose_submit_32_32_desc[] = "Transpose submission 32*32";
void transpose_submit_32_32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k, l, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    for (i = 0; i < 32; i = i + 8)
    {
        for (j = 0; j < 32; j = j + 8)
        {
            if (i == j)
            {
                for (k = i; k < i + 8; k++)
                {
                    tmp0 = A[k][j];
                    tmp1 = A[k][j + 1];
                    tmp2 = A[k][j + 2];
                    tmp3 = A[k][j + 3];
                    tmp4 = A[k][j + 4];
                    tmp5 = A[k][j + 5];
                    tmp6 = A[k][j + 6];
                    tmp7 = A[k][j + 7];
                    B[k][j] = tmp0;
                    B[k][j + 1] = tmp1;
                    B[k][j + 2] = tmp2;
                    B[k][j + 3] = tmp3;
                    B[k][j + 4] = tmp4;
                    B[k][j + 5] = tmp5;
                    B[k][j + 6] = tmp6;
                    B[k][j + 7] = tmp7;
                }
                for (k = i; k < i + 8; k++)
                {
                    for (l = k + 1; l < i + 8; l++)
                    {
                        tmp0 = B[k][l];
                        B[k][l] = B[l][k];
                        B[l][k] = tmp0;
                    }
                }
            }
            else
            {
                for (k = i; k < i + 8; k++)
                {
                    for (l = j; l < j + 8; l++)
                    {
                        B[l][k] = A[k][l];
                    }
                }
            }
        }
    }
}

char transpose_submit_64_64_desc[] = "Transpose submission 64*64";
void transpose_submit_64_64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    for (i = 0; i < 64; i += 8)
    {
        for (j = 0; j < 64; j += 8)
        {
            for (k = i; k < i + 4; k++)
            {
                tmp0 = A[k][j];
                tmp1 = A[k][j + 1];
                tmp2 = A[k][j + 2];
                tmp3 = A[k][j + 3];
                tmp4 = A[k][j + 4];
                tmp5 = A[k][j + 5];
                tmp6 = A[k][j + 6];
                tmp7 = A[k][j + 7];

                B[j][k] = tmp0;
                B[j + 1][k] = tmp1;
                B[j + 2][k] = tmp2;
                B[j + 3][k] = tmp3;
                B[j][k + 4] = tmp4;
                B[j + 1][k + 4] = tmp5;
                B[j + 2][k + 4] = tmp6;
                B[j + 3][k + 4] = tmp7;
            }
            for (k = j; k < j + 4; k++)
            {
                tmp0 = A[i + 4][k];
                tmp1 = A[i + 5][k];
                tmp2 = A[i + 6][k];
                tmp3 = A[i + 7][k];
                tmp4 = B[k][i + 4];
                tmp5 = B[k][i + 5];
                tmp6 = B[k][i + 6];
                tmp7 = B[k][i + 7];

                B[k][i + 4] = tmp0;
                B[k][i + 5] = tmp1;
                B[k][i + 6] = tmp2;
                B[k][i + 7] = tmp3;
                B[k + 4][i] = tmp4;
                B[k + 4][i + 1] = tmp5;
                B[k + 4][i + 2] = tmp6;
                B[k + 4][i + 3] = tmp7;
            }
            for (k = i + 4; k < i + 8; k++)
            {
                tmp0 = A[k][j + 4];
                tmp1 = A[k][j + 5];
                tmp2 = A[k][j + 6];
                tmp3 = A[k][j + 7];

                B[j + 4][k] = tmp0;
                B[j + 5][k] = tmp1;
                B[j + 6][k] = tmp2;
                B[j + 7][k] = tmp3;
            }
        }
    }
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32 && N == 32)
    {
        transpose_submit_32_32(M, N, A, B);
    }
    else if (M == 64 && N == 64)
    {
        transpose_submit_64_64(M, N, A, B);
    }
    else
    {
        int i, j, tmp;

        for (i = 0; i < N; i++)
        {
            for (j = 0; j < M; j++)
            {
                tmp = A[i][j];
                B[j][i] = tmp;
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

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
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
    //registerTransFunction(transpose_submit_32_32, transpose_submit_32_32_desc);
    //registerTransFunction(transpose_submit_64_64, transpose_submit_64_64_desc);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
