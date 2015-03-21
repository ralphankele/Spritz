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

#include "spritzClass.h"
#include <iostream>
#include <time.h>
#include <math.h>
#include <string.h>

//Constructor
spritzClass::spritzClass(){
    //do nothing
}
//Destructor
spritzClass::~spritzClass(){
    //do nothing
}

//Sponge Functions
void spritzClass::InitializeState(){
    //printf("N: %d\n", N);
    
    this->i = 0;
    this->j = 0;
    this->k = 0;
    this->z = 0;
    this->a = 0;
    this->w = 1;
    
    this->S = (uint8_t*) malloc (N*sizeof(uint8_t));
    // S[N] = identity permutation
    for (unsigned int v = 0; v <= N-1; v++) {
        this->S[v] = v;
    }
}

void spritzClass::Absorb(uint8_t* I, int length){
    for (unsigned int v = 0; v <= length-1; v++) {
        AbsorbByte(I[v]);
    }
}

void spritzClass::AbsorbByte(uint8_t b){
    AbsorbNibble(Low(b));
    AbsorbNibble(High(b));
}

void spritzClass::AbsorbNibble(uint8_t x){
    if (this->a == (int)floor(this->N/2)) {
        Shuffle();
    }
    //debug();
    Swap(this->a, ((int)floor(this->N/2) +  x) % this->N);
    //debug();
    this->a = this->a + 1;
}

void spritzClass::AbsorbStop(){
    if (this->a == (int)floor(this->N/2)) {
        Shuffle();
    }
    this->a = this->a + 1;
}

void spritzClass::Shuffle(){
    //printf("Shuffle called\n");
    Whip(2*this->N);
    /*printf("*********************\n");
    Whip(10);
    printf("*********************\n");
    exit(0);*/
    //debug();
    
    Crush();
    Whip(2*this->N);
    Crush();
    Whip(2*this->N);
    this->a = 0;
}

void spritzClass::Whip(int r){
    for (unsigned int v = 0; v < r; v++) {
        Update();
        //printf("Whip\n");
        //debug();
    }
    do {
        this->w = this->w + 1;
    }
    while (Gcd(this->w, this->N) != 1);
    //c.w += 2;
}

void spritzClass::Crush(){
    for (unsigned int v = 0; v <= (int)floor(this->N/2)-1; v++) {
        if (this->S[v] > this->S[this->N-1-v]) {
            Swap(v, this->N-1-v);
        }
    }
}

uint8_t* spritzClass::Squeeze(uint8_t r){
    if(this->a > 0){
        Shuffle();
    }
    uint8_t* P = new uint8_t[r];
    for (unsigned int v = 0; v <= r-1; v++) {
        P[v] = Drip();
    }
    return P;
}

uint8_t spritzClass::Drip(){
    if (this->a > 0) {
        Shuffle();
    }
    Update();
    return Output();
}

void spritzClass::Update(){
    this->i = (this->i + this->w) % (this->N);
    this->j = (this->k + this->S[(this->j + this->S[this->i]) % (this->N)]) % (this->N);
    this->k = (this->i + (this->k + this->S[this->j]) % (this->N)) % (this->N);
   /* c.i = (c.i + c.w) & (N-1);
    c.j = (c.k + c.S[(c.j + c.S[c.i]) & (N-1)]) & (N-1);
    c.k = (c.i + (c.k + c.S[c.j]) & (N-1)) & (N-1);*/
   /* c.i = (c.i + c.w) & (0xff);
    c.j = (c.k + c.S[(c.j + c.S[c.i]) & (0xff)]) & (0xff);
    c.k = (c.i + (c.k + c.S[c.j]) & N) & (N-1);*/
    Swap(this->i, this->j);
}

