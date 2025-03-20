/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/*
 * onebitParity - returns 1 if x contains an odd number of 0's
 *   Examples: onebitParity(5) = 0, onebitParity(7) = 1
 *   Legal ops: & ^ << >>
 *   Max ops: 20
 *   Rating: 1
 */
int onebitParity(int x) {
    x = ( x >> 16 ) ^ x;
    x = ( x >> 8 ) ^ x;
    x = ( x >> 4 ) ^ x;
    x = ( x >> 2 ) ^ x;
    x = ( x >> 1 ) ^ x;

    return (x & 1);
}
/* 
 * checkSubstraction - Determine if can compute x-y without overflow
 *   Example: checkSubstraction(0x80000000,0x80000000) = 1,
 *            checkSubstraction(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 1
 */
int checkSubstraction(int x, int y) {
    int minusY = ~y + 1;
    int diff = x + minusY;
    int signX = x >> 31;
    int signY = y >> 31;
    int signDiff = diff >> 31;
    return (!(signX ^ signY) | !(signX ^ signDiff));
}
/*
 * twoscom2SignedVal - Convert from two's complement to signed-magnitude
 *   where the MSB is the sign bit
 *   You can assume that x > TMin
 *   Example: twoscom2SignedVal(-5) = 0x80000005.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 1
 */
int twoscom2SignedVal(int x) {
    int mask = x >> 31;

    return (mask << 31) | ((x + mask) ^ mask);

}
/*
 * nibbleReverse - Reverse nibbles(4bits) in a 32-bit word
 *   Examples: nibbleReverse(0x80000002) = 0x20000008
 *             nibbleReverse(0x89ABCDEF) = 0xFEDCBA98
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 1
 */
int nibbleReverse(int x) {
    int a = (0x0F << 8) + 0x0F; // a = 0x0F0F
    int a1 = (a << 16) + a; // a1 = 0x0F0F0F0F
    int b = (0xFF << 16) + 0xFF; // b = 0x00FF00FF
    int c = (0xFF << 8) + 0xFF; // c = 0x0000FFFF

    x = (a1 & (x >> 4)) | ((a1 & x) << 4);
    x = (b & (x >> 8)) | ((b & x) << 8);
    
    return ((c & (x >> 16)) | (x << 16));
}
/* 
 * bitFilter - Generate a mask consisting of all 1's and filter input with it.
 * Examples: bitFilter(0xFF00, 11, 4) = 0x0F00,
 * bitFilter(0x2A00, 13, 9) = 0x2A00,
 * bitFilter(0x1300, 4, 2) = 0
 * Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 * If lowbit > highbit, then mask should be all 0's
 *   Legal ops: & | << >>
 *   Max ops: 20
 *   Rating: 1
 */
int bitFilter(int input, int highbit, int lowbit) {
    int highMask = (1 << highbit); // 1 at the highbit
    int one = (0xFF << 8) | 0xFF; // one = 0x0000FFFF
    int allOnes = (one << 16) | one; // allOnes = 0xFFFFFFFF
    int lowMask = allOnes << lowbit; // lowMask contains 1 from bit 31 to lowbit

    // copy 1 to all the bits lower than highbit
    highMask |= highMask >> 16;
    highMask |= highMask >> 8;
    highMask |= highMask >> 4;
    highMask |= highMask >> 2;
    highMask |= highMask >> 1;
    // now highMask contains 1 from bit 0 to highbit.

    return input & (highMask & lowMask);
}
/*
 * addAndDivideBy4 - adds two numbers and divide by 4 (round toward 0). But when overflow occurs
 *          while adding two numbers, returns the first operand.
 *   Examples: addAndDivideBy4(1073741824,1073741824) = 1073741824
 *             addAndDivideBy4(-2147483648,-1) = -2147483648
 *             addAndDivideBy4(32,9) = 10
 *             addAndDivideBy4(-22,9) = -3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 1
 */
