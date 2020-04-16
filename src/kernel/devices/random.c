#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <spinel/random.h>

// Not sure if this actually constitutes a "device",
// but it's accessible as /dev/random, sooo...
// This is currently based on the ChaCha20 stream cipher
// Y'know, gotta have a CSPRNG for panic()
// For now, I've implemented this myself rather than using
// a public domain implementation as a part of the learning
// experience that Spinel embodies
// If speed becomes a concern down the line, this is probably why!

// A ChaCha block is defined as such:
// A 16-byte (128-bit) constant,
// a 32-byte (256-bit) key,
// an 8-byte (64-bit) position,
// and an 8-byte (64-bit) nonce
/*
    "expa"      "nd 3"      "2-by"  "te k"
    Key         Key         Key     Key
    Key         Key         Key     Key
    Position    Position    Nonce   Nonce
 */

#define ChaChaRounds 20

// ChaCha ouputs 64 bytes of output per block (4 * 16 = sizeof(uint32_t) * 16)
// The key is regenerated each time a random number request is called, so 1 block
// is reserved expressly for this purpose
#define MaxBlocksToGen 32
#define SizeOfBlock ((sizeof(uint32_t) * 16))
#define MaxNumRandomBytes (((MaxBlocksToGen) - 1) * (SizeOfBlock))

// If C had references, this might work as an inline function
// constDistRotLeft is defined below as an inline function
#define QuarterRound(a, b, c, d) do {\
    a += b;\
    d ^= a;\
    d = constDistRotLeft(d, 16);\
    c += d;\
    b ^= c;\
    b = constDistRotLeft(b, 12);\
    a += b;\
    d ^= a;\
    d = constDistRotLeft(d, 8);\
    c += d;\
    b ^= c;\
    b = constDistRotLeft(b, 7);\
} while (0)

// TODO: I'm not sure how endianness affects the constant
// This might affect usage of files encrypted on a machine with
// a different endianness vs. the current machine
// expa
static const uint32_t ConstantA = 0x65787061;
// nd 3
static const uint32_t ConstantB = 0x6E642033;
// 2-by
static const uint64_t ConstantC = 0x322D6279;
// te k
static const uint32_t ConstantD = 0x7465206B;
static const size_t KeySize = 32;
static const size_t KeyOffset = 4;

static bool randomInitialized = false;
// TODO: is this correct?
static uint32_t rngState[16] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
};

// Perform x <<< y
// TODO: is this correct?
static inline uint32_t constDistRotLeft(uint32_t x, uint32_t y) {
    return (x << y) | (x >> (32 - y));
}

void chachaCipherBlock(uint32_t in[16], uint32_t out[16]) {
    // TODO: implementation I'm referencing creates local array and only
    // copies to out at the end, is this necessary?
    memcpy((void*)out, in, sizeof(uint32_t) * 16);

    for (unsigned i = 0; i < ChaChaRounds; i++) {
        if (i % 2 == 0) {
            // even
            // Quarter round on each column
            QuarterRound(out[0], out[4], out[8], out[12]);
            QuarterRound(out[1], out[5], out[9], out[13]);
            QuarterRound(out[2], out[6], out[10], out[14]);
            QuarterRound(out[3], out[7], out[11], out[15]);
        } else {
            // Quarter round on diagonals
            QuarterRound(out[0], out[5], out[10], out[15]);
            QuarterRound(out[1], out[6], out[11], out[12]);
            QuarterRound(out[2], out[7], out[8], out[13]);
            QuarterRound(out[3], out[4], out[9], out[14]);
        }
    }

    // Now the results are combined with the initial values
    for (int i = 0; i < 16; i++) {
        out[i] += in[i];
    }

    // increment position, then nonce
    in[13]++;
    if (in[13] == 0) {
        // overflow
        in[12]++;
        if (in[12] == 0) {
            in[15]++;
            if (in[15] == 0) {
                in[14]++;
            }
        }
    }
}

void initRandom(void) {
    // initial seed for the RNG
    printf("Initializing RNG...\n");
    rngState[0] = ConstantA;
    rngState[1] = ConstantB;
    rngState[2] = ConstantC;
    rngState[3] = ConstantD;
    randomInitialized = true;
    printf("RNG initialized.\n");
}

ssize_t readRandom(uint8_t* buf, size_t length) {
    assert(randomInitialized);
    if (buf == NULL) {
        return -EFAULT;
    }

    if (length > MaxNumRandomBytes) {
        length = MaxNumRandomBytes;
    }

    size_t bufOffset = 0;
    ssize_t remainingLength = length;
    while (remainingLength > 0) {
        uint32_t block[16];
        chachaCipherBlock(rngState, block);
        memcpy(
            buf + bufOffset, block,
            remainingLength > SizeOfBlock ? SizeOfBlock : remainingLength
        );
        memset(block, 0, SizeOfBlock);
        if (remainingLength <= SizeOfBlock) {
            // Don't even try and keep going
            break;
        } else {
            remainingLength -= SizeOfBlock;
        }
        bufOffset += SizeOfBlock;
    }

    // Generate new key
    uint32_t block[16];
    chachaCipherBlock(rngState, block);
    memcpy(rngState + KeyOffset, block, KeySize);
    memset(block, 0, SizeOfBlock);

    return length;
}

ssize_t feedRandom(uint8_t* buf, size_t length) {
    if (buf == NULL) {
        return -EFAULT;
    }

    // TODO: this probably isn't that great,
    // but it mixes entropy into the RNG, which is the goal here

    size_t poolIdx = 0;
    if (randomInitialized) {
        // Get a random starting location -- this probably doesn't need to be
        // good and can perhaps even be deterministic, but it might help with
        // mixing of entropy
        poolIdx = randomRange32(KeyOffset, KeyOffset + 8);
    }
    for (size_t i = 0; i < length; i++) {
        rngState[poolIdx] ^= buf[i] ^ buf[poolIdx];
        poolIdx = (poolIdx + 1) % KeyOffset + 8;
    }

    return length;
}

uint32_t random32(void) {
    uint8_t bytes[4];
    readRandom(bytes, 4);
    // TODO: this is probably not great
    return *(uint32_t*)bytes;
}

uint32_t randomRange32(uint32_t rangeStart, uint32_t rangeEnd) {
    // Similar to arc4random_uniform in BSD systems, this avoids modulo bias
    uint32_t rangeLen = rangeEnd - rangeStart;
    uint32_t res;
    while (1) {
        res = random32();
        // -rangeLen == 2**32 - rangeLen
        if (res >= -rangeLen % rangeLen) {
            break;
        }
    }
    return (res % rangeLen) + rangeStart;
}

uint64_t random64(void) {
    uint8_t bytes[8];
    readRandom(bytes, 8);
    return *(uint64_t*)bytes;
}