uint8_t spritzClass::Output(){
    this->z = this->S[(this->j + this->S[(this->i + this->S[(this->z + this->k) % (this->N)]) % (this->N)]) % (this->N)];
    // c.z = c.S[(c.j + c.S[(c.i + c.S[(c.z + c.k) & (N-1)]) & (N-1)]) & (N-1)];
    //c.z = c.S[(c.j + c.S[(c.i + c.S[(c.z + c.k) & 0xff]) & 0xff]) & 0xff];
    return this->z;
}

//Inverse Functions
void spritzClass::inverseWhip(int r){
    //restore w
    do {
        this->w = this->w - 1;
    } while (Gcd(this->w, this->N) != 1);
    
    //restore i,j,k, S
    for (unsigned int v = 0; v < r; v++){
        inverseUpdate();
        //printf("invWhip\n");
        //debug();
        //exit(0);
    }
}

void spritzClass::inverseUpdate(){
    //printf("invUpdate: i:%d, j:%d, k:%d, w:%d, N:%d\n", c.i, c.j, c.k, c.w, N);
    // restore state
    Swap(this->j, this->i);
    
    //restore k
    this->k = (((((((this->k - this->i)%this->N)+this->N)%this->N) - this->S[this->j])%this->N)+this->N)%this->N;
   
    // restore j
    //c.j = c.k + c.S[c.j + c.S[c.i]];
    for (unsigned int v = 0; v < this->N; v++){
        //printf("(((c.j - c.k)%N)+N)%N): %d || ", ((((c.j - c.k)%N)+N)%N));
        //printf("c.S[(((v+c.S[c.i])%N)+N)%N]: %d\n", c.S[(((v+c.S[c.i])%N)+N)%N]);
        if( ((((this->j - this->k)%this->N)+this->N)%this->N) == this->S[(((v+this->S[this->i])%this->N)+this->N)%this->N]){
            this->j = v;
            break;
            //printf("c.j = %d\n", v);
        }
    }
    
    // restore i
    this->i = (((this->i - this->w) % this->N) + this->N) % this->N;
}

void spritzClass::inverseAbsorbNibble(uint8_t x){
    // restore a
    this->a = this->a - 1;
    
    // restore permutation S
    Swap((int)floor(this->N/2)+x, this->a);
    
    // ignore invShuffle - don't take inputs larger than N/4
}

//Helper Functions
uint8_t spritzClass::Low(uint8_t b){
    return b & 0x0F;
}

uint8_t spritzClass::High(uint8_t b){
    return (b & 0xF0) >> 4;
}

void spritzClass::Swap(int first, int second){
    uint8_t temp = this->S[first];
    this->S[first] = this->S[second];
    this->S[second] = temp;
}

int spritzClass::Gcd(uint8_t a, int b){
    int c;
    while ( a != 0 ) {
        c = a; a = b%a;  b = c;
    }
    return b;
}

void spritzClass::debug(){
    printf("i: %d, j: %d, k: %d, z: %d, a:%d, w:%d\n", this->i, this->j, this->k, this->z, this->a, this->w);
    for(int x = 0; x < this->N; x++){
        if(x > 15 && x%16 == 0)
            printf("\n");
        printf("%03d ", this->S[x]);
    }
    printf("\n");
}

//Functions
uint8_t* spritzClass::Encrypt(uint8_t* key, int keylength, uint8_t* message, int messagelength){
    KeySetup(key, keylength);
    uint8_t* ciphertext = new uint8_t[messagelength];
    for (unsigned int v = 0; v < messagelength; v++) {
        ciphertext[v] = (message[v] + Squeeze(messagelength)[v]) & 0xff;
    }
    return ciphertext;
}

uint8_t* spritzClass::Decrypt(uint8_t* key, int keylength, uint8_t* ciphertext, int ciphertextlength){
    KeySetup(key, keylength);
    uint8_t* message = new uint8_t[ciphertextlength];
    for (unsigned int v = 0; v < ciphertextlength; v++) {
        message[v] = (ciphertext[v] - Squeeze(ciphertextlength)[v]) & 0xff;
    }
    return message;
}

