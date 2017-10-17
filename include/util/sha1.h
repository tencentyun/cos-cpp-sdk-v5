#ifndef SHA_H
#define SHA_H
#include <stdlib.h>
#include <stdio.h>

#include <string>

namespace qcloud_cos {

/* Useful defines & typedefs */
typedef unsigned char SHA_BYTE;	/* 8-bit quantity */

// SHA_LONG already defined in openssl/sha.h
typedef unsigned int  SHA_LONG;	/* 32-or-more-bit quantity */
#define SHA_BYTE_ORDER 1234
#define SHA_VERSION 1

#define SHA_BLOCKSIZE		64
#define SHA_DIGESTSIZE		20

typedef struct {
    SHA_LONG digest[5];		/* message digest */
    SHA_LONG count_lo, count_hi;	/* 64-bit bit count */
    SHA_BYTE data[SHA_BLOCKSIZE];	/* SHA data buffer */
    int local;			/* unprocessed amount in data */
} SHA_INFO;


void ShaInit(SHA_INFO *);
void ShaUpdate(SHA_INFO *, SHA_BYTE *, int);
void ShaFinal(unsigned char [20], SHA_INFO *);

void ShaOutput(unsigned char [20],unsigned char [40]);
const char* ShaVersion(void);

class Sha1 {
public:
    Sha1();
    ~Sha1();

    void Append(const char* data, unsigned int size);
    std::string Final();

private:
    SHA_INFO    m_sha;
};

}
#endif /* SHA_H */
