// ===========================================================================
// Attacks against SPRITZ implementation
// =========================================================================
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

#ifndef _ATTACK_H_
#define _ATTACK_H_

#include <vector>
#include <list>

static int _debug = 0;

union input{
    uint8_t b[sizeof(int)];
    int i;
};

struct thread_data{
    int  thread_id;
    int  n;
    unsigned long long maxsize;
    int  start;
    int  end;
    std::vector<std::vector<uint8_t>>permutations;
};

//Helper Functions
void enableDebugOutput(int value);
int factorial(int n);
double max(double *array, int size);
double sum(double *array, int size);

//Attack-Functions
// Distribution stuff
int tryattack();
int distributionOfOutputValues(int q);  //count how often each value occur in the output stream
int distributionOfOutputValuesWithAbsorb(int q);
int distributionOfOutputValuesWithAbsorbAndShuffle(int q);
int pairsOfSuccessiveOutputValues(int q);
int seperateDistributionOfOutputValues(int q);  //observe first byte and count how often each value occurs
int distributionOfOutputValuesPerByte(int n, int position);
int testWhip(int q);
int testCrush(int q);
int testShuffle(int q);
int distributionOfRegisterI(int q);
int distributionOfRegisterJ(int q);
int distributionOfRegisterK(int q);
int distributionOfRegisterZ(int q);
int combinedDistributionOfIJK(int n);
int combinedDistributionOfIZ2Z(int n);
int combinedDistributionOfIZ3Z(int n);
int distantEquality(int n);	//other Spritz paper
// end distribution stuff

// correlation stuff
int correlationAfterAbsorb(int n);
int correlationAfterWhip(int n);
int correlationAfterWhipAndCrush(int n);
int correlationAfterShuffle(int n);
// end correlation stuff

//weak key stuff
int correlationBetweenFixedInputAndOutput(int n);
int correlationBetweenRandomInputAndOutput(int n);
int k1IsPrefixOfK2(int n);
int increasingInputIncreasingOutput(int n);
int decreasingInputDecreasingOutput(int n);
int wholeKeyWeakKeyPatterns(int n);
int alternatingWeakKeyPatterns(int n);
int rotationalKeyPatterns(int n);
int distributionWholeKeyWeakKeyPatterns(int n);
int distributionAlternatingWeakKeyPatterns(int n);
int distributionRotationalWeakKeyPatterns(int n);
int checkWeakKey(int n);
int mersenneTwisterRNG();
int aesCTRRNG();
//end weak key stuff

// collision attack stuff
int collisionAttack(int n);     // search for collisions in Crush
int collisionAttack2(int n);    // search for collisions in Whip
int collisionAttack3(int n);    // search for collisions in Whip with long random inputs
int collisionAttackN16();
int collisionAttackN24();
int collisionAttackN28();   // largest possible N for collisions in Absorb
int searchForCollidingInputs(int n);
//end collision attack stuff

//equal states stuff
int numOfStates(int n);
int numOfPermutationsAfterAbsorb(int n);
int checkEqualRegisters(int n);
int checkEqualRegistersAndPermutations(int n);
int checkEqualRegistersWithRandomPermutations(int n);
int checkEqualRegistersAndPermutationsWithRandomPermutations(int n);
//end equal states stuff

// sign of permutation stuff
signed int calculateSignOfPermutation(int n, uint8_t* S);
int signOfPermutationAfterAbsorb(int n);
int distributionOfIAndJAfterShuffle(int n);
// end sign of permutation stuff

// cycles stuff
//int searchForCycles(int n);
int findCyclesInUpdate(int n);
int findCyclesInDrip(int n);
void *searchForCycles(void *threadarg);
int findCyclesInDripThreaded(int n);
int verifyCycle(int n);
// end cycles stuff

// state rotations - rc4-hash
int partialStateRotations(int n);
int partialStateRotations2(int n);
// end state rotations stuff

// state recovery stuff
//typedef struct recoverState state;

struct state {
    int8_t **S;
    uint8_t *i;
    uint8_t *j;
    uint8_t *k;
    uint8_t *z;
    uint8_t w;
    std::vector<uint8_t> possibleEntries;
    int direction;
};

int cycleWhenNoSwapIsDone(int n);
int valuesForStateRecovery(int n);
int initializeRecovery(int n, int step, struct state *s);
int recoverState(int n);
int recoverState2(int n, int a);
//int recursiveRecoverState(int n, int step, struct state *s); //recursive function
//int recursiveRecoverState2(int n, int step, struct state *_s);	//use backjumping instead of  backtracking
int recursiveRecoverState3(int n, int step, struct state *s, std::vector<uint8_t>prevGuessedValues);
std::vector<uint8_t> simulateUpdateAndOutput(int n, int step, struct state *s, uint8_t* z_n, int* currentSibling, int *valueExists); //simulate spritz update+output
int guessPermutationEntry(int n, int step, struct state *s, int *currentSibling, int pos);
int printRecoveredState(int n, int step, struct state *s);
int reduceSearchSpace(struct state *s, std::vector<uint8_t>guessedValues, int step);  //first()
int checkIfValueAlreadyExistsInPermutation(uint8_t value, int n, int step, struct state *s);
int recoverInitialStateFromState(int n, int step, struct state *s);