void spritzClass::KeySetup(uint8_t* key, int keylength){
    InitializeState();
    Absorb(key, keylength);
}

uint8_t* spritzClass::EncryptWithIV(uint8_t* key, int keylength, uint8_t* IV, int IVlength, uint8_t* message, int messagelength){
    KeySetup(key, keylength);
    AbsorbStop();
    Absorb(IV, IVlength);
    uint8_t* ciphertext = new uint8_t[messagelength];
    for (unsigned int v = 0; v < messagelength; v++) {
        ciphertext[v] = (message[v] + Squeeze(messagelength)[v]) & 0xff;
    }
    return ciphertext;
}

uint8_t* spritzClass::DecryptWithIV(uint8_t* key, int keylength, uint8_t* IV, int IVlength, uint8_t* ciphertext, int ciphertextlength){
    KeySetup(key, keylength);
    AbsorbStop();
    Absorb(IV, IVlength);
    uint8_t* message = new uint8_t[ciphertextlength];
    for (unsigned int v = 0; v < ciphertextlength; v++) {
        message[v] = (ciphertext[v] - Squeeze(ciphertextlength)[v]) & 0xff;
    }
    return message;
}

uint8_t* spritzClass::Hash(uint8_t* message, int messagelength, uint8_t* r){
    InitializeState();
    Absorb(message, messagelength);
    AbsorbStop();
    Absorb(r, 1);
    return Squeeze(*r);
}

uint8_t* spritzClass::MAC(uint8_t* key, int keylength, uint8_t* message, int messagelength, uint8_t* r){
    InitializeState();
    Absorb(key, keylength);
    AbsorbStop();
    Absorb(message, messagelength);
    AbsorbStop();
    Absorb(r,1);
    return Squeeze(*r);
}

/*uint8_t* spritzClass::AEAD(uint8_t* key, int keylength, uint8_t* nonce, int noncelength, uint8_t* ad, int adlength, uint8_t* message, int messagelength, uint8_t* r){
    InitializeState();
    Absorb(key, keylength);
    AbsorbStop();
    Absorb(nonce, noncelength);
    AbsorbStop();
    Absorb(ad, adlength);
    AbsorbStop();
    //todo divide M in blocks of size N/4
    uint8_t* ciphertext = new uint8_t[messagelength];
    for (int i = 0; i < t; i++) {
        ciphertext[i] = (message[i] + Squeeze(messagelength)[i]) & 0xff;
        Absorb(ciphertext[i], 8);//TODO
    }
    AbsorbStop();
    Absorb(r, 1);
    Squeeze(*r);
    //concat squeeze output with ciphertext
}*/

