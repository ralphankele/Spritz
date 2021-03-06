// ===========================================================================
// SPRITZ implementation
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

#ifndef _SPRITZ_H_
#define _SPRITZ_H_

#include <cstdint>
//#include <string>

static int N = 256;

typedef struct cipher cipher;

struct cipher {
    uint8_t i;
    uint8_t j;
    uint8_t k;
    uint8_t z;
    uint8_t a;
    uint8_t w;
    uint8_t *S;
};

// Functions
uint8_t* Encrypt(uint8_t* key, int keylength, uint8_t* message, int messagelength);
uint8_t* Decrypt(uint8_t* key, int keylength, uint8_t* ciphertext, int ciphertextlength);
void KeySetup(uint8_t* key, int keylength);
uint8_t* EncryptWithIV(uint8_t* key, int keylength, uint8_t* IV, int IVlength, uint8_t* message, int messagelength);
uint8_t* DecryptWithIV(uint8_t* key, int keylength, uint8_t* IV, int IVlength, uint8_t* ciphertext, int ciphertextlength);

uint8_t* Hash(uint8_t* message, int messagelength, uint8_t* r);
uint8_t* MAC(uint8_t* key, int keylength, uint8_t* message, int messagelength, uint8_t* r);

/*uint8_t* AEAD(uint8_t* key, int keylength, uint8_t* nonce, int noncelength, uint8_t* ad, int adlength, uint8_t* message, int messagelength, uint8_t* r);*/

//Sponge Functions
void InitializeState();
void Absorb(uint8_t* I, int length);
void AbsorbByte(uint8_t b);
void AbsorbNibble(uint8_t x);
void AbsorbStop();
void Shuffle();
void Whip(int r);
void Crush();
uint8_t* Squeeze(uint8_t r);
uint8_t Drip();
void Update();
uint8_t Output();

// Modified Functions
void Update_reducedSwapFrequency(int swapFrequency);

//Inverse Functions
void inverseWhip(int r);
void inverseUpdate();
void inverseAbsorbNibble(uint8_t x);
void inverseOutput(uint8_t z);	//do not use - didn't work correct
void inverseDrip(uint8_t z);	//without inverse Shuffle -> inverting Crush not possible
void inverseShuffle();	//not implemented


//Helper Functions
uint8_t Low(uint8_t);
uint8_t High(uint8_t);
void Swap(int, int);
int Gcd(uint8_t, int);
void debug();
void debug2();
void setN(int n);
cipher getCipher();
void setCipher(cipher _c);

//Test-Functions
int testvectors();

#endif