//new approach
int recursiveRecoverState4(int n, int step, struct state *s, std::vector<uint8_t>guessedValues);
int recursiveRecoverState5(int n, int step, struct state *_s);
int copyState(int n, int step, struct state *destination, struct state *source);
int freeState(struct state *s);
int verifyRecoveredState(int n, int step, struct state *s);

//probabilistic approach
struct probState {
    double **S;
    uint8_t *i;
    double *j;
    double *k;
    uint8_t *z;
    uint8_t w;
};

int initializeProbabilisticState(struct probState *s, int n);
int probabilisticStateRecovery(int n);
int updateProbabilities(int n, int step, struct probState *s);
int checkSConverged(double **array, int size, double convergeThreshold);
// end state recovery stuff

//complexity calculation
int complex(int a, int n);
double c1(int a, int n);
double c2(int a, int n);
double c3(int a, int n);
double e(int a, int n);
double f(int a, int n);

// complexity for RC4 with own formulas
double complexRC4(int a, int n);
double c1RC4(int a, int n);
double c2RC4(int a, int n);
double c3RC4(int a, int n);

// complexity functions for Spritz
double complexSpritz(int a, int n);
long double c1Spritz(int a, int n);
long double c2Spritz(int a, int n);
long double c3Spritz(int a, int n);
long double c4Spritz(int a, int n);
long double c5Spritz(int a, int n);
long double c6Spritz(int a, int n);

static long double c1LookupTable[257] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 0};

static long double c2LookupTable[257] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 0};

static long double c3LookupTable[257] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 0};

static long double c4LookupTable[257] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 0};

static long double c5LookupTable[257] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 0};

static long double c6LookupTable[257] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 0};

//end complexity calculation

//Statistical Tests
long double chiSquaredStatistic(int q, int T, int* O);
double uniformDistribution(int x);
double correlationCoefficient(int n, uint8_t* X, uint8_t* Y);
int testcopystate();

//Test-Functions
int testStatisticFunctions();
int test_rotate();

static const uint8_t state_n8[8][8] = {
    {0, 1, 2, 3, 4, 5, 6, 7},
    {0, 1, 5, 3, 4, 2, 6, 7},
    {0, 3, 5, 1, 4, 2, 6, 7},
    {0, 3, 5, 1, 7, 2, 6, 4},
    {0, 3, 5, 2, 7, 1, 6, 4},
    {0, 6, 5, 2, 7, 1, 3, 4},
    {4, 6, 5, 2, 7, 1, 3, 0},
    {1, 6, 5, 2, 7, 4, 3, 0}
};
 
static const uint8_t state_n16[16][16] = {
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},	//0
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},	//1
    {0,1,5,3,4,2,6,7,8,9,10,11,12,13,14,15},	//2
    {0,3,5,1,4,2,6,7,8,9,10,11,12,13,14,15},	//3
    {0,3,5,1,15,2,6,7,8,9,10,11,12,13,14,4},	//4
    {0,3,5,2,15,1,6,7,8,9,10,11,12,13,14,4},	//5
    {0,3,5,2,15,1,4,7,8,9,10,11,12,13,14,6},	//6
    {0,3,5,2,7,1,4,15,8,9,10,11,12,13,14,6},	//7
    {0,3,8,2,7,1,4,15,5,9,10,11,12,13,14,6},	//8
    {0,3,8,2,7,1,4,15,5,14,10,11,12,13,9,6},	//9
    {0,3,8,2,7,1,4,15,5,14,6,11,12,13,9,10},	//10
    {11,3,8,2,7,1,4,15,5,14,6,0,12,13,9,10},	//11
    {11,12,8,2,7,1,4,15,5,14,6,0,3,13,9,10},	//12
    {11,12,8,2,7,1,4,15,5,14,6,0,3,13,9,10},	//13
    {11,12,9,2,7,1,4,15,5,14,6,0,3,13,8,10},	//14
    {11,12,9,2,7,1,4,10,5,14,6,0,3,13,8,15},	//15
};