//Test-Functions
int spritzClass::testvectors(){
    
    unsigned char message_1[4] = "ABC";
    int messagelength_1 = 3;
    unsigned char message_2[5] = "spam";
    int messagelength_2 = 4;
    unsigned char message_3[8] = "arcfour";
    int messagelength_3 = 7;
    uint8_t hashlength[1];
    memset(hashlength, 32, 1);
    
    /**************************/
    /* Basic Output Testvector*/
    /**************************/
    printf("Performing Basic Output Tests...\n");
    // m = ABC
    uint8_t output_1_ref[8] = {0x77, 0x9a, 0x8e, 0x01, 0xf9, 0xe9, 0xcb, 0xc0};
    InitializeState();
    Absorb(message_1, messagelength_1);
    uint8_t output1[8] = {0x00};
    for (int i = 0; i < 8; i++) {
        output1[i] = Drip();
        printf("%02x ", output1[i]);
    }
    printf("\n");
    if (memcmp(output1, output_1_ref, 8) != 0) {
        return -1;
    }
    
    // m = spam
    uint8_t output_2_ref[8] = {0xf0, 0x60, 0x9a, 0x1d, 0xf1, 0x43, 0xce, 0xbf};
    InitializeState();
    Absorb(message_2, messagelength_2);
    uint8_t output2[8] = {0x00};
    for (int i = 0; i < 8; i++) {
        output2[i] = Drip();
        printf("%02x ", output2[i]);
    }
    printf("\n");
    if (memcmp(output2, output_2_ref, 8) != 0) {
        return -1;
    }
    
    // m = arcfour
    uint8_t output_3_ref[8] = {0x1a, 0xfa, 0x8b, 0x5e, 0xe3, 0x37, 0xdb, 0xc7};
    InitializeState();
    Absorb(message_3, messagelength_3);
    uint8_t output3[8] = {0x00};
    for (int i = 0; i < 8; i++) {
        output3[i] = Drip();
        printf("%02x ", output3[i]);
    }
    printf("\n");
    if (memcmp(output3, output_3_ref, 8) != 0) {
        return -1;
    }

    /*******************/
    /* Hash Testvectors*/
    /*******************/
    printf("Performing Hash Tests...\n");
    // m = ABC
    uint8_t hash_1_ref[32] = {0x02, 0x8f, 0xa2, 0xb4, 0x8b, 0x93, 0x4a, 0x18,
                              0x62, 0xb8, 0x69, 0x10, 0x51, 0x3a, 0x47, 0x67,
                              0x7c, 0x1c, 0x2d, 0x95, 0xec, 0x3e, 0x75, 0x70,
                              0x78, 0x6f, 0x1c, 0x32, 0x8b, 0xbd, 0x4a, 0x47};
    uint8_t* hash_1 = Hash(message_1, messagelength_1, hashlength);
    
    for (int i = 0; i<32; i++) {
        printf("%02x ", hash_1[i]);
    }
    printf("\n");
    if (memcmp(hash_1_ref, hash_1, 32) != 0) {
        return -2;
    }
    
    // m = spam
    uint8_t hash_2_ref[32] = {0xac, 0xbb, 0xa0, 0x81, 0x3f, 0x30, 0x0d, 0x3a,
                              0x30, 0x41, 0x0d, 0x14, 0x65, 0x74, 0x21, 0xc1,
                              0x5b, 0x55, 0xe3, 0xa1, 0x4e, 0x32, 0x36, 0xb0,
                              0x39, 0x89, 0xe7, 0x97, 0xc7, 0xaf, 0x47, 0x89};
    uint8_t* hash_2 = Hash(message_2, messagelength_2, hashlength);
    
    for (int i = 0; i<32; i++) {
        printf("%02x ", hash_2[i]);
    }
    printf("\n");
    if (memcmp(hash_2_ref, hash_2, 32) != 0) {
        return -2;
    }
    
    // m = arcfour
    uint8_t hash_3_ref[32] = {0xff, 0x8c, 0xf2, 0x68, 0x09, 0x4c, 0x87, 0xb9,
                              0x5f, 0x74, 0xce, 0x6f, 0xee, 0x9d, 0x30, 0x03,
                              0xa5, 0xf9, 0xfe, 0x69, 0x44, 0x65, 0x3c, 0xd5,
                              0x0e, 0x66, 0xbf, 0x18, 0x9c, 0x63, 0xf6, 0x99};
    uint8_t* hash_3 = Hash(message_3, messagelength_3, hashlength);
    
    for (int i = 0; i<32; i++) {
        printf("%02x ", hash_3[i]);
    }
    printf("\n");
    if (memcmp(hash_3_ref, hash_3, 32) != 0) {
        return -2;
    }
    
    /**********************/
    /* Enc/Dec Testvectors*/
    /**********************/
    printf("Performing Encryption/Decryption Tests...\n");
    // m = ABC
    uint8_t key[3] = {0x00, 0x01, 0x02};
    int keylength = 3;
    
    uint8_t* ciphertext = Encrypt(key, keylength, message_1, strlen((char*)message_1));
    uint8_t* plaintext = Decrypt(key, keylength, ciphertext, strlen((char*)message_1));
    
    printf("ciphertext: ");
    for (int i = 0; i<strlen((char*)message_1); i++) {
        printf("%x ", ciphertext[i]);
    }
    printf(" plaintext: %s  \n", plaintext);
    
    if (memcmp(message_1, plaintext, strlen((char*)message_1)) != 0) {
        return -3;
    }
    
    uint8_t IV[3] = {0x03, 0x04, 0x05};
    
    uint8_t* ciphertext_2 = EncryptWithIV(key, keylength, IV, 3, message_1, strlen((char*)message_1));
    uint8_t* plaintext_2 = DecryptWithIV(key, keylength, IV, 3, ciphertext_2, strlen((char*)message_1));
    
    printf("ciphertext: ");
    for (int i = 0; i<strlen((char*)message_1); i++) {
        printf("%x ", ciphertext_2[i]);
    }
    printf(" plaintext: %s  \n", plaintext_2);
    
    if (memcmp(message_1, plaintext_2, strlen((char*)message_1)) != 0) {
        return -3;
    }
    
    /**********************/
    /* MAC Testvectors    */
    /**********************/
    printf("Performing MAC Tests...\n");
    // m = ABC
    uint8_t mac_ref[32] = {0xbe, 0x8e, 0xdc, 0xf2, 0x76, 0xcf, 0x57, 0xb4,
                           0x0e, 0xbc, 0x8e, 0x22, 0x43, 0x45, 0x7e, 0x3e,
                           0xb7, 0xc6, 0x4d, 0x4e, 0x99, 0x1e, 0x93, 0x58,
                           0xce, 0x81, 0xef, 0xb1, 0x6c, 0xce, 0xc7, 0xed};
    uint8_t* mac = MAC(key, keylength, message_1, strlen((char*)message_1), hashlength);

    for (int i = 0; i<32; i++) {
        printf("%02x ", mac[i]);
    }
    printf("\n");
    if (memcmp(mac_ref, mac, 32) != 0) {
        return -4;
    }
    
    /***********************/
    /* Inverse Testvectors */
    /***********************/
    printf("Performing Inverse Tests...\n");
    InitializeState();
    uint8_t iBeforeWhip = this->i;
    uint8_t jBeforeWhip = this->j;
    uint8_t kBeforeWhip = this->k;
    
    uint8_t SBeforeWhip[this->N];
    memcpy(SBeforeWhip, this->S, this->N);
    Whip(2*this->N);
    inverseWhip(2*this->N);
    
    if (memcmp(SBeforeWhip, this->S, this->N) != 0 || iBeforeWhip != this->i || jBeforeWhip != this->j || kBeforeWhip != this->k){
        return -5;
    }
    
    InitializeState();
    uint8_t value = 0x0f;
    uint8_t SBeforeAbsorb[this->N];
    memcpy(SBeforeAbsorb, this->S, this->N);
    AbsorbNibble(value);
    inverseAbsorbNibble(value);
    
    if (memcmp(SBeforeAbsorb, this->S, this->N) != 0){
        return -5;
    }

    return 0;
}

/*int main(int argc, char** argv){
    
    if(signed int ret = testvectors() != 0)
        printf("Testvectors failed (%d)!\n", ret);
    else
        printf("All Tests ok!\n");
    
    //tryattack();
    
    return 0;
}*/
void spritzClass::setN(int n){this->N = n;}

spritzClass spritzClass::getCipher(){return (*this);}

void spritzClass::setCipher(spritzClass _c){
    this->i = _c.i;
    this->j = _c.j;
    this->k = _c.k;
    this->z = _c.z;
    this->a = _c.a;
    this->w = _c.w;
    this->N = _c.N;
    
    for (int i = 0; i <= this->N-1; i++) {
        this->S[i] = _c.S[i];
    }
}

void spritzClass::setS(uint8_t *S){
    for (int i = 0; i <= this->N-1; i++) {
        this->S[i] = S[i];
    }
}

