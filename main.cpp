// Copyright (c) 2014 Ralph Ankele

// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


#include "spritz.h"
#include "spritzClass.h"
#include "attack.h"
#include <iostream>
#include <string>

int main(int argc, char** argv){

    setN(256);
    // test spritz & statistic functions
    if(testvectors() != 0)
        printf("Testvectors for Spritz failed!\n");
    else
        printf("Spritz Tests ok!\n");
    
   /* spritzClass *spritz = new spritzClass();
    spritz->setN(256);
    if(spritz->testvectors() != 0)
        printf("Testvectors for spritzClass failed!\n");
    else
        printf("spritzClass Tests ok!\n");
    delete spritz;
    
    if(testStatisticFunctions() != 0)
        printf("Statistic Testvectors failed!\n");
    else
        printf("Statistic Tests ok!\n");
    */
     
    // attack cipher
    //int N_sizes[6] = {16, 24, 32, 64, 128, 256};
    
    //for (unsigned int v = 0; v < 6; v++) {
       /* printf("current N: %d\n", N_sizes[v]);
        enableDebugOutput(1);
        setN(N_sizes[v]);*/
        //distributionOfOutputValues(N_sizes[v]);
        //distributionOfOutputValuesWithAbsorb(N_sizes[v]);
        //distributionOfOutputValuesWithAbsorbAndShuffle(N_sizes[v]);
        //pairsOfSuccessiveOutputValues(N_sizes[v]);
        //seperateDistributionOfOutputValues(N_sizes[v]);
        //distributionOfOutputValuesPerByte(N_sizes[v], 255);
        //testShuffle(N_sizes[v]);
        //testWhip(N_sizes[v]);
        //distributionOfRegisterI(N_sizes[v]);
        //distributionOfRegisterJ(N_sizes[v]);
        //distributionOfRegisterK(N_sizes[v]);
        //distributionOfRegisterZ(N_sizes[v]);
        //correlationAfterAbsorb(N_sizes[v]);
        //correlationAfterWhip(N_sizes[v]);
        //correlationAfterWhipAndCrush(N_sizes[v]);
        //correlationAfterShuffle(N_sizes[v]);
        //correlationBetweenFixedInputAndOutput(N_sizes[v]);
        //collisionAttack(N_sizes[v]);
   // }
    
    // Manual Tests
	//setN(64);

    //recoverState2(n, a), n = permutation size, a = number of pre-assigned values
    //while(recoverState2(8, 5) == -1);
    //printf("retval: %d\n", recoverState2(64, 63));
    
    //recoverStatePatternApproach(8, 7);
    //distantEquality(16);
    //probabilisticStateRecovery(3);
    complexSpritz(0, 16);
    //complexRC4(0, 8);
    //complex(0, 8);
    //testcopystate();
    //aesCTRRNG();
    //mersenneTwisterRNG();
    //valuesForStateRecovery(64);
    //recoverState(32);
    //cycleWhenNoSwapIsDone(256);
    //numOfPermutationsAfterAbsorb(5);	currently not working - fix it
    //tryattack();
    //enableDebugOutput(1);
    //checkWeakKey(256);
    //rotationalKeyPatterns(256);
    //wholeKeyWeakKeyPatterns(16);
    //alternatingWeakKeyPatterns(256);
    //distributionWholeKeyWeakKeyPatterns(128);
    //distributionAlternatingWeakKeyPatterns(16);
    //distributionRotationalWeakKeyPatterns(16);
    //k1IsPrefixOfK2(18);
    //increasingInputIncreasingOutput(256);
    //decreasingInputDecreasingOutput(16);
    //verifyCycle(8);
    //findCyclesInDripThreaded(32);
    //findCyclesInUpdate(256);
    //findCyclesInDrip(6);
    //checkEqualRegisters(4);
    //checkEqualRegistersAndPermutations(10);
    //checkEqualRegistersAndPermutationsWithRandomPermutations(256);
    //checkEqualRegistersWithRandomPermutations(256);
    //collisionAttack3(16);
    //correlationBetweenRandomInputAndOutput(256);
    //searchForCycles(5);
    //collisionAttack2(48);
    //searchForCollidingInputs(8);
    //signOfPermutationAfterAbsorb(28);
    //distributionOfIAndJAfterShuffle(256);
    //numOfStates(5);
    //combinedDistributionOfIJK(16);
    //combinedDistributionOfIZ2Z(16);
    //combinedDistributionOfIZ3Z(16);
    //distributionOfOutputValuesWithAbsorb(16);
    //seperateDistributionOfOutputValues(16);
    //collisionAttack(28);
    //collisionAttackN16();
    //collisionAttackN24();
    //collisionAttackN28();
    //correlationAfterAbsorb(240);
    //pairsOfSuccessiveOutputValues(16);
    //partialStateRotations(256);
    //partialStateRotations2(256);
    //distributionOfOutputValues(8);
    //distributionOfOutputValuesWithAbsorb(8);
    //distributionOfOutputValuesWithAbsorbAndShuffle(8);	//bias with N = 8
    //pairsOfSuccessiveOutputValues(8);
    //seperateDistributionOfOutputValues(8);	//bias with N = 8
    //distributionOfOutputValuesPerByte(16, 2);
    //testShuffle(N_sizes[v]);
    //testWhip(N_sizes[v]);
    //distributionOfRegisterI(8);
    //distributionOfRegisterJ(8);
    //distributionOfRegisterK(8);
    //distributionOfRegisterZ(8);
    //correlationAfterAbsorb(N_sizes[v]);
    //correlationAfterWhip(N_sizes[v]);
    //correlationAfterWhipAndCrush(N_sizes[v]);
    //correlationAfterShuffle(N_sizes[v]);
    //correlationBetweenFixedInputAndOutput(N_sizes[v]);
    //collisionAttack(N_sizes[v]);
    //uint8_t S[16] = {16,15,14,13,12,11,10,9,8,7,6,5,4,3,1,2};
    //printf("sign: %d\n",calculateSignOfPermutation(16, S));
    //test_rotate();
    
    return 0;
}
