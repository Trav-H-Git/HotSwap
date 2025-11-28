#pragma once
#include "core/types/xen_primitives.h"
#include "core/types/xen_string.h"
#include <cstdint>
//#include <cstring>
static constexpr uint32_t DEF_SEED = 0x07F07C65;


static inline uint32_t HashMix32(uint32_t h1)
{
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;
    return h1;
}

static inline uint32_t HashMurmur3Int32(uint32_t _in, uint32_t _seed = DEF_SEED) {
	_in *= 0xcc9e2d51;
	_in = (_in << 15) | (_in >> 17);
	_in *= 0x1b873593;

	_seed ^= _in;
	_seed = (_seed << 13) | (_seed >> 19);
	_seed = _seed * 5 + 0xe6546b64;

	return _seed;
}

static inline uint32_t HashMurmur3Int64(uint64_t _in, uint32_t _seed = DEF_SEED) {
	uint32_t h1 = HashMurmur3Int32(static_cast<uint32_t>(_in), _seed);
	return HashMurmur3Int32(static_cast<uint32_t>(_in >> 32), h1);
}


static inline uint32_t HashMurmur3Ptr(const void* key, int len, uint32_t seed = DEF_SEED) {
    const uint8_t* data = static_cast<const uint8_t*>(key);
    const int nblocks = len / 4;

    uint32_t h1 = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // Body: Process 4-byte chunks
    const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);
    for (int i = -nblocks; i; i++) {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> (32 - 15));  // ROTL32
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> (32 - 13));  // ROTL32
        h1 = h1 * 5 + 0xe6546b64;
    }

    // Tail: Process remaining bytes
    const uint8_t* tail = data + nblocks * 4;
    uint32_t k1 = 0;

    switch (len & 3) {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1: k1 ^= tail[0];
        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> (32 - 15));  // ROTL32
        k1 *= c2;
        h1 ^= k1;
    }

    // Finalization: Mix the hash
    h1 ^= len;
    return HashMix32(h1);
}


static constexpr int PRIME_CAPACITY_TABLE_LENGTH = 29;

inline const static uint32_t prime_capacity_table[PRIME_CAPACITY_TABLE_LENGTH] = {
5,
13,
23,
47,
97,
193,
389,
769,
1543,
3079,
6151,
12289,
24593,
49157,
98317,
196613,
393241,
786433,
1572869,
3145739,
6291469,
12582917,
25165843,
50331653,
100663319,
201326611,
402653189,
805306457,
1610612741,
};

// Computed with elem_i = UINT64_C (0 x FFFFFFFF FFFFFFFF ) / d_i + 1, where d_i is the i-th element of the above array.
inline const static uint64_t prime_capacity_table_inverse[PRIME_CAPACITY_TABLE_LENGTH] = {
	3689348814741910324,
	1418980313362273202,
	802032351030850071,
	392483916461905354,
	190172619316593316,
	95578984837873325,
	47420935922132524,
	23987963684927896,
	11955116055547344,
	5991147799191151,
	2998982941588287,
	1501077717772769,
	750081082979285,
	375261795343686,
	187625172388393,
	93822606204624,
	46909513691883,
	23456218233098,
	11728086747027,
	5864041509391,
	2932024948977,
	1466014921160,
	733007198436,
	366503839517,
	183251896093,
	91625960335,
	45812983922,
	22906489714,
	11453246088
};

struct uint128_t {
	uint64_t low;
	uint64_t high;
};

static __forceinline uint128_t Multiply64To128(uint64_t a, uint64_t b)
{
	// Split the inputs into high and low 32-bit parts
	uint64_t a_lo = (a & 0x00000000FFFFFFFF);
	uint64_t a_hi = a >> 32;
	uint64_t b_lo = (b & 0x00000000FFFFFFFF);
	uint64_t b_hi = b >> 32;

	// Calculate the partial products
	uint64_t lo_lo = a_lo * b_lo;
	uint64_t hi_lo = a_hi * b_lo;
	uint64_t lo_hi = a_lo * b_hi;
	uint64_t hi_hi = a_hi * b_hi;

	// Combine the results
	uint64_t carry = (lo_lo >> 32) + (hi_lo & 0xFFFFFFFF) + (lo_hi & 0xFFFFFFFF);
	uint64_t low = (carry << 32) | (lo_lo & 0xFFFFFFFF);
	uint64_t high = hi_hi + (hi_lo >> 32) + (lo_hi >> 32) + (carry >> 32);

	return { low, high };
}

static __forceinline uint32_t FastMod(uint32_t _hash, int _capacity_idx)
{
	uint128_t val = Multiply64To128(_hash * prime_capacity_table_inverse[_capacity_idx], prime_capacity_table[_capacity_idx]);
	//_hash % prime_capacity_table[_capacity_idx]
	return (uint32_t)val.high;
}

struct Hashes
{
	inline static uint32 Hash(const String& _key){return HashMurmur3Ptr((const void*)_key.CStr(), _key.Length());}
	inline static uint32 Hash(const uint64 _key){ return HashMurmur3Int64(_key);}
	inline static uint32 Hash(const uint32 _key){ return HashMurmur3Int32(_key);}
	inline static uint32 Hash(const uint16 _key){ return HashMix32(_key);}
	inline static uint32 Hash(const int _key){ return HashMix32(_key);}
};
