/*
 * hof - helpers of fortune.
 *
 * Copyright © 2015 Jérôme Glisse
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * Authors: Jérôme Glisse <jglisse@redhat.com>
 */
#include <stdio.h>
#include "hof.h"

/*****************************************************************************
 * RANDOM
 *
 * Try to use /dev/random, or fallback to random()
 */
static FILE *_hof_random_file = NULL;

uint32_t hof_random32()
{
    uint32_t v;

    if (!_hof_random_file) {
        _hof_random_file = fopen("/dev/random", "r");
        if (!_hof_random_file) {
            return random();
        }
    }
    if (fread(&v, 4, 1, _hof_random_file) != 1) {
        return random();
    }
    return v;
}


/*****************************************************************************
 * STRING HASH
 *
 * Using murmurhash, for algorithm see :
 *   http://en.wikipedia.org/wiki/MurmurHash
 */
static uint32_t _hof_hash32_seed = 0;

uint32_t hof_string_hash32(const char *string)
{
    static const uint32_t c1 = 0xcc9e2d51;
    static const uint32_t c2 = 0x1b873593;
    static const uint32_t r1 = 15;
    static const uint32_t r2 = 13;
    static const uint32_t m = 5;
    static const uint32_t n = 0xe6546b64;
    uint32_t hash;
    int i = 0;

    /* Initialize seed of needed. */
    while (!_hof_hash32_seed) {
        _hof_hash32_seed = hof_random32();
    }

    hash = _hof_hash32_seed;

    /* Process 32bits blocks first. */
    while (string[i+0] && string[i+1] && string[i+2] && string[i+3]) {
        uint32_t k = *((uint32_t *)&string[i]);

        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
        i += 4;
    }

    /* Process remaining bits (24bits at most). */
    if (string[i]) {
        uint32_t k;

        k = string[i+1] && string[i+2] ? string[i+2] << 16 : 0;
        k |= string[i+1] ? string[i+1] << 8 : 0;
        k |= string[i+0];

        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;
        hash ^= k;

        i += 1 + !!string[i+1] + !!string[i+2];
    }

    hash ^= i;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}
