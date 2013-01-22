#define cKeccakFixedOutputLengthInBytes 32
#define cKeccakB    1600
#define cKeccakR    1088

#define CRYPTO_BYTES 32
#define CRYPTO_VERSION "3.2.5"

#define cKeccakR_SizeInBytes    (cKeccakR / 8)

#ifndef crypto_hash_BYTES
    #ifdef cKeccakFixedOutputLengthInBytes
        #define crypto_hash_BYTES cKeccakFixedOutputLengthInBytes
    #else
        #define crypto_hash_BYTES cKeccakR_SizeInBytes
    #endif
#endif
#if (crypto_hash_BYTES > cKeccakR_SizeInBytes)
    #error "Full squeezing not yet implemented"
#endif

#if     (cKeccakB   == 1600)
    typedef unsigned long long  UINT64;
    typedef UINT64 tKeccakLane;
    #define cKeccakNumberOfRounds   24
#elif   (cKeccakB   == 800)
    typedef unsigned int        UINT32;
    // WARNING: on 8-bit and 16-bit platforms, this should be replaced by:
    //typedef unsigned long       UINT32;
    typedef UINT32 tKeccakLane;
    #define cKeccakNumberOfRounds   22
#elif   (cKeccakB   == 400)
    typedef unsigned short      UINT16;
    typedef UINT16 tKeccakLane;
    #define cKeccakNumberOfRounds   20
#elif   (cKeccakB   == 200)
    typedef unsigned char       UINT8;
    typedef UINT8 tKeccakLane;
    #define cKeccakNumberOfRounds   18
#else
    #error  "Unsupported Keccak-f width"
#endif

#define cKeccakLaneSizeInBits   (sizeof(tKeccakLane) * 8)

#define ROL(a, offset) ((((tKeccakLane)a) << ((offset) % cKeccakLaneSizeInBits)) ^ (((tKeccakLane)a) >> (cKeccakLaneSizeInBits-((offset) % cKeccakLaneSizeInBits))))
#if ((cKeccakB/25) == 8)
    #define ROL_mult8(a, offset) ((tKeccakLane)a)
#else
    #define ROL_mult8(a, offset) ROL(a, offset)
#endif

typedef struct {
  tKeccakLane state[5*5];
  unsigned char tail[cKeccakR_SizeInBytes];
  int taillen;
} SHA3_256_CTX;

void KeccakF( tKeccakLane * state, const tKeccakLane *in, int laneCount );
void SHA3_256_Init( SHA3_256_CTX *s );
void SHA3_256_Update( SHA3_256_CTX *s, const unsigned char *in, unsigned long long inlen );
void SHA3_256_Final( unsigned char *out, SHA3_256_CTX *s );
void KeccakF( tKeccakLane * state, const tKeccakLane *in, int laneCount );