static const uint8_t state_n32[32][32] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
    {0, 1, 5, 3, 4, 2, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
    {0, 1, 5, 17, 4, 2, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 3, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
    {0, 1, 5, 17, 18, 2, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 3, 4, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
    {0, 1, 5, 17, 18, 7, 6, 2, 8, 9, 10, 11, 12, 13, 14, 15, 16, 3, 4, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
    {0, 1, 5, 17, 18, 7, 12, 2, 8, 9, 10, 11, 6, 13, 14, 15, 16, 3, 4, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
    {0, 1, 5, 17, 18, 7, 12, 31, 8, 9, 10, 11, 6, 13, 14, 15, 16, 3, 4, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 17, 18, 7, 12, 31, 22, 9, 10, 11, 6, 13, 14, 15, 16, 3, 4, 19, 20, 21, 8, 23, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 17, 18, 7, 12, 31, 22, 23, 10, 11, 6, 13, 14, 15, 16, 3, 4, 19, 20, 21, 8, 9, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 17, 18, 7, 12, 31, 22, 23, 8, 11, 6, 13, 14, 15, 16, 3, 4, 19, 20, 21, 10, 9, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 17, 18, 7, 12, 31, 11, 23, 8, 22, 6, 13, 14, 15, 16, 3, 4, 19, 20, 21, 10, 9, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 17, 18, 7, 12, 31, 11, 23, 8, 22, 10, 13, 14, 15, 16, 3, 4, 19, 20, 21, 6, 9, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 17, 18, 7, 12, 31, 11, 23, 8, 22, 10, 15, 14, 13, 16, 3, 4, 19, 20, 21, 6, 9, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 17, 18, 7, 12, 31, 11, 23, 8, 22, 10, 15, 9, 13, 16, 3, 4, 19, 20, 21, 6, 14, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 13, 18, 7, 12, 31, 11, 23, 8, 22, 10, 15, 9, 17, 16, 3, 4, 19, 20, 21, 6, 14, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 13, 16, 7, 12, 31, 11, 23, 8, 22, 10, 15, 9, 17, 18, 3, 4, 19, 20, 21, 6, 14, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 13, 16, 7, 12, 31, 11, 23, 8, 22, 10, 15, 9, 17, 18, 4, 3, 19, 20, 21, 6, 14, 24, 25, 26, 27, 28, 29, 30, 2},
    {0, 1, 5, 13, 16, 7, 12, 31, 11, 23, 8, 22, 10, 15, 9, 17, 18, 4, 29, 19, 20, 21, 6, 14, 24, 25, 26, 27, 28, 3, 30, 2},
    {0, 1, 5, 13, 16, 7, 12, 31, 11, 19, 8, 22, 10, 15, 9, 17, 18, 4, 29, 23, 20, 21, 6, 14, 24, 25, 26, 27, 28, 3, 30, 2},
    {0, 1, 5, 13, 20, 7, 12, 31, 11, 19, 8, 22, 10, 15, 9, 17, 18, 4, 29, 23, 16, 21, 6, 14, 24, 25, 26, 27, 28, 3, 30, 2},
    {0, 1, 5, 13, 20, 7, 12, 31, 11, 19, 8, 22, 10, 15, 9, 17, 18, 4, 29, 23, 16, 30, 6, 14, 24, 25, 26, 27, 28, 3, 21, 2},
    {0, 1, 5, 13, 20, 7, 12, 31, 11, 19, 8, 22, 6, 15, 9, 17, 18, 4, 29, 23, 16, 30, 10, 14, 24, 25, 26, 27, 28, 3, 21, 2},
    {0, 1, 5, 13, 20, 7, 12, 31, 11, 19, 8, 22, 6, 15, 9, 17, 18, 4, 14, 23, 16, 30, 10, 29, 24, 25, 26, 27, 28, 3, 21, 2},
    {0, 1, 5, 13, 20, 7, 12, 31, 11, 19, 8, 22, 6, 15, 9, 17, 18, 4, 14, 23, 24, 30, 10, 29, 16, 25, 26, 27, 28, 3, 21, 2},
    {0, 1, 5, 25, 20, 7, 12, 31, 11, 19, 8, 22, 6, 15, 9, 17, 18, 4, 14, 23, 24, 30, 10, 29, 16, 13, 26, 27, 28, 3, 21, 2},
    {0, 1, 5, 25, 20, 7, 12, 31, 11, 19, 8, 22, 6, 15, 9, 17, 18, 4, 14, 23, 24, 30, 10, 29, 16, 13, 3, 27, 28, 26, 21, 2},
    {0, 1, 5, 25, 20, 7, 12, 27, 11, 19, 8, 22, 6, 15, 9, 17, 18, 4, 14, 23, 24, 30, 10, 29, 16, 13, 3, 31, 28, 26, 21, 2},
    {0, 1, 5, 25, 20, 7, 12, 27, 11, 19, 28, 22, 6, 15, 9, 17, 18, 4, 14, 23, 24, 30, 10, 29, 16, 13, 3, 31, 8, 26, 21, 2},
    {0, 1, 5, 25, 20, 7, 12, 27, 11, 26, 28, 22, 6, 15, 9, 17, 18, 4, 14, 23, 24, 30, 10, 29, 16, 13, 3, 31, 8, 19, 21, 2},
    {0, 1, 5, 25, 20, 7, 12, 27, 11, 26, 28, 22, 6, 15, 9, 17, 18, 4, 14, 23, 24, 30, 10, 29, 16, 13, 21, 31, 8, 19, 3, 2},
    {0, 1, 5, 25, 20, 7, 12, 27, 11, 26, 28, 22, 6, 15, 2, 17, 18, 4, 14, 23, 24, 30, 10, 29, 16, 13, 21, 31, 8, 19, 3, 9},
    {18, 1, 5, 25, 20, 7, 12, 27, 11, 26, 28, 22, 6, 15, 2, 17, 0, 4, 14, 23, 24, 30, 10, 29, 16, 13, 21, 31, 8, 19, 3, 9}
};

#endif
