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
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <map>
#include <array>
#include <utility>
#include <algorithm>
#include <pthread.h>
#include <thread>
#include <chrono>
#include <random>

#define NUM_THREADS     4

//Helper-Functions
void enableDebugOutput(int value){
    _debug = value;
}

int factorial(int n)
{
    return (n == 0 || n == 1) ? 1 : factorial(n - 1) * n;
}

double max(double *array, int size){
    double maxElement = 0;
    for (unsigned int i = 0; i < size; i++) {
        if (array[i] > maxElement) {
            maxElement = array[i];
        }
    }
    return maxElement;
}

double sum(double *array, int size){
    double sum = 0.0f;
    for (unsigned int i = 0; i < size; i++) {
        sum += array[i];
    }
    return sum;
}

//Attack-Functions
int tryattack(){
    
    printf("\n Try some attacks...\n");
    //uint8_t m1[4] = {0x09, 0x00, 0x00, 0x00};
    uint8_t m1[129] = {0x00};
    memset(m1, 0x01, 64);
    InitializeState();
    //debug();
    Absorb(m1, 129);
    debug();
    for (int i = 0; i < 10; i++) {
        //debug();
        printf("%02x ", Drip());
    }
    printf("\n");
    
    printf("*******************************\n");
    
    uint8_t m2[129] = {0x00};
    memset(m2+64, 0x01, 64);
    InitializeState();
    //debug();
    Absorb(m2, 129);
    debug();
    for (int i = 0; i < 10; i++) {
        //debug();
        printf("%02x ", Drip());
    }
    printf("\n");
    
    /* uint8_t m3[64] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
     
     InitializeState();
     Absorb(m3, 64);
     debug();
     uint8_t output3[32] = {0x00};
     for (int i = 0; i < 1; i++) {
     output3[i] = Drip();
     //debug();
     printf("%02x ", output3[i]);
     }
     printf("\n");*/
    
    return 0;
}

