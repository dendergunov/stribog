#ifndef STRIBOG_HPP
#define STRIBOG_HPP

//permutation

//hash512()
void hash_512(const unsigned char* message, unsigned long long length, unsigned char out[512]);
void hash_256(const unsigned char* message, unsigned long long length, unsigned char out[256]);

#endif // STRIBOG_H
