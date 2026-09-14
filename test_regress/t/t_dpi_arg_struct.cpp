// DESCRIPTION: Verilator: Verilog Test module
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of either the GNU Lesser General Public License Version 3
// or the Perl Artistic License Version 2.0.
// SPDX-FileCopyrightText: 2026 Wilson Snyder
// SPDX-License-Identifier: LGPL-3.0-only OR Artistic-2.0

#include <cstdint>
#include <cstdio>
#include <cstdlib>

// The C view of the SV unpacked struct; layout must match member for member.
struct all_s {
    int8_t b;
    int16_t s;
    int32_t i;
    int64_t l;
    int64_t arr[4];
};

#define CHECK(got, exp) \
    do { \
        if ((got) != (exp)) { \
            printf("%%Error: %s:%d: got=0x%llx exp=0x%llx\n", __FILE__, __LINE__, \
                   (unsigned long long)(got), (unsigned long long)(exp)); \
            abort(); \
        } \
    } while (0)

extern "C" {

// SV exports, declared against the matching C struct.
void e_struct_input(const all_s* valp);
void e_struct_output(all_s* valp);

void i_struct_input(const all_s* valp) {
    CHECK(valp->b, (int8_t)0x12);
    CHECK(valp->s, (int16_t)0x1234);
    CHECK(valp->i, (int32_t)0x12345678);
    CHECK(valp->l, (int64_t)0x123456789abcdef0LL);
    for (int j = 0; j < 4; ++j) CHECK(valp->arr[j], (int64_t)(j + 1));
}

void i_struct_output(all_s* valp) {
    valp->b = (int8_t)0x21;
    valp->s = (int16_t)0x4321;
    valp->i = (int32_t)0x87654321;
    valp->l = (int64_t)0xf0debc9a78563412ULL;
    for (int j = 0; j < 4; ++j) valp->arr[j] = j + 10;
}

void i_call_exports() {
    all_s in{};
    in.b = 0x33;
    in.i = 0x33333333;
    e_struct_input(&in);

    all_s out{};
    e_struct_output(&out);
    CHECK(out.b, (int8_t)0x44);
    CHECK(out.s, (int16_t)0x4444);
    CHECK(out.i, (int32_t)0x44444444);
    CHECK(out.l, (int64_t)0x4444444444444444LL);
    for (int j = 0; j < 4; ++j) CHECK(out.arr[j], (int64_t)(j + 40));
}

void i_struct_inout(all_s* valp) {
    CHECK(valp->b, (int8_t)0x21);
    CHECK(valp->s, (int16_t)0x4321);
    CHECK(valp->i, (int32_t)0x87654321);
    CHECK(valp->l, (int64_t)0xf0debc9a78563412ULL);
    valp->b += 1;
    valp->s += 1;
    valp->i += 1;
    valp->l += 1;
    for (int j = 0; j < 4; ++j) valp->arr[j] += 1;
}
}