int addAndDivideBy4(int x, int y) {
    int sum = x + y;
    int signX = x >> 31;
    int signY = y >> 31;
    int signSum = (sum >> 31) & 1;
    int XYSame = ((signX ^ signY) ^ 1);
    int XSumDifferent = (signX ^ signSum);
    int overflow = ((XYSame & XSumDifferent) << 31) >> 31; // overflow: 0xFFFFFFFF, not overflow: 0x0
    int mask = (signSum << 1) + signSum; //x negative : 3, positive: 0

    return (overflow & x) | (~overflow & ((sum+mask) >> 2));
    // if x and y have the different sign, then x+y will not overflow
    // if x and y have same sign, then x+y will overflow if x and sum have different signs
}
/* 
 * numZerosFirst - returns count of number of continuous 0's from first bits
 *   Example: numZerosFirst(0) = 32
 *   Example: numZerosFirst(0x80000000) = 0
 *   Example: numZerosFirst(0x40000000) = 1
 *   Example: numZerosFirst(0x00008000) = 16
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Rating: 1 
 */
int numZerosFirst(int x) {
    int five2 = 0x55;
    int five4 = (five2 << 8) + five2;
    int mask5 = (five4 << 16) + five4; // 0x55555555
    int three2 = 0x33;
    int three4 = (three2 << 8) + three2;
    int mask3 = (three4 << 16) + three4; // 0x33333333
    int f = (0x0F << 8) + 0x0F;
    int shift4Mask = (f << 16) + f; // 0x0F0F0F0F
    int shift24Mask = (0x0F << 24); // 0x0F000000

    x |= x >> 16;
    x |= x >> 8;
    x |= x >> 4;
    x |= x >> 2;
    x |= x >> 1;
    x = ~x;

    x = x + (~((x >> 1) & mask5) + 1); // x = x - ((x >> 1) & 0x55555555);
    x = (x & mask3) + ((x >> 2) & mask3); // x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = ((x + (x >> 4)) & shift4Mask); // ((x + (x >> 4)) & 0x0F0F0F0F);
    x = x + ((x << 8) & shift24Mask);
    x = x + ((x << 16) & shift24Mask);
    x = x + ((x << 24) & shift24Mask);

    return x >> 24;
}
/*
 * absFloat - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 1
 */
unsigned absFloat(unsigned uf) {
    unsigned exponent = uf & 0x7F800000;
    unsigned fraction = uf & 0x007FFFFF;

    if (exponent == 0x7F800000 && fraction != 0) // NaN
        return uf;

    return uf & 0x7FFFFFFF;
}
/* 
 * castFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 1
 */
int castFloat2Int(unsigned uf) {
    unsigned sign = uf & 0x80000000;
    unsigned exponent = (uf >> 23) & 0xFF;
    unsigned fraction = uf & 0x007FFFFF;

    if(exponent < 127) // uf < 1
        return 0;

    if(exponent >=158) // out of range
        return 0x80000000u;

    fraction = fraction | (1 << 23); // fraction = (1 + fraction)

    // (1 + fraction) * 2^(exponent - 127)
    // 1 is at bit 23
    // shift right 127 times, shift right 23 times to make 1 at bit 0, and then shift left (exponent) times
    // so shift left (exponent - 150) times if exponent > 150
    // and shift right (150 - exponent) times if exponent < 150
    if(exponent >150)
        fraction <<= (exponent - 150);
    else
        fraction >>= (150 - exponent);

    if(sign)
        return ~fraction + 1; // two's complement

    return fraction;
}
/* 
 * compareFloat - Compute f < g for floating point arguments f and g.
 *   Both the arguments are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   If either argument is NaN, return 0.
 *   +0 and -0 are considered equal.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 1
 */
int compareFloat(unsigned uf, unsigned ug) {
    unsigned exponentUF = uf & 0x7F800000;
    unsigned fractionUF = uf & 0x007FFFFF;
    int signUF = ((uf & 0x80000000) >> 31) & 1;
    unsigned exponentUG = ug & 0x7F800000;
    unsigned fractionUG = ug & 0x007FFFFF;
    unsigned signUG = ((ug & 0x80000000) >> 31) & 1;


    if (exponentUF == 0x7F800000 && fractionUF != 0) // NaN
        return 0;

    if (exponentUG == 0x7F800000 && fractionUG != 0) // NaN
        return 0;

    if (exponentUF == 0 && fractionUF == 0 && exponentUG == 0 && fractionUG == 0) // +0 and -0 are considered equal
        return 0;

    if (signUF != signUG) // different sign
        return signUF; // if uf is negative, return 1, else return 0

    return (!signUF && uf < ug) || (signUF && uf > ug);
}