int distributionOfOutputValues(int q){
    int maxsize = (int)pow(2.0f, 20.0f);
    int O[q];
    memset(O, 0, q*sizeof(int));
    
    InitializeState();
    for (unsigned int i = 0; i < maxsize; i++) {
        O[Drip()]++;
    }
    
    if (_debug){
        for (unsigned int i = 0; i< q; i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(q, maxsize, O);
    double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int distributionOfOutputValuesWithAbsorb(int q){
    int maxsize = (int)pow(2.0f, 20.0f);
    int O[q];
    memset(O, 0, q*sizeof(int));
    srand (time(NULL));
    
    InitializeState();
    //uint8_t input[3] = {0x00, 0x01, 0x02};
    for (unsigned int i = 0; i < maxsize; i++) {
        uint8_t key[q/4];
        for (int k = 0; k<q/4; k++) {
            memset(key+k, static_cast<uint8_t>(rand()%256), 1);
        }
        Absorb(key, q/4);
        
        //printf("drip: %d\n", Drip());
        O[Drip()]++;
    }
    
    if (_debug) {
        for (unsigned int i = 0; i< q; i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(q, maxsize, O);
    double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}


int distributionOfOutputValuesWithAbsorbAndShuffle(int q){
    int maxsize = (int)pow(2.0f, 20.0f);
    int O[q];
    memset(O, 0, q*sizeof(int));
    
    InitializeState();
    uint8_t input[(q/2)+1];     // N/2 + 1 --> Shuffle gets called in Absorb
    memset(input, 0x00, ((q/2)+1)*sizeof(uint8_t));
    
    for (unsigned int i = 0; i < maxsize; i++) {
        Absorb(input, ((q/2)+1));
        //printf("drip: %d\n", Drip());
        O[Drip()]++;
    }
    
    if (_debug) {
        for (unsigned int i = 0; i< q; i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(q, maxsize, O);
    double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int pairsOfSuccessiveOutputValues(int q){
    int maxsize = (int)pow(2.0f, 20.0f);
    int O[(int)pow(q, 2.0f)];   //q=N^2 -->successive output pairs
    memset(O, 0x00, (int)pow(q, 2.0f)*sizeof(int));
    //int O[q];
    //memset(O, 0x00, q);
    std::map<std::pair<uint8_t,uint8_t>, int>observations;
    
    for (unsigned int i = 0; i< q; i++) {
        for (unsigned int j = 0; j< q; j++) {
            observations.insert(std::make_pair(std::make_pair(i, j), 0));
        }
    }
    
    
    InitializeState();
    for (unsigned int i = 0; i < maxsize; i++) {
        //printf("drip: %d\n", Drip());
        uint8_t bit1 = Drip();
        uint8_t bit2 = Drip();
        //O[bit1*bit2]++;
        observations.at(std::make_pair(bit1, bit2))++;
    }
    
    unsigned int cnt = 0;
    for (std::map<std::pair<uint8_t,uint8_t>, int>::iterator it=observations.begin(); it!=observations.end(); ++it) {
        O[cnt++] = it->second;
    }
    
    if (_debug) {
        /*for (std::map<std::pair<uint8_t,uint8_t>, int>::iterator it=observations.begin(); it!=observations.end(); ++it) {
         printf("%d\n", it->second);
         }
         printf("\n *****");*/
        
        /*for (unsigned int i = 0; i< (int)pow(q, 2.0f); i++) {
         if (O[i] != 0) {
         printf("O[%d] = %d\n", i, O[i]);
         }
         }*/
    }
    
    double chiSquare = chiSquaredStatistic((int)pow(q, 2.0f), maxsize, O);
    double degreeOfFreedom = pow(q-1.0f, 2.0f);
    //double chiSquare = chiSquaredStatistic(q, maxsize, O);
    //double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int seperateDistributionOfOutputValues(int q){
    int maxsize = (int)pow(2.0f, 20.0f);
    int O[q];
    memset(O, 0, q*sizeof(int));
    srand (time(NULL));
    
    for (unsigned int i = 0; i < maxsize; i++) {
        //Keysetup
        InitializeState();
        uint8_t key[q/4];
        for (int k = 0; k<q/4; k++) {
            memset(key+k, static_cast<uint8_t>(rand()%256), 1);
        }
        Absorb(key, q/4);
        
        //encrypt
        O[Drip()]++;
    }
    
    if (_debug){
        for (unsigned int i = 0; i< q; i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(q, maxsize, O);
    double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int distributionOfOutputValuesPerByte(int n, int position){
    int maxsize = (int)pow(2.0f, 20.0f);
    int O[n];
    memset(O, 0x00, n*sizeof(int));
    srand (time(NULL));
    
    for (unsigned int i = 0; i < maxsize; i++) {
        //Keysetup
        InitializeState();
        uint8_t key[n/4];
        for (int k = 0; k<n/4; k++) {
            memset(key+k, static_cast<uint8_t>(rand()%256), 1);
        }
        Absorb(key, n/4);
        
        /*for (unsigned int k = 0; k< n/4; k++) {
         printf("key[%d] = %d\n", k, key[k]);
         }
         printf("\n");*/
        
        for (unsigned int j = 0; j < position; j++) {
            Drip();
        }
        
        //encrypt
        O[Drip()]++;
    }
    
    if (_debug){
        for (unsigned int i = 0; i< n; i++) {
            printf("%d;%d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(n, maxsize, O);
    double degreeOfFreedom = n-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int testWhip(int q){
    int maxsize = 2*q;
    int O[q];
    memset(O, 0, q*sizeof(int));
    
    InitializeState();
    
    for (unsigned int i = 0; i < maxsize; i++) {
        Whip(i);
        O[getCipher().S[0]]++;
    }
    
    if (_debug) {
        for (unsigned int i = 0; i< q; i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(q, maxsize, O);
    double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int testCrush(int q){
    return 0;
}

int testShuffle(int q){
    int maxsize = (int)pow(2.0f, 22.0f);
    int O[q];
    memset(O, 0, q*sizeof(int));
    
    InitializeState();
    
    for (unsigned int i = 0; i < maxsize; i++) {
        Shuffle();
        O[Drip()]++;
    }
    
    if (_debug) {
        for (unsigned int i = 0; i< q; i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(q, maxsize, O);
    double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int distributionOfRegisterI(int q){
    int maxsize = (int)pow(2.0f, 20.0f);
    int O[q];
    memset(O, 0, q*sizeof(int));
    
    InitializeState();
    for (unsigned int i = 0; i < maxsize; i++) {
        Update();
        O[getCipher().i]++;
    }
    
    if (_debug){
        for (unsigned int i = 0; i< q; i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(q, maxsize, O);
    double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int distributionOfRegisterJ(int q){
    int maxsize = (int)pow(2.0f, 20.0f);
    int O[q];
    memset(O, 0, q*sizeof(int));
    
    InitializeState();
    for (unsigned int i = 0; i < maxsize; i++) {
        Update();
        O[getCipher().j]++;
    }
    
    if (_debug){
        for (unsigned int i = 0; i< q; i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(q, maxsize, O);
    double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int distributionOfRegisterK(int q){
    int maxsize = (int)pow(2.0f, 20.0f);
    int O[q];
    memset(O, 0, q*sizeof(int));
    
    InitializeState();
    for (unsigned int i = 0; i < maxsize; i++) {
        Update();
        O[getCipher().k]++;
    }
    
    if (_debug){
        for (unsigned int i = 0; i< q; i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(q, maxsize, O);
    double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int distributionOfRegisterZ(int q){
    int maxsize = (int)pow(2.0f, 20.0f);
    int O[q];
    memset(O, 0, q*sizeof(int));
    
    InitializeState();
    for (unsigned int i = 0; i < maxsize; i++) {
        Update();
        O[Output()]++;
    }
    
    if (_debug){
        for (unsigned int i = 0; i< q; i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(q, maxsize, O);
    double degreeOfFreedom = q-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int combinedDistributionOfIJK(int n){
    int maxsize = (int)pow(2.0f, 30.0f);
    int *O = new int[(int)pow(n, 3.0f)];   //q=N^3 -->distribution of i,j,k
    //int O[(int)pow(n, 3.0f)];   //q=N^3 -->distribution of i,j,k
    memset(O, 0x00, ((int)pow(n, 3.0f))*4);
    
    std::map<std::array<uint8_t, 3>, int>observations;
    for (unsigned int i = 0; i< n; i++) {
        for (unsigned int j = 0; j< n; j++) {
            for (unsigned int k = 0; k < n; k++) {
                std::array<uint8_t, 3>tripplet = {{static_cast<uint8_t>(i),static_cast<uint8_t>(j),static_cast<uint8_t>(k)}};
                observations.insert(std::make_pair(tripplet, 0));
            }
        }
    }
    
    //InitializeState();
    // random starting state
    srand (time(NULL));
    cipher c;
    c.i = 0;
    c.j = 0;
    c.k = 0;
    c.z = 0;
    c.a = 0;
    c.w = 1;
    c.S = (uint8_t*) malloc (n*sizeof(uint8_t));
    for (unsigned int v = 0; v <= n-1; v++) {
        c.S[v] = static_cast<uint8_t>(rand()%n);
    }
    
    // observation
    for (unsigned int i = 0; i < maxsize; i++) {
        Update();
        std::array<uint8_t, 3>tripplet = {{static_cast<uint8_t>(getCipher().i),static_cast<uint8_t>(getCipher().j),static_cast<uint8_t>(getCipher().k)}};
        observations.at(tripplet)++;
    }
    
    /*for (unsigned int i = 0; i< n; i++) {
     for (unsigned int j = 0; j< n; j++) {
     for (unsigned int k = 0; k < n; k++) {
     std::array<uint8_t, 3>tripplet = {static_cast<uint8_t>(i),static_cast<uint8_t>(j),static_cast<uint8_t>(k)};
     printf("value at i:%d, j:%d, k:%d = %d\n",i,j,k,observations.at(tripplet));
     }
     }
     }*/
    
    unsigned int cnt = 0;
    for (std::map<std::array<uint8_t, 3>, int>::iterator it=observations.begin(); it!=observations.end(); ++it) {
        O[cnt++] = it->second;
    }
    
    if (_debug){
        for (unsigned int i = 0; i< (int)pow(n, 3.0f); i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic((int)pow(n, 3.0f), maxsize, O);
    double degreeOfFreedom = pow(n-1.0f, 3.0f);
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int combinedDistributionOfIZ2Z(int n){
    int maxsize = (int)pow(2.0f, 10.0f);
    int *O = new int[(int)pow(n, 3.0f)];   //q=N^3 -->distribution of i,j,k
    memset(O, 0x00, ((int)pow(n, 3.0f))*4);
    
    std::map<std::array<uint8_t, 3>, int>observations;
    for (unsigned int i = 0; i< n; i++) {
        for (unsigned int z2 = 0; z2< n; z2++) {
            for (unsigned int z = 0; z < n; z++) {
                std::array<uint8_t, 3>tripplet = {{static_cast<uint8_t>(i),static_cast<uint8_t>(z2),static_cast<uint8_t>(z)}};
                observations.insert(std::make_pair(tripplet, 0));
            }
        }
    }
    
    /*for (unsigned int i = 0; i< n; i++) {
     for (unsigned int j = 0; j< n; j++) {
     for (unsigned int k = 0; k < n; k++) {
     std::array<uint8_t, 3>tripplet = {static_cast<uint8_t>(i),static_cast<uint8_t>(j),static_cast<uint8_t>(k)};
     printf("value at i:%d, j:%d, k:%d = %d\n",i,j,k,observations.at(tripplet));
     }
     }
     }*/
    
    std::vector<uint8_t> zValues;
    
    // 2^18 random starting state
    srand (time(NULL));
    cipher c;
    for (unsigned int i = 0; i < maxsize; i++) {
        zValues.push_back(0);
        zValues.push_back(0);
        
        c.i = 0;
        c.j = 0;
        c.k = 0;
        c.z = 0;
        c.a = 0;
        c.w = 1;
        c.S = (uint8_t*) malloc (n*sizeof(uint8_t));
        for (unsigned int v = 0; v <= n-1; v++) {
            c.S[v] = static_cast<uint8_t>(rand()%n);
        }
        setCipher(c);
        setN(n);
        
        // observation
        for (unsigned int j = 0; j < maxsize; j++) {
            Drip();
            std::array<uint8_t, 3>tripplet = {{static_cast<uint8_t>(getCipher().i), zValues.at(j),static_cast<uint8_t>(getCipher().z)}};
            
            // printf("i:%d, z2:%d, z:%d\n", getCipher().i, zValues[j], getCipher().z);
            
            observations.at(tripplet)++;
            zValues.push_back(getCipher().z);
        }
        zValues.clear();
    }
    
    unsigned int cnt = 0;
    for (std::map<std::array<uint8_t, 3>, int>::iterator it=observations.begin(); it!=observations.end(); ++it) {
        O[cnt++] = it->second;
    }
    
    if (_debug){
        for (unsigned int i = 0; i< (int)pow(n, 3.0f); i++) {
            printf("O[%d] = %d\n", i, O[i]);
        }
    }
    
    long double chiSquare = chiSquaredStatistic((int)pow(n, 3.0f), maxsize, O);
    double degreeOfFreedom = pow(n, 3.0f)-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %Lf\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int combinedDistributionOfIZ3Z(int n){
    int maxsize = (int)pow(2.0f, 10.0f);
    int *O = new int[(int)pow(n, 3.0f)];   //q=N^3 -->distribution of i,j,k
    memset(O, 0x00, ((int)pow(n, 3.0f))*4);
    
    std::map<std::array<uint8_t, 3>, int>observations;
    for (unsigned int i = 0; i< n; i++) {
        for (unsigned int z3 = 0; z3< n; z3++) {
            for (unsigned int z = 0; z < n; z++) {
                std::array<uint8_t, 3>tripplet = {{static_cast<uint8_t>(i),static_cast<uint8_t>(z3),static_cast<uint8_t>(z)}};
                observations.insert(std::make_pair(tripplet, 0));
            }
        }
    }
    
    /*for (unsigned int i = 0; i< n; i++) {
     for (unsigned int j = 0; j< n; j++) {
     for (unsigned int k = 0; k < n; k++) {
     std::array<uint8_t, 3>tripplet = {static_cast<uint8_t>(i),static_cast<uint8_t>(j),static_cast<uint8_t>(k)};
     printf("value at i:%d, j:%d, k:%d = %d\n",i,j,k,observations.at(tripplet));
     }
     }
     }*/
    
    std::vector<uint8_t> zValues;
    
    // 2^18 random starting state
    srand (time(NULL));
    cipher c;
    for (unsigned int i = 0; i < maxsize; i++) {
        zValues.push_back(0);
        zValues.push_back(0);
        zValues.push_back(0);
        
        c.i = 0;
        c.j = 0;
        c.k = 0;
        c.z = 0;
        c.a = 0;
        c.w = 1;
        c.S = (uint8_t*) malloc (n*sizeof(uint8_t));
        for (unsigned int v = 0; v <= n-1; v++) {
            c.S[v] = static_cast<uint8_t>(rand()%n);
        }
        setCipher(c);
        setN(n);
        
        // observation
        for (unsigned int j = 0; j < maxsize; j++) {
            Drip();
            //zValues[j] = zValues[-3]
            std::array<uint8_t, 3>tripplet = {{static_cast<uint8_t>(getCipher().i), zValues.at(j),static_cast<uint8_t>(getCipher().z)}};
            
            // printf("i:%d, z2:%d, z:%d\n", getCipher().i, zValues[j], getCipher().z);
            
            observations.at(tripplet)++;
            zValues.push_back(getCipher().z);
        }
        zValues.clear();
    }
    
    unsigned int cnt = 0;
    for (std::map<std::array<uint8_t, 3>, int>::iterator it=observations.begin(); it!=observations.end(); ++it) {
        O[cnt++] = it->second;
    }
    
    if (_debug){
        for (unsigned int i = 0; i< (int)pow(n, 3.0f); i++) {
            printf("%d;%d\n", i, O[i]);
        }
    }
    
    long double chiSquare = chiSquaredStatistic((int)pow(n, 3.0f), maxsize, O);
    double degreeOfFreedom = pow(n, 3.0f)-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %Lf\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected! *** \n");
        return -1;
    }
    
    return 0;
}

int distantEquality(int n){
    int maxsize = (int)pow(2.0f, 22.0f/*38.0f*/);
    int keyLen = 16;
    int distance[8] = {1,2,3,4,5,6,7,8};
    srand(time(NULL));
    
    InitializeState();
    uint8_t key[keyLen];
    for (unsigned int j = 0; j < keyLen; j++) {
        memset(key+j, rand()%256, 1);
    }
    Absorb(key, keyLen);
    
    for (unsigned int d = 1; d < 2/*8*/; d++) {
        int count = 0;
        std::vector<uint8_t>z;
        for (unsigned int j = 0; j < maxsize+distance[d]/*distance[d]+1*/; j++) {
            z.push_back(Drip());
        }
        
        std::vector<uint8_t>::iterator it1 = z.begin();
        std::vector<uint8_t>::iterator it2 = z.begin()+distance[d];
        for (unsigned int i = 0/*distance[d]*/; i < maxsize; i++) {
            //printf("p1 = %d, p2 = %d\n", *it1, *it2);
            if (*it1 == *it2) {	//check for equality of z and z+distance
                count++;
            }
            
            //z.push_back(Drip());
            it1++;
            it2++;
        }
        /*for (unsigned int i = 0; i < maxsize; i++) {
            printf("%d\n", z[i]);
        }*/
    
        double p = 1.0f/n;
        double E = maxsize * p;
        double standartDeviation = sqrt(E*(1.0f-p));
        printf("standartDeviation: %f\n", standartDeviation);
        
        double deviation = (count - E)/standartDeviation;
    
        printf("deviation: %f\n", deviation);
        if (deviation > 4.0f) {
            printf("*** BIAS detected! deviation: %f\n", deviation);
            return -1;
        }
    }
    
    return 0;
}

int correlationAfterAbsorb(int n){
    srand (time(NULL));
    
    InitializeState();
    uint8_t stateAfterInitialize[n];
    for (unsigned int i = 0; i<n; i++) {
        stateAfterInitialize[i] = getCipher().S[i];
    }
    
    uint8_t input[n/4];
    for (unsigned int i = 0; i < n/4; i++) {
        memset(input, rand()%256, 1);
    }
    Absorb(input,n/4);
    double corr = correlationCoefficient(n, stateAfterInitialize, getCipher().S);
    
    if (_debug) {
        for (unsigned int i = 0; i < n; i++) {
            if(i > 15 && i%16 == 0)
                printf("\n");
            printf("%03d ",getCipher().S[i]);
        }
        printf("\n");
    }
    
    printf("corrcoeff absorb: %f\n", corr);
    return 0;
}

int correlationAfterWhip(int n){
    InitializeState();
    uint8_t stateAfterInitialize[n];
    for (unsigned int i = 0; i<n; i++) {
        stateAfterInitialize[i] = getCipher().S[i];
    }
    Whip(2*n);
    double corr = correlationCoefficient(n, stateAfterInitialize, getCipher().S);
    
    printf("corrcoeff whip: %f\n", corr);
    return 0;
}

int correlationAfterWhipAndCrush(int n){
    InitializeState();
    uint8_t stateAfterInitialize[n];
    for (unsigned int i = 0; i<n; i++) {
        stateAfterInitialize[i] = getCipher().S[i];
    }
    Whip(2*n);
    Crush();
    double corr = correlationCoefficient(n, stateAfterInitialize, getCipher().S);
    
    printf("corrcoeff crush: %f\n", corr);
    return 0;
}

int correlationAfterShuffle(int n){
    InitializeState();
    uint8_t stateAfterInitialize[n];
    for (unsigned int i = 0; i<n; i++) {
        stateAfterInitialize[i] = getCipher().S[i];
    }
    Shuffle();
    double corr = correlationCoefficient(n, stateAfterInitialize, getCipher().S);
    
    printf("corrcoeff shuffle: %f\n", corr);
    return 0;
}


//begin - Weak key stuff
int correlationBetweenFixedInputAndOutput(int n){
    InitializeState();
    uint8_t input[n/4];
    memset(input, 0x00, n/4);
    Absorb(input, n/4);
    
    uint8_t stateAfterAbsorb[n];
    for (unsigned int i = 0; i<n; i++) {
        stateAfterAbsorb[i] = getCipher().S[i];
    }
    
    Squeeze(n/4);
    double corr = correlationCoefficient(n, stateAfterAbsorb, getCipher().S);
    
    printf("corrcoeff in/out: %f\n", corr);
    return 0;
}

int correlationBetweenRandomInputAndOutput(int n){
    int numberofIterations = (int)pow(2.0f, 20.0f);
    int inputlength = 14;
    double treshhold = 0.95f;
    srand (time(NULL));
    
    int cnt = 0;
    for (unsigned int k=0; k<numberofIterations; k++) {
        InitializeState();
        
        uint8_t input1[inputlength];
        memset(input1, 0x00, inputlength);
        // generate random input with size inputlenght
        for (unsigned int j = 0; j < inputlength; j++) {
            memset(input1+j, rand()%256, 1);// make total random inputs
        }
        Absorb(input1, inputlength);
        
        uint8_t stateAfterAbsorb[n];
        for (unsigned int i = 0; i<n; i++) {
            stateAfterAbsorb[i] = getCipher().S[i];
        }
        
        uint8_t *output = Squeeze(inputlength);
        double corr = correlationCoefficient(inputlength, input1, output);
        
        if (corr >= treshhold) {
            cnt++;
            if (_debug) {
                printf("corrcoeff in/out: %f\n", corr);
                for(int x = 0; x < inputlength; x++){
                    if(x > 15 && x%16 == 0)
                        printf("\n");
                    printf("%03d ", input1[x]);
                }
                printf("\n");
                
                for(int x = 0; x < inputlength; x++){
                    if(x > 15 && x%16 == 0)
                        printf("\n");
                    printf("%03d ", output[x]);
                }
                printf("\n");
            }
        }
    }
    printf("Total number of inputs: %d\n", numberofIterations);
    printf("Output is highly correlated to input for %d inputs\n", cnt);
    
    return 0;
}

int k1IsPrefixOfK2(int n){
    int numberofIterations = (int)pow(2.0f, 20.0f);
    int outputLength = 16;
    int keyLength = 4;
    double treshhold = 0.95f;
    int cnt = 0;
    srand (time(NULL));
    
    for (unsigned int k = 0; k < numberofIterations; k++) {
        uint8_t k1[keyLength-1];
        memset(k1, 0x00, keyLength-1);
        for (unsigned int j = 0; j < keyLength-1; j++) {
            //memset(k1+j, rand()%256, 1);
            k1[j] = rand()%256;
        }
        
        InitializeState();
        Absorb(k1, keyLength-1);
        uint8_t *output1 = (uint8_t*) malloc (outputLength*sizeof(uint8_t));
        output1 = Squeeze(outputLength);
        
        uint8_t k2[keyLength];
        memset(k2, 0x00, (keyLength));
        memcpy(k2, k1, keyLength-1);    // k1 is prefix of k2
        k2[keyLength-1] = rand()%256;       // add one more random byte so that k1 != k2
        
        InitializeState();
        Absorb(k2, keyLength);
        uint8_t *output2 = (uint8_t*) malloc (outputLength*sizeof(uint8_t));
        output2 = Squeeze(outputLength);
        
        //correlate output of squeeze of both keys k1,k2
        double corr = correlationCoefficient(outputLength, output1, output2);
        if(corr >= treshhold){
            cnt++;
            if (_debug) {
                printf("corrcoeff out1/out2: %f\n", corr);
                printf("k1: ");
                for(int x = 0; x < keyLength-1; x++){
                    if(x > 15 && x%16 == 0)
                        printf("\n");
                    printf("%03d ", k1[x]);
                }
                printf("\n");
                
                printf("k2: ");
                for(int x = 0; x < keyLength; x++){
                    if(x > 15 && x%16 == 0)
                        printf("\n");
                    printf("%03d ", k2[x]);
                }
                printf("\n");
                
                printf("out1: ");
                for(int x = 0; x < outputLength; x++){
                    if(x > 15 && x%16 == 0)
                        printf("\n");
                    printf("%03d ", output1[x]);
                }
                printf("\n");
                
                printf("out2: ");
                for(int x = 0; x < outputLength; x++){
                    if(x > 15 && x%16 == 0)
                        printf("\n");
                    printf("%03d ", output2[x]);
                }
                printf("\n");
            }
        }
    }
    
    printf("There are %d highly correlated outputs with prefix keys\n", cnt);
    
    return 0;
}

int increasingInputIncreasingOutput(int n){
    int numberofIterations = (int)pow(2.0f, 20.0f);
    double treshhold = 0.95f;
    int cnt = 0;
    int minKeyLength = 14;
    int maxKeyLength = 2*n;
    srand (time(NULL));
    
    for (unsigned int k = 0; k < numberofIterations; k++) {
        int start = rand()%n;
        int keyLength = rand()%maxKeyLength + (start + minKeyLength);
        //printf("start: %d, keylen: %d", start, keyLength);
        
        if (start >= keyLength/* || !((keyLength-start)%256)*/) {   // magic
            //printf(" start>=keylen\n");
            continue;
        }
        keyLength -= start;
        if(keyLength < 1)
            continue;
        
        //printf(", new keylen: %d\n", keyLength);
        uint8_t k1[keyLength];
        memset(k1, 0x00, keyLength);
        for (unsigned int j = start; j < keyLength; j++) {
            k1[j-start] = j;
        }
        
        InitializeState();
        Absorb(k1, keyLength);
        uint8_t *output1 = (uint8_t*) malloc ((keyLength)*sizeof(uint8_t));
        for (unsigned int i = 0; i < keyLength; i++) {
            output1[i] = Drip();
        }
        //output1 = Squeeze(keyLength);
        
        //correlate increasing input to output
        double corr = correlationCoefficient(keyLength, k1, output1);
        //double corr = correlationCoefficient(keyLength, k1, Squeeze(keyLength));
        if(corr >= treshhold){
            cnt++;
            if (_debug) {
                printf("corrcoeff out1/out2: %f\n", corr);
                printf("k1: ");
                for(int x = 0; x < keyLength; x++){
                    if(x > 15 && x%16 == 0)
                        printf("\n");
                    printf("%03d ", k1[x]);
                }
                printf("\n");
                
                printf("out1: ");
                for(int x = 0; x < keyLength; x++){
                    if(x > 15 && x%16 == 0)
                        printf("\n");
                    printf("%03d ", output1[x]);
                }
                printf("\n");
                
            }
        }
        free(output1);
        free(getCipher().S);
    }
    
    printf("There are %d highly correlated increasing outputs for increasing inputs\n", cnt);
    
    return 0;
}

int decreasingInputDecreasingOutput(int n){
    int numberofIterations = (int)pow(2.0f, 20.0f);
    double treshhold = 0.95f;
    int cnt = 0;
    int maxKeyLength = 2*n;
    int minKeyLength = 14;
    srand (time(NULL));
    
    for (unsigned int k = 0; k < numberofIterations; k++) {
        int start = rand()%n;
        int keyLength = rand()%maxKeyLength + (start + minKeyLength);
        
        if (start >= keyLength/* || !((keyLength-start)%256)*/) {   // magic
            continue;
        }
        keyLength -= start;
        if(keyLength < 1)
            continue;
        
        uint8_t k1[keyLength];
        memset(k1, 0x00, keyLength);
        for (unsigned int j = start; j < keyLength; j++) {
            k1[j-start] = keyLength-(j-start);
        }
        
        InitializeState();
        Absorb(k1, keyLength);
        uint8_t *output1 = (uint8_t*) malloc ((keyLength)*sizeof(uint8_t));
        for (unsigned int i = 0; i < keyLength; i++) {
            output1[i] = Drip();
        }
        //output1 = Squeeze(keyLength);
        
        //correlate increasing input to output
        double corr = correlationCoefficient(keyLength, k1, output1);
        if(corr >= treshhold){
            cnt++;
            if (_debug) {
                printf("corrcoeff out1/out2: %f\n", corr);
                printf("k1: ");
                for(int x = 0; x < keyLength; x++){
                    if(x > 15 && x%16 == 0)
                        printf("\n");
                    printf("%03d ", k1[x]);
                }
                printf("\n");
                
                printf("out1: ");
                for(int x = 0; x < keyLength; x++){
                    if(x > 15 && x%16 == 0)
                        printf("\n");
                    printf("%03d ", output1[x]);
                }
                printf("\n");
            }
        }
        free(output1);
        free(getCipher().S);
    }
    
    printf("There are %d highly correlated decreasing outputs for decreasing inputs\n", cnt);
    
    return 0;
}

int distributionWholeKeyWeakKeyPatterns(int n){
    int numberofPlaintexts = (int)pow(2.0f, 20.0f);
    int lengthOfPlaintext = n/4;//64;
    srand(time(NULL));
    int keylen = 16;
    uint8_t pattern[8] = {0x00, 0xff, 0x0f, 0xf0, 0x55, 0xaa, 0x01, 0xfe};
    
    for (unsigned int k = 0; k < 8; k++) {
        int *O = new int[numberofPlaintexts];
        //int O[numberofPlaintexts];
        memset(O, 0x00, numberofPlaintexts);
        printf("Check weak key with pattern: %02x\n", pattern[k]);
        for (unsigned int pt = 0; pt < numberofPlaintexts; pt++) {
            // generate random plaintext
            uint8_t plaintext[lengthOfPlaintext];
            memset(plaintext, 0x00, lengthOfPlaintext);
            for (unsigned int r = 0; r < lengthOfPlaintext; r++) {
                plaintext[r] = rand()%256;
            }
            
            // create weak key
            uint8_t weakKey[keylen];
            memset(weakKey, 0x00, keylen);
            //set key to specific value
            memset(weakKey, pattern[k], keylen);
            
            O[Encrypt(weakKey, keylen, plaintext, lengthOfPlaintext)[0]]++;
        }
        
        if (_debug){
            for (unsigned int i = 0; i< n; i++) {
                printf("%d;%d\n", i, O[i]);
            }
        }
        
        double chiSquare = chiSquaredStatistic(n, numberofPlaintexts, O);
        delete O;
        
        double degreeOfFreedom = n-1.0f;
        double standartDeviation = sqrt(2.0f*degreeOfFreedom);
        
        if (_debug) {
            printf("chiSquare: %f\n",chiSquare);
            printf("expected chiSquare: %f\n", degreeOfFreedom);
            printf("standart deviation: %f\n", standartDeviation);
        }
        
        if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
            printf("*** BIAS detected! for key %d *** \n", pattern[k]);
            //return -1;
        }
    }
    
    return 0;
}

int wholeKeyWeakKeyPatterns(int n){
    int maxKeyLength = 512;
    int cnt = 0;
    double treshhold = 0.01f;
    uint8_t pattern[8] = {0x00, 0xff, 0x0f, 0xf0, 0x55, 0xaa, 0x01, 0xfe};
    
    for (unsigned int p = 0; p < 8; p++) {
        for (unsigned int k = 1; k < maxKeyLength; k++) {
            uint8_t k1[k];
            memset(k1, 0x00, k);
            
            //set key to specific value
            memset(k1, pattern[p], k);    // key = 0xff ... 0xff
            
            InitializeState();
            setN(n);
            Absorb(k1, k);
            uint8_t *output = (uint8_t*) malloc ((k)*sizeof(uint8_t));
            memset(output, 0x00, k);
            for (unsigned int i = 0; i < k; i++) {
                output[i] = Drip();
            }
            
            //correlate key with output
            double corr = correlationCoefficient(k, k1, output);
            //printf("corrcoeff key/output: %f\n", corr);
            
            if(corr >= treshhold){
                cnt++;
                if (_debug) {
                    printf("corrcoeff key/output: %f\n", corr);
                    printf("key: ");
                    for(int x = 0; x < k; x++){
                        if(x > 15 && x%16 == 0)
                            printf("\n");
                        printf("%03d ", k1[x]);
                    }
                    printf("\n");
                    
                    printf("output: ");
                    for(int x = 0; x < k; x++){
                        if(x > 15 && x%16 == 0)
                            printf("\n");
                        printf("%03d ", output[x]);
                    }
                    printf("\n");
                }
            }
            free(output);
            free(getCipher().S);
        }
        printf("There are %d outputs that correlate for the given key with pattern: %02x\n", cnt, pattern[p]);
    }
    return 0;
}

int alternatingWeakKeyPatterns(int n){
    int maxKeyLength = 512;
    int cnt = 0;
    double treshhold = 0.95f;
    uint8_t pattern1[7] = {0x00, 0x01, 0x0f, 0xf0, 0x55, 0xaa, 0xfe};
    uint8_t pattern2[7] = {0x01, 0x00, 0xf0, 0x0f, 0xaa, 0x55, 0xef};
    
    for (unsigned int p = 0; p < 7; p++) {
        for (unsigned int k = 10; k < maxKeyLength; k++) {
            uint8_t k1[k];
            memset(k1, 0x00, k);
            
            //set key to specific value
            for (unsigned int i = 0; i < k; i++) {
                if(i%2)
                    memset(k1, pattern1[p], 1);
                else
                    memset(k1, pattern2[p], 1);
            }
            
            InitializeState();
            setN(n);
            Absorb(k1, k);
            uint8_t *output = (uint8_t*) malloc ((k)*sizeof(uint8_t));
            memset(output, 0x00, k);
            for (unsigned int i = 0; i < k; i++) {
                output[i] = Drip();
            }
            
            //correlate key with output
            double corr = correlationCoefficient(k, k1, output);
            //printf("corrcoeff key/output: %f\n", corr);
            
            if(corr >= treshhold){
                cnt++;
                if (_debug) {
                    printf("corrcoeff key/output: %f\n", corr);
                    printf("key: ");
                    for(int x = 0; x < k; x++){
                        if(x > 15 && x%16 == 0)
                            printf("\n");
                        printf("%03d ", k1[x]);
                    }
                    printf("\n");
                    
                    printf("output: ");
                    for(int x = 0; x < k; x++){
                        if(x > 15 && x%16 == 0)
                            printf("\n");
                        printf("%03d ", output[x]);
                    }
                    printf("\n");
                }
            }
            free(output);
            free(getCipher().S);
        }
        printf("There are %d outputs that correlate for the given key with pattern: %02x, %02x\n", cnt, pattern1[p], pattern2[p]);
        cnt=0;
    }
    return 0;
}

int rotationalKeyPatterns(int n){
    int maxKeyLength = 512;
    uint8_t pattern[8] = {0x00, 0xff, 0x0f, 0xf0, 0x55, 0xaa, 0x01, 0xfe};
    double treshhold = 0.95f;
    
    for (unsigned int pt = 0; pt < 8; pt++) {
        int cnt = 0;
        for (unsigned int k = 10; k<maxKeyLength; k++) {
            std::vector<uint8_t> key;
            key.push_back(pattern[pt]);
            for (unsigned int i = 1; i < k; i++) {
                key.push_back(0x00);
            }
            
            for (unsigned int rot = 1; rot < k; rot++) {
                std::rotate(key.begin(), key.begin()+rot, key.end());
                InitializeState();
                Absorb(&key[0], k);
                uint8_t *output = (uint8_t*) malloc ((k)*sizeof(uint8_t));
                memset(output, 0x00, k);
                for (unsigned int i = 0; i < k; i++) {
                    output[i] = Drip();
                }
                
                //correlate key with output
                double corr = correlationCoefficient(k, &key[0], output);
                //printf("corrcoeff key/output: %f\n", corr);
                
                if(corr >= treshhold){
                    cnt++;
                    if (_debug) {
                        printf("corrcoeff key/output: %f, k: %d, rot: %d\n", corr, k, rot);
                        printf("key: ");
                        for(int x = 0; x < k; x++){
                            if(x > 15 && x%16 == 0)
                                printf("\n");
                            printf("%03d ", key[x]);
                        }
                        printf("\n");
                        
                        printf("output: ");
                        for(int x = 0; x < k; x++){
                            if(x > 15 && x%16 == 0)
                                printf("\n");
                            printf("%03d ", output[x]);
                        }
                        printf("\n");
                    }
                }
                free(output);
                free(getCipher().S);
            }
        }
        printf("For key with pattern %02x fully rotated with length(0..512) there are %d correlations\n", pattern[pt], cnt);
    }
    
    return 0;
}

int distributionAlternatingWeakKeyPatterns(int n){
    int numberofPlaintexts = (int)pow(2.0f, 20.0f);
    int lengthOfPlaintext = n/4;//64;
    srand(time(NULL));
    int keylen = 16;
    uint8_t pattern1[7] = {0x00, 0x01, 0x0f, 0xf0, 0x55, 0xaa, 0xfe};
    uint8_t pattern2[7] = {0x01, 0x00, 0xf0, 0x0f, 0xaa, 0x55, 0xef};
    
    for (unsigned int k = 0; k < 7; k++) {
        int *O = new int[numberofPlaintexts];
        memset(O, 0x00, numberofPlaintexts);
        printf("Check weak key with pattern: %02x, %02x\n", pattern1[k], pattern2[k]);
        for (unsigned int pt = 0; pt < numberofPlaintexts; pt++) {
            // generate random plaintext
            uint8_t plaintext[lengthOfPlaintext];
            memset(plaintext, 0x00, lengthOfPlaintext);
            for (unsigned int r = 0; r < lengthOfPlaintext; r++) {
                plaintext[r] = rand()%256;
            }
            
            // create weak key
            uint8_t weakKey[keylen];
            memset(weakKey, 0x00, keylen);
            //set key to specific value
            for (unsigned int i = 0; i < keylen; i++) {
                if(i%2)
                    memset(weakKey, pattern1[k], 1);
                else
                    memset(weakKey, pattern2[k], 1);
            }
            
            O[Encrypt(weakKey, keylen, plaintext, lengthOfPlaintext)[0]]++;
        }
        
        if (_debug){
            for (unsigned int i = 0; i< n; i++) {
                printf("%d;%d\n", i, O[i]);
            }
        }
        
        double chiSquare = chiSquaredStatistic(n, numberofPlaintexts, O);
        delete O;
        
        double degreeOfFreedom = n-1.0f;
        double standartDeviation = sqrt(2.0f*degreeOfFreedom);
        
        if (_debug) {
            printf("chiSquare: %f\n",chiSquare);
            printf("expected chiSquare: %f\n", degreeOfFreedom);
            printf("standart deviation: %f\n", standartDeviation);
        }
        
        if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
            printf("*** BIAS detected! for key %02x, %02x *** \n", pattern1[k], pattern2[k]);
            //return -1;
        }
    }
    
    return 0;
}

int distributionRotationalWeakKeyPatterns(int n){
    int numberofPlaintexts = (int)pow(2.0f, 15.0f);
    int lengthOfPlaintext = n/4;//64;
    srand(time(NULL));
    int keylen = 16;
    uint8_t pattern[8] = {0xff, 0x0f, 0xf0, 0x55, 0xaa, 0x01, 0xfe, 0x00};
    
    for (unsigned int k = 0; k < 8; k++) {
        //int **O = new int[numberofPlaintexts][16];
        int **O = new int*[numberofPlaintexts];
        for(int i = 0; i < numberofPlaintexts; ++i) {
            O[i] = new int[keylen];
        }
        //int O[numberofPlaintexts];
        //memset(O, 0x00, numberofPlaintexts);
        printf("Check weak key with pattern: %02x\n", pattern[k]);
        
        // create weak key
        std::vector<uint8_t> weakKey;
        //set key to specific value
        weakKey.push_back(pattern[k]);
        for (unsigned int i = 1; i < keylen; i++) {
            weakKey.push_back(0x00);
        }
        
        for (unsigned int rot = 1; rot < keylen; rot++) {
            std::rotate(weakKey.begin(), weakKey.begin()+rot, weakKey.end());
            
            for (unsigned int pt = 0; pt < numberofPlaintexts; pt++) {
                // generate random plaintext
                uint8_t plaintext[lengthOfPlaintext];
                memset(plaintext, 0x00, lengthOfPlaintext);
                for (unsigned int r = 0; r < lengthOfPlaintext; r++) {
                    plaintext[r] = rand()%256;
                }
                
                O[Encrypt(&weakKey[0], keylen, plaintext, lengthOfPlaintext)[0]][rot]++;
            }
        }
        
        /*if (_debug){
         for (unsigned int i = 0; i< n; i++) {
         printf("%d;%d\n", i, O[i]);
         }
         }*/
        
        for (unsigned int rot = 1; rot < keylen; rot++) {
            double chiSquare = chiSquaredStatistic(n, numberofPlaintexts, O[rot]);
            /*for(int i = 0; i < numberofPlaintexts; ++i) {
             delete [] O[i];
             }
             delete [] O;*/
            
            double degreeOfFreedom = n-1.0f;
            double standartDeviation = sqrt(2.0f*degreeOfFreedom);
            
            if (_debug) {
                printf("chiSquare: %f\n",chiSquare);
                printf("expected chiSquare: %f\n", degreeOfFreedom);
                printf("standart deviation: %f\n", standartDeviation);
            }
            
            if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
                printf("*** BIAS detected! for key: %02x, rot: %d *** \n", pattern[k], rot);
                //return -1;
            }
        }
    }
    return 0;
}

int checkWeakKey(int n){
    int maxsize = (int)pow(2.0f, 20.0f);
    int plaintextLength = 16;
    int outputBytePosition = 0;
    srand(time(NULL));
    int O[256] = {0x00};
    
    //uint8_t weakKey[14] = {56,8,54,9,202,233,247,85,117,115,254,1,69,190};
    //uint8_t weakKey[14] = {163,40,37,215,184,85,235,194,175,224,93,227,119,52};
    uint8_t weakKey[14] = {214,251,95,80,124,242,11,14,12,214,142,179,208,212};
    
    for (unsigned int i = 0; i < maxsize; i++) {
        //generate random plaintext
        uint8_t plaintext[plaintextLength];
        memset(plaintext, 0x00, plaintextLength);
        for (unsigned int j = 0; j < plaintextLength; j++) {
            plaintext[j] = rand()%256;
        }
        //encrypt plaintext with weak key
        O[Encrypt(weakKey, 14, plaintext, plaintextLength)[outputBytePosition]]++;
    }
    
    if (_debug) {
        for (unsigned int i = 0; i < n; i++) {
            printf("%d;%d\n", i, O[i]);
        }
    }
    
    double chiSquare = chiSquaredStatistic(n, maxsize, O);
    //delete O;
    double degreeOfFreedom = n-1.0f;
    double standartDeviation = sqrt(2.0f*degreeOfFreedom);
    
    if (_debug) {
        printf("chiSquare: %f\n",chiSquare);
        printf("expected chiSquare: %f\n", degreeOfFreedom);
        printf("standart deviation: %f\n", standartDeviation);
    }
    
    if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
        printf("*** BIAS detected!  *** \n");
        //return -1;
    }
    
    
    return 0;
}

int mersenneTwisterRNG(){
    int maxsize = (int)pow(2.0f, 20.0f);
    int keyLen = 14;//256;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0, 256);
    uint8_t O[256] = {0x00};
    uint8_t identityPermutation[256] = {0x00};
    double threshold = 0.95f;
    
    for (unsigned int i = keyLen; i > 0; i--) {
        identityPermutation[i] = i;
    }
    
    int cnt = 0;
    for (int i=0; i<maxsize; ++i){
        for (int j = 0; j < keyLen; j++) {
            //printf("%d\n", (int)dist(mt));
            O[j] = (int)dist(mt);
        }
        //correlate key with output
        double corr = correlationCoefficient(keyLen, O, identityPermutation);
        
        if(corr >= threshold){
            cnt++;
        }
    }
    
    printf("There are %d descending outputs for 2^20 inputs\n", cnt);
    return 0;
}

/*
 * AES CTR implementation partly taken from:
 * http://stackoverflow.com/questions/3141860/aes-ctr-256-encryption-mode-of-operation-on-openssl
 */

#include <openssl/aes.h>
#include <openssl/rand.h>

struct ctr_state
{
    unsigned char ivec[AES_BLOCK_SIZE];
    unsigned int num;
    unsigned char ecount[AES_BLOCK_SIZE];
};

AES_KEY key;
unsigned char iv[AES_BLOCK_SIZE];
struct ctr_state state;

int init_ctr(struct ctr_state *state, const unsigned char iv[16])
{
    /* aes_ctr128_encrypt requires 'num' and 'ecount' set to zero on the first call. */
    state->num = 0;
    memset(state->ecount, 0, AES_BLOCK_SIZE);
    
    /* Initialise counter in 'ivec' to 0 */
    memset(state->ivec + 8, 0, 8);
    
    /* Copy IV into 'ivec' */
    memcpy(state->ivec, iv, 8);
    
    return 0;
}

int aesCTRRNG(){
    int maxsize = (int)pow(2.0f, 20.0f);
    int keyLen = 14;//256;
    
    uint8_t O[256] = {0x00};
    uint8_t identityPermutation[256] = {0x00};
    double threshold = 0.95f;
    
    for (unsigned int i = keyLen; i > 0; i--) {
        identityPermutation[i] = i;
    }
    
    int cnt = 0;
    for (unsigned int i = 0; i < maxsize; i++) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        //Initialize IV
        if(!RAND_bytes(iv, AES_BLOCK_SIZE)){
            printf("Could not create random bytes.\n");
            return -1;
        }
        
        unsigned const char* enc_key = (unsigned const char*)"1234567812345678";
        
        //Initializing the encryption KEY
        if (AES_set_encrypt_key(enc_key, 128, &key) < 0){
            printf("Could not set encryption key.\n");
            return -1;
        }
        
        //Counter call
        init_ctr(&state, iv);
        for (unsigned int k = 0; k < keyLen; k++) {
            unsigned char indata[1] = {0x00};
            unsigned char outdata[1];
            AES_ctr128_encrypt(indata, outdata, 1, &key, state.ivec, state.ecount, &state.num);
            O[k] = *outdata;
        }
#pragma GCC diagnostic pop
        //correlate key with output
        double corr = correlationCoefficient(keyLen, O, identityPermutation);
        
        if(corr >= threshold){
            cnt++;
        }
    }
    
    printf("There are %d descending outputs for 2^20 inputs\n", cnt);
    return 0;
}

// end - weak key stuff

int collisionAttack(int n){
    double treshhold = 0.75;
    //double treshhold = 1.00f;
    //std::list<std::pair<int, std::pair<uint8_t*, uint8_t*>>> correlatedStates;
    int cnt = 0, cnt2 = 0;
    
    uint8_t input1[n/2];
    memset(input1, 0x00, n/2);
    for (unsigned int k=0; k<256*(n/2); k++) {
        InitializeState();
        
        memset(input1+(int)floor(k/256), k%256, 1);
        Absorb(input1, n/2);
        
        uint8_t stateAfterAbsorb[n];
        for (unsigned int i = 0; i<n; i++) {
            stateAfterAbsorb[i] = getCipher().S[i];
        }
        
        Whip(2*n);
        
        uint8_t stateAfterWhip[n];
        for (unsigned int i = 0; i<n; i++) {
            stateAfterWhip[i] = getCipher().S[i];
        }
        
        Crush();
        
        uint8_t stateAfterCrush[n];
        for (unsigned int i = 0; i<n; i++) {
            stateAfterCrush[i] = getCipher().S[i];
        }
        
        uint8_t input2[n/2];
        memset(input2, 0x00, n/2);
        for (unsigned int i = 0; i < 256*(n/2); i++) {
            InitializeState();
            memset(input2+(int)floor(i/256), i%256, 1);
            
            if (memcmp(input1, input2, n/2) == 0) {
                //printf("Same inputs!\n");
                continue;
            }
            
            /* if (_debug) {
             for (unsigned int j= 0; j< n/4; j++) {
             if(j > 15 && j%16 == 0)
             printf("\n");
             printf("%03d ", input2[j]);
             }
             printf("\n");
             }*/
            
            Absorb(input2, n/2);
            
            double corr = correlationCoefficient(n, stateAfterAbsorb, getCipher().S);
            /*if (_debug) {
             printf("corr: %f\n", corr);
             }*/
            if (corr >= 1.0f || corr <= -1.0f) {
                cnt2++;
            }
            
            if (corr >= treshhold || corr <= (treshhold * -1)) {
                //std::pair<uint8_t*, uint8_t*> states(stateAfterAbsorb, getCipher().S);
                //std::pair<int, std::pair<uint8_t*, uint8_t*>> correlatedState(corr, states);
                //correlatedStates.push_back(correlatedState);
                
                Whip(2*n);
                double corr2 = correlationCoefficient(n, stateAfterWhip, getCipher().S);
                /*if (_debug) {
                 printf("corr2: %f\n", corr2);
                 }*/
                if (corr2 >= 0.75 || corr2 <= -0.75) {
                    //if (corr2 >= 0.90f || corr2 <= -0.90f) {
                    //check for collision
                    Crush();
                    if (memcmp(stateAfterCrush, getCipher().S, n) == 0) {
                        if (_debug) {
                            printf("Found collision!!!\n");
                            for (unsigned int j= 0; j< n/2; j++) {
                                if(j > 15 && j%16 == 0)
                                    printf("\n");
                                printf("%02x ", input1[j]);
                            }
                            printf("\n");
                            for (unsigned int j= 0; j< n/2; j++) {
                                if(j > 15 && j%16 == 0)
                                    printf("\n");
                                printf("%02x ", input2[j]);
                            }
                            printf("\n");
                            //exit(0);
                        }
                        cnt++;
                    }
                }
            }
        }
        if (_debug) {
            //printf("size of list: %lu\n", correlatedStates.size());
        }
    }
    printf("collisions found: %d, collisions after absorb: %d\n", cnt, cnt2);
    
    return 0;
}

int collisionAttack2(int n){
    double treshhold = 0.75f;//1.00f;
    int maxsize = (int)pow(2.0f, n/4);
    int cnt = 0, cnt2 = 0;
    
    for (unsigned int whips = 1; whips < 2*n; whips++) {
        input in1;
        for (unsigned int k=0; k<maxsize; k++) {
            InitializeState();
            
            in1.i = k;
            Absorb(in1.b, n/4);
            
            uint8_t stateAfterAbsorb[n];
            for (unsigned int i = 0; i<n; i++) {
                stateAfterAbsorb[i] = getCipher().S[i];
            }
            
            Whip(whips);  // make # of Whips variable
            
            uint8_t stateAfterWhip[n];
            for (unsigned int i = 0; i<n; i++) {
                stateAfterWhip[i] = getCipher().S[i];
            }
            
            input in2;
            for (unsigned int i = 0; i < maxsize; i++) {
                InitializeState();
                in2.i = i;
                
                if (in1.i == in2.i) {
                    //printf("Same inputs!\n");
                    continue;
                }
                
                Absorb(in2.b, n/4);
                
                double corr = correlationCoefficient(n, stateAfterAbsorb, getCipher().S);
                /*if (_debug) {
                 printf("corr: %f\n", corr);
                 }*/
                if (corr >= 1.0f || corr <= -1.0f) {
                    cnt2++;
                }
                
                if (corr >= treshhold || corr <= (treshhold * -1)) {
                    Whip(whips);  // make # of Whips variable
                    
                    if (memcmp(stateAfterWhip, getCipher().S, n) == 0) {
                        if (_debug) {
                            printf("Found collision!!!\n");
                            for (unsigned int j= 0; j< n/2; j++) {
                                if(j > 15 && j%16 == 0)
                                    printf("\n");
                                printf("%02x ", in1.b[j]);
                            }
                            printf("\n");
                            for (unsigned int j= 0; j< n/2; j++) {
                                if(j > 15 && j%16 == 0)
                                    printf("\n");
                                printf("%02x ", in2.b[j]);
                            }
                            printf("\n");
                            //exit(0);
                        }
                        cnt++;
                    }
                }
            }
        }
        printf("# of whips: %d, collisions found: %d, collisions after absorb: %d\n", whips, cnt, cnt2);
    }
    
    return 0;
}

int collisionAttack3(int n){
    double treshhold = 0.75f;//1.00f;
    int inputlength = 64;
    int numberofIterations = (int)pow(2.0f, 10.0f);
    int cnt = 0, cnt2 = 0;
    srand (time(NULL));
    
    for (unsigned int whips = 1; whips < 2*n; whips++) {
        for (unsigned int k=0; k<numberofIterations; k++) {
            InitializeState();
            
            uint8_t input1[inputlength];
            memset(input1, 0x00, inputlength);
            // generate random input with size inputlenght
            for (unsigned int j = 0; j < inputlength; j++) {
                memset(input1+j, rand()%256, 1);// make total random inputs
            }
            Absorb(input1, inputlength);
            
            uint8_t stateAfterAbsorb[n];
            for (unsigned int i = 0; i<n; i++) {
                stateAfterAbsorb[i] = getCipher().S[i];
            }
            
            Whip(whips);  // make # of Whips variable
            
            uint8_t stateAfterWhip[n];
            for (unsigned int i = 0; i<n; i++) {
                stateAfterWhip[i] = getCipher().S[i];
            }
            
            for (unsigned int i = 0; i < numberofIterations; i++) {
                InitializeState();
                
                uint8_t input2[inputlength];
                memset(input2, 0x00, inputlength);
                // generate random input with size inputlenght
                for (unsigned int j = 0; j < inputlength; j++) {
                    memset(input2+j, rand()%256, 1);// make total random inputs
                }
                if (memcmp(input1, input2, inputlength) == 0) {
                    //printf("Same inputs!\n");
                    continue;
                }
                Absorb(input2, inputlength);
                
                double corr = correlationCoefficient(n, stateAfterAbsorb, getCipher().S);
                /*if (_debug) {
                 printf("corr: %f\n", corr);
                 }*/
                if (corr >= 1.0f || corr <= -1.0f) {
                    cnt2++;
                }
                
                if (corr >= treshhold || corr <= (treshhold * -1)) {
                    Whip(whips);  // make # of Whips variable
                    
                    if (memcmp(stateAfterWhip, getCipher().S, n) == 0) {
                        if (_debug) {
                            printf("Found collision!!!\n");
                            for (unsigned int j= 0; j< n/2; j++) {
                                if(j > 15 && j%16 == 0)
                                    printf("\n");
                                printf("%02x ", input1[j]);
                            }
                            printf("\n");
                            for (unsigned int j= 0; j< n/2; j++) {
                                if(j > 15 && j%16 == 0)
                                    printf("\n");
                                printf("%02x ", input2[j]);
                            }
                            printf("\n");
                            //exit(0);
                        }
                        cnt++;
                    }
                }
            }
        }
        printf("# of whips: %d, collisions found: %d, collisions after absorb: %d\n", whips, cnt, cnt2);
    }
    
    return 0;
}

int collisionAttackN16(){
    printf("\n Try some attacks...\n");
    
    uint8_t m1[4] = {0x09, 0x00, 0x00, 0x00};
    InitializeState();
    debug();
    Absorb(m1, 4);
    debug();
    for (int i = 0; i < 10; i++) {
        //debug();
        printf("%02x ", Drip());
    }
    printf("\n");
    
    printf("*******************************\n");
    
    uint8_t m2[4] = {0x80, 0x00, 0x00, 0x00 };
    InitializeState();
    debug();
    Absorb(m2, 4);
    debug();
    for (int i = 0; i < 10; i++) {
        //debug();
        printf("%02x ", Drip());
    }
    printf("\n");
    
    return 0;
}

int collisionAttackN24(){
    
    printf("\n Try some attacks...\n");
    //uint8_t m1[6] = {0x0d, 0x00, 0x00, 0x00, 0x00, 0x00};
    //uint8_t m1[6] = {0x1d, 0x00, 0x00, 0x00, 0x00, 0x00};
    //uint8_t m1[6] = {0xff, 0xff, 0x00, 0x00, 0x00, 0x00};
    uint8_t m1[6] = {0xdd, 0x00, 0x00, 0x00, 0x00, 0x00};
    InitializeState();
    debug();
    Absorb(m1, 6);
    debug();
    for (int i = 0; i < 10; i++) {
        //debug();
        printf("%02x ", Drip());
    }
    printf("\n");
    
    printf("*******************************\n");
    
    //uint8_t m2[6] = {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00};
    //uint8_t m2[6] = {0xc1, 0x00, 0x00, 0x00, 0x00, 0x00};
    //uint8_t m2[6] = {0xff, 0xee, 0x00, 0x00, 0x00, 0x00};
    uint8_t m2[6] = {0xcc, 0x00, 0x00, 0x00, 0x00, 0x00};
    InitializeState();
    debug();
    Absorb(m2, 6);
    debug();
    for (int i = 0; i < 10; i++) {
        //debug();
        printf("%02x ", Drip());
    }
    printf("\n");
    
    return 0;
}

int collisionAttackN28(){
    uint8_t m1[14] = {0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t m2[14] = {0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    InitializeState();
    debug();
    Absorb(m1, 14);
    debug();
    for (int i = 0; i < 10; i++) {
        //debug();
        printf("%02x ", Drip());
    }
    printf("\n");
    
    printf("*******************************\n");
    
    InitializeState();
    debug();
    Absorb(m2, 14);
    debug();
    for (int i = 0; i < 10; i++) {
        //debug();
        printf("%02x ", Drip());
    }
    printf("\n");
    
    return 0;
}

int numOfStates(int n){
    
    //generate n! states
    cipher c;
    c.i = 0;
    c.j = 0;
    c.k = 0;
    c.z = 0;
    c.a = 0;
    c.w = 1;
    
    std::vector<uint8_t> S;
    // S[N] = identity permutation
    for (unsigned int v = 0; v <= n-1; v++) {
        S.push_back(v);
    }
    
    int numberOfPermutations = 1;
    std::list<std::vector<uint8_t>>permutations;
    permutations.push_back(S);
    
    //calculate all possible permutations
    while ( std::next_permutation(S.begin(),S.begin()+n) ){
        permutations.push_back(S);
        numberOfPermutations++;
    }
    printf("number of permutations: %d\n", numberOfPermutations);
    
    /*for (std::list<std::vector<uint8_t>>::iterator it=permutations.begin(); it != permutations.end(); ++it) {
     for (int x = 0; x<n; x++) {
     printf("%d", (*it)[x]);
     }
     printf("\n");
     }*/
    
    std::list<std::vector<uint8_t>>permutationsAfterCrush;
    for (std::list<std::vector<uint8_t>>::iterator it=permutations.begin(); it != permutations.end(); ++it) {;
        c.S = &(*it)[0];
        setCipher(c);
        setN(n);
        
        Whip(2*n);
        //Crush();
        /*Whip(2*n);
         Crush();
         Whip(2*n);*/
        
        int equal = 0;
        for (std::list<std::vector<uint8_t>>::iterator it2=permutationsAfterCrush.begin(); it2 != permutationsAfterCrush.end(); ++it2) {
            for (unsigned int i = 0; i<n; i++) {
                if ((*it2)[i] != getCipher().S[i]) {
                    break;
                }
                if (i == n-1) {
                    equal = 1;
                }
            }
            
        }
        if (!equal) {
            permutationsAfterCrush.push_back(std::vector<uint8_t>(getCipher().S, getCipher().S + n));
        }
    }
    
    printf("number of different permutations after Crush: %lu\n", permutationsAfterCrush.size());
    printf("number of same permutations after Crush: %lu\n", numberOfPermutations-permutationsAfterCrush.size());
    
    /* for (std::list<std::vector<uint8_t>>::iterator it=permutationsAfterCrush.begin(); it != permutationsAfterCrush.end(); ++it) {
     for (int x = 0; x<n; x++) {
     printf("%d", (*it)[x]);
     }
     printf("\n");
     }*/
    
    return 0;
}

int numOfPermutationsAfterAbsorb(int n){
    int maxsize = 10;//(int)pow(2.0f, 20.0f);
    int inputLength = n/4;
    
    std::map<std::vector<uint8_t>, int>permutations;
    int firstrun = 1;
    
    for (unsigned int i = 0; i < maxsize; i++) {
        InitializeState();
        
        //generate random input
        uint8_t input[inputLength];
        memset(input, 0x00, inputLength);
        for (unsigned int j = 0; j < inputLength; j++) {
            input[j] = rand()%256;
        }
        Absorb(input, inputLength);
        if (firstrun) {
            permutations.insert(std::pair<std::vector<uint8_t>, int>(std::vector<uint8_t>(*getCipher().S, n), 1));
            firstrun = 0;
            printf("There are %lu permutations\n", permutations.size());
        }
        
        int equal = 0;
        for (std::map<std::vector<uint8_t>, int>::iterator it=permutations.begin(); it!=permutations.end(); it++) {
            printf("There are %lu permutations\n", permutations.size());
            for (unsigned int j = 0; j < n; j++) {
                printf("it[j]: %d, S[j]: %d\n", it->first[j], getCipher().S[j]);
                if (it->first[j] == getCipher().S[j] ) {
                    equal = 1;
                } else {
                    equal = 0;
                    break;
                }
            }
            if (equal) {
                printf("%d\n", it->first[0]);
                it->second++;
                break;
            }
        }
        if (equal == 0) {
            printf("insert new one!\n");
            printf("There are %lu permutations\n", permutations.size());
            permutations.insert(std::pair<std::vector<uint8_t>, int>(std::vector<uint8_t>(*getCipher().S, n), 1));
            printf("There are %lu permutations\n", permutations.size());
            printf("insert ok!\n");
        }
    }
    
    printf("There are %lu permutations\n", permutations.size());
    return 0;
}

int checkEqualRegisters(int n){
    
    //generate n! states
    cipher c;
    c.i = 0;
    c.j = 0;
    c.k = 0;
    c.z = 0;
    c.a = 0;
    c.w = 1;
    
    std::vector<uint8_t> S;
    // S[N] = identity permutation
    for (unsigned int v = 0; v <= n-1; v++) {
        S.push_back(v);
    }
    
    int numberOfPermutations = 1;
    std::list<std::vector<uint8_t>>permutations;
    //insert identity permutation
    permutations.push_back(S);
    
    //calculate all possible permutations
    while ( std::next_permutation(S.begin(),S.begin()+n) ){
        permutations.push_back(S);
        numberOfPermutations++;
    }
    printf("number of permutations: %d\n", numberOfPermutations);
    
    //std::list<std::vector<uint8_t>>permutationsAfterCrush;
    std::list<std::array<uint8_t, 6>>registers;
    for (std::list<std::vector<uint8_t>>::iterator it=permutations.begin(); it != permutations.end(); ++it) {
        c.S = &(*it)[0];
        setCipher(c);
        setN(n);
        
        Whip(2*n);
        //Crush();
        
        int equal = 0;
        for (std::list<std::array<uint8_t, 6>>::iterator it2=registers.begin(); it2 != registers.end(); ++it2) {
            if (getCipher().i == (*it2)[0] &&
                getCipher().j == (*it2)[1] &&
                getCipher().k == (*it2)[2] &&
                getCipher().z == (*it2)[3] &&
                getCipher().a == (*it2)[4] &&
                getCipher().w == (*it2)[5]) {
                equal = 1;
                // printf("equal!\n");
                /* printf("i:%d, j: %d, k:%d, z:%d, a:%d, w:%d\n", (*it2)[0], (*it2)[1], (*it2)[2], (*it2)[3], (*it2)[4], (*it2)[5]);*/
                break;
            }
        }
        
        if(!equal){
            std::array<uint8_t, 6>reg = {{getCipher().i, getCipher().j, getCipher().k, getCipher().z, getCipher().a, getCipher().w}};
            registers.push_back(reg);
        }
    }
    
    printf("number of same registers: %lu\n", permutations.size()-registers.size());
    
    /*for (std::list<std::array<uint8_t, 6>>::iterator it2=registers.begin(); it2 != registers.end(); ++it2) {
     printf("i:%d, j: %d, k:%d, z:%d, a:%d, w:%d\n", (*it2)[0], (*it2)[1], (*it2)[2], (*it2)[3], (*it2)[4], (*it2)[5]);
     }*/
    
    return 0;
}

int checkEqualRegistersAndPermutations(int n){
    
    //generate n! states
    cipher c;
    c.i = 0;
    c.j = 0;
    c.k = 0;
    c.z = 0;
    c.a = 0;
    c.w = 1;
    
    std::vector<uint8_t> S;
    // S[N] = identity permutation
    for (unsigned int v = 0; v <= n-1; v++) {
        S.push_back(v);
    }
    
    int numberOfPermutations = 1;
    std::list<std::vector<uint8_t>>permutations;
    permutations.push_back(S);
    
    //calculate all possible permutations
    while ( std::next_permutation(S.begin(),S.begin()+n) ){
        permutations.push_back(S);
        numberOfPermutations++;
    }
    printf("number of permutations: %d\n", numberOfPermutations);
    
    /*for (std::list<std::vector<uint8_t>>::iterator it=permutations.begin(); it != permutations.end(); ++it) {
     for (int x = 0; x<n; x++) {
     printf("%d", (*it)[x]);
     }
     printf("\n");
     }*/
    
    std::list<std::vector<uint8_t>>permutationsAfterCrush;
    std::list<std::array<uint8_t, 6>>registers;
    for (std::list<std::vector<uint8_t>>::iterator it=permutations.begin(); it != permutations.end(); ++it) {;
        c.S = &(*it)[0];
        setCipher(c);
        setN(n);
        
        Whip(2*n);
        Crush();
        /*Whip(2*n);
         Crush();
         Whip(2*n);*/
        
        int equal = 0;
        for (std::list<std::vector<uint8_t>>::iterator it2=permutationsAfterCrush.begin(); it2 != permutationsAfterCrush.end(); ++it2) {
            for (unsigned int i = 0; i<n; i++) {
                if ((*it2)[i] != getCipher().S[i]) {
                    break;
                }
                if (i == n-1) {
                    equal = 1;  // states are equal
                    // check if registers are equal too
                    int equal_reg = 0;
                    for (std::list<std::array<uint8_t, 6>>::iterator it3=registers.begin(); it3 != registers.end(); ++it3){
                        if (getCipher().i == (*it3)[0] &&
                            getCipher().j == (*it3)[1] &&
                            getCipher().k == (*it3)[2] &&
                            getCipher().z == (*it3)[3] &&
                            getCipher().a == (*it3)[4] &&
                            getCipher().w == (*it3)[5]){
                            equal_reg = 1;
                            break;
                        }
                    }
                    if(!equal_reg){
                        std::array<uint8_t, 6>reg = {{getCipher().i, getCipher().j, getCipher().k, getCipher().z, getCipher().a, getCipher().w}};
                        registers.push_back(reg);
                    }
                }
            }
            
        }
        if (!equal) {
            permutationsAfterCrush.push_back(std::vector<uint8_t>(getCipher().S, getCipher().S + n));
        }
    }
    
    printf("number of same permutations: %lu\n", numberOfPermutations-permutationsAfterCrush.size());
    printf("number of same permutations and same registers: %lu\n", numberOfPermutations-permutationsAfterCrush.size()-registers.size());
    
    /* for (std::list<std::vector<uint8_t>>::iterator it=permutationsAfterCrush.begin(); it != permutationsAfterCrush.end(); ++it) {
     for (int x = 0; x<n; x++) {
     printf("%d", (*it)[x]);
     }
     printf("\n");
     }*/
    
    return 0;
}

int checkEqualRegistersWithRandomPermutations(int n){
    int maxsize = (int)pow(2.0f, 20.0f);
    srand(time(NULL));
    
    std::list<std::array<uint8_t, 6>>registers;
    //generate 2^20 random states
    for (unsigned int i = 0; i < maxsize; i++) {
        cipher c;
        c.i = 0;
        c.j = 0;
        c.k = 0;
        c.z = 0;
        c.a = 0;
        c.w = 1;
        
        std::vector<uint8_t> S;
        for (unsigned int v = 0; v <= n-1; v++) {
            S.push_back(rand()%n);
        }
        
        c.S = &S[0];
        setCipher(c);
        setN(n);
        
        Whip(2*n);
        Crush();
        
        int equal = 0;
        for (std::list<std::array<uint8_t, 6>>::iterator it2=registers.begin(); it2 != registers.end(); ++it2) {
            if (getCipher().i == (*it2)[0] &&
                getCipher().j == (*it2)[1] &&
                getCipher().k == (*it2)[2] &&
                getCipher().z == (*it2)[3] &&
                getCipher().a == (*it2)[4] &&
                getCipher().w == (*it2)[5]) {
                equal = 1;
                break;
            }
        }
        
        if(!equal){
            std::array<uint8_t, 6>reg = {{getCipher().i, getCipher().j, getCipher().k, getCipher().z, getCipher().a, getCipher().w}};
            registers.push_back(reg);
        }
    }
    
    printf("number of permutations: %d\n", maxsize);
    printf("number of same registers: %lu\n", maxsize-registers.size());
    
    /*for (std::list<std::array<uint8_t, 6>>::iterator it2=registers.begin(); it2 != registers.end(); ++it2) {
     printf("i:%d, j: %d, k:%d, z:%d, a:%d, w:%d\n", (*it2)[0], (*it2)[1], (*it2)[2], (*it2)[3], (*it2)[4], (*it2)[5]);
     }*/
    
    return 0;
}

int checkEqualRegistersAndPermutationsWithRandomPermutations(int n){
    int maxsize = (int)pow(2.0f, 18.0f);
    srand(time(NULL));
    
    std::list<std::vector<uint8_t>>permutationsAfterCrush;
    std::list<std::array<uint8_t, 6>>registers;
    //generate 2^10 random states
    for (unsigned int j = 0; j < maxsize; j++) {
        cipher c;
        c.i = 0;
        c.j = 0;
        c.k = 0;
        c.z = 0;
        c.a = 0;
        c.w = 1;
        
        std::vector<uint8_t> S;
        for (unsigned int v = 0; v <= n-1; v++) {
            S.push_back(rand()%n);
        }
        
        c.S = &S[0];
        setCipher(c);
        setN(n);
        
        /*printf("***\n");
         for(int x = 0; x < n; x++){
         if(x > 15 && x%16 == 0)
         printf("\n");
         printf("%03d ", getCipher().S[x]);
         }
         printf("***\n");*/
        
        
        Whip(2*n);
        Crush();
        /*Whip(2*n);
         Crush();
         Whip(2*n);*/
        
        int equal = 0;
        for (std::list<std::vector<uint8_t>>::iterator it2=permutationsAfterCrush.begin(); it2 != permutationsAfterCrush.end(); ++it2) {
            for (unsigned int i = 0; i<n; i++) {
                if ((*it2)[i] != getCipher().S[i]) {
                    break;
                }
                if (i == n-1) {
                    //printf("found two equal permutations\n");
                    equal = 1;  // states are equal
                    // check if registers are equal too
                    int equal_reg = 0;
                    for (std::list<std::array<uint8_t, 6>>::iterator it3=registers.begin(); it3 != registers.end(); ++it3){
                        if (getCipher().i == (*it3)[0] &&
                            getCipher().j == (*it3)[1] &&
                            getCipher().k == (*it3)[2] &&
                            getCipher().z == (*it3)[3] &&
                            getCipher().a == (*it3)[4] &&
                            getCipher().w == (*it3)[5]){
                            equal_reg = 1;
                            break;
                        }
                    }
                    if(!equal_reg){
                        std::array<uint8_t, 6>reg = {{getCipher().i, getCipher().j, getCipher().k, getCipher().z, getCipher().a, getCipher().w}};
                        registers.push_back(reg);
                    }
                }
            }
        }
        if (!equal) {
            permutationsAfterCrush.push_back(std::vector<uint8_t>(getCipher().S, getCipher().S + n));
        }
    }
    
    printf("number of same permutations: %lu\n", maxsize-permutationsAfterCrush.size());
    printf("number of same permutations and registers: %lu\n", maxsize-permutationsAfterCrush.size()-registers.size());
    
    return 0;
}

signed int calculateSignOfPermutation(int n, uint8_t* S){
    int parity[n];
    memset(parity, 0x00, n*sizeof(int));
    int i = 1;
    while(i < n){
        int j = i;
        while(j < n){
            if(S[i-1] > S[j]){
                parity[j]++;
            }
            j++;
        }
        i++;
    }
    
    int paritySum = 0;
    for (unsigned int v = 0; v<n; v++){
        //printf("[%d]: %d\n", v, parity[v]);
        paritySum += parity[v];
    }
    
    if (paritySum % 2 == 0){
        return 1;               //even
    } else {
        return -1;              //odd
    }
    
    return 0;
}

int distributionOfIAndJAfterShuffle(int n){
    srand (time(NULL));
    InitializeState();
    
    // Generate random key
    uint8_t key[n/4];
    for (int k = 0; k<n/4; k++) {
        memset(key+k, static_cast<uint8_t>(rand()%256), 1);
    }
    Absorb(key, n/4);
    
    int swapsCrush = 0;
    int iEqualJ = 0;
    // Shuffle - begin
    //************************
    // Whip(2N)
    for (unsigned int v = 0; v < 2*n; v++){
        Update();
        //compare i & j
        if(getCipher().i == getCipher().j){
            iEqualJ++;
        }
    }
    cipher _c = getCipher();
    do {
        _c.w = _c.w + 1;
    }
    while (Gcd(_c.w, n) != 1);
    setCipher(_c);
    
    //************************
    // Crush
    for (unsigned int v = 0; v <= (int)floor(n/2)-1; v++){
        if (getCipher().S[v] > getCipher().S[n-1-v]) {
            Swap(v, n-1-v);
            // count swaps
            swapsCrush++;
        }
    }
    
    //************************
    // Whip(2N)
    for (unsigned int v = 0; v < 2*n; v++){
        Update();
        //compare i & j
        if(getCipher().i == getCipher().j){
            iEqualJ++;
        }
    }
    _c = getCipher();
    do {
        _c.w = _c.w + 1;
    }
    while (Gcd(_c.w, n) != 1);
    setCipher(_c);
    
    //************************
    // Crush
    for (unsigned int v = 0; v <= (int)floor(n/2)-1; v++){
        if (getCipher().S[v] > getCipher().S[n-1-v]) {
            Swap(v, n-1-v);
            // count swaps
            swapsCrush++;
        }
    }
    
    //************************
    // Whip(2N)
    for (unsigned int v = 0; v < 2*n; v++){
        Update();
        //compare i & j
        if(getCipher().i == getCipher().j){
            iEqualJ++;
        }
    }
    _c = getCipher();
    do {
        _c.w = _c.w + 1;
    }
    while (Gcd(_c.w, n) != 1);
    setCipher(_c);
    
    //************************
    // Shuffle - end
    
    printf("Swaps in Crush: %d\n", swapsCrush);
    printf("i equal to j in Whip: %d\n", iEqualJ);
    
    return 0;
}


int signOfPermutationAfterAbsorb(int n){
    int maxsize = (int)pow(2.0f, 10);
    int cnt = 0;
    int aEqualNHalfPlusX = 0;
    
    for(int i = 0; i < maxsize; i++){
        //srand (time(NULL));
        InitializeState();
        //for(int i = 0; i < (int)floor(n/2); i++){
        // Generate random key
        /*uint8_t key[n/4];
         for (int k = 0; k<n/4; k++) {
         memset(key+k, static_cast<uint8_t>(rand()%256), 1);
         }*/
        
        //uint8_t key = static_cast<uint8_t>(rand()%256);
        uint8_t key = static_cast<uint8_t>(i);
        
        
        AbsorbNibble(Low(key));
        if(getCipher().S[getCipher().a] == getCipher().S[(int)floor(n/2)+Low(key)]){
            aEqualNHalfPlusX++;
        }
        if(_debug){
            printf("%d\n", calculateSignOfPermutation(n, getCipher().S));
            for(unsigned int i = 0; i< n; i++){
                printf("%d ", getCipher().S[i]);
            }
            printf("\n");
        }
        if(calculateSignOfPermutation(n, getCipher().S) == 1){
            cnt++;
        }
        
        AbsorbNibble(High(key));
        if(getCipher().S[getCipher().a] == getCipher().S[(int)floor(n/2)+Low(key)]){
            aEqualNHalfPlusX++;
        }
        if(_debug){
            printf("%d\n", calculateSignOfPermutation(n, getCipher().S));
            for(unsigned int i = 0; i< n; i++){
                printf("%d ", getCipher().S[i]);
            }
            printf("\n");
        }
        if(calculateSignOfPermutation(n, getCipher().S) == 1){
            cnt++;
        }
    }
    //printf("total number: %d, number of 1's: %d\n", n, cnt);
    printf("total number: %d, number of 1's: %d\n", 2*maxsize, cnt);
    printf("S[a] == S[floor(N/2)+x]: %d\n", aEqualNHalfPlusX);
    
    return 0;
}

//preimage attack
int searchForCollidingInputs(int n){
    int maxsize_input2 = (int)pow(2.0f, n/4);//256*4;
    int maxsize_input1 = (int)pow(2.0f, n);
    
    uint8_t identityPermutation[n];
    memset(identityPermutation, 0x00, n);
    for (unsigned int v = 0; v < n; v++){
        identityPermutation[v] = v;
    }
    
    input in;
    for (unsigned int i = 0; i < maxsize_input1; i++){
        InitializeState();
        
        // Absorb all possible input
        in.i = i;
        Absorb(in.b, n);
        
        Whip(2*n);
        uint8_t S[n];
        memcpy(S, getCipher().S, n);
        
        //modify state so it is sorted one more time
        //partly descending order Crush
        for (unsigned int k = 0; k < (int)floor(n/2.0f); k++){
            if(S[k] < S[n-1-k]){
                Swap(k, n-1-k);
                break;
            }
        }
        
        inverseWhip(2*n);
        
        uint8_t permutationAfterInverseWhip[n];
        memcpy(permutationAfterInverseWhip, getCipher().S, n);
        
        // Test with all possible input to come back to InitialState
        for(unsigned int input2 = 0; input2 < maxsize_input2; input2++){
            for (unsigned int nibL = 0; nibL < 16; nibL++){
                for (unsigned int nibH = 0; nibH < 16; nibH++){
                    
                    inverseAbsorbNibble(nibL);
                    inverseAbsorbNibble(nibH);
                    
                    // todo calculate correlation, look how close to identity permutation
                    double corr = correlationCoefficient(n, identityPermutation, getCipher().S);
                    if (corr >= 0.955 ) {
                        printf("Near Collision found!\n");
                        //printf("corr: %f\n", corr);
                        for (unsigned int j= 0; j< n; j++) {
                            if(j > 15 && j%16 == 0)
                                printf("\n");
                            printf("%03d ", getCipher().S[j]);
                        }
                        printf("\n");
                    }
                    
                    if(memcmp(getCipher().S, identityPermutation, n) == 0){
                        printf("Collision found!\n");
                        // todo reconstruct input2
                        return 1;
                    }
                    if (input2 % (n/4) == 0) {
                        // revert
                        cipher _c;
                        _c.i = getCipher().i;
                        _c.j = getCipher().j;
                        _c.k = getCipher().k;
                        _c.a = getCipher().a;
                        _c.z = getCipher().z;
                        _c.w = getCipher().w;
                        memcpy(_c.S, permutationAfterInverseWhip, n);
                        setCipher(_c);
                        setN(n);
                    }
                }
            }
        }
        
    }
    
    return 0;
}

/*int searchForCycles(int n){
 int maxsize = (int)pow(2.0f, 25.0f);
 
 // generate n! starting states
 cipher c;
 c.i = 0;
 c.j = 0;
 c.k = 0;
 c.z = 0;
 c.a = 0;
 c.w = 1;
 
 std::vector<uint8_t> S;
 // S[N] = identity permutation
 for (unsigned int v = 0; v <= n-1; v++) {
 S.push_back(v);
 }
 
 int numberOfPermutations = 1;
 std::list<std::vector<uint8_t>>permutations;
 permutations.push_back(S);
 
 //calculate all possible permutations
 while ( std::next_permutation(S.begin(),S.begin()+n) ){
 permutations.push_back(S);
 numberOfPermutations++;
 }
 printf("number of permutations: %d\n", numberOfPermutations);
 
 int cnt = 0;
 for (std::list<std::vector<uint8_t>>::iterator it=permutations.begin(); it != permutations.end(); ++it) {
 c.S = &(*it)[0];
 setCipher(c);
 setN(n);
 
 int O[n];
 memset(O, 0, n*sizeof(int));
 for (unsigned int i = 0; i < maxsize; i++) {
 O[Drip()]++;
 }
 
 //if (_debug){
 //    for (unsigned int i = 0; i< n; i++) {
 //        printf("O[%d] = %d\n", i, O[i]);
 //    }
 //}
 
 double chiSquare = chiSquaredStatistic(n, maxsize, O);
 double degreeOfFreedom = n-1.0f;
 double standartDeviation = sqrt(2.0f*degreeOfFreedom);
 
 if (_debug) {
 printf("chiSquare: %f\n",chiSquare);
 printf("expected chiSquare: %f\n", degreeOfFreedom);
 printf("standart deviation: %f\n", standartDeviation);
 }
 
 if (chiSquare > (4.0f*standartDeviation)+degreeOfFreedom) {
 printf("*** BIAS detected! for permutation: %d\n", cnt);
 for (unsigned int i = 0; i< n; i++) {
 printf("O[%d] = %d\n", i, O[i]);
 }
 printf("****\n");
 //return -1;
 }
 cnt++;
 }
 
 return 0;
 }*/

int findCyclesInUpdate(int n){
    /*unsigned long long fact = factorial(n);
     //printf("%llu\n",fact);
     unsigned long long nToPowerOf3 = (unsigned long long)pow(n, 3.0f);
     //printf("%llu\n",nToPowerOf3);
     //printf("largest size for cycle lenght: %llu\n", fact * nToPowerOf3);*/
    unsigned long long maxsize = (unsigned long long)pow(2.0f, 16.0f);//nToPowerOf3 * fact;
    
    cipher c;
    c.a = 0;
    c.z = 0;
    c.w = 1;
    // change registers for all possible values
    c.i = 0;
    c.j = 0;
    c.k = 0;
    
    std::vector<uint8_t> S;
    // S[N] = identity permutation
    for (unsigned int v = 0; v <= n-1; v++) {
        S.push_back(v);
    }
    
    int numberOfPermutations = 1;
    std::list<std::vector<uint8_t>>permutations;
    permutations.push_back(S);
    
    //generate all possible permutations
    while ( std::next_permutation(S.begin(),S.begin()+n) && numberOfPermutations < 10000){
        permutations.push_back(S);
        numberOfPermutations++;
    }
    printf("number of permutations: %d\n", numberOfPermutations);
    
    unsigned long long shortestCycle = (unsigned long long)INFINITY;//maxsize;
    printf("largest size for cycle lenght: %llu\n", maxsize);
    for (std::list<std::vector<uint8_t>>::iterator it=permutations.begin(); it != permutations.end(); ++it) {
        c.S = &(*it)[0];
        setCipher(c);
        setN(n);
        
        //cipher initialState = getCipher();
        int cnt = 0;
        while (cnt < maxsize) {
            Update();
            cnt++;
            if (memcmp(&(*it)[0], getCipher().S, n) == 0 &&
                0 == getCipher().i &&
                0 == getCipher().j &&
                0 == getCipher().k) {
                /*printf("i:%d, j:%d, k:%d\n", initialState.i, initialState.j, initialState.k);
                 for(int x = 0; x < n; x++){
                 if(x > 15 && x%16 == 0)
                 printf("\n");
                 printf("%03d ", initialState.S[x]);
                 }
                 printf("\n");
                 printf("Found a cycle with length: %d\n", cnt);*/
                if (cnt < shortestCycle) {
                    shortestCycle = cnt;
                    if (shortestCycle == n) {
                        printf("Shortest cycle found: %llu\n", shortestCycle);
                        return 0;
                    }
                }
                break;
            }
        }
    }
    printf("Shortest cycle found: %llu\n", shortestCycle);
    
    return 0;
}

int findCyclesInDrip(int n){
    unsigned long long fact = factorial(n);
    //printf("%llu\n",fact);
    unsigned long long nToPowerOf4 = (unsigned long long)pow(n, 4.0f);
    //printf("%llu\n",nToPowerOf4);
    unsigned long long maxsize = nToPowerOf4 * fact;
    //printf("largest size for cycle lenght: %llu\n", fact * nToPowerOf4);
    //unsigned long long maxsize = (unsigned long long)pow(2.0f, 16.0f);
    
    cipher c;
    c.a = 0;
    c.w = 1;
    // change registers for all possible values
    c.z = 0;
    c.i = 0;
    c.j = 0;
    c.k = 0;
    
    std::vector<uint8_t> S;
    // S[N] = identity permutation
    for (unsigned int v = 0; v <= n-1; v++) {
        S.push_back(v);
    }
    
    int numberOfPermutations = 1;
    std::list<std::vector<uint8_t>>permutations;
    permutations.push_back(S);
    
    //generate all possible permutations
    while ( std::next_permutation(S.begin(),S.begin()+n) /*&& numberOfPermutations < maxsize*/){
        permutations.push_back(S);
        numberOfPermutations++;
    }
    printf("number of permutations: %d\n", numberOfPermutations);
    
    unsigned long long shortestCycle = maxsize;
    uint8_t permutationOfShortestCycle[n];
    memset(permutationOfShortestCycle, 0x00, n);
    
    printf("largest size for cycle lenght: %llu\n", maxsize);
    for (std::list<std::vector<uint8_t>>::iterator it=permutations.begin(); it != permutations.end(); ++it) {
        c.S = &(*it)[0];
        setCipher(c);
        setN(n);
        
        //debug();
        
        //cipher initialState = getCipher();
        int cnt = 0;
        while (cnt < maxsize) {
            Drip();
            cnt++;
            /*if (memcmp(initialState.S, getCipher().S, n) == 0 &&
             initialState.i == getCipher().i &&
             initialState.j == getCipher().j &&
             initialState.k == getCipher().k &&
             initialState.z == getCipher().z) {*/
            if (memcmp(&(*it)[0], getCipher().S, n) == 0 &&
                0 == getCipher().i &&
                0 == getCipher().j &&
                0 == getCipher().k &&
                0 == getCipher().z) {
                /*printf("i:%d, j:%d, k:%d\n", initialState.i, initialState.j, initialState.k);
                 for(int x = 0; x < n; x++){
                 if(x > 15 && x%16 == 0)
                 printf("\n");
                 printf("%03d ", initialState.S[x]);
                 }
                 printf("\n");
                 printf("Found a cycle with length: %d\n", cnt);*/
                if (cnt < shortestCycle) {
                    shortestCycle = cnt;
                    memcpy(permutationOfShortestCycle, &(*it)[0], n);
                    
                    /*if (shortestCycle <= 10*n) {
                     printf("i:%d, j:%d, k:%d, z:%d\n", getCipher().i, getCipher().j, getCipher().k, getCipher().z);
                     for(int x = 0; x < n; x++){
                     if(x > 15 && x%16 == 0)
                     printf("\n");
                     printf("%03d ", getCipher().S[x]);
                     }
                     printf("\n");
                     printf("Shortest cycle found: %llu\n", shortestCycle);
                     return 0;
                     }*/
                }
                break;
            }
        }
    }
    printf("Shortest cycle found: %llu\n", shortestCycle);
    for(int x = 0; x < n; x++){
        if(x > 15 && x%16 == 0)
            printf("\n");
        printf("%03d ", permutationOfShortestCycle[x]);
    }
    printf("\n");
    
    return 0;
}

void *searchForCycles(void *threadarg)
{
    // get input data
    struct thread_data *data;
    data = (struct thread_data *) threadarg;
    
    printf("Thread: %d: start working...\n", data->thread_id);
    printf("Thread: %d: start: %d, end: %d\n", data->thread_id, data->start, data->end);
    
    // let thread sleep for 1 second so that other threads can startup
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // do stuff
    spritzClass *spritz = new spritzClass();
    spritz->InitializeState();
    spritz->setN(data->n);
    
    unsigned long long shortestCycle = data->maxsize;
    uint8_t permutationOfShortestCycle[data->n];
    memset(permutationOfShortestCycle, 0x00, data->n);
    
    for (std::vector<std::vector<uint8_t>>::iterator it=data->permutations.begin()+data->start; it != data->permutations.begin()+data->end; ++it) {
        spritz->setS(&(*it)[0]);
        
        /*printf("Thread: %d: ", data->thread_id);
         for(int x = 0; x < data->n; x++){
         if(x > 15 && x%16 == 0)
         printf("\n");
         printf("%03d ", c.S[x]);
         }
         printf("\n");*/
        
        int cnt = 0;
        while (cnt < data->maxsize) {
            spritz->Drip();
            cnt++;
            
            if (memcmp(&(*it)[0], spritz->getS(), data->n) == 0 &&
                0 == spritz->getI() &&
                0 == spritz->getJ() &&
                0 == spritz->getK() &&
                0 == spritz->getZ()) {
                // Found cycle -> check if its the smallest cycle
                //printf("cnt: %d\n", cnt);
                
                if (cnt < shortestCycle) {
                    shortestCycle = cnt;
                    memcpy(permutationOfShortestCycle, &(*it)[0], data->n);
                    
                    // if (shortestCycle <= 10*n) {
                    //     printf("i:%d, j:%d, k:%d, z:%d\n", getCipher().i, getCipher().j, getCipher().k, getCipher().z);
                    //     for(int x = 0; x < n; x++){
                    //         if(x > 15 && x%16 == 0)
                    //             printf("\n");
                    //         printf("%03d ", getCipher().S[x]);
                    //     }
                    //     printf("\n");
                    //     printf("Shortest cycle found: %llu\n", shortestCycle);
                    //     return 0;
                    // }
                }
                break;
            }
        }
    }
    
    if (shortestCycle < data->maxsize) {
        printf("Thread: %d: Shortest cycle found: %llu\n", data->thread_id, shortestCycle);
        for(int x = 0; x < data->n; x++){
            if(x > 15 && x%16 == 0)
                printf("\n");
            printf("%03d ", permutationOfShortestCycle[x]);
        }
        printf("\n");
    } else {
        printf("Thread %d: No short cycle found\n", data->thread_id);
    }
    
    delete spritz;
    
    // exit thread
    pthread_exit(NULL);
}

struct thread_data thread_data_array[NUM_THREADS];

int findCyclesInDripThreaded(int n){
    /*unsigned long long fact = factorial(n);
     //printf("%llu\n",fact);
     unsigned long long nToPowerOf4 = (unsigned long long)pow(n, 4.0f);
     //printf("%llu\n",nToPowerOf4);
     unsigned long long maxsize = nToPowerOf4 * fact;
     //printf("largest size for cycle lenght: %llu\n", fact * nToPowerOf4);*/
    unsigned long long maxsize = (unsigned long long)pow(2.0f, 15.0f);
    
    std::vector<uint8_t> S;
    // S[N] = identity permutation
    for (unsigned int v = 0; v <= n-1; v++) {
        S.push_back(v);
    }
    
    int numberOfPermutations = 1;
    std::vector<std::vector<uint8_t>>permutations;
    permutations.push_back(S);
    
    //generate all possible permutations
    while ( std::next_permutation(S.begin(),S.begin()+n) && numberOfPermutations < maxsize){
        permutations.push_back(S);
        numberOfPermutations++;
    }
    printf("number of permutations: %d\n", numberOfPermutations);
    
    // create threads - do threat stuff
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    int rc;
    void *status;
    
    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    for(unsigned int t=0; t<NUM_THREADS; t++){
        //set data for thread
        thread_data_array[t].thread_id = t;
        thread_data_array[t].start = (t*floor(numberOfPermutations/NUM_THREADS));
        thread_data_array[t].end = (t*floor(numberOfPermutations/NUM_THREADS))+(floor(numberOfPermutations/NUM_THREADS));
        thread_data_array[t].permutations = permutations;
        thread_data_array[t].maxsize = maxsize;
        thread_data_array[t].n = n;
        
        //create thread
        printf("findCyclesInDripThreaded: creating thread %d\n", t);
        rc = pthread_create(&threads[t], &attr, searchForCycles, (void *) &thread_data_array[t]);
        
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    // end threads
    
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for(unsigned int t=0; t<NUM_THREADS; t++) {
        rc = pthread_join(threads[t], &status);
        if (rc){
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %d having a status of %ld\n",t,(long)status);
    }
    
    /* Last thing that main() should do */
    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL);
    
    return 0;
}

int verifyCycle(int n){
    cipher badCipher;
    setN(n);
    badCipher.i = 0;
    badCipher.j = 0;
    badCipher.k = 0;
    badCipher.a = 0;
    badCipher.z = 0;
    badCipher.w = 1;
    
    /*uint8_t weakPermutation[256] = {88, 66, 21, 13, 190, 54, 123, 63, 130, 252, 23, 61, 167, 4, 125, 74,
     31, 197, 250, 198, 245, 192, 121, 91, 48, 79, 33, 73, 149, 89, 210, 113,
     6, 205, 58, 161, 18, 119, 242, 214, 131, 220, 68, 227, 96, 64, 204, 10,
     49, 77, 182, 16, 180, 215, 241, 251, 132, 127, 246, 27, 163, 110, 217, 99,
     70, 230, 11, 176, 224, 35, 162, 17, 34, 102, 151, 75, 207, 112, 158, 82,
     90, 118, 100, 186, 194, 139, 84, 80, 244, 254, 51, 136, 140, 81, 171, 231,
     8, 188, 134, 196, 103, 106, 229, 243, 152, 144, 122, 185, 240, 37, 148, 154,
     137, 65, 237, 233, 22, 172, 157, 111, 175, 232, 105, 153, 253, 155, 94, 143,
     30, 40, 3, 200, 174, 181, 115, 203, 146, 216, 41, 195, 239, 156, 92, 29,
     104, 208, 191, 26, 59, 20, 129, 42, 109, 45, 142, 201, 85, 255, 56, 36,
     178, 69, 108, 169, 7, 28, 228, 14, 160, 62, 39, 248, 177, 236, 187, 138,
     98, 107, 249, 52, 173, 86, 43, 72, 5, 78, 170, 166, 235, 24, 202, 213,
     184, 211, 209, 87, 101, 159, 206, 116, 76, 218, 225, 120, 179, 114, 247, 226,
     1, 47, 53, 57, 221, 55, 38, 60, 147, 124, 150, 19, 95, 117, 67, 193,
     126, 2, 50, 71, 238, 141, 164, 97, 133, 168, 12, 234, 145, 212, 183, 223,
     9, 135, 44, 189, 0, 15, 219, 199, 93, 128, 83, 32, 165, 222, 46, 25};*/
    
    //uint8_t weakPermutation[16] = {13, 2, 5, 4, 10, 6, 11, 12, 0, 9, 14, 1, 3, 8, 15, 7};
    uint8_t weakPermutation[8] = {1, 0, 7, 6, 5, 4, 3, 2};
    
    //uint8_t weakPermutation[5] = {0,3,4,1,2};
    
    badCipher.S = weakPermutation;
    
    setCipher(badCipher);
    setN(n);
    debug();
    for (unsigned int i = 0; i < n; i++) {
        printf("***************\n");
        Drip();
        debug();
    }
    return 0;
}

int partialStateRotations(int n){
    cipher c;
    setN(n);
    c.i = 0;
    c.j = 0;
    c.k = 0;
    c.a = 0;
    c.z = 0;
    c.w = 1;
    srand (time(NULL));
    
    c.S = (uint8_t*) malloc (n*sizeof(uint8_t));
    for (unsigned int v = 0; v <= n-1; v++) {
        c.S[v] = static_cast<uint8_t>(rand()%n);
    }
    
    setCipher(c);
    //InitializeState();
    
    debug();
    
    uint8_t m1[n/4];
    memset(m1, 0x00, n/4);
    
    Absorb(m1, n/4);
    debug();
    
    return 0;
}

int partialStateRotations2(int n){
    cipher c;
    setN(n);
    c.i = 0;
    c.j = 0;
    c.k = 0;
    c.a = 0;
    c.z = 0;
    c.w = 1;
    srand (time(NULL));
    
    c.S = (uint8_t*) malloc (n*sizeof(uint8_t));
    for (unsigned int v = 0; v <= n-1; v++) {
        c.S[v] = v;//static_cast<uint8_t>(rand()%n);
    }
    
    setCipher(c);
    //InitializeState();
    
    debug();
    
    uint8_t m1[n/4];
    memset(m1, 0x00, n/4);
    
    Absorb(m1, n/4);
    debug();
    
    return 0;
}

int cycleWhenNoSwapIsDone(int n){
    InitializeState();
    /*uint8_t input[n];
     memset(input, 0x00, n);
     Absorb(input, n);*/
    for (unsigned int i = 0; i < 100*n; i++) {
        if(i > 15 && i%16 == 0)
            printf("\n");
        if(i > (6*n)-1 && i%(6*n) == 0 )
            printf("\n");
        /*if(i > (n-1) && i%n == 0 )
         printf("\n");*/
        Update_reducedSwapFrequency(0);
        printf("%03d ", Output());
    }
    printf("\n");
    return 0;
}

int valuesForStateRecovery(int n){
    InitializeState();
    /*uint8_t key[16] = {0x42};
     Absorb(key, 16);
     Shuffle();*/
    debug();
    for (unsigned int i = 0; i < 2*n; i++) {
        printf("%d \n", Drip());
        //Drip();
        //debug2();
    }
    printf("\n");
    
    return 0;
}

int initializeRecovery(int n, int step, struct state *s){
    //step = 2;	// to save memory
    s->S = (int8_t**)malloc(step*sizeof(int8_t*));
    for (unsigned int i = 0; i < step; i++) {
        s->S[i] = (int8_t*)malloc(n*sizeof(int8_t*) );
    }
    
    for (unsigned int i = 0; i < step; i++) {
        for (unsigned int j = 0; j < n; j++) {
            s->S[i][j] = -1;	//initialize state array with all -1
        }
    }
    
    s->i = (uint8_t*) malloc(step*sizeof(uint8_t));
    s->j = (uint8_t*) malloc(step*sizeof(uint8_t));
    s->k = (uint8_t*) malloc(step*sizeof(uint8_t));
    s->z = (uint8_t*) malloc((/*2*n*/n+3)*sizeof(uint8_t));
    
    return 0;
}

int recoverState(int n){
    //int maxSteps = n;
    struct state s;
    
    //s.z = (uint8_t*) malloc(maxSteps*sizeof(uint8_t));
    initializeRecovery(n, 1, &s);
    
    s.i[0] = 0;
    s.j[0] = 0;
    s.k[0] = 0;
    s.z[0] = 0;
    s.w = 1;
    
    //insert known values
    //N = 64
    s.z[1] = 4;
    s.z[2] = 20;
    s.z[3] = 2;
    s.z[4] = 11;
    s.z[5] = 54;
    s.z[6] = 52;
    s.z[7] = 3;
    s.z[8] = 8;
    s.z[9] = 55;
    s.z[10] = 11;
    s.z[11] = 13;
    s.z[12] = 13;
    s.z[13] = 52;
    s.z[14] = 11;
    s.z[15] = 42;
    s.z[16] = 42;
    s.z[17] = 22;
    s.z[18] = 61;
    s.z[19] = 18;
    s.z[20] = 10;
    s.z[21] = 14;
    s.z[22] = 19;
    s.z[23] = 63;
    s.z[24] = 46;
    s.z[25] = 13;
    s.z[26] = 31;
    s.z[27] = 16;
    s.z[28] = 46;
    s.z[29] = 2;
    s.z[30] = 53;
    s.z[31] = 30;
    s.z[32] = 22;
    s.z[33] = 8;
    s.z[34] = 59;
    s.z[35] = 32;
    s.z[36] = 29;
    s.z[37] = 52;
    s.z[38] = 0;
    s.z[39] = 55;
    
    s.z[40] = 7;
    s.z[41] = 10;
    s.z[42] = 12;
    s.z[43] = 2;
    s.z[44] = 19;
    s.z[45] = 9;
    s.z[46] = 20;
    s.z[47] = 11;
    s.z[48] = 5;
    s.z[49] = 16;
    s.z[50] = 20;
    s.z[51] = 17;
    s.z[52] = 27;
    s.z[53] = 23;
    s.z[54] = 18;
    s.z[55] = 25;
    s.z[56] = 25;
    s.z[57] = 9;
    s.z[58] = 7;
    s.z[59] = 28;
    s.z[60] = 7;
    s.z[61] = 10;
    s.z[62] = 12;
    s.z[63] = 2;
    s.z[64] = 19;	//additional 3 values to sort out wrong solutions
    s.z[65] = 9;
    s.z[66] = 20;
    
    //N = 32
    s.z[1] = 4;
    s.z[2] = 20;
    s.z[3] = 23;
    s.z[4] = 0;
    s.z[5] = 25;
    s.z[6] = 28;
    s.z[7] = 25;
    s.z[8] = 30;
    s.z[9] = 19;
    s.z[10] = 22;
    s.z[11] = 2;
    s.z[12] = 0;
    s.z[13] = 14;
    s.z[14] = 31;
    s.z[15] = 7;
    s.z[16] = 10;
    s.z[17] = 12;
    s.z[18] = 2;
    s.z[19] = 19;
    s.z[20] = 9;
    s.z[21] = 20;
    s.z[22] = 11;
    s.z[23] = 5;
    s.z[24] = 16;
    s.z[25] = 20;
    s.z[26] = 17;
    s.z[27] = 27;
    s.z[28] = 23;
    s.z[29] = 18;
    s.z[30] = 25;
    s.z[31] = 25;
    s.z[32] = 9;	//additional 3 values to sort out wrong solutions
    s.z[33] = 7;
    s.z[34] = 28;
    
    //N = 16
    /*s.z[1] = 4;
    s.z[2] = 4;
    s.z[3] = 7;
    s.z[4] = 10;
    s.z[5] = 15;
    s.z[6] = 11;
    s.z[7] = 14;
    s.z[8] = 13;
    s.z[9] = 2;
    s.z[10] = 2;
    s.z[11] = 4;
    s.z[12] = 13;
    s.z[13] = 6;
    s.z[14] = 14;
    s.z[15] = 13;
    s.z[16] = 6;
    s.z[17] = 14;
    s.z[18] = 3;
    s.z[19] = 1;
    s.z[20] = 1;
    s.z[21] = 1;
    s.z[22] = 1;
    s.z[23] = 12;
    s.z[24] = 14;
    s.z[25] = 5;
    s.z[26] = 1;
    s.z[27] = 3;
    s.z[28] = 1;
    s.z[29] = 13;
    s.z[30] = 14;
    s.z[31] = 2;*/
    //s.z[32] = 10;
    
    //N = 8
    /*s.z[1] = 4;
     s.z[2] = 1;
     s.z[3] = 4;
     s.z[4] = 0;
     s.z[5] = 1;
     s.z[6] = 7;
     s.z[7] = 7;*/
    
    //N = 4
    /*s.z[1] = 0;
     s.z[2] = 2;
     s.z[3] = 2;
     s.z[4] = 1;*/
    
    s.S[0][0] = 0;
    s.S[0][1] = 1;
    s.S[0][2] = 2;
    s.S[0][3] = 3;
    s.S[0][4] = 4;
    s.S[0][5] = 5;
    s.S[0][6] = 6;
    s.S[0][7] = 7;
    s.S[0][8] = 8;
    s.S[0][9] = 9;
    s.S[0][10] = 10;
    s.S[0][11] = 11;
	s.S[0][12] = 12;
    s.S[0][13] = 13;
    //s.S[0][14] = 14;
    //s.S[0][15] = 15;
    //s.S[0][16] = 16;
    //s.S[0][17] = 17;
	//s.S[0][18] = 18;
    //s.S[0][19] = 19;
    //s.S[0][20] = 20;
    //s.S[0][21] = 21;
    //s.S[0][22] = 22;
    //s.S[0][23] = 23;
    //s.S[0][24] = 24;
    //s.S[0][25] = 25;
    //s.S[0][26] = 26;
    //s.S[0][27] = 27;
    //s.S[0][28] = 28;
    //s.S[0][29] = 29;
    //s.S[0][30] = 30;
    //s.S[0][31] = 31;
    
    /*for (unsigned int i = 12; i < n; i++) {
     s.possibleEntries.push_back(i);
     }*/
    
    printRecoveredState(n, 0, &s);
    
    //s.direction = 1;
    //recursiveRecoverState(n, 1, &s);
    //recursiveRecoverState2(n, 1, &s);
    //recursiveRecoverState3(n, 0, &s, std::vector<uint8_t>());
    //int returnvalue = recursiveRecoverState4(n, 1, &s, std::vector<uint8_t>());
    int returnvalue = recursiveRecoverState5(n, 1, &s);
    printf("returnvalue: %d\n", returnvalue);
    
    return 0;
}

int recoverState2(int n, int a){
    struct state s;
    initializeRecovery(n, 1, &s);
    
    //insert known values
    InitializeState();
    srand(time(NULL));
    uint8_t key[16];
    for(unsigned int i = 0; i < 16; i++){
        memset(key+i, rand()%256, 1);
        //printf("%02x ", key[i]);
    }
    //printf("\n");
    
    Absorb(key, 16);
    Shuffle();
    
    //assume we know the initial values of the registers
    s.i[0] = getCipher().i;
    s.j[0] = getCipher().j;
    s.k[0] = getCipher().k;
    s.z[0] = getCipher().z;
    s.w = getCipher().w;
    
    debug();
    for(unsigned int i = 0; i < a; i++){
        s.S[0][i] = getCipher().S[i];
        /*if(i > 15 && i%16 == 0)
            printf("\n");
        printf("%03d ", s.S[0][i]);*/
    }
    //printf("\n");
    
    //printf("%d \n", s.z[0]);
    for (unsigned int i = 1; i < n+3; i++) {
        s.z[i] = Drip();
        //printf("%d \n", s.z[i]);
    }
    
    
    printRecoveredState(n, 0, &s);
    
    //start state recovery
    int returnvalue = recursiveRecoverState5(n, 1, &s);
    printf("returnvalue: %d\n", returnvalue);
    
    return returnvalue;
}

/*int recursiveRecoverState(int n, int step, struct state *_s){
 printf("step: %d\n", step);
 //TODO abbruchbedingung
 if (step > n || step < 1) {
 printf("step outside of range!\n");
 return 0;
 }
 
 if (_s->possibleEntries.size() == 0) {
 printf("no entries avaliable\n");
 return 0;
 }
 
 //initialize state
 struct state s;
 initializeRecovery(n, step+1, &s);
 
 for (unsigned int i = 0; i <= step; i++) {
 if (i == step) {
 memcpy(s.S[i], _s->S[i-1], n);
 break;
 }
 memcpy(s.S[i], _s->S[i], n);
 }
 
 memcpy(s.i, _s->i, step);
 memcpy(s.j, _s->j, step);
 memcpy(s.k, _s->k, step);
 memcpy(s.z, _s->z, n);
 s.possibleEntries = _s->possibleEntries;
 s.direction = _s->direction;
 printf("start recovery\n");
 
 //i_t = i_{t-1} + w
 s.i[step] = (s.i[step-1] + 1)%n;
 
 if (s.S[step-1][s.i[step]] == -1) {
 //guess S_{t-1}[i_t]
 s.S[step-1][s.i[step]] = guessPermutationEntry(n, step-1, &s);
 }
 if (s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] == -1) {
 //guess S_{t-1}[j_{t-1} + S_{t-1}[i_t]]
 s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] = guessPermutationEntry(n, step-1, &s);
 }
 
 //j_t = k_{t-1} + S_{t-1}[j_{t-1} + S_{t-1}[i_t]]
 s.j[step] = (s.k[step-1] + s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n])%n;
 
 if (s.S[step-1][s.j[step]] == -1) {
 //guess S_{t-1}[j_t]
 s.S[step-1][s.j[step]] = guessPermutationEntry(n, step-1, &s);
 }
 
 //k_t = i_t + k_{t-1} + S_{t-1}[j_t]
 s.k[step] = (s.i[step] + (s.k[step-1] + s.S[step-1][s.j[step]])%n)%n;
 
 //Swap S_t[i_t], S_t[j_t]
 s.S[step][s.i[step]] = s.S[step-1][s.j[step]];
 s.S[step][s.j[step]] = s.S[step-1][s.i[step]];
 
 if (s.S[step][(s.z[step-1] + s.k[step])%n] == -1) {
 //guess S_t[z_{t-1}+k_t]
 s.S[step][(s.z[step-1] + s.k[step])%n] = guessPermutationEntry(n, step, &s);
 }
 if (s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] == -1) {
 //guess S_t[i_t + S_t[z_{t-1} + k_t]]
 s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] = guessPermutationEntry(n, step, &s);
 }
 
 //z' =  S_t[j_t + S_t[i_t + S_t[z_{t-1} + k_t]]]
 uint8_t _z = s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n];
 
 //TODO if _z != z -> contradiction
 //if (_z != s.z[step]) {
 //    printf("z_new != z\n");
 //    return recursiveRecoverState(n, step, &(*_s));	//go one step backward
 //}
 
 for (unsigned int i = 0; i < n; i++) {
 if (_z == s.S[step][i]) {
 if ((s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n != i) {
 // contradiction
 printf("contradiction! 1\n");
 if (!s.direction) {
 step -= 1;
 return recursiveRecoverState(n, step, &(*_s));
 }
 if (_s->possibleEntries.size() == 0) {
 return 0;
 }
 s.direction = 0;
 return recursiveRecoverState(n, step, &(*_s));	//go one step backward
 } else {
 //todo do something
 }
 } else {
 if ((s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n == i) {
 //contradiction
 printf("contradiction! 2\n");
 if (!s.direction) {
 step -= 1;
 return recursiveRecoverState(n, step, &(*_s));
 }
 if (_s->possibleEntries.size() == 0) {
 return 0;
 }
 s.direction = 0;
 return recursiveRecoverState(n, step, &(*_s));	//go one step backward
 }
 }
 }
 
 s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n] = _z;
 
 for (unsigned int i = 0; i <= step; i++) {
 printRecoveredState(n, i, &s);
 }
 
 //recursive forward
 s.direction = 1;
 return recursiveRecoverState(n, step+1, &s);
 }*/
/*
 int recursiveRecoverState2(int n, int step, struct state *_s){
 printf("step: %d\n", step);
 //TODO abbruchbedingung
 if (step > n || step < 1) {
 printf("step outside of range!\n");
 return 0;
 }
 
 if (_s->possibleEntries.size() == 0) {
 printf("no entries avaliable\n");
 return 0;
 }
 
 //initialize state
 struct state s;
 initializeRecovery(n, step+1, &s);
 
 for (unsigned int i = 0; i <= step; i++) {
 if (i == step) {
 memcpy(s.S[i], _s->S[i-1], n);
 break;
 }
 memcpy(s.S[i], _s->S[i], n);
 }
 
 memcpy(s.i, _s->i, step);
 memcpy(s.j, _s->j, step);
 memcpy(s.k, _s->k, step);
 memcpy(s.z, _s->z, n);
 s.possibleEntries = _s->possibleEntries;
 //s.direction = _s->direction;
 printf("start recovery\n");
 
 //i_t = i_{t-1} + w
 s.i[step] = (s.i[step-1] + 1)%n;
 
 if (s.S[step-1][s.i[step]] == -1) {
 //guess S_{t-1}[i_t]
 s.S[step-1][s.i[step]] = guessPermutationEntry(n, step-1, &s);
 }
 if (s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] == -1) {
 //guess S_{t-1}[j_{t-1} + S_{t-1}[i_t]]
 s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] = guessPermutationEntry(n, step-1, &s);
 }
 
 //j_t = k_{t-1} + S_{t-1}[j_{t-1} + S_{t-1}[i_t]]
 s.j[step] = (s.k[step-1] + s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n])%n;
 
 if (s.S[step-1][s.j[step]] == -1) {
 //guess S_{t-1}[j_t]
 s.S[step-1][s.j[step]] = guessPermutationEntry(n, step-1, &s);
 }
 
 //k_t = i_t + k_{t-1} + S_{t-1}[j_t]
 s.k[step] = (s.i[step] + (s.k[step-1] + s.S[step-1][s.j[step]])%n)%n;
 
 //Swap S_t[i_t], S_t[j_t]
 s.S[step][s.i[step]] = s.S[step-1][s.j[step]];
 s.S[step][s.j[step]] = s.S[step-1][s.i[step]];
 
 if (s.S[step][(s.z[step-1] + s.k[step])%n] == -1) {
 //guess S_t[z_{t-1}+k_t]
 s.S[step][(s.z[step-1] + s.k[step])%n] = guessPermutationEntry(n, step, &s);
 }
 if (s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] == -1) {
 //guess S_t[i_t + S_t[z_{t-1} + k_t]]
 s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] = guessPermutationEntry(n, step, &s);
 }
 
 //z' =  S_t[j_t + S_t[i_t + S_t[z_{t-1} + k_t]]]
 uint8_t _z = s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n];
 
 //TODO if _z != z -> contradiction
 //if (_z != s.z[step]) {
 // printf("z_new != z\n");
 // return recursiveRecoverState(n, step, &(*_s));	//go one step backward
 // }
 
 for (unsigned int i = 0; i < n; i++) {
 if (_z == s.S[step][i]) {
 if ((s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n != i) {
 // contradiction
 printf("contradiction! 1\n");
 //if (!s.direction) {
 //    step -= 1;
 //    return recursiveRecoverState2(n, 1, &(*_s));
 //}
 if (_s->possibleEntries.size() == 0) {
 return 0;
 }
 //s.direction = 0;
 return recursiveRecoverState2(n, 1, &(*_s));	//go one step backward
 } else {
 //todo do something
 }
 } else {
 if ((s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n == i) {
 //contradiction
 printf("contradiction! 2\n");
 //if (!s.direction) {
 //    step -= 1;
 //    return recursiveRecoverState2(n, 1, &(*_s));
 //}
 if (_s->possibleEntries.size() == 0) {
 return 0;
 }
 //s.direction = 0;
 return recursiveRecoverState2(n, 1, &(*_s));	//go one step backward
 }
 }
 }
 
 s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n] = _z;
 
 for (unsigned int i = 0; i <= step; i++) {
 printRecoveredState(n, i, &s);
 }
 
 //recursive forward
 //s.direction = 1;
 return recursiveRecoverState2(n, step+1, &s);
 }*/

int recursiveRecoverState3(int n, int step, struct state *_s, std::vector<uint8_t>prevGuessedValues){
    
    //if accept() then output
    /*std::set<uint8_t>currentPermutation(reinterpret_cast<uint8_t>((*_s).S[step]));
     if (currentPermutation.size() == n) {
     printf("full permutation with all unique elements\n");
     //todo -> calculate back to initialState
     return 0;
     }*/
    if (step+1 == 5*n) {
        printf("full permutation with all unique elements\n");
        exit(0);
        return 0;
    }
    
    if((*_s).possibleEntries.size() < 1){
        printf("no possible entries availabel\n");
        exit(0);
        return 0;
    }
    
    //initialize state
    struct state s;
    initializeRecovery(n, step+2, &s);
    for (unsigned int i = 0; i <= step+1; i++) {
        if (i == step+1) {
            memcpy(s.S[i], _s->S[i-1], n);
            break;
        }
        memcpy(s.S[i], _s->S[i], n);
    }
    memcpy(s.i, _s->i, step+1);
    memcpy(s.j, _s->j, step+1);
    memcpy(s.k, _s->k, step+1);
    memcpy(s.z, _s->z, n);
    s.possibleEntries = _s->possibleEntries;
    
    
    printRecoveredState(n, step, &s);
    
    //printf("start recovery\n");
    
    //s <- first()
    step = reduceSearchSpace(&s, prevGuessedValues, step);
    printf("step: %d\n", step);
    //printRecoveredState(n, step, &s);
    
    //while s != ^ do
    int siblingCount = 0;
    while (siblingCount < (int)pow((double)s.possibleEntries.size(), 5.0f)) {
        //printf("count: %d\n", childCount);
        
        struct state s_sibling;
        initializeRecovery(n, step+1, &s_sibling);
        memcpy(s_sibling.S, s.S, step*n);
        memcpy(s_sibling.i, s.i, step);
        memcpy(s_sibling.j, s.j, step);
        memcpy(s_sibling.k, s.k, step);
        memcpy(s_sibling.z, s.z, n);
        s_sibling.possibleEntries = s.possibleEntries;
        
        //s <- next()
        uint8_t z_n = 0;
        int valueExists = 0;
        std::vector<uint8_t>guessedValues = simulateUpdateAndOutput(n, step, &s_sibling, &z_n, &siblingCount, &valueExists);
        siblingCount++;
        
        //if reject() then return
        if (z_n != s_sibling.z[step]) {
            printf("z' != z\n");
            continue;
        }
        else{
            printf("i: %d, j: %d, k: %d\n", s_sibling.i[step+1], s_sibling.j[step-1], s_sibling.k[step]);
        }
        if (valueExists) {
            printf("value exists\n");
            continue;
        }
        for (unsigned int i = 0; i < n; i++) {	//loop through permutation S
            if (z_n == s_sibling.S[step][i]) {	//check if new z_n already exists somewhere
                if ((s_sibling.j[step] + s_sibling.S[step][(s_sibling.i[step] + s_sibling.S[step][(s_sibling.z[step-1] + s_sibling.k[step])%n])%n])%n != i) {	//index of new z_n is equal to already known value
                    printf("contradition\n");
                    continue;	//contradiction
                }
            } else {  //new z_n does not already exist in S
                if ((s_sibling.j[step] + s_sibling.S[step][(s_sibling.i[step] + s_sibling.S[step][(s_sibling.z[step-1] + s_sibling.k[step])%n])%n])%n == i) {	// index for new z_n points to known value
                    printf("contradiction\n");
                    continue;	//contradiction
                }
            }
        }
        
        printRecoveredState(n, step, &s_sibling);
        //recursion()
        recursiveRecoverState3(n, step, &s_sibling, guessedValues);
    }
    
    return 0;
}

// first()
int reduceSearchSpace(struct state *s, std::vector<uint8_t>guessedValues, int step){
    for (unsigned int i = 0; i < guessedValues.size(); i++) {
        for (unsigned int j = 0; j < s->possibleEntries.size(); j++) {
            if (guessedValues[i] == s->possibleEntries[j]) {
                //remove all previously guessed values from possible values
                s->possibleEntries.erase(s->possibleEntries.begin()+j);
            }
        }
    }
    return step+1;	//increase step by one for next child
}

// next()
std::vector<uint8_t> simulateUpdateAndOutput(int n, int step, struct state *s, uint8_t* z_n, int* currentSibling, int *valueExists){
    std::vector<uint8_t>guessedValues;
    
    //i_t = i_{t-1} + w
    s->i[step] = (s->i[step-1] + 1)%n;
    
    if (s->S[step-1][s->i[step]] == -1) {
        //guess S_{t-1}[i_t]
        printf("guess Si\n");
        uint8_t value = guessPermutationEntry(n, step-1, &(*s), &(*currentSibling), 1);
        if (checkIfValueAlreadyExistsInPermutation(value, n, step, &(*s))){
            *valueExists = 1;
            return guessedValues;
        }
        s->S[step-1][s->i[step]] = value;
        guessedValues.push_back(value);
    }
    if (s->S[step-1][(s->j[step-1] + s->S[step-1][s->i[step]])%n] == -1) {
        //guess S_{t-1}[j_{t-1} + S_{t-1}[i_t]]
        printf("guess S(j+Si)\n");
        uint8_t value = guessPermutationEntry(n, step-1, &(*s), &(*currentSibling), 2);
        if (checkIfValueAlreadyExistsInPermutation(value, n, step, &(*s))){
            *valueExists = 1;
            return guessedValues;
        }
        s->S[step-1][(s->j[step-1] + s->S[step-1][s->i[step]])%n] = value;
        guessedValues.push_back(value);
    }
    
    //j_t = k_{t-1} + S_{t-1}[j_{t-1} + S_{t-1}[i_t]]
    s->j[step] = (s->k[step-1] + s->S[step-1][(s->j[step-1] + s->S[step-1][s->i[step]])%n])%n;
    
    if (s->S[step-1][s->j[step]] == -1) {
        //guess S_{t-1}[j_t]
        printf("guess Sj\n");
        uint8_t value = guessPermutationEntry(n, step-1, &(*s), &(*currentSibling), 3);
        if (checkIfValueAlreadyExistsInPermutation(value, n, step, &(*s))){
            *valueExists = 1;
            return guessedValues;
        }
        s->S[step-1][s->j[step]] = value;
        guessedValues.push_back(value);
    }
    
    //k_t = i_t + k_{t-1} + S_{t-1}[j_t]
    s->k[step] = (s->i[step] + (s->k[step-1] + s->S[step-1][s->j[step]])%n)%n;
    
    //Swap S_t[i_t], S_t[j_t]
    s->S[step][s->i[step]] = s->S[step-1][s->j[step]];
    s->S[step][s->j[step]] = s->S[step-1][s->i[step]];
    
    if (s->S[step][(s->z[step-1] + s->k[step])%n] == -1) {
        //guess S_t[z_{t-1}+k_t]
        printf("guess S(z+k)\n");
        uint8_t value = guessPermutationEntry(n, step, &(*s), &(*currentSibling), 4);
        if (checkIfValueAlreadyExistsInPermutation(value, n, step, &(*s))){
            *valueExists = 1;
            return guessedValues;
        }
        s->S[step][(s->z[step-1] + s->k[step])%n] = value;
        guessedValues.push_back(value);
    }
    if (s->S[step][(s->i[step] + s->S[step][(s->z[step-1] + s->k[step])%n])%n] == -1) {
        //guess S_t[i_t + S_t[z_{t-1} + k_t]]
        printf("guess S(i+S(z+k)) = S(%d) = i: %d z: %d k: %d j: %d S(z+k): %d\n", s->i[step] + s->S[step][(s->z[step-1] + s->k[step])%n], s->i[step], s->z[step-1], s->k[step], s->j[step], s->S[step][(s->z[step-1] + s->k[step])%n]);
        uint8_t value = guessPermutationEntry(n, step, &(*s), &(*currentSibling), 5);
        printf("guessed value: %d\n", value);
        if (checkIfValueAlreadyExistsInPermutation(value, n, step, &(*s))){
            *valueExists = 1;
            return guessedValues;
        }
        s->S[step][(s->i[step] + s->S[step][(s->z[step-1] + s->k[step])%n])%n] = value;
        guessedValues.push_back(value);
    }
    
    //z' =  S_t[j_t + S_t[i_t + S_t[z_{t-1} + k_t]]]
    *z_n = s->S[step][(s->j[step] + s->S[step][(s->i[step] + s->S[step][(s->z[step-1] + s->k[step])%n])%n])%n];
    printf("z_n: %d", s->S[step][(s->j[step] + s->S[step][(s->i[step] + s->S[step][(s->z[step-1] + s->k[step])%n])%n])%n]);
    
    return guessedValues;
}

int guessPermutationEntry(int n, int step, struct state *s, int *currentSibling, int pos){
    /*srand(time(NULL));
     std::list<uint8_t>::iterator it = s->possibleEntries.begin();
     //take randomly one entry of remaining possible values
     std::advance (it,rand()%s->possibleEntries.size());
     int value = (*it);
     s->possibleEntries.erase(it);*/
    return s->possibleEntries[((int)floor(*currentSibling/((int)pow(s->possibleEntries.size(), pos)))) % ((int)s->possibleEntries.size())];
}

int checkIfValueAlreadyExistsInPermutation(uint8_t value, int n, int step, struct state *s){
    for (unsigned int i = 0; i < n; i++) {
        if (s->S[step][i] == value) {
            return i;
        }
    }
    return -1;
}

int recoverInitialStateFromState(int n, int step, struct state *s){
    //use inverseUpdate to recover initial state from any given state
    step -=1;
    spritzClass *spritz = new spritzClass();
    spritz->InitializeState();
    spritz->setN(n);
    spritz->setS((uint8_t*)s->S[step]);
    spritz->setI(s->i[step]);
    spritz->setJ(s->j[step]);
    spritz->setK(s->k[step]);
    spritz->setZ(s->z[step]);
    spritz->setW(s->w);
    
    printf("inverseUpdate:\n");
    for (int i = step; i > 0; i--) {
        /*for(int x = 0; x < n; x++){
            if(x > 15 && x%16 == 0)
                printf("\n");
            printf("%03d ", spritz->getS()[x]);
        }
        printf("\n");*/
        spritz->inverseUpdate();
    }
    spritz->debug();
    
    delete spritz;
    return 0;
}

int printRecoveredState(int n, int step, struct state *s){
    printf("%03d | %03d %03d %03d | ",step, s->i[step], s->j[step], s->k[step]);
    for (unsigned int i = 0; i < n; i++) {
        printf("%03d ", s->S[step][i]);
    }
    printf("| %03d\n", s->z[step]);
    return 0;
}

int recursiveRecoverState4(int n, int step, struct state *_s, std::vector<uint8_t>guessedValues){
    printf("*****\nstep: %d\n", step);
    
    // if just one possible entry is left - set it directly
    /*if (_s->possibleEntries.size() == 1) {
     for (unsigned int i = 0; i < n; i++) {
     if (_s->S[step-1][i] == -1) {
     _s->S[step-1][i] = _s->possibleEntries[0];
     break;
     }
     }
     }*/
    
    //if accept then output and return
    int allValuesFound = 1;
    for (unsigned int i = 0; i < n; i++) {
        if (_s->S[step-1][i] == -1) {
            allValuesFound = 0;
            break;
        }
    }
    if (allValuesFound/* || step == n*/) {
        //TODO calculate back to initial state and print out
        printf("accept\n");
        for (unsigned int i = 0; i < step; i++) {
            printRecoveredState(n, i, _s);
        }
        
        return 0;
    }
    
    //create new state
    struct state s;
    initializeRecovery(n, step+1, &s);
    for (unsigned int i = 0; i <= step; i++) {
        if (i == step) {
            memcpy(s.S[i], _s->S[i-1], n);
            break;
        }
        memcpy(s.S[i], _s->S[i], n);
    }
    memcpy(s.i, _s->i, step);
    memcpy(s.j, _s->j, step);
    memcpy(s.k, _s->k, step);
    memcpy(s.z, _s->z, n);
    s.possibleEntries = _s->possibleEntries;
    
    //delete all previously guessed values from the search space
    //permutation must have unique elements
    for (unsigned int i = 0; i < guessedValues.size(); i++) {
        for (unsigned int j = 0; j < s.possibleEntries.size(); j++) {
            //printf("%d == %d\n", guessedValues[i], s.possibleEntries[j]);
            if (guessedValues[i] == s.possibleEntries[j]) {
                s.possibleEntries.erase(s.possibleEntries.begin()+j);
            }
        }
    }
    
    printf("previously guessed values: ");
    for (unsigned int i = 0; i < guessedValues.size(); i++) {
        printf("%d ", guessedValues[i]);
    }
    printf("\npossible values: ");
    for (unsigned int i = 0; i < s.possibleEntries.size(); i++) {
        printf("%d ", s.possibleEntries[i]);
    }
    printf("\n");
    
    std::vector<uint8_t>values;
    //simulate update
    
    int guessV1 = 0;
    for (unsigned int v1 = 0; v1 < s.possibleEntries.size(); v1++) {
        // calculate i
        s.i[step] = (s.i[step-1] + 1)%n;
        
        if (s.S[step-1][s.i[step]] == -1) {
            //guess S_{t-1}[i_t]
            printf("guess Si = S(%d): %d\n",s.i[step], s.possibleEntries[v1]);
            s.S[step-1][s.i[step]] = s.possibleEntries[v1];
            guessV1 = 1;
        } else {
            v1 = s.possibleEntries.size();
        }
        
        int guessV2 = 0;
        for (unsigned int v2 = 0; v2 < s.possibleEntries.size(); v2++) {
            if (s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] == -1) {
                //guess S_{t-1}[j_{t-1} + S_{t-1}[i_t]]
                printf("guess S(j+Si) = S(%d): %d\n",(s.j[step-1] + s.S[step-1][s.i[step]])%n, s.possibleEntries[v2]);
                s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] = s.possibleEntries[v2];
                guessV2 = 1;
            } else {
                v2 = s.possibleEntries.size();
            }
            
            //calculate j
            s.j[step] = (s.k[step-1] + s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n])%n;
            
            int guessV3 = 0;
            for (unsigned int v3 = 0; v3 < s.possibleEntries.size(); v3++) {
                if (s.S[step-1][s.j[step]] == -1) {
                    //guess S_{t-1}[j_t]
                    printf("guess Sj = S(%d): %d\n",s.j[step], s.possibleEntries[v3]);
                    s.S[step-1][s.j[step]] = s.possibleEntries[v3];
                    guessV3 = 1;
                } else {
                    v3 = s.possibleEntries.size();
                }
                
                //calculate k
                s.k[step] = (s.i[step] + (s.k[step-1] + s.S[step-1][s.j[step]])%n)%n;
                
                //Swap S_t[i_t], S_t[j_t]
                s.S[step][s.i[step]] = s.S[step-1][s.j[step]];
                s.S[step][s.j[step]] = s.S[step-1][s.i[step]];
                
                int guessV4 = 0;
                for (unsigned int v4 = 0; v4 < s.possibleEntries.size(); v4++) {
                    if (s.S[step][(s.z[step-1] + s.k[step])%n] == -1) {
                        //guess S_t[z_{t-1}+k_t]
                        printf("guess S(z+k) = S(%d): %d\n",(s.z[step-1] + s.k[step])%n, s.possibleEntries[v4]);
                        s.S[step][(s.z[step-1] + s.k[step])%n] = s.possibleEntries[v4];
                        guessV4 = 1;
                    } else {
                        v4 = s.possibleEntries.size();
                    }
                    
                    int guessV5 = 0;
                    for (unsigned int v5 = 0; v5 < s.possibleEntries.size(); v5++) {
                        if (s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] == -1) {
                            //guess S_t[i_t + S_t[z_{t-1} + k_t]]
                            printf("guess S(i+S(z+k)) = S(%d): %d\n",(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n, s.possibleEntries[v5]);
                            s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] = s.possibleEntries[v5];
                            guessV5 = 1;
                        } else {
                            v5 = s.possibleEntries.size();
                        }
                        
                        //printf("calculate z\n");
                        printf("z' = %d, z = %d\n", s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n], s.z[step]);
                        //calculate z
                        /*if(s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n] == -1){
                         printf("z cannot be verified!\n");
                         printf("possible index for z: %d\n", (s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n);
                         printf("z = %d\n", s.z[step]);
                         
                         
                         //first approach set S(z) = j+S(i+S(z{t-i}+k))
                         //if z not already in S
                         if (checkIfValueAlreadyExistsInPermutation(s.z[step], n, step, &s) == -1) {
                         s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n] = s.z[step];
                         values.push_back(s.z[step]);
                         
                         if (guessV1) {
                         values.push_back(s.possibleEntries[v1]);
                         }
                         if (guessV2) {
                         values.push_back(s.possibleEntries[v2]);
                         }
                         if (guessV3) {
                         values.push_back(s.possibleEntries[v3]);
                         }
                         if (guessV4) {
                         values.push_back(s.possibleEntries[v4]);
                         }
                         if (guessV5) {
                         values.push_back(s.possibleEntries[v5]);
                         }
                         } else {
                         //undo guesses
                         if (guessV5) {
                         s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] = -1;
                         guessV5 = 0;
                         }
                         if (guessV4) {
                         s.S[step][(s.z[step-1] + s.k[step])%n] = -1;
                         guessV4 = 0;
                         }
                         //Swap S_t[i_t], S_t[j_t]
                         s.S[step-1][s.j[step]] = s.S[step][s.i[step]];
                         s.S[step-1][s.i[step]] = s.S[step][s.j[step]];
                         if (guessV3) {
                         s.S[step-1][s.j[step]] = -1;
                         guessV3 = 0;
                         }
                         if (guessV2) {
                         s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] = -1;
                         guessV2 = 0;
                         }
                         if (guessV1) {
                         s.S[step-1][s.i[step]] = -1;
                         guessV1 = 0;
                         }
                         }
                         
                         //print current recovery
                         printf("1:current recovery: step: %d\n", step);
                         for (unsigned int i = 0; i <= step; i++) {
                         printRecoveredState(n, i, &s);
                         }
                         printf("z not verified - S(z) = z\n");
                         recursiveRecoverState4(n, step+1, &s, values);
                         //return recursiveRecoverState4(n, step+1, &s, values);
                         
                         }*/
                        
                        //if z not in S
                        /*int val = checkIfValueAlreadyExistsInPermutation(s.z[step], n, step, &s);
                         if (val == -1){
                         // if S(z) is unknown
                         if(s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n] == -1){
                         printf("set z to S(%d)\n", (s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n);
                         s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n] = s.z[step];
                         values.push_back(s.z[step]);
                         
                         if (guessV1) {
                         values.push_back(s.possibleEntries[v1]);
                         }
                         if (guessV2) {
                         values.push_back(s.possibleEntries[v2]);
                         }
                         if (guessV3) {
                         values.push_back(s.possibleEntries[v3]);
                         }
                         if (guessV4) {
                         values.push_back(s.possibleEntries[v4]);
                         }
                         if (guessV5) {
                         values.push_back(s.possibleEntries[v5]);
                         }
                         } else {
                         //contradiction
                         printf("Contradiction\n");
                         printf("Cutting branch off\n");
                         //return -1;
                         continue;
                         }
                         }
                         //if z already in S
                         if(val != -1){
                         // if index of z in S fits
                         if (val == (s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n) {
                         printf("z = ok\n");
                         } else {
                         //contradiction
                         printf("Contradiction\n");
                         printf("Cutting branch off\n");
                         //return -1;
                         continue;
                         }
                         }*/
                        
                        if (s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n] == s.z[step]){
                            if (guessV1) {
                                values.push_back(s.possibleEntries[v1]);
                            }
                            if (guessV2) {
                                values.push_back(s.possibleEntries[v2]);
                            }
                            if (guessV3) {
                                values.push_back(s.possibleEntries[v3]);
                            }
                            if (guessV4) {
                                values.push_back(s.possibleEntries[v4]);
                            }
                            if (guessV5) {
                                values.push_back(s.possibleEntries[v5]);
                            }
                            
                            //print current recovery
                            printf("2:current recover: step: %d\n", step);
                            for (unsigned int i = 0; i <= step; i++) {
                                printRecoveredState(n, i, &s);
                            }
                            
                            // z correct for step
                            printf("recursion z correct\n");
                            recursiveRecoverState4(n, step+1, &s, values);
                            //return recursiveRecoverState4(n, step+1, &s, values);
                        }
                        // if z not at current index
                        if (s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n] == -1) {
                            //or anywhere else in S
                            int zInS = 0;
                            for (unsigned int i = 0; i < n; i++) {
                                if(s.S[step][i] == s.z[step]){
                                    zInS = 1;
                                }
                            }
                            if (!zInS) {
                                //set z to current index
                                s.S[step][(s.j[step] + s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n])%n] = s.z[step];
                                recursiveRecoverState4(n, step+1, &s, values);
                            } else {
                                // do nothing in this step
                                //undo guesses
                                if (guessV5) {
                                    s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] = -1;
                                    guessV5 = 0;
                                }
                                if (guessV4) {
                                    s.S[step][(s.z[step-1] + s.k[step])%n] = -1;
                                    guessV4 = 0;
                                }
                                //Swap S_t[i_t], S_t[j_t]
                                s.S[step-1][s.j[step]] = s.S[step][s.i[step]];
                                s.S[step-1][s.i[step]] = s.S[step][s.j[step]];
                                if (guessV3) {
                                    s.S[step-1][s.j[step]] = -1;
                                    guessV3 = 0;
                                }
                                if (guessV2) {
                                    s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] = -1;
                                    guessV2 = 0;
                                }
                                if (guessV1) {
                                    s.S[step-1][s.i[step]] = -1;
                                    guessV1 = 0;
                                }
                                recursiveRecoverState4(n, step+1, &s, std::vector<uint8_t>());
                            }
                        }
                        
                        //reset guessed variable
                        if (guessV5) {
                            s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] = -1;
                            guessV5 = 0;
                        }
                    }
                    //reset guessed variable
                    if (guessV4) {
                        s.S[step][(s.z[step-1] + s.k[step])%n] = -1;
                        guessV4 = 0;
                    }
                }
                //Swap S_t[i_t], S_t[j_t]
                s.S[step-1][s.j[step]] = s.S[step][s.i[step]];
                s.S[step-1][s.i[step]] = s.S[step][s.j[step]];
                
                //reset guessed variable
                if (guessV3) {
                    s.S[step-1][s.j[step]] = -1;
                    guessV3 = 0;
                }
            }
            //reset guessed variable
            if (guessV2) {
                s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] = -1;
                guessV2 = 0;
            }
        }
        //reset guessed variable
        if (guessV1) {
            s.S[step-1][s.i[step]] = -1;
            guessV1 = 0;
        }
    }
    
    printf("Cutting branch off\n");
    // error
    return -1;
}

int count = 0;

int recursiveRecoverState5(int n, int step, struct state *_s){
    //printf("*****\nstep: %d\n", step);
    //printf("%d\n", count);
    /*for (unsigned int i = 0; i < step; i++) {
     printRecoveredState(n, i, _s);
     }*/
    //step = 1;	// to save memory
    
    //if accept then output and return
    int allValuesFound = 1;
    for (unsigned int i = 0; i < n; i++) {
        if (_s->S[step-1][i] == -1) {
            allValuesFound = 0;
            break;
        }
    }
    if (allValuesFound /*|| step == n*/) {
        //if (memcmp(state_n16[step-1], _s->S[step-1], n) == 0) {
        //if (memcmp(state_n32[step-1], _s->S[step-1], n) == 0) {
    	//if (memcmp(state_n8[step-1], _s->S[step-1], n) == 0) {
        printf("solution found - check if correct\n");
        
        struct state s;
        copyState(n, step-1, &s, _s);
        //compute with all possible solutions forward until all wrong solutions yield in a contradiction
        if (verifyRecoveredState(n, step, &s) == 0) {
            printf("accept\n");
            for (unsigned int i = 0; i < step; i++) {
                printRecoveredState(n, i, _s);
            }
            printf("count: %d\n", count);
            
            //calculate back to initial state
            //recoverInitialStateFromState(n, step-1, &s);
            recoverInitialStateFromState(n, step, &s);  //bugfix?
            exit(0);
        }
        freeState(&s);
        return 0;
    }
    
    //create new state
    struct state s;
    initializeRecovery(n, step+1, &s);
    for (unsigned int i = 0; i <= step; i++) {
        if (i == step) {
            memcpy(s.S[i], _s->S[i-1], n);
            break;
        }
        memcpy(s.S[i], _s->S[i], n);
    }
    memcpy(s.i, _s->i, step/*+1*/);
    memcpy(s.j, _s->j, step/*+1*/);
    memcpy(s.k, _s->k, step/*+1*/);
    memcpy(s.z, _s->z, /*2*n*/n+3);
    s.w = _s->w;
    
    //perform update
    int guessV1 = 0;
    for (unsigned int v1 = 0; v1 < n; v1++) {
        // calculate i
        s.i[step] = (s.i[step-1] + s.w)%n;
        if (s.S[step-1][s.i[step]] == -1) {
            //guess S_{t-1}[i_t]
            //printf("guess Si = S(%d): %d\n",s.i[step], v1);
            if (checkIfValueAlreadyExistsInPermutation(v1, n, step, &s) != -1) {
                //printf("v1: guess %d already exists in S\n", v1);
                continue;
            } else {
                s.S[step-1][s.i[step]] = v1;
                guessV1 = 1;
                count++;
            }
        } else {
            //printf("Si already known: S(%d) = %d\n",s.i[step],  s.S[step-1][s.i[step]]);
            v1 = n;
            guessV1 = 0;
        }
        int guessV2 = 0;
        for (unsigned int v2 = 0; v2 < n; v2++) {
            struct state s2;
            copyState(n, step, &s2, &s);
            if (s2.S[step-1][(s2.j[step-1] + s2.S[step-1][s2.i[step]])%n] == -1) {
                //guess S_{t-1}[j_{t-1} + S_{t-1}[i_t]]
                //printf("guess S(j+Si) = S(%d): %d\n",(s2.j[step-1] + s2.S[step-1][s2.i[step]])%n, v2);
                if (checkIfValueAlreadyExistsInPermutation(v2, n, step, &s2) != -1) {
                    //printf("v2: guess %d already exists in S\n", v2);
                    continue;
                } else {
                    s2.S[step-1][(s2.j[step-1] + s2.S[step-1][s2.i[step]])%n] = v2;
                    guessV2 = 1;
                    count++;
                }
            } else {
                //printf("S(j+Si) already known: S(%d) = %d\n", (s2.j[step-1] + s2.S[step-1][s2.i[step]])%n, s2.S[step-1][(s2.j[step-1] + s2.S[step-1][s2.i[step]])%n]);
                v2 = n;
                guessV2 = 0;
            }
            //calculate j
            s2.j[step] = (s2.k[step-1] + s2.S[step-1][(s2.j[step-1] + s2.S[step-1][s2.i[step]])%n])%n;
            int guessV3 = 0;
            for (unsigned int v3 = 0; v3 < n; v3++) {
                struct state s3;
                copyState(n, step, &s3, &s2);
                if (s3.S[step-1][s3.j[step]] == -1) {
                    //guess S_{t-1}[j_t]
                    //printf("guess Sj = S(%d): %d\n",s3.j[step], v3);
                    if (checkIfValueAlreadyExistsInPermutation(v3, n, step, &s3) != -1) {
                        //printf("v3: guess %d already exists in S\n", v3);
                        continue;
                    } else {
                        s3.S[step-1][s3.j[step]] = v3;
                        guessV3 = 1;
                        count++;
                    }
                } else {
                    //printf("Sj already known: S(%d) = %d\n", s3.j[step], s3.S[step-1][s3.j[step]]);
                    v3 = n;
                    guessV3 = 0;
                }
                //calculate k
                s3.k[step] = (s3.i[step] + (s3.k[step-1] + s3.S[step-1][s3.j[step]])%n)%n;
                //Swap S_t[i_t], S_t[j_t]
                s3.S[step][s3.i[step]] = s3.S[step-1][s3.j[step]];
                s3.S[step][s3.j[step]] = s3.S[step-1][s3.i[step]];
                int guessV4 = 0;
                for (unsigned int v4 = 0; v4 < n; v4++) {
                    struct state s4;
                    copyState(n, step, &s4, &s3);
                    if (s4.S[step][(s4.z[step-1] + s4.k[step])%n] == -1) {
                        //guess S_t[z_{t-1}+k_t]
                        //printf("guess S(z+k) = S(%d): %d\n",(s4.z[step-1] + s4.k[step])%n, v4);
                        if (checkIfValueAlreadyExistsInPermutation(v4, n, step, &s4) != -1) {
                            //printf("v4: guess %d already exists in S\n", v4);
                            continue;
                        } else {
                            s4.S[step][(s4.z[step-1] + s4.k[step])%n] = v4;
                            guessV4 = 1;
                            count++;
                        }
                    } else {
                        //printf("S(z+k) already known: S(%d) = %d\n", (s4.z[step-1] + s4.k[step])%n, s4.S[step][(s4.z[step-1] + s4.k[step])%n]);
                        v4 = n;
                        guessV4 = 0;
                    }
                    int guessV5 = 0;
                    for (unsigned int v5 = 0; v5 < n; v5++) {
                        struct state s5;
                        copyState(n, step, &s5, &s4);
                        if (s5.S[step][(s5.i[step] + s5.S[step][(s5.z[step-1] + s5.k[step])%n])%n] == -1) {
                            //guess S_t[i_t + S_t[z_{t-1} + k_t]]
                            //printf("guess S(i+S(z+k)) = S(%d): %d\n",(s5.i[step] + s5.S[step][(s5.z[step-1] + s5.k[step])%n])%n, v5);
                            if (checkIfValueAlreadyExistsInPermutation(v5, n, step, &s5) != -1) {
                                //printf("v5: guess %d already exists in S\n", v5);
                                continue;
                            } else {
                                s5.S[step][(s5.i[step] + s5.S[step][(s5.z[step-1] + s5.k[step])%n])%n] = v5;
                                guessV5 = 1;
                                count++;
                            }
                        } else {
                            //printf("S(i+S(z+k)) already known: S(%d) = %d\n", (s.i[step] + s.S[step][(s5.z[step-1] + s5.k[step])%n])%n, s5.S[step][(s5.i[step] + s5.S[step][(s5.z[step-1] + s5.k[step])%n])%n]);
                            v5 = n;
                            guessV5 = 0;
                        }
                        
                        int zInS = 0, contradiction = 0;
                        for (unsigned int i = 0; i < n; i++) {
                            //if z = any value in S
                            if (s5.S[step][i] == s5.z[step]) {
                                //check if index is equal
                                if (i != (s5.j[step] + s5.S[step][(s5.i[step] + s5.S[step][(s5.z[step-1] + s5.k[step])%n])%n])%n) {
                                    // contradiction
                                    contradiction = 1;
                                } else {
                                    //printf("recursion1\n");
                                    //recursion
                                    recursiveRecoverState5(n, step+1, &s5);
                                }
                                zInS = 1;
                                break;
                            }
                        }
                        if (zInS && contradiction) {
                            //printf("z in S but index wrong\n");
                            //undo guesses
                            /*if (guessV5) {
                             s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] = -1;
                             guessV5 = 0;
                             }
                             if (guessV4) {
                             s.S[step][(s.z[step-1] + s.k[step])%n] = -1;
                             guessV4 = 0;
                             }
                             //Swap S_t[i_t], S_t[j_t]
                             s.S[step-1][s.j[step]] = s.S[step][s.i[step]];
                             s.S[step-1][s.i[step]] = s.S[step][s.j[step]];
                             if (guessV3) {
                             s.S[step-1][s.j[step]] = -1;
                             guessV3 = 0;
                             }
                             if (guessV2) {
                             s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] = -1;
                             guessV2 = 0;
                             }
                             if (guessV1) {
                             s.S[step-1][s.i[step]] = -1;
                             guessV1 = 0;
                             }*/
                            continue;
                        }
                        //if z != any value in S
                        if (!zInS) {
                            //if there is no other value at position of index
                            if (s5.S[step][(s5.j[step] + s5.S[step][(s5.i[step] + s5.S[step][(s5.z[step-1] + s5.k[step])%n])%n])%n] == -1) {
                                s5.S[step][(s5.j[step] + s5.S[step][(s5.i[step] + s5.S[step][(s5.z[step-1] + s5.k[step])%n])%n])%n] = s5.z[step];
                                count++;
                                //recursion
                                //printf("recursion2\n");
                                recursiveRecoverState5(n, step+1, &s5);
                            } else {
                                //contradiction
                                //printf("z not in S but index points to element\n");
                                //undo guesses
                                /*if (guessV5) {
                                 s.S[step][(s.i[step] + s.S[step][(s.z[step-1] + s.k[step])%n])%n] = -1;
                                 guessV5 = 0;
                                 }
                                 if (guessV4) {
                                 s.S[step][(s.z[step-1] + s.k[step])%n] = -1;
                                 guessV4 = 0;
                                 }
                                 //Swap S_t[i_t], S_t[j_t]
                                 s.S[step-1][s.j[step]] = s.S[step][s.i[step]];
                                 s.S[step-1][s.i[step]] = s.S[step][s.j[step]];
                                 if (guessV3) {
                                 s.S[step-1][s.j[step]] = -1;
                                 guessV3 = 0;
                                 }
                                 if (guessV2) {
                                 s.S[step-1][(s.j[step-1] + s.S[step-1][s.i[step]])%n] = -1;
                                 guessV2 = 0;
                                 }
                                 if (guessV1) {
                                 s.S[step-1][s.i[step]] = -1;
                                 guessV1 = 0;
                                 }*/
                                continue;
                            }
                        }
                        freeState(&s5);
                    }
                    freeState(&s4);
                }
                freeState(&s3);
            }
            freeState(&s2);
        }
        //do not free s
    }
    
    //should not happen
    return -1;
}

int testcopystate(){
    struct state s;
    initializeRecovery(4, 2, &s);
    s.S[0][0] = 0;
    s.S[0][1] = 1;
    s.S[0][2] = 2;
    s.S[0][3] = 3;
    s.S[1][0] = 4;
    s.S[1][1] = 5;
    s.S[1][2] = 6;
    s.S[1][3] = 7;
    s.i[0] = 0;
    s.i[1] = 1;
    s.j[0] = 0;
    s.j[1] = 2;
    s.k[0] = 0;
    s.k[1] = 3;
    s.z[0] = 1;
    s.z[1] = 2;
    
    struct state s2;
    copyState(4, 1, &s2, &s);
    
    printRecoveredState(4, 0, &s2);
    printRecoveredState(4, 1, &s2);
    //printRecoveredState(4, 2, &s2);
    return 0;
}

int copyState(int n, int step, struct state *destination, struct state *source){
    //step = 1;	// to save memory
    initializeRecovery(n, step+1, destination);
    for (unsigned int i = 0; i <= step; i++) {
        memcpy(destination->S[i], source->S[i], n);
    }
    memcpy(destination->i, source->i, step+1);
    memcpy(destination->j, source->j, step+1);
    memcpy(destination->k, source->k, step+1);
    memcpy(destination->z, source->z, /*2*n*/n+3);
    (*destination).w = (*source).w;
    return 0;
}

int freeState(struct state *s){
    free(s->S);
    free(s->i);
    free(s->j);
    free(s->k);
    free(s->z);
    
    //free(s);
    return 0;
}

int verifyRecoveredState(int n, int step, struct state *s){
    int threshold = 3;//n;
    
    step -=1;
    spritzClass *spritz = new spritzClass();
    spritz->InitializeState();
    spritz->setN(n);
    spritz->setS((uint8_t*)s->S[step]);
    spritz->setI(s->i[step]);
    spritz->setJ(s->j[step]);
    spritz->setK(s->k[step]);
    spritz->setZ(s->z[step]);
    spritz->setW(s->w);
    
    for (unsigned int i = 0; i < threshold; i++) {
        spritz->Update();
        uint8_t z = spritz->Output();
        //printf("step: %d z_output: %d, z: %d\n", step, s->z[(step+1)+i], z);
        if (z != s->z[(step+1)+i]) {
            return -1;
        }
    }
    
    delete spritz;
    return 0;
}

int initializeProbabilisticState(struct probState *s, int n){
    s->S = (double**) malloc(n*sizeof(double*));
    for (unsigned int i = 0; i < n; i++) {
        s->S[i] = (double*) malloc(n*sizeof(double));
    }
    s->i = (uint8_t*) malloc(n*sizeof(uint8_t));
    s->j = (double*) malloc(n*sizeof(double));
    s->k = (double*) malloc(n*sizeof(double));
    s->z = (uint8_t*) malloc(n*sizeof(uint8_t));
    s->w = 1;

    for (unsigned int i = 0; i < n; i++) {
        s->j[i] = (double)(1.0f/n);		//uniform distribution for initial j
        s->k[i] = (double)(1.0f/n);		//uniform distribution for initial k
        for (unsigned int j = 0; j < n; j++) {
            s->S[i][j] = (double)(1.0f/n);		//uniform distribution for initial S
        }
    }
    
    return 0;
}

int updateProbabilities(int n, int step, struct probState *s){
    
    double jnew[n], knew[n], Snew[n][n];
    
    for (unsigned int i = 0; i < n; i++) {
        jnew[i] = 0.0f;
        knew[i] = 0.0f;
        for (unsigned int j = 0; j < n; j++) {
            Snew[i][j] = 0.0f;
        }
    }
    
    //printf("1\n");
    
    //for (unsigned int iold = 0; iold < n; iold++) {
    	uint8_t i = step%n;//(iold + s->w)%n;
        
        for (unsigned int jold = 0; jold < n; jold++) {
            for (unsigned int kold = 0; kold < n; kold++) {
                
                for (unsigned int a = 0; a < n; a++) {	// a = S_{t-1}[i_t]
                    for (unsigned int b = 0; b < n; b++) {	// b = S_{t-1}[j_{t-1} + a]
                        uint8_t j = (kold + b)%n;
                        
                        for (unsigned int c = 0; c < n; c++) {	// c = S_{t-1}[j_t]
                            uint8_t k = (i + (kold + c)%n)%n;
                            
                            //for (unsigned int d = 0; d < n; d++) {	// d = S_t[z_{t-1}+k_t]
                                for (unsigned int e = 0; e < n; e++) {	// e = S_t[i_t + d]
                                    uint8_t zindex = (j+e)%n;
                                    double prob = 0.0f;
                                    
                                    //todo special cases
                                    
                                    prob = s->j[jold] * s->k[kold] * s->S[i][a] * s->S[j][c] * s->S[zindex][s->z[step-1]];
                                    //printf("prob: %.02f, j[jold=%d]: %.02f, k[kold=%d]: %.02f, S[i:%d][a:%d]: %.02f, S[j=%d][c=%d]: %.02f, S[zi=%d][z=%d]: %.02f\n", prob, jold, s->j[jold], kold, s->k[kold], i, a, s->S[i][a], j, c, s->S[j][c], zindex, s->z[step-1], s->S[zindex][s->z[step-1]]);
                                    
                                    jnew[j] += prob;
                                    knew[k] += prob;
                                    
                                    for (unsigned int f = 0; f < n; f++) {
                                        if(f == i){
                                            Snew[i][c] += prob;	//c because of swap
                                        }
                                        if (f == j) {
                                            Snew[j][a] += prob;	//a because of swap
                                        }
                                        if (f == zindex) {
                                            Snew[zindex][s->z[step-1]] += prob;
                                        }
                                        for (unsigned int g = 0; g < n; g++) {
                                            if (g == i || g == j || g == zindex) {
                                                continue;
                                            }
                                            Snew[f][g] += prob * s->S[f][g];
                                        }
                                    }
                                }
                            //}
                        }
                    }
                }
                
            }
        }
    //}
    
    //normalizing
    double total = sum(jnew, n);
    //printf("totj: %f\n", total);
    if (total > 0.0f) {
        for (unsigned int i = 0; i < n; i++) {
            jnew[i] /= total;
        }
    }

    total = sum(knew, n);
    //printf("totk: %f\n", total);
    if (total > 0.0f) {
    	for (unsigned int i = 0; i < n; i++) {
        	knew[i] /= total;
    	}
    }
    for (unsigned int i = 0; i < n; i++) {
        total = sum(Snew[i], n);
        //printf("tot[%d]: %f\n",i, total);
        if (total > 0.0f) {
            for (unsigned int j = 0; j < n; j++) {
                Snew[i][j] /= total;
            }
        }
    }
    
    //printf("2\n");
    //copy back on probState
    for (unsigned int i = 0; i < n; i++) {
        s->j[i] = jnew[i];
        s->k[i] = knew[i];
        for (unsigned int j = 0; j < n; j++) {
            s->S[i][j] = Snew[i][j];
        }
    }
    
    return 0;
}

int probabilisticStateRecovery(int n){
    int keyLength = 16;
    int steps = 100;
    double convergeThreshold = 0.999;
    struct probState s;
    
    initializeProbabilisticState(&s, n);
    srand(time(NULL));
    
    uint8_t key[keyLength];
    for (unsigned int i = 0; i < keyLength; i++) {
        memset(key, rand()%256, 1);
    }
    setN(n);
    InitializeState();
    Absorb(key, keyLength);
    
    Shuffle();
    printf("initial S:\n");
    for(int x = 0; x < n; x++){
        if(x > 15 && x%16 == 0)
            printf("\n");
        printf("%03d ", getCipher().S[x]);
    }
    printf("\n");
    
    for (unsigned int i = 0; i < n; i++) {
        Update();
        s.z[i] = Output();	//store output in z
    }
    
    //print j,k,S
    printf("step: 0\nj: ");
    for (unsigned int i = 0; i < n; i++) {
        printf("%.02f ", s.j[i]);
    }
    printf("\nk: ");
    for (unsigned int i = 0; i < n; i++) {
        printf("%.02f ", s.k[i]);
    }
    printf("\n");
    for (unsigned int i = 0; i < n; i++) {
        printf("%d: ", i);
        for (unsigned int j = 0; j < n; j++) {
            printf("%.02f ", s.S[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    for (unsigned int t = 1; t < steps; t++) {
        updateProbabilities(n, t, &s);
        if (max(s.j, n) > convergeThreshold &&
            max(s.k, n) > convergeThreshold &&
            checkSConverged(s.S, n, convergeThreshold)) {
            printf("Converged after %d steps\n", t);
            break;
        }
        
        //print j,k,S
        printf("step: %d\nj: ", t);
        for (unsigned int i = 0; i < n; i++) {
            printf("%.02f ", s.j[i]);
        }
        printf("\nk: ");
        for (unsigned int i = 0; i < n; i++) {
            printf("%.02f ", s.k[i]);
        }
        printf("\n");
        for (unsigned int i = 0; i < n; i++) {
            printf("%d: ", i);
            for (unsigned int j = 0; j < n; j++) {
                printf("%.02f ", s.S[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
    
    return 0;
}

int checkSConverged(double **array, int size, double convergeThreshold){
	for (unsigned int i = 0; i < size; i++) {
 		if (max(array[i], size) < convergeThreshold) {
 			return 0;
 		}
 	}
 	return 1;
 }

int complex(int a, int n){
    printf("complexity: %f\n", c1(a, n));
    return 0;
}

double c1(int a, int n){
    /*if (a >= 256) {
        return 0.0f;
    }
    if (a == 255) {
        return 1.0f;
    }*/
    //return (a/(int)pow(2.0f, n)) * c2(a, n) + ((1-(a/(int)pow(2.0f, n))) * ((int)pow(2.0f, n)-a) * c2(a+1, n));
    if(c1LookupTable[a] == -1){
        c1LookupTable[a] = (a/pow(2.0f, n)) * c2(a, n) + ((1.0f-(a/pow(2.0f, n))) * (pow(2.0f, n)-a) * c2(a+1.0f, n));
    }
    return c1LookupTable[a];
}

double c2(int a, int n){
    /*if (a >= 256) {
        return 0.0f;
    }
    if (a == 255) {
        return 1.0f;
    }*/
    //return ((a/(int)pow(2.0f, n))*((int)pow(1-(a/(int)pow(2.0f, n)), 2.0f)*(1+c1(a+1,n)) + (c1(a+1,n)/(int)pow(2.0f, n)))) + ((1-a/(int)pow(2.0f, n))*c3(a,n));
    if(c2LookupTable[a] == -1){
    	c2LookupTable[a] = ((a/pow(2.0f, n))*(pow(1.0f-(a/pow(2.0f, n)), 2.0f)*(1.0f+c1(a+1,n)) + (/*c1(a+1.0f,n)*/1.0f/pow(2.0f, n)))) + ((1.0f-a/pow(2.0f, n))*c3(a,n));
    }
    return c2LookupTable[a];
}

double c3(int a, int n){
    /*if (a >= 256) {
        return 0.0f;
    }
    if (a == 255) {
        return 1.0f;
    }*/
    //return (1-a/(int)pow(2.0f, n))*(f(a,n) + ((2*a+1)/(int)pow(2.0f, n))*c1(a+1, n) + ((int)pow(2.0f, n)-a)*e(a,n)*c1(a+2, n));
    if(c3LookupTable[a] == -1){
    	c3LookupTable[a] = (1.0f-a/pow(2.0f, n))*(f(a,n) + ((2.0f*a+1.0f)/pow(2.0f, n))*c1(a+1.0f, n) + (pow(2.0f, n)-a)*e(a,n)*c1(a+2.0f, n));
    }
    return c3LookupTable[a];
}

double e(int a, int n){
    //return (1-(a+1)/(int)pow(2.0f, n))*(1-1/((int)pow(2.0f, n)-a));
    return (1.0f-(a+1.0f)/pow(2.0f, n))*(1.0f-1.0f/(pow(2.0f, n)-a));
}

double f(int a, int n){
    //return ((int)pow(2.0f, n)-a)*(1+e(a, n))+a/(int)pow(2.0f, n);
    return (pow(2.0f, n)-a)*(1.0f+e(a, n))+a/pow(2.0f, n);
}

// complexity for RC4 with own formulas
double complexRC4(int a, int n){
    //std::fill_n(c1LookupTable, 257, -1);
    //std::fill_n(c2LookupTable, 257, -1);
    //std::fill_n(c3LookupTable, 257, -1);
    
    /*c1LookupTable[256] = 0;
    c1LookupTable[255] = 1;
    c2LookupTable[256] = 0;
    c2LookupTable[255] = 1;
    c3LookupTable[256] = 0;
    c3LookupTable[255] = 1;*/
    
    printf("complexity(a = %d, n = 2^%d): %f\n", a, n, c1RC4(a, n));
    return 0;
}

double c1RC4(int a, int n){
    if(c1LookupTable[a] == -1){
        //c1LookupTable[a] = ((a/(int)pow(2.0f, n))*c2RC4(a, n) + (1-(a/(int)pow(2.0f, n)))*((int)pow(2.0f, n)-a)*c2RC4(a+1, n));
        c1LookupTable[a] = ((a/pow(2.0f, n))*c2RC4(a, n) + (1.0f-(a/pow(2.0f, n)))*(pow(2.0f, n)-a)*c2RC4(a+1.0f, n));
        printf("calc c1(%d) = %Lf\n", a, c1LookupTable[a]);
    }
    return c1LookupTable[a];
}

double c2RC4(int a, int n){
    if(c2LookupTable[a] == -1){
        //c2LookupTable[a] = ((a/(int)pow(2.0f, n))*((int)pow((1-(a/(int)pow(2.0f, n))), 2.0f)*c1RC4(a+1, n) + (1-(a/(int)pow(2.0f, n)))*((2*a)/((int)pow(2.0f, n))) + (a/(int)pow(2.0f, n))*1/*c1RC4(a, n)*/) + (1-(a/(int)pow(2.0f, n)))*c3RC4(a,n));
        c2LookupTable[a] = ((a/pow(2.0f, n))*(pow((1.0f-(a/pow(2.0f, n))), 2.0f)*c1RC4(a+1.0f, n) + (1.0f-(a/pow(2.0f, n)))*((2.0f*a)/(pow(2.0f, n))) + (a/pow(2.0f, n))*1.0f/*c1RC4(a, n)*/) + (1.0f-(a/pow(2.0f, n)))*c3RC4(a,n));
        printf("calc c2(%d) = %Lf\n", a, c2LookupTable[a]);
    }
    return c2LookupTable[a];
}

double c3RC4(int a, int n){
    if(c3LookupTable[a] == -1){
        //c3LookupTable[a] = ((a/(int)pow(2.0f, n))*((a/(int)pow(2.0f, n))*1 + (1-(a/(int)pow(2.0f, n)))*c1RC4(a+1, n)) + (1-(a/(int)pow(2.0f, n)))*((int)pow(2.0f, n)*((a/(int)pow(2.0f, n)) + (1-(a/(int)pow(2.0f, n)))*((a/(int)pow(2.0f, n)) + (1-(a/(int)pow(2.0f, n)))*c1RC4(a+2,n)))));
        c3LookupTable[a] = ((a/pow(2.0f, n))*((a/pow(2.0f, n))*1.0f + (1.0f-(a/pow(2.0f, n)))*c1RC4(a+1.0f, n)) + (1.0f-(a/pow(2.0f, n)))*(pow(2.0f, n)*((a/pow(2.0f, n)) + (1.0f-(a/pow(2.0f, n)))*((a/pow(2.0f, n)) + (1.0f-(a/pow(2.0f, n)))*c1RC4(a+2.0f,n)))));
        printf("calc c3(%d) = %Lf\n", a, c3LookupTable[a]);
    }
    return c3LookupTable[a];
}

// complexity functions for Spritz
double complexSpritz(int a, int n){
    printf("complexity for Spritz(a = %d, N = %d): %Lf\n", a, n, c1Spritz(a,n));
    return 0.0f;
}

long double c1Spritz(int a, int n){
    if(c1LookupTable[a] == -1){
        c1LookupTable[a] = ((a/n)*c2Spritz(a, n) + (1.0f-a/n)*(n-a)*c2Spritz(a+1.0f, n));
        //printf("c1(%d) = %f\n", a, c1LookupTable[a]);
    }
    return c1LookupTable[a];
}

long double c2Spritz(int a, int n){
    if(c2LookupTable[a] == -1){
        c2LookupTable[a] = ((a/n)*c3Spritz(a,n) + (1.0f-a/n)*(n-a)*c3Spritz(a+1.0f, n));
    }
    return c2LookupTable[a];
}

long double c3Spritz(int a, int n){
    if(c3LookupTable[a] == -1){
        c3LookupTable[a] = ((a/n)*c4Spritz(a,n) + (1.0f-a/n)*(n-a)*c4Spritz(a+1.0f, n));
    }
    return c3LookupTable[a];
}

long double c4Spritz(int a, int n){
    if(c4LookupTable[a] == -1){
        c4LookupTable[a] = ((a/n)*c5Spritz(a,n) + (1.0f-a/n)*(n-a)*c5Spritz(a+1.0f, n));
    }
    return c4LookupTable[a];
}

long double c5Spritz(int a, int n){
    if(c5LookupTable[a] == -1){
        c5LookupTable[a] = ((a/n)*c6Spritz(a,n) + (1.0f-a/n)*(n-a)*c6Spritz(a+1.0f, n));
    }
    return c5LookupTable[a];
}

long double c6Spritz(int a, int n){
    if(c6LookupTable[a] == -1){
        //c6LookupTable[a] = ((a/n)*(a/n + (1.0f-a/n)*1.0f/*c1Spritz(a, n)*/) + (1.0f-a/n)*(a/n + (1.0f-a/n)*c1Spritz(a+1.0f, n)));
        c6LookupTable[a] = ((a/n)*((1.0f-a/n)*1.0f + 1.0f/*c1Spritz(a, n)*/) + (1.0f-a/n)*(a/n + c1Spritz(a+1.0f, n)));
    }
    return c6LookupTable[a];
}


//Statistical Tests
long double chiSquaredStatistic(int q, int T, int* O){
    double E[q];
    long double chiSquare = 0.0f;
    
    for (unsigned int u = 0; u <= q-1; u++) {
        E[u] = T/q;
        chiSquare += pow(O[u] - E[u], 2.0f) / E[u];
    }
    return chiSquare;
}

double uniformDistribution(int x){
    return (1.0f/x);
}

// Pearson's correlation coefficient
// r_xy = (n*\sum x_i*y_i - \sum x_i * \sum y_i) / (\sqrt(n*\sum x_i^2 - (\sum x_i)^2) * (\sqrt(n*\sum y_i^2 - (\sum y_i)^2)))
double correlationCoefficient(int n, uint8_t* X, uint8_t* Y){
    int sum_xiyi = 0, sum_xi = 0, sum_yi = 0, sum_xi_sq = 0, sum_yi_sq = 0;
    
    for (unsigned int i = 0; i < n; i++) {
        sum_xiyi += (X[i]*Y[i]);
        sum_xi += X[i];
        sum_yi += Y[i];
        sum_xi_sq += pow(X[i], 2.0f);
        sum_yi_sq += pow(Y[i], 2.0f);
    }
    
    /*printf("sumxiyi: %d\n", sum_xiyi);
     printf("sumxi: %d \n", sum_xi);
     printf("sumyi: %d\n", sum_yi);
     printf("sumxi_sq: %d\n", sum_xi_sq);
     printf("sumyi_sq: %d\n", sum_yi_sq);*/
    
    return ((n*sum_xiyi) - (sum_xi*sum_yi)) / (sqrt((n*sum_xi_sq)-pow(sum_xi, 2.0f))*sqrt((n*sum_yi_sq)-pow(sum_yi, 2.0f)));
}


//Test-Functions
int testStatisticFunctions(){
    // test uniform distribution
    double testvector_uniform = 1.0f/256.0f;
    if (uniformDistribution(256) != testvector_uniform) {
        return -1;
    }
    
    //test chiSquaredStatistic
    long double chiSquare = 1.44;
    int O[2] = {44, 56};
    if (chiSquaredStatistic(2,100, O) != chiSquare) {
        return -2;
    }
    
    double corrCoeff = 0.529809f;
    uint8_t X[6] = {43,21,25,42,57,59};
    uint8_t Y[6] = {99,65,79,75,87,81};
    //printf("corrCoeff: %f\n", correlationCoefficient(6, X, Y));
    if (fabs(correlationCoefficient(4, X, Y) - corrCoeff) <= std::numeric_limits<double>::epsilon()) {
        return -3;
    }
    
    return 0;
}

int test_rotate(){
    std::vector<uint8_t>test;
    test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    test.push_back(4);
    
    std::rotate(test.begin(), test.begin()+2, test.end());
    
    for (int i = 0; i < test.size(); i++) {
        printf("%d ", test[i]);
    }
    return 0;
}


