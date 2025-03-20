#include <stdlib.h>
#include "matmul.h"
#include <emmintrin.h>
#define BLOCKSIZE 64

void matmul(const int M, const int N, const int K, const int *inputA, const int *inputB, int *output) {
    for (int i = 0; i < M; i += BLOCKSIZE) {
        for (int k = 0; k < K; k += BLOCKSIZE) {
            for (int j = 0; j < N; j += BLOCKSIZE) {
                for (int ii = i; ii < i + BLOCKSIZE; ii++) {
                    for (int kk = k; kk < k + BLOCKSIZE; kk ++) {
                        int value = inputA[ii * K + kk];
                        __m128i a = _mm_set1_epi32(value);
                        for (int jj = j; jj < j + BLOCKSIZE; jj+=4) {
                            __m128i b = _mm_loadu_si128((const __m128i*)(inputB + kk * N + jj));

                            __m128i tmp1 = _mm_mul_epu32(a,b);
                            __m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(a,4), _mm_srli_si128(b,4));
                            __m128i product = _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (3,1,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (3,1,2,0)));
                            
                            __m128i current = _mm_loadu_si128((__m128i*)(output + ii * N + jj));
                            _mm_storeu_si128((__m128i*)(output + ii * N + jj), _mm_add_epi32(product, current));
                        }
                    }
                }
            }
        }
    }
}