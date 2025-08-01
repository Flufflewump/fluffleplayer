// SPDX-License-Identifier: BSD-3-Clause AND ISC AND MIT
/*BSD 3-Clause License

Copyright (c) 2014-2017, ipkn
              2020-2022, CrowCpp
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the author nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The Crow logo and other graphic material (excluding third party logos) used are under exclusive Copyright (c) 2021-2022, Farook Al-Sammarraie (The-EDev), All rights reserved.
*/
#pragma once

namespace crow
{
    constexpr const char VERSION[] = "master";
}

/*
 * SHA1 Wikipedia Page: http://en.wikipedia.org/wiki/SHA-1
 *
 * Copyright (c) 2012-22 SAURAV MOHAPATRA <mohaps@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file TinySHA1.hpp
 * \author SAURAV MOHAPATRA <mohaps@gmail.com>
 * \date 2012-22
 * \brief TinySHA1 - a header only implementation of the SHA1 algorithm in C++. Based
 * on the implementation in boost::uuid::details.
 *
 * In this file are defined:
 * - sha1::SHA1
 */
#ifndef _TINY_SHA1_HPP_
#define _TINY_SHA1_HPP_
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdint.h>

/**
 * \namespace sha1
 * \brief Here is defined the SHA1 class
 */
namespace sha1
{
    /**
     * \class SHA1
     * \brief A tiny SHA1 algorithm implementation used internally in the
     * Crow server (specifically in crow/websocket.h).
     */
    class SHA1
    {
    public:
        typedef uint32_t digest32_t[5];
        typedef uint8_t digest8_t[20];
        inline static uint32_t LeftRotate(uint32_t value, size_t count) {
            return (value << count) ^ (value >> (32-count));
        }
        SHA1(){ reset(); }
        virtual ~SHA1() {}
        SHA1(const SHA1& s) { *this = s; }
        const SHA1& operator = (const SHA1& s) {
            memcpy(m_digest, s.m_digest, 5 * sizeof(uint32_t));
            memcpy(m_block, s.m_block, 64);
            m_blockByteIndex = s.m_blockByteIndex;
            m_byteCount = s.m_byteCount;
            return *this;
        }
        SHA1& reset() {
            m_digest[0] = 0x67452301;
            m_digest[1] = 0xEFCDAB89;
            m_digest[2] = 0x98BADCFE;
            m_digest[3] = 0x10325476;
            m_digest[4] = 0xC3D2E1F0;
            m_blockByteIndex = 0;
            m_byteCount = 0;
            return *this;
        }
        SHA1& processByte(uint8_t octet) {
            this->m_block[this->m_blockByteIndex++] = octet;
            ++this->m_byteCount;
            if(m_blockByteIndex == 64) {
                this->m_blockByteIndex = 0;
                processBlock();
            }
            return *this;
        }
        SHA1& processBlock(const void* const start, const void* const end) {
            const uint8_t* begin = static_cast<const uint8_t*>(start);
            const uint8_t* finish = static_cast<const uint8_t*>(end);
            while(begin != finish) {
                processByte(*begin);
                begin++;
            }
            return *this;
        }
        SHA1& processBytes(const void* const data, size_t len) {
            const uint8_t* block = static_cast<const uint8_t*>(data);
            processBlock(block, block + len);
            return *this;
        }
        const uint32_t* getDigest(digest32_t digest) {
            size_t bitCount = this->m_byteCount * 8;
            processByte(0x80);
            if (this->m_blockByteIndex > 56) {
                while (m_blockByteIndex != 0) {
                    processByte(0);
                }
                while (m_blockByteIndex < 56) {
                    processByte(0);
                }
            } else {
                while (m_blockByteIndex < 56) {
                    processByte(0);
                }
            }
            processByte(0);
            processByte(0);
            processByte(0);
            processByte(0);
            processByte( static_cast<unsigned char>((bitCount>>24) & 0xFF));
            processByte( static_cast<unsigned char>((bitCount>>16) & 0xFF));
            processByte( static_cast<unsigned char>((bitCount>>8 ) & 0xFF));
            processByte( static_cast<unsigned char>((bitCount)     & 0xFF));

            memcpy(digest, m_digest, 5 * sizeof(uint32_t));
            return digest;
        }
        const uint8_t* getDigestBytes(digest8_t digest) {
            digest32_t d32;
            getDigest(d32);
            size_t di = 0;
            digest[di++] = ((d32[0] >> 24) & 0xFF);
            digest[di++] = ((d32[0] >> 16) & 0xFF);
            digest[di++] = ((d32[0] >> 8) & 0xFF);
            digest[di++] = ((d32[0]) & 0xFF);

            digest[di++] = ((d32[1] >> 24) & 0xFF);
            digest[di++] = ((d32[1] >> 16) & 0xFF);
            digest[di++] = ((d32[1] >> 8) & 0xFF);
            digest[di++] = ((d32[1]) & 0xFF);

            digest[di++] = ((d32[2] >> 24) & 0xFF);
            digest[di++] = ((d32[2] >> 16) & 0xFF);
            digest[di++] = ((d32[2] >> 8) & 0xFF);
            digest[di++] = ((d32[2]) & 0xFF);

            digest[di++] = ((d32[3] >> 24) & 0xFF);
            digest[di++] = ((d32[3] >> 16) & 0xFF);
            digest[di++] = ((d32[3] >> 8) & 0xFF);
            digest[di++] = ((d32[3]) & 0xFF);

            digest[di++] = ((d32[4] >> 24) & 0xFF);
            digest[di++] = ((d32[4] >> 16) & 0xFF);
            digest[di++] = ((d32[4] >> 8) & 0xFF);
            digest[di++] = ((d32[4]) & 0xFF);
            return digest;
        }

    protected:
        void processBlock() {
            uint32_t w[80];
            for (size_t i = 0; i < 16; i++) {
                w[i]  = (m_block[i*4 + 0] << 24);
                w[i] |= (m_block[i*4 + 1] << 16);
                w[i] |= (m_block[i*4 + 2] << 8);
                w[i] |= (m_block[i*4 + 3]);
            }
            for (size_t i = 16; i < 80; i++) {
                w[i] = LeftRotate((w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]), 1);
            }

            uint32_t a = m_digest[0];
            uint32_t b = m_digest[1];
            uint32_t c = m_digest[2];
            uint32_t d = m_digest[3];
            uint32_t e = m_digest[4];

            for (std::size_t i=0; i<80; ++i) {
                uint32_t f = 0;
                uint32_t k = 0;

                if (i<20) {
                    f = (b & c) | (~b & d);
                    k = 0x5A827999;
                } else if (i<40) {
                    f = b ^ c ^ d;
                    k = 0x6ED9EBA1;
                } else if (i<60) {
                    f = (b & c) | (b & d) | (c & d);
                    k = 0x8F1BBCDC;
                } else {
                    f = b ^ c ^ d;
                    k = 0xCA62C1D6;
                }
                uint32_t temp = LeftRotate(a, 5) + f + e + k + w[i];
                e = d;
                d = c;
                c = LeftRotate(b, 30);
                b = a;
                a = temp;
            }

            m_digest[0] += a;
            m_digest[1] += b;
            m_digest[2] += c;
            m_digest[3] += d;
            m_digest[4] += e;
        }
    private:
        digest32_t m_digest;
        uint8_t m_block[64];
        size_t m_blockByteIndex;
        size_t m_byteCount;
    };
}
#endif

// settings for crow
// TODO(ipkn) replace with runtime config. libucl?

/* #ifdef - enables debug mode */
//#define CROW_ENABLE_DEBUG

/* #ifdef - enables logging */
#define CROW_ENABLE_LOGGING

/* #ifdef - enforces section 5.2 and 6.1 of RFC6455 (only accepting masked messages from clients) */
//#define CROW_ENFORCE_WS_SPEC

/* #define - specifies log level */
/*
    Debug       = 0
    Info        = 1
    Warning     = 2
    Error       = 3
    Critical    = 4

    default to INFO
*/
#ifndef CROW_LOG_LEVEL
#define CROW_LOG_LEVEL 1
#endif

#ifndef CROW_STATIC_DIRECTORY
#define CROW_STATIC_DIRECTORY "static/"
#endif
#ifndef CROW_STATIC_ENDPOINT
#define CROW_STATIC_ENDPOINT "/static/<path>"
#endif

// compiler flags

#if defined(_MSC_VER)
#if _MSC_VER < 1900
#define CROW_MSVC_WORKAROUND
#define constexpr const
#define noexcept throw()
#endif
#endif


#include <cstdint>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <cstring>
#include <cctype>
#include <functional>
#include <string>
#include <string_view>
#include <sstream>
#include <unordered_map>
#include <random>
#include <algorithm>


#include <filesystem>

// TODO(EDev): Adding C++20's [[likely]] and [[unlikely]] attributes might be useful
#if defined(__GNUG__) || defined(__clang__)
#define CROW_LIKELY(X) __builtin_expect(!!(X), 1)
#define CROW_UNLIKELY(X) __builtin_expect(!!(X), 0)
#else
#define CROW_LIKELY(X) (X)
#define CROW_UNLIKELY(X) (X)
#endif

namespace crow
{
    /// @cond SKIP
    namespace black_magic
    {
#ifndef CROW_MSVC_WORKAROUND
        /// Out of Range Exception for const_str
        struct OutOfRange
        {
            OutOfRange(unsigned /*pos*/, unsigned /*length*/) {}
        };
        /// Helper function to throw an exception if i is larger than len
        constexpr unsigned requires_in_range(unsigned i, unsigned len)
        {
            return i >= len ? throw OutOfRange(i, len) : i;
        }

        /// A constant string implementation.
        class const_str
        {
            const char* const begin_;
            unsigned size_;

        public:
            template<unsigned N>
            constexpr const_str(const char (&arr)[N]):
              begin_(arr), size_(N - 1)
            {
                static_assert(N >= 1, "not a string literal");
            }
            constexpr char operator[](unsigned i) const
            {
                return requires_in_range(i, size_), begin_[i];
            }

            constexpr operator const char*() const
            {
                return begin_;
            }

            constexpr const char* begin() const { return begin_; }
            constexpr const char* end() const { return begin_ + size_; }

            constexpr unsigned size() const
            {
                return size_;
            }
        };

        constexpr unsigned find_closing_tag(const_str s, unsigned p)
        {
            return s[p] == '>' ? p : find_closing_tag(s, p + 1);
        }

        /// Check that the CROW_ROUTE string is valid
        constexpr bool is_valid(const_str s, unsigned i = 0, int f = 0)
        {
            return i == s.size()   ? f == 0 :
                   f < 0 || f >= 2 ? false :
                   s[i] == '<'     ? is_valid(s, i + 1, f + 1) :
                   s[i] == '>'     ? is_valid(s, i + 1, f - 1) :
                                     is_valid(s, i + 1, f);
        }

        constexpr bool is_equ_p(const char* a, const char* b, unsigned n)
        {
            return *a == 0 && *b == 0 && n == 0 ? true :
                   (*a == 0 || *b == 0)         ? false :
                   n == 0                       ? true :
                   *a != *b                     ? false :
                                                  is_equ_p(a + 1, b + 1, n - 1);
        }

        constexpr bool is_equ_n(const_str a, unsigned ai, const_str b, unsigned bi, unsigned n)
        {
            return ai + n > a.size() || bi + n > b.size() ? false :
                   n == 0                                 ? true :
                   a[ai] != b[bi]                         ? false :
                                                            is_equ_n(a, ai + 1, b, bi + 1, n - 1);
        }

        constexpr bool is_int(const_str s, unsigned i)
        {
            return is_equ_n(s, i, "<int>", 0, 5);
        }

        constexpr bool is_uint(const_str s, unsigned i)
        {
            return is_equ_n(s, i, "<uint>", 0, 6);
        }

        constexpr bool is_float(const_str s, unsigned i)
        {
            return is_equ_n(s, i, "<float>", 0, 7) ||
                   is_equ_n(s, i, "<double>", 0, 8);
        }

        constexpr bool is_str(const_str s, unsigned i)
        {
            return is_equ_n(s, i, "<str>", 0, 5) ||
                   is_equ_n(s, i, "<string>", 0, 8);
        }

        constexpr bool is_path(const_str s, unsigned i)
        {
            return is_equ_n(s, i, "<path>", 0, 6);
        }
#endif
        template<typename T>
        struct parameter_tag
        {
            static const int value = 0;
        };
#define CROW_INTERNAL_PARAMETER_TAG(t, i) \
    template<>                            \
    struct parameter_tag<t>               \
    {                                     \
        static const int value = i;       \
    }
        CROW_INTERNAL_PARAMETER_TAG(int, 1);
        CROW_INTERNAL_PARAMETER_TAG(char, 1);
        CROW_INTERNAL_PARAMETER_TAG(short, 1);
        CROW_INTERNAL_PARAMETER_TAG(long, 1);
        CROW_INTERNAL_PARAMETER_TAG(long long, 1);
        CROW_INTERNAL_PARAMETER_TAG(unsigned int, 2);
        CROW_INTERNAL_PARAMETER_TAG(unsigned char, 2);
        CROW_INTERNAL_PARAMETER_TAG(unsigned short, 2);
        CROW_INTERNAL_PARAMETER_TAG(unsigned long, 2);
        CROW_INTERNAL_PARAMETER_TAG(unsigned long long, 2);
        CROW_INTERNAL_PARAMETER_TAG(double, 3);
        CROW_INTERNAL_PARAMETER_TAG(std::string, 4);
#undef CROW_INTERNAL_PARAMETER_TAG
        template<typename... Args>
        struct compute_parameter_tag_from_args_list;

        template<>
        struct compute_parameter_tag_from_args_list<>
        {
            static const int value = 0;
        };

        template<typename Arg, typename... Args>
        struct compute_parameter_tag_from_args_list<Arg, Args...>
        {
            static const int sub_value =
              compute_parameter_tag_from_args_list<Args...>::value;
            static const int value =
              parameter_tag<typename std::decay<Arg>::type>::value ? sub_value * 6 + parameter_tag<typename std::decay<Arg>::type>::value : sub_value;
        };

        static inline bool is_parameter_tag_compatible(uint64_t a, uint64_t b)
        {
            if (a == 0)
                return b == 0;
            if (b == 0)
                return a == 0;
            int sa = a % 6;
            int sb = a % 6;
            if (sa == 5) sa = 4;
            if (sb == 5) sb = 4;
            if (sa != sb)
                return false;
            return is_parameter_tag_compatible(a / 6, b / 6);
        }

        static inline unsigned find_closing_tag_runtime(const char* s, unsigned p)
        {
            return s[p] == 0   ? throw std::runtime_error("unmatched tag <") :
                   s[p] == '>' ? p :
                                 find_closing_tag_runtime(s, p + 1);
        }

        static inline uint64_t get_parameter_tag_runtime(const char* s, unsigned p = 0)
        {
            return s[p] == 0   ? 0 :
                   s[p] == '<' ? (
                                   std::strncmp(s + p, "<int>", 5) == 0  ? get_parameter_tag_runtime(s, find_closing_tag_runtime(s, p)) * 6 + 1 :
                                   std::strncmp(s + p, "<uint>", 6) == 0 ? get_parameter_tag_runtime(s, find_closing_tag_runtime(s, p)) * 6 + 2 :
                                   (std::strncmp(s + p, "<float>", 7) == 0 ||
                                    std::strncmp(s + p, "<double>", 8) == 0) ?
                                                                           get_parameter_tag_runtime(s, find_closing_tag_runtime(s, p)) * 6 + 3 :
                                   (std::strncmp(s + p, "<str>", 5) == 0 ||
                                    std::strncmp(s + p, "<string>", 8) == 0) ?
                                                                           get_parameter_tag_runtime(s, find_closing_tag_runtime(s, p)) * 6 + 4 :
                                   std::strncmp(s + p, "<path>", 6) == 0 ? get_parameter_tag_runtime(s, find_closing_tag_runtime(s, p)) * 6 + 5 :
                                                                           throw std::runtime_error("invalid parameter type")) :
                                 get_parameter_tag_runtime(s, p + 1);
        }
#ifndef CROW_MSVC_WORKAROUND
        constexpr uint64_t get_parameter_tag(const_str s, unsigned p = 0)
        {
            return p == s.size() ? 0 :
                   s[p] == '<'   ? (
                                   is_int(s, p)   ? get_parameter_tag(s, find_closing_tag(s, p)) * 6 + 1 :
                                     is_uint(s, p)  ? get_parameter_tag(s, find_closing_tag(s, p)) * 6 + 2 :
                                     is_float(s, p) ? get_parameter_tag(s, find_closing_tag(s, p)) * 6 + 3 :
                                     is_str(s, p)   ? get_parameter_tag(s, find_closing_tag(s, p)) * 6 + 4 :
                                     is_path(s, p)  ? get_parameter_tag(s, find_closing_tag(s, p)) * 6 + 5 :
                                                      throw std::runtime_error("invalid parameter type")) :
                                 get_parameter_tag(s, p + 1);
        }
#endif

        template<typename... T>
        struct S
        {
            template<typename U>
            using push = S<U, T...>;
            template<typename U>
            using push_back = S<T..., U>;
            template<template<typename... Args> class U>
            using rebind = U<T...>;
        };

        // Check whether the template function can be called with specific arguments
        template<typename F, typename Set>
        struct CallHelper;
        template<typename F, typename... Args>
        struct CallHelper<F, S<Args...>>
        {
            template<typename F1, typename... Args1, typename = decltype(std::declval<F1>()(std::declval<Args1>()...))>
            static char __test(int);

            template<typename...>
            static int __test(...);

            static constexpr bool value = sizeof(__test<F, Args...>(0)) == sizeof(char);
        };

        // Check Tuple contains type T
        template<typename T, typename Tuple>
        struct has_type;

        template<typename T>
        struct has_type<T, std::tuple<>> : std::false_type
        {};

        template<typename T, typename U, typename... Ts>
        struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>>
        {};

        template<typename T, typename... Ts>
        struct has_type<T, std::tuple<T, Ts...>> : std::true_type
        {};

        // Find index of type in tuple
        template<class T, class Tuple>
        struct tuple_index;

        template<class T, class... Types>
        struct tuple_index<T, std::tuple<T, Types...>>
        {
            static const int value = 0;
        };

        template<class T, class U, class... Types>
        struct tuple_index<T, std::tuple<U, Types...>>
        {
            static const int value = 1 + tuple_index<T, std::tuple<Types...>>::value;
        };

        // Extract element from forward tuple or get default
        template<typename T, typename Tup>
        typename std::enable_if<has_type<T&, Tup>::value, typename std::decay<T>::type&&>::type
          tuple_extract(Tup& tup)
        {
            return std::move(std::get<T&>(tup));
        }

        template<typename T, typename Tup>
        typename std::enable_if<!has_type<T&, Tup>::value, T>::type
          tuple_extract(Tup&)
        {
            return T{};
        }

        // Kind of fold expressions in C++11
        template<bool...>
        struct bool_pack;
        template<bool... bs>
        using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;

        template<int N>
        struct single_tag_to_type
        {};

        template<>
        struct single_tag_to_type<1>
        {
            using type = int64_t;
        };

        template<>
        struct single_tag_to_type<2>
        {
            using type = uint64_t;
        };

        template<>
        struct single_tag_to_type<3>
        {
            using type = double;
        };

        template<>
        struct single_tag_to_type<4>
        {
            using type = std::string;
        };

        template<>
        struct single_tag_to_type<5>
        {
            using type = std::string;
        };


        template<uint64_t Tag>
        struct arguments
        {
            using subarguments = typename arguments<Tag / 6>::type;
            using type =
              typename subarguments::template push<typename single_tag_to_type<Tag % 6>::type>;
        };

        template<>
        struct arguments<0>
        {
            using type = S<>;
        };

        template<typename... T>
        struct last_element_type
        {
            using type = typename std::tuple_element<sizeof...(T) - 1, std::tuple<T...>>::type;
        };


        template<>
        struct last_element_type<>
        {};


        // from http://stackoverflow.com/questions/13072359/c11-compile-time-array-with-logarithmic-evaluation-depth
        template<class T>
        using Invoke = typename T::type;

        template<unsigned...>
        struct seq
        {
            using type = seq;
        };

        template<class S1, class S2>
        struct concat;

        template<unsigned... I1, unsigned... I2>
        struct concat<seq<I1...>, seq<I2...>> : seq<I1..., (sizeof...(I1) + I2)...>
        {};

        template<class S1, class S2>
        using Concat = Invoke<concat<S1, S2>>;

        template<unsigned N>
        struct gen_seq;
        template<unsigned N>
        using GenSeq = Invoke<gen_seq<N>>;

        template<unsigned N>
        struct gen_seq : Concat<GenSeq<N / 2>, GenSeq<N - N / 2>>
        {};

        template<>
        struct gen_seq<0> : seq<>
        {};
        template<>
        struct gen_seq<1> : seq<0>
        {};

        template<typename Seq, typename Tuple>
        struct pop_back_helper;

        template<unsigned... N, typename Tuple>
        struct pop_back_helper<seq<N...>, Tuple>
        {
            template<template<typename... Args> class U>
            using rebind = U<typename std::tuple_element<N, Tuple>::type...>;
        };

        template<typename... T>
        struct pop_back //: public pop_back_helper<typename gen_seq<sizeof...(T)-1>::type, std::tuple<T...>>
        {
            template<template<typename... Args> class U>
            using rebind = typename pop_back_helper<typename gen_seq<sizeof...(T) - 1>::type, std::tuple<T...>>::template rebind<U>;
        };

        template<>
        struct pop_back<>
        {
            template<template<typename... Args> class U>
            using rebind = U<>;
        };

        // from http://stackoverflow.com/questions/2118541/check-if-c0x-parameter-pack-contains-a-type
        template<typename Tp, typename... List>
        struct contains : std::true_type
        {};

        template<typename Tp, typename Head, typename... Rest>
        struct contains<Tp, Head, Rest...> : std::conditional<std::is_same<Tp, Head>::value, std::true_type, contains<Tp, Rest...>>::type
        {};

        template<typename Tp>
        struct contains<Tp> : std::false_type
        {};

        template<typename T>
        struct empty_context
        {};

        template<typename T>
        struct promote
        {
            using type = T;
        };

#define CROW_INTERNAL_PROMOTE_TYPE(t1, t2) \
    template<>                             \
    struct promote<t1>                     \
    {                                      \
        using type = t2;                   \
    }

        CROW_INTERNAL_PROMOTE_TYPE(char, int64_t);
        CROW_INTERNAL_PROMOTE_TYPE(short, int64_t);
        CROW_INTERNAL_PROMOTE_TYPE(int, int64_t);
        CROW_INTERNAL_PROMOTE_TYPE(long, int64_t);
        CROW_INTERNAL_PROMOTE_TYPE(long long, int64_t);
        CROW_INTERNAL_PROMOTE_TYPE(unsigned char, uint64_t);
        CROW_INTERNAL_PROMOTE_TYPE(unsigned short, uint64_t);
        CROW_INTERNAL_PROMOTE_TYPE(unsigned int, uint64_t);
        CROW_INTERNAL_PROMOTE_TYPE(unsigned long, uint64_t);
        CROW_INTERNAL_PROMOTE_TYPE(unsigned long long, uint64_t);
        CROW_INTERNAL_PROMOTE_TYPE(float, double);
#undef CROW_INTERNAL_PROMOTE_TYPE

        template<typename T>
        using promote_t = typename promote<T>::type;

    } // namespace black_magic

    namespace detail
    {

        template<class T, std::size_t N, class... Args>
        struct get_index_of_element_from_tuple_by_type_impl
        {
            static constexpr auto value = N;
        };

        template<class T, std::size_t N, class... Args>
        struct get_index_of_element_from_tuple_by_type_impl<T, N, T, Args...>
        {
            static constexpr auto value = N;
        };

        template<class T, std::size_t N, class U, class... Args>
        struct get_index_of_element_from_tuple_by_type_impl<T, N, U, Args...>
        {
            static constexpr auto value = get_index_of_element_from_tuple_by_type_impl<T, N + 1, Args...>::value;
        };
    } // namespace detail

    namespace utility
    {
        template<class T, class... Args>
        T& get_element_by_type(std::tuple<Args...>& t)
        {
            return std::get<detail::get_index_of_element_from_tuple_by_type_impl<T, 0, Args...>::value>(t);
        }

        template<typename T>
        struct function_traits;

#ifndef CROW_MSVC_WORKAROUND
        template<typename T>
        struct function_traits : public function_traits<decltype(&T::operator())>
        {
            using parent_t = function_traits<decltype(&T::operator())>;
            static const size_t arity = parent_t::arity;
            using result_type = typename parent_t::result_type;
            template<size_t i>
            using arg = typename parent_t::template arg<i>;
        };
#endif

        template<typename ClassType, typename R, typename... Args>
        struct function_traits<R (ClassType::*)(Args...) const>
        {
            static const size_t arity = sizeof...(Args);

            typedef R result_type;

            template<size_t i>
            using arg = typename std::tuple_element<i, std::tuple<Args...>>::type;
        };

        template<typename ClassType, typename R, typename... Args>
        struct function_traits<R (ClassType::*)(Args...)>
        {
            static const size_t arity = sizeof...(Args);

            typedef R result_type;

            template<size_t i>
            using arg = typename std::tuple_element<i, std::tuple<Args...>>::type;
        };

        template<typename R, typename... Args>
        struct function_traits<std::function<R(Args...)>>
        {
            static const size_t arity = sizeof...(Args);

            typedef R result_type;

            template<size_t i>
            using arg = typename std::tuple_element<i, std::tuple<Args...>>::type;
        };
        /// @endcond

        inline static std::string base64encode(const unsigned char* data, size_t size, const char* key = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")
        {
            std::string ret;
            ret.resize((size + 2) / 3 * 4);
            auto it = ret.begin();
            while (size >= 3)
            {
                *it++ = key[(static_cast<unsigned char>(*data) & 0xFC) >> 2];
                unsigned char h = (static_cast<unsigned char>(*data++) & 0x03) << 4;
                *it++ = key[h | ((static_cast<unsigned char>(*data) & 0xF0) >> 4)];
                h = (static_cast<unsigned char>(*data++) & 0x0F) << 2;
                *it++ = key[h | ((static_cast<unsigned char>(*data) & 0xC0) >> 6)];
                *it++ = key[static_cast<unsigned char>(*data++) & 0x3F];

                size -= 3;
            }
            if (size == 1)
            {
                *it++ = key[(static_cast<unsigned char>(*data) & 0xFC) >> 2];
                unsigned char h = (static_cast<unsigned char>(*data++) & 0x03) << 4;
                *it++ = key[h];
                *it++ = '=';
                *it++ = '=';
            }
            else if (size == 2)
            {
                *it++ = key[(static_cast<unsigned char>(*data) & 0xFC) >> 2];
                unsigned char h = (static_cast<unsigned char>(*data++) & 0x03) << 4;
                *it++ = key[h | ((static_cast<unsigned char>(*data) & 0xF0) >> 4)];
                h = (static_cast<unsigned char>(*data++) & 0x0F) << 2;
                *it++ = key[h];
                *it++ = '=';
            }
            return ret;
        }

        inline static std::string base64encode(std::string data, size_t size, const char* key = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")
        {
            return base64encode((const unsigned char*)data.c_str(), size, key);
        }

        inline static std::string base64encode_urlsafe(const unsigned char* data, size_t size)
        {
            return base64encode(data, size, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_");
        }

        inline static std::string base64encode_urlsafe(std::string data, size_t size)
        {
            return base64encode((const unsigned char*)data.c_str(), size, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_");
        }

        inline static std::string base64decode(const char* data, size_t size)
        {
            // We accept both regular and url encoding here, as there does not seem to be any downside to that.
            // If we want to distinguish that we should use +/ for non-url and -_ for url.

            // Mapping logic from characters to [0-63]
            auto key = [](char c) -> unsigned char {
                if ((c >= 'A') && (c <= 'Z')) return c - 'A';
                if ((c >= 'a') && (c <= 'z')) return c - 'a' + 26;
                if ((c >= '0') && (c <= '9')) return c - '0' + 52;
                if ((c == '+') || (c == '-')) return 62;
                if ((c == '/') || (c == '_')) return 63;
                return 0;
            };

            // Not padded
            if (size % 4 == 2)             // missing last 2 characters
                size = (size / 4 * 3) + 1; // Not subtracting extra characters because they're truncated in int division
            else if (size % 4 == 3)        // missing last character
                size = (size / 4 * 3) + 2; // Not subtracting extra characters because they're truncated in int division

            // Padded
            else if (size >= 2 && data[size - 2] == '=') // padded with '=='
                size = (size / 4 * 3) - 2;               // == padding means the last block only has 1 character instead of 3, hence the '-2'
            else if (size >= 1 && data[size - 1] == '=') // padded with '='
                size = (size / 4 * 3) - 1;               // = padding means the last block only has 2 character instead of 3, hence the '-1'

            // Padding not needed
            else
                size = size / 4 * 3;

            std::string ret;
            ret.resize(size);
            auto it = ret.begin();

            // These will be used to decode 1 character at a time
            unsigned char odd;  // char1 and char3
            unsigned char even; // char2 and char4

            // Take 4 character blocks to turn into 3
            while (size >= 3)
            {
                // dec_char1 = (char1 shifted 2 bits to the left) OR ((char2 AND 00110000) shifted 4 bits to the right))
                odd = key(*data++);
                even = key(*data++);
                *it++ = (odd << 2) | ((even & 0x30) >> 4);
                // dec_char2 = ((char2 AND 00001111) shifted 4 bits left) OR ((char3 AND 00111100) shifted 2 bits right))
                odd = key(*data++);
                *it++ = ((even & 0x0F) << 4) | ((odd & 0x3C) >> 2);
                // dec_char3 = ((char3 AND 00000011) shifted 6 bits left) OR (char4)
                even = key(*data++);
                *it++ = ((odd & 0x03) << 6) | (even);

                size -= 3;
            }
            if (size == 2)
            {
                // d_char1 = (char1 shifted 2 bits to the left) OR ((char2 AND 00110000) shifted 4 bits to the right))
                odd = key(*data++);
                even = key(*data++);
                *it++ = (odd << 2) | ((even & 0x30) >> 4);
                // d_char2 = ((char2 AND 00001111) shifted 4 bits left) OR ((char3 AND 00111100) shifted 2 bits right))
                odd = key(*data++);
                *it++ = ((even & 0x0F) << 4) | ((odd & 0x3C) >> 2);
            }
            else if (size == 1)
            {
                // d_char1 = (char1 shifted 2 bits to the left) OR ((char2 AND 00110000) shifted 4 bits to the right))
                odd = key(*data++);
                even = key(*data++);
                *it++ = (odd << 2) | ((even & 0x30) >> 4);
            }
            return ret;
        }

        inline static std::string base64decode(const std::string& data, size_t size)
        {
            return base64decode(data.data(), size);
        }

        inline static std::string base64decode(const std::string& data)
        {
            return base64decode(data.data(), data.length());
        }

        inline static std::string normalize_path(const std::string& directoryPath)
        {
            std::string normalizedPath = directoryPath;
            std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');
            if (!normalizedPath.empty() && normalizedPath.back() != '/')
                normalizedPath += '/';
            return normalizedPath;
        }

        inline static void sanitize_filename(std::string& data, char replacement = '_')
        {
            if (data.length() > 255)
                data.resize(255);

            static const auto toUpper = [](char c) {
                return ((c >= 'a') && (c <= 'z')) ? (c - ('a' - 'A')) : c;
            };
            // Check for special device names. The Windows behavior is really odd here, it will consider both AUX and AUX.txt
            // a special device. Thus we search for the string (case-insensitive), and then check if the string ends or if
            // is has a dangerous follow up character (.:\/)
            auto sanitizeSpecialFile = [](std::string& source, unsigned ofs, const char* pattern, bool includeNumber, char replacement_) {
                unsigned i = ofs;
                size_t len = source.length();
                const char* p = pattern;
                while (*p)
                {
                    if (i >= len) return;
                    if (toUpper(source[i]) != *p) return;
                    ++i;
                    ++p;
                }
                if (includeNumber)
                {
                    if ((i >= len) || (source[i] < '1') || (source[i] > '9')) return;
                    ++i;
                }
                if ((i >= len) || (source[i] == '.') || (source[i] == ':') || (source[i] == '/') || (source[i] == '\\'))
                {
                    source.erase(ofs + 1, (i - ofs) - 1);
                    source[ofs] = replacement_;
                }
            };
            bool checkForSpecialEntries = true;
            for (unsigned i = 0; i < data.length(); ++i)
            {
                // Recognize directory traversals and the special devices CON/PRN/AUX/NULL/COM[1-]/LPT[1-9]
                if (checkForSpecialEntries)
                {
                    checkForSpecialEntries = false;
                    switch (toUpper(data[i]))
                    {
                        case 'A':
                            sanitizeSpecialFile(data, i, "AUX", false, replacement);
                            break;
                        case 'C':
                            sanitizeSpecialFile(data, i, "CON", false, replacement);
                            sanitizeSpecialFile(data, i, "COM", true, replacement);
                            break;
                        case 'L':
                            sanitizeSpecialFile(data, i, "LPT", true, replacement);
                            break;
                        case 'N':
                            sanitizeSpecialFile(data, i, "NUL", false, replacement);
                            break;
                        case 'P':
                            sanitizeSpecialFile(data, i, "PRN", false, replacement);
                            break;
                        case '.':
                            sanitizeSpecialFile(data, i, "..", false, replacement);
                            break;
                    }
                }

                // Sanitize individual characters
                unsigned char c = data[i];
                if ((c < ' ') || ((c >= 0x80) && (c <= 0x9F)) || (c == '?') || (c == '<') || (c == '>') || (c == ':') || (c == '*') || (c == '|') || (c == '\"'))
                {
                    data[i] = replacement;
                }
                else if ((c == '/') || (c == '\\'))
                {
                    if (CROW_UNLIKELY(i == 0)) //Prevent Unix Absolute Paths (Windows Absolute Paths are prevented with `(c == ':')`)
                    {
                        data[i] = replacement;
                    }
                    else
                    {
                        checkForSpecialEntries = true;
                    }
                }
            }
        }

        inline static std::string random_alphanum(std::size_t size)
        {
            static const char alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist(0, sizeof(alphabet) - 2);
            std::string out;
            out.reserve(size);
            for (std::size_t i = 0; i < size; i++)
                out.push_back(alphabet[dist(rng)]);
            return out;
        }

        inline static std::string join_path(std::string path, const std::string& fname)
        {
            return (std::filesystem::path(path) / fname).string();
        }

        /**
         * @brief Checks two string for equality.
         * Always returns false if strings differ in size.
         * Defaults to case-insensitive comparison.
         */
        inline static bool string_equals(const std::string_view l, const std::string_view r, bool case_sensitive = false)
        {
            if (l.length() != r.length())
                return false;

            for (size_t i = 0; i < l.length(); i++)
            {
                if (case_sensitive)
                {
                    if (l[i] != r[i])
                        return false;
                }
                else
                {
                    if (std::toupper(l[i]) != std::toupper(r[i]))
                        return false;
                }
            }

            return true;
        }

        template<typename T, typename U>
        inline static T lexical_cast(const U& v)
        {
            std::stringstream stream;
            T res;

            stream << v;
            stream >> res;

            return res;
        }

        template<typename T>
        inline static T lexical_cast(const char* v, size_t count)
        {
            std::stringstream stream;
            T res;

            stream.write(v, count);
            stream >> res;

            return res;
        }


        /// Return a copy of the given string with its
        /// leading and trailing whitespaces removed.
        inline static std::string trim(const std::string& v)
        {
            if (v.empty())
                return "";

            size_t begin = 0, end = v.length();

            size_t i;
            for (i = 0; i < v.length(); i++)
            {
                if (!std::isspace(v[i]))
                {
                    begin = i;
                    break;
                }
            }

            if (i == v.length())
                return "";

            for (i = v.length(); i > 0; i--)
            {
                if (!std::isspace(v[i - 1]))
                {
                    end = i;
                    break;
                }
            }

            return v.substr(begin, end - begin);
        }

        /**
         * @brief splits a string based on a separator
         */
        inline static std::vector<std::string> split(const std::string& v, const std::string& separator)
        {
            std::vector<std::string> result;
            size_t startPos = 0;

            for (size_t foundPos = v.find(separator); foundPos != std::string::npos; foundPos = v.find(separator, startPos))
            {
                result.push_back(v.substr(startPos, foundPos - startPos));
                startPos = foundPos + separator.size();
            }

            result.push_back(v.substr(startPos));
            return result;
        }

        /**
         * @brief Returns the first occurence that matches between two ranges of iterators
         * @param first1 begin() iterator of the first range
         * @param last1 end() iterator of the first range
         * @param first2 begin() iterator of the second range
         * @param last2 end() iterator of the second range
         * @return first occurence that matches between two ranges of iterators 
        */
        template<typename Iter1, typename Iter2>
        inline static Iter1 find_first_of(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2)
        {
            for (; first1 != last1; ++first1)
            {
                if (std::find(first2, last2, *first1) != last2)
                {
                    return first1;
                }
            }
            return last1;
        }
    } // namespace utility
} // namespace crow


#ifdef CROW_USE_BOOST
#include <boost/asio.hpp>
#include <boost/asio/version.hpp>
#ifdef CROW_ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif
#else
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#include <asio.hpp>
#include <asio/version.hpp>
#ifdef CROW_ENABLE_SSL
#include <asio/ssl.hpp>
#endif
#endif

#if (defined(CROW_USE_BOOST) && BOOST_VERSION >= 107000) || (ASIO_VERSION >= 101008)
#define GET_IO_CONTEXT(s) ((asio::io_context&)(s).get_executor().context())
#else
#define GET_IO_CONTEXT(s) ((s).get_io_service())
#endif

namespace crow
{
#ifdef CROW_USE_BOOST
    namespace asio = boost::asio;
    using error_code = boost::system::error_code;
#else
    using error_code = asio::error_code;
#endif
    using tcp = asio::ip::tcp;
    using stream_protocol = asio::local::stream_protocol;

    /// A wrapper for the asio::ip::tcp::socket and asio::ssl::stream
    struct SocketAdaptor
    {
        using context = void;
        SocketAdaptor(asio::io_context& io_context, context*):
          socket_(io_context)
        {}

        asio::io_context& get_io_context()
        {
            return GET_IO_CONTEXT(socket_);
        }

        /// Get the TCP socket handling data transfers, regardless of what layer is handling transfers on top of the socket.
        tcp::socket& raw_socket()
        {
            return socket_;
        }

        /// Get the object handling data transfers, this can be either a TCP socket or an SSL stream (if SSL is enabled).
        tcp::socket& socket()
        {
            return socket_;
        }

        tcp::endpoint remote_endpoint() const
        {
            return socket_.remote_endpoint();
        }

        std::string address() const
        {
            return socket_.remote_endpoint().address().to_string();
        }

        bool is_open() const
        {
            return socket_.is_open();
        }

        void close()
        {
            error_code ec;
            socket_.close(ec);
        }

        void shutdown_readwrite()
        {
            error_code ec;
            socket_.shutdown(asio::socket_base::shutdown_type::shutdown_both, ec);
        }

        void shutdown_write()
        {
            error_code ec;
            socket_.shutdown(asio::socket_base::shutdown_type::shutdown_send, ec);
        }

        void shutdown_read()
        {
            error_code ec;
            socket_.shutdown(asio::socket_base::shutdown_type::shutdown_receive, ec);
        }

        template<typename F>
        void start(F f)
        {
            f(error_code());
        }

        tcp::socket socket_;
    };

    struct UnixSocketAdaptor
    {
        using context = void;
        UnixSocketAdaptor(asio::io_context& io_context, context*):
          socket_(io_context)
        {
        }

        asio::io_context& get_io_context()
        {
            return GET_IO_CONTEXT(socket_);
        }

        stream_protocol::socket& raw_socket()
        {
            return socket_;
        }

        stream_protocol::socket& socket()
        {
            return socket_;
        }

        stream_protocol::endpoint remote_endpoint()
        {
            return socket_.local_endpoint();
        }

        std::string address() const
        {
            return "";
        }

        bool is_open()
        {
            return socket_.is_open();
        }

        void close()
        {
            error_code ec;
            socket_.close(ec);
        }

        void shutdown_readwrite()
        {
            error_code ec;
            socket_.shutdown(asio::socket_base::shutdown_type::shutdown_both, ec);
        }

        void shutdown_write()
        {
            error_code ec;
            socket_.shutdown(asio::socket_base::shutdown_type::shutdown_send, ec);
        }

        void shutdown_read()
        {
            error_code ec;
            socket_.shutdown(asio::socket_base::shutdown_type::shutdown_receive, ec);
        }

        template<typename F>
        void start(F f)
        {
            f(error_code());
        }

        stream_protocol::socket socket_;
    };

#ifdef CROW_ENABLE_SSL
    struct SSLAdaptor
    {
        using context = asio::ssl::context;
        using ssl_socket_t = asio::ssl::stream<tcp::socket>;
        SSLAdaptor(asio::io_context& io_context, context* ctx):
          ssl_socket_(new ssl_socket_t(io_context, *ctx))
        {}

        asio::ssl::stream<tcp::socket>& socket()
        {
            return *ssl_socket_;
        }

        tcp::socket::lowest_layer_type&
          raw_socket()
        {
            return ssl_socket_->lowest_layer();
        }

        tcp::endpoint remote_endpoint()
        {
            return raw_socket().remote_endpoint();
        }

        std::string address() const
        {
            return ssl_socket_->lowest_layer().remote_endpoint().address().to_string();
        }

        bool is_open()
        {
            return ssl_socket_ ? raw_socket().is_open() : false;
        }

        void close()
        {
            if (is_open())
            {
                error_code ec;
                raw_socket().close(ec);
            }
        }

        void shutdown_readwrite()
        {
            if (is_open())
            {
                error_code ec;
                raw_socket().shutdown(asio::socket_base::shutdown_type::shutdown_both, ec);
            }
        }

        void shutdown_write()
        {
            if (is_open())
            {
                error_code ec;
                raw_socket().shutdown(asio::socket_base::shutdown_type::shutdown_send, ec);
            }
        }

        void shutdown_read()
        {
            if (is_open())
            {
                error_code ec;
                raw_socket().shutdown(asio::socket_base::shutdown_type::shutdown_receive, ec);
            }
        }

        asio::io_context& get_io_context()
        {
            return GET_IO_CONTEXT(raw_socket());
        }

        template<typename F>
        void start(F f)
        {
            ssl_socket_->async_handshake(asio::ssl::stream_base::server,
                                         [f](const error_code& ec) {
                                             f(ec);
                                         });
        }

        std::unique_ptr<asio::ssl::stream<tcp::socket>> ssl_socket_;
    };
#endif
} // namespace crow


#include <string>

namespace crow
{
    /// An abstract class that allows any other class to be returned by a handler.
    struct returnable
    {
        std::string content_type;
        virtual std::string dump() const = 0;

        returnable(std::string ctype):
          content_type{ctype}
        {}

        virtual ~returnable(){}
    };
} // namespace crow


#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>

namespace crow
{

// ----------------------------------------------------------------------------
// qs_parse (modified)
// https://github.com/bartgrantham/qs_parse
// ----------------------------------------------------------------------------
/*  Similar to strncmp, but handles URL-encoding for either string  */
int qs_strncmp(const char* s, const char* qs, size_t n);


/*  Finds the beginning of each key/value pair and stores a pointer in qs_kv.
 *  Also decodes the value portion of the k/v pair *in-place*.  In a future
 *  enhancement it will also have a compile-time option of sorting qs_kv
 *  alphabetically by key.  */
size_t qs_parse(char* qs, char* qs_kv[], size_t qs_kv_size, bool parse_url);


/*  Used by qs_parse to decode the value portion of a k/v pair  */
int qs_decode(char * qs);


/*  Looks up the value according to the key on a pre-processed query string
 *  A future enhancement will be a compile-time option to look up the key
 *  in a pre-sorted qs_kv array via a binary search.  */
//char * qs_k2v(const char * key, char * qs_kv[], int qs_kv_size);
 char * qs_k2v(const char * key, char * const * qs_kv, size_t qs_kv_size, int nth);


/*  Non-destructive lookup of value, based on key.  User provides the
 *  destinaton string and length.  */
char * qs_scanvalue(const char * key, const char * qs, char * val, size_t val_len);

// TODO: implement sorting of the qs_kv array; for now ensure it's not compiled
#undef _qsSORTING

// isxdigit _is_ available in <ctype.h>, but let's avoid another header instead
#define CROW_QS_ISHEX(x)    ((((x)>='0'&&(x)<='9') || ((x)>='A'&&(x)<='F') || ((x)>='a'&&(x)<='f')) ? 1 : 0)
#define CROW_QS_HEX2DEC(x)  (((x)>='0'&&(x)<='9') ? (x)-48 : ((x)>='A'&&(x)<='F') ? (x)-55 : ((x)>='a'&&(x)<='f') ? (x)-87 : 0)
#define CROW_QS_ISQSCHR(x) ((((x)=='=')||((x)=='#')||((x)=='&')||((x)=='\0')) ? 0 : 1)

inline int qs_strncmp(const char * s, const char * qs, size_t n)
{
    unsigned char u1, u2, unyb, lnyb;

    while(n-- > 0)
    {
        u1 = static_cast<unsigned char>(*s++);
        u2 = static_cast<unsigned char>(*qs++);

        if ( ! CROW_QS_ISQSCHR(u1) ) {  u1 = '\0';  }
        if ( ! CROW_QS_ISQSCHR(u2) ) {  u2 = '\0';  }

        if ( u1 == '+' ) {  u1 = ' ';  }
        if ( u1 == '%' ) // easier/safer than scanf
        {
            unyb = static_cast<unsigned char>(*s++);
            lnyb = static_cast<unsigned char>(*s++);
            if ( CROW_QS_ISHEX(unyb) && CROW_QS_ISHEX(lnyb) )
                u1 = (CROW_QS_HEX2DEC(unyb) * 16) + CROW_QS_HEX2DEC(lnyb);
            else
                u1 = '\0';
        }

        if ( u2 == '+' ) {  u2 = ' ';  }
        if ( u2 == '%' ) // easier/safer than scanf
        {
            unyb = static_cast<unsigned char>(*qs++);
            lnyb = static_cast<unsigned char>(*qs++);
            if ( CROW_QS_ISHEX(unyb) && CROW_QS_ISHEX(lnyb) )
                u2 = (CROW_QS_HEX2DEC(unyb) * 16) + CROW_QS_HEX2DEC(lnyb);
            else
                u2 = '\0';
        }

        if ( u1 != u2 )
            return u1 - u2;
        if ( u1 == '\0' )
            return 0;
    }
    if ( CROW_QS_ISQSCHR(*qs) )
        return -1;
    else
        return 0;
}


inline size_t qs_parse(char* qs, char* qs_kv[], size_t qs_kv_size, bool parse_url = true)
{
    size_t i, j;
    char * substr_ptr;

    for(i=0; i<qs_kv_size; i++)  qs_kv[i] = NULL;

    // find the beginning of the k/v substrings or the fragment
    substr_ptr = parse_url ? qs + strcspn(qs, "?#") : qs;
    if (parse_url)
    {
        if (substr_ptr[0] != '\0')
            substr_ptr++;
        else
            return 0; // no query or fragment
    }

    i=0;
    while(i<qs_kv_size)
    {
        qs_kv[i] = substr_ptr;
        j = strcspn(substr_ptr, "&");
        if ( substr_ptr[j] == '\0' ) { i++; break;  } // x &'s -> means x iterations of this loop -> means *x+1* k/v pairs
        substr_ptr += j + 1;
        i++;
    }

    // we only decode the values in place, the keys could have '='s in them
    // which will hose our ability to distinguish keys from values later
    for(j=0; j<i; j++)
    {
        substr_ptr = qs_kv[j] + strcspn(qs_kv[j], "=&#");
        if ( substr_ptr[0] == '&' || substr_ptr[0] == '\0')  // blank value: skip decoding
            substr_ptr[0] = '\0';
        else
            qs_decode(++substr_ptr);
    }

#ifdef _qsSORTING
// TODO: qsort qs_kv, using qs_strncmp() for the comparison
#endif

    return i;
}


inline int qs_decode(char * qs)
{
    int i=0, j=0;

    while( CROW_QS_ISQSCHR(qs[j]) )
    {
        if ( qs[j] == '+' ) {  qs[i] = ' ';  }
        else if ( qs[j] == '%' ) // easier/safer than scanf
        {
            if ( ! CROW_QS_ISHEX(qs[j+1]) || ! CROW_QS_ISHEX(qs[j+2]) )
            {
                qs[i] = '\0';
                return i;
            }
            qs[i] = (CROW_QS_HEX2DEC(qs[j+1]) * 16) + CROW_QS_HEX2DEC(qs[j+2]);
            j+=2;
        }
        else
        {
            qs[i] = qs[j];
        }
        i++;  j++;
    }
    qs[i] = '\0';

    return i;
}


inline char * qs_k2v(const char * key, char * const * qs_kv, size_t qs_kv_size, int nth = 0)
{
    size_t i;
    size_t key_len, skip;

    key_len = strlen(key);

#ifdef _qsSORTING
// TODO: binary search for key in the sorted qs_kv
#else  // _qsSORTING
    for(i=0; i<qs_kv_size; i++)
    {
        // we rely on the unambiguous '=' to find the value in our k/v pair
        if ( qs_strncmp(key, qs_kv[i], key_len) == 0 )
        {
            skip = strcspn(qs_kv[i], "=");
            if ( qs_kv[i][skip] == '=' )
                skip++;
            // return (zero-char value) ? ptr to trailing '\0' : ptr to value
            if(nth == 0)
                return qs_kv[i] + skip;
            else
                --nth;
        }
    }
#endif  // _qsSORTING

    return nullptr;
}

inline std::unique_ptr<std::pair<std::string, std::string>> qs_dict_name2kv(const char * dict_name, char * const * qs_kv, size_t qs_kv_size, int nth = 0)
{
    size_t i;
    size_t name_len, skip_to_eq, skip_to_brace_open, skip_to_brace_close;

    name_len = strlen(dict_name);

#ifdef _qsSORTING
// TODO: binary search for key in the sorted qs_kv
#else  // _qsSORTING
    for(i=0; i<qs_kv_size; i++)
    {
        if ( strncmp(dict_name, qs_kv[i], name_len) == 0 )
        {
            skip_to_eq = strcspn(qs_kv[i], "=");
            if ( qs_kv[i][skip_to_eq] == '=' )
                skip_to_eq++;
            skip_to_brace_open = strcspn(qs_kv[i], "[");
            if ( qs_kv[i][skip_to_brace_open] == '[' )
                skip_to_brace_open++;
            skip_to_brace_close = strcspn(qs_kv[i], "]");

            if ( skip_to_brace_open <= skip_to_brace_close &&
                 skip_to_brace_open > 0 &&
                 skip_to_brace_close > 0 &&
                 nth == 0 )
            {
                auto key = std::string(qs_kv[i] + skip_to_brace_open, skip_to_brace_close - skip_to_brace_open);
                auto value = std::string(qs_kv[i] + skip_to_eq);
                return std::unique_ptr<std::pair<std::string, std::string>>(new std::pair<std::string, std::string>(key, value));
            }
            else
            {
                --nth;
            }
        }
    }
#endif  // _qsSORTING

    return nullptr;
}


inline char * qs_scanvalue(const char * key, const char * qs, char * val, size_t val_len)
{
    size_t i, key_len;
    const char * tmp;

    // find the beginning of the k/v substrings
    if ( (tmp = strchr(qs, '?')) != NULL )
        qs = tmp + 1;

    key_len = strlen(key);
    while(qs[0] != '#' && qs[0] != '\0')
    {
        if ( qs_strncmp(key, qs, key_len) == 0 )
            break;
        qs += strcspn(qs, "&") + 1;
    }

    if ( qs[0] == '\0' ) return NULL;

    qs += strcspn(qs, "=&#");
    if ( qs[0] == '=' )
    {
        qs++;
        i = strcspn(qs, "&=#");
#ifdef _MSC_VER
        strncpy_s(val, val_len, qs, (val_len - 1)<(i + 1) ? (val_len - 1) : (i + 1));
#else
        strncpy(val, qs, (val_len - 1)<(i + 1) ? (val_len - 1) : (i + 1));
#endif
		qs_decode(val);
    }
    else
    {
        if ( val_len > 0 )
            val[0] = '\0';
    }

    return val;
}
}
// ----------------------------------------------------------------------------


namespace crow
{
    struct request;
    /// A class to represent any data coming after the `?` in the request URL into key-value pairs.
    class query_string
    {
    public:
        static const int MAX_KEY_VALUE_PAIRS_COUNT = 256;

        query_string() = default;

        query_string(const query_string& qs):
          url_(qs.url_)
        {
            for (auto p : qs.key_value_pairs_)
            {
                key_value_pairs_.push_back((char*)(p - qs.url_.c_str() + url_.c_str()));
            }
        }

        query_string& operator=(const query_string& qs)
        {
            url_ = qs.url_;
            key_value_pairs_.clear();
            for (auto p : qs.key_value_pairs_)
            {
                key_value_pairs_.push_back((char*)(p - qs.url_.c_str() + url_.c_str()));
            }
            return *this;
        }

        query_string& operator=(query_string&& qs) noexcept
        {
            key_value_pairs_ = std::move(qs.key_value_pairs_);
            char* old_data = (char*)qs.url_.c_str();
            url_ = std::move(qs.url_);
            for (auto& p : key_value_pairs_)
            {
                p += (char*)url_.c_str() - old_data;
            }
            return *this;
        }


        query_string(std::string params, bool url = true):
          url_(std::move(params))
        {
            if (url_.empty())
                return;

            key_value_pairs_.resize(MAX_KEY_VALUE_PAIRS_COUNT);
            size_t count = qs_parse(&url_[0], &key_value_pairs_[0], MAX_KEY_VALUE_PAIRS_COUNT, url);

            key_value_pairs_.resize(count);
            key_value_pairs_.shrink_to_fit();
        }

        void clear()
        {
            key_value_pairs_.clear();
            url_.clear();
        }

        friend std::ostream& operator<<(std::ostream& os, const query_string& qs)
        {
            os << "[ ";
            for (size_t i = 0; i < qs.key_value_pairs_.size(); ++i)
            {
                if (i)
                    os << ", ";
                os << qs.key_value_pairs_[i];
            }
            os << " ]";
            return os;
        }

        /// Get a value from a name, used for `?name=value`.

        ///
        /// Note: this method returns the value of the first occurrence of the key only, to return all occurrences, see \ref get_list().
        char* get(const std::string& name) const
        {
            char* ret = qs_k2v(name.c_str(), key_value_pairs_.data(), key_value_pairs_.size());
            return ret;
        }

        /// Works similar to \ref get() except it removes the item from the query string.
        char* pop(const std::string& name)
        {
            char* ret = get(name);
            if (ret != nullptr)
            {
                const std::string key_name = name + '=';
                for (unsigned int i = 0; i < key_value_pairs_.size(); i++)
                {
                    std::string str_item(key_value_pairs_[i]);
                    if (str_item.find(key_name)==0)
                    {
                        key_value_pairs_.erase(key_value_pairs_.begin() + i);
                        break;
                    }
                }
            }
            return ret;
        }

        /// Returns a list of values, passed as `?name[]=value1&name[]=value2&...name[]=valuen` with n being the size of the list.

        ///
        /// Note: Square brackets in the above example are controlled by `use_brackets` boolean (true by default). If set to false, the example becomes `?name=value1,name=value2...name=valuen`
        std::vector<char*> get_list(const std::string& name, bool use_brackets = true) const
        {
            std::vector<char*> ret;
            std::string plus = name + (use_brackets ? "[]" : "");
            char* element = nullptr;

            int count = 0;
            while (1)
            {
                element = qs_k2v(plus.c_str(), key_value_pairs_.data(), key_value_pairs_.size(), count++);
                if (!element)
                    break;
                ret.push_back(element);
            }
            return ret;
        }

        /// Similar to \ref get_list() but it removes the
        std::vector<char*> pop_list(const std::string& name, bool use_brackets = true)
        {
            std::vector<char*> ret = get_list(name, use_brackets);
            const size_t name_len = name.length();
            if (!ret.empty())
            {
                for (unsigned int i = 0; i < key_value_pairs_.size(); i++)
                {
                    std::string str_item(key_value_pairs_[i]);
                    if (str_item.find(name)==0) {
                      if (use_brackets && str_item.find("[]=",name_len)==name_len) {
                        key_value_pairs_.erase(key_value_pairs_.begin() + i--);
                      } else if (!use_brackets && str_item.find('=',name_len)==name_len ) {
                           key_value_pairs_.erase(key_value_pairs_.begin() + i--);
                       }
                    }
                }
            }
            return ret;
        }

        /// Works similar to \ref get_list() except the brackets are mandatory must not be empty.

        ///
        /// For example calling `get_dict(yourname)` on `?yourname[sub1]=42&yourname[sub2]=84` would give a map containing `{sub1 : 42, sub2 : 84}`.
        ///
        /// if your query string has both empty brackets and ones with a key inside, use pop_list() to get all the values without a key before running this method.
        std::unordered_map<std::string, std::string> get_dict(const std::string& name) const
        {
            std::unordered_map<std::string, std::string> ret;

            int count = 0;
            while (1)
            {
                if (auto element = qs_dict_name2kv(name.c_str(), key_value_pairs_.data(), key_value_pairs_.size(), count++))
                    ret.insert(*element);
                else
                    break;
            }
            return ret;
        }

        /// Works the same as \ref get_dict() but removes the values from the query string.
        std::unordered_map<std::string, std::string> pop_dict(const std::string& name)
        {
            const std::string name_value = name +'[';
            std::unordered_map<std::string, std::string> ret = get_dict(name);
            if (!ret.empty())
            {
                for (unsigned int i = 0; i < key_value_pairs_.size(); i++)
                {
                    std::string str_item(key_value_pairs_[i]);
                    if (str_item.find(name_value)==0)
                    {
                        key_value_pairs_.erase(key_value_pairs_.begin() + i--);
                    }
                }
            }
            return ret;
        }

        std::vector<std::string> keys() const
        {
            std::vector<std::string> keys;
            keys.reserve(key_value_pairs_.size());

            for (const char* const element : key_value_pairs_)
            {
                const char* delimiter = strchr(element, '=');
                if (delimiter)
                    keys.emplace_back(element, delimiter);
                else
                    keys.emplace_back(element);
            }

            return keys;
        }

    private:
        std::string url_;
        std::vector<char*> key_value_pairs_;
    };

} // namespace crow

// This file is generated from nginx/conf/mime.types using nginx_mime2cpp.py on 2021-12-03.
#include <unordered_map>
#include <string>

namespace crow
{
    const std::unordered_map<std::string, std::string> mime_types{
      {"gz", "application/gzip"},
      {"shtml", "text/html"},
      {"htm", "text/html"},
      {"html", "text/html"},
      {"css", "text/css"},
      {"xml", "text/xml"},
      {"gif", "image/gif"},
      {"jpg", "image/jpeg"},
      {"jpeg", "image/jpeg"},
      {"js", "application/javascript"},
      {"atom", "application/atom+xml"},
      {"rss", "application/rss+xml"},
      {"mml", "text/mathml"},
      {"txt", "text/plain"},
      {"jad", "text/vnd.sun.j2me.app-descriptor"},
      {"wml", "text/vnd.wap.wml"},
      {"htc", "text/x-component"},
      {"avif", "image/avif"},
      {"png", "image/png"},
      {"svgz", "image/svg+xml"},
      {"svg", "image/svg+xml"},
      {"tiff", "image/tiff"},
      {"tif", "image/tiff"},
      {"wbmp", "image/vnd.wap.wbmp"},
      {"webp", "image/webp"},
      {"ico", "image/x-icon"},
      {"jng", "image/x-jng"},
      {"bmp", "image/x-ms-bmp"},
      {"woff", "font/woff"},
      {"woff2", "font/woff2"},
      {"ear", "application/java-archive"},
      {"war", "application/java-archive"},
      {"jar", "application/java-archive"},
      {"json", "application/json"},
      {"hqx", "application/mac-binhex40"},
      {"doc", "application/msword"},
      {"pdf", "application/pdf"},
      {"ai", "application/postscript"},
      {"eps", "application/postscript"},
      {"ps", "application/postscript"},
      {"rtf", "application/rtf"},
      {"m3u8", "application/vnd.apple.mpegurl"},
      {"kml", "application/vnd.google-earth.kml+xml"},
      {"kmz", "application/vnd.google-earth.kmz"},
      {"xls", "application/vnd.ms-excel"},
      {"eot", "application/vnd.ms-fontobject"},
      {"ppt", "application/vnd.ms-powerpoint"},
      {"odg", "application/vnd.oasis.opendocument.graphics"},
      {"odp", "application/vnd.oasis.opendocument.presentation"},
      {"ods", "application/vnd.oasis.opendocument.spreadsheet"},
      {"odt", "application/vnd.oasis.opendocument.text"},
      {"pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
      {"xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
      {"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
      {"wmlc", "application/vnd.wap.wmlc"},
      {"wasm", "application/wasm"},
      {"7z", "application/x-7z-compressed"},
      {"cco", "application/x-cocoa"},
      {"jardiff", "application/x-java-archive-diff"},
      {"jnlp", "application/x-java-jnlp-file"},
      {"run", "application/x-makeself"},
      {"pm", "application/x-perl"},
      {"pl", "application/x-perl"},
      {"pdb", "application/x-pilot"},
      {"prc", "application/x-pilot"},
      {"rar", "application/x-rar-compressed"},
      {"rpm", "application/x-redhat-package-manager"},
      {"sea", "application/x-sea"},
      {"swf", "application/x-shockwave-flash"},
      {"sit", "application/x-stuffit"},
      {"tk", "application/x-tcl"},
      {"tcl", "application/x-tcl"},
      {"crt", "application/x-x509-ca-cert"},
      {"pem", "application/x-x509-ca-cert"},
      {"der", "application/x-x509-ca-cert"},
      {"xpi", "application/x-xpinstall"},
      {"xhtml", "application/xhtml+xml"},
      {"xspf", "application/xspf+xml"},
      {"zip", "application/zip"},
      {"dll", "application/octet-stream"},
      {"exe", "application/octet-stream"},
      {"bin", "application/octet-stream"},
      {"deb", "application/octet-stream"},
      {"dmg", "application/octet-stream"},
      {"img", "application/octet-stream"},
      {"iso", "application/octet-stream"},
      {"msm", "application/octet-stream"},
      {"msp", "application/octet-stream"},
      {"msi", "application/octet-stream"},
      {"kar", "audio/midi"},
      {"midi", "audio/midi"},
      {"mid", "audio/midi"},
      {"mp3", "audio/mpeg"},
      {"ogg", "audio/ogg"},
      {"m4a", "audio/x-m4a"},
      {"ra", "audio/x-realaudio"},
      {"3gp", "video/3gpp"},
      {"3gpp", "video/3gpp"},
      {"ts", "video/mp2t"},
      {"mp4", "video/mp4"},
      {"mpg", "video/mpeg"},
      {"mpeg", "video/mpeg"},
      {"mov", "video/quicktime"},
      {"webm", "video/webm"},
      {"flv", "video/x-flv"},
      {"m4v", "video/x-m4v"},
      {"mng", "video/x-mng"},
      {"asf", "video/x-ms-asf"},
      {"asx", "video/x-ms-asf"},
      {"wmv", "video/x-ms-wmv"},
      {"avi", "video/x-msvideo"}};
}



#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

namespace crow
{
    enum class LogLevel
    {
#ifndef ERROR
#ifndef DEBUG
        DEBUG = 0,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
#endif
#endif

        Debug = 0,
        Info,
        Warning,
        Error,
        Critical,
    };

    class ILogHandler
    {
    public:
        virtual ~ILogHandler() = default;

        virtual void log(const std::string& message, LogLevel level) = 0;
    };

    class CerrLogHandler : public ILogHandler
    {
    public:
        void log(const std::string &message, LogLevel level) override
        {
            std::string log_msg;
            log_msg.reserve(message.length() + 1+32+3+8+2);
            log_msg
                .append("(")
                .append(timestamp())
                .append(") [");

            switch (level)
            {
                case LogLevel::Debug:
                    log_msg.append("DEBUG   ");
                    break;
                case LogLevel::Info:
                    log_msg.append("INFO    ");
                    break;
                case LogLevel::Warning:
                    log_msg.append("WARNING ");
                    break;
                case LogLevel::Error:
                    log_msg.append("ERROR   ");
                    break;
                case LogLevel::Critical:
                    log_msg.append("CRITICAL");
                    break;
            }

            log_msg.append("] ")
            .append(message);

            std::cerr << log_msg << std::endl;
        }

    private:
        static std::string timestamp()
        {
            char date[32];
            time_t t = time(0);

            tm my_tm;

#if defined(_MSC_VER) || defined(__MINGW32__)
#ifdef CROW_USE_LOCALTIMEZONE
            localtime_s(&my_tm, &t);
#else
            gmtime_s(&my_tm, &t);
#endif
#else
#ifdef CROW_USE_LOCALTIMEZONE
            localtime_r(&t, &my_tm);
#else
            gmtime_r(&t, &my_tm);
#endif
#endif

            size_t sz = strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", &my_tm);
            return std::string(date, date + sz);
        }
    };

    class logger
    {
    public:
        logger(LogLevel level):
          level_(level)
        {}
        ~logger()
        {
#ifdef CROW_ENABLE_LOGGING
            if (level_ >= get_current_log_level())
            {
                get_handler_ref()->log(stringstream_.str(), level_);
            }
#endif
        }

        //
        template<typename T>
        logger& operator<<(T const& value)
        {
#ifdef CROW_ENABLE_LOGGING
            if (level_ >= get_current_log_level())
            {
                stringstream_ << value;
            }
#endif
            return *this;
        }

        //
        static void setLogLevel(LogLevel level) { get_log_level_ref() = level; }

        static void setHandler(ILogHandler* handler) { get_handler_ref() = handler; }

        static LogLevel get_current_log_level() { return get_log_level_ref(); }

    private:
        //
        static LogLevel& get_log_level_ref()
        {
            static LogLevel current_level = static_cast<LogLevel>(CROW_LOG_LEVEL);
            return current_level;
        }
        static ILogHandler*& get_handler_ref()
        {
            static CerrLogHandler default_handler;
            static ILogHandler* current_handler = &default_handler;
            return current_handler;
        }

        //
        std::ostringstream stringstream_;
        LogLevel level_;
    };
} // namespace crow

#define CROW_LOG_CRITICAL                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Critical) \
    crow::logger(crow::LogLevel::Critical)
#define CROW_LOG_ERROR                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Error) \
    crow::logger(crow::LogLevel::Error)
#define CROW_LOG_WARNING                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Warning) \
    crow::logger(crow::LogLevel::Warning)
#define CROW_LOG_INFO                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Info) \
    crow::logger(crow::LogLevel::Info)
#define CROW_LOG_DEBUG                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Debug) \
    crow::logger(crow::LogLevel::Debug)


#ifdef CROW_USE_BOOST
#include <boost/asio.hpp>
#include <boost/asio/basic_waitable_timer.hpp>
#else
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#include <asio.hpp>
#include <asio/basic_waitable_timer.hpp>
#endif

#include <chrono>
#include <functional>
#include <map>
#include <vector>


namespace crow
{
#ifdef CROW_USE_BOOST
    namespace asio = boost::asio;
    using error_code = boost::system::error_code;
#else
    using error_code = asio::error_code;
#endif
    namespace detail
    {

        /// A class for scheduling functions to be called after a specific
        /// amount of ticks. Ther tick length can  be handed over in constructor, 
        /// the default tick length is equal to 1 second.
        class task_timer
        {
        public:
            using task_type = std::function<void()>;
            using identifier_type = size_t;

        private:
            using clock_type = std::chrono::steady_clock;
            using time_type = clock_type::time_point;
        public:
            task_timer(asio::io_context& io_context,
                       const std::chrono::milliseconds tick_length =
                            std::chrono::seconds(1)) :
              io_context_(io_context), timer_(io_context_),
              tick_length_ms_(tick_length)
            {
                timer_.expires_after(tick_length_ms_);
                timer_.async_wait(
                  std::bind(&task_timer::tick_handler, this,
                  std::placeholders::_1));
            }

            ~task_timer() { timer_.cancel(); }

            /// Cancel the scheduling of the given task 
            ///
            /// \param identifier_type task identifier of the task to cancel.
            void cancel(identifier_type id)
            {
                tasks_.erase(id);
                CROW_LOG_DEBUG << "task_timer task cancelled: " << this << ' ' << id;
            }

            /// Schedule the given task to be executed after the default amount
            /// of ticks.

            ///
            /// \return identifier_type Used to cancel the thread.
            /// It is not bound to this task_timer instance and in some cases
            /// could lead to undefined behavior if used with other task_timer
            /// objects or after the task has been successfully executed.
            identifier_type schedule(const task_type& task)
            {
                return schedule(task, get_default_timeout());
            }

            /// Schedule the given task to be executed after the given time.

            ///
            /// \param timeout The amount of ticks to wait before execution.
            ///
            /// \return identifier_type Used to cancel the thread.
            /// It is not bound to this task_timer instance and in some cases
            /// could lead to undefined behavior if used with other task_timer
            /// objects or after the task has been successfully executed.
            identifier_type schedule(const task_type& task, uint8_t timeout)
            {
                tasks_.insert({++highest_id_,
                               {clock_type::now() + (timeout * tick_length_ms_),
                                task}});
                CROW_LOG_DEBUG << "task_timer scheduled: " << this << ' ' <<
                                  highest_id_;
                return highest_id_;
            }

            /// Set the default timeout for this task_timer instance.
            /// (Default: 5)

            ///
            /// \param timeout The amount of ticks to wait before
            /// execution. 
            /// For tick length \see tick_length_ms_ 
            void set_default_timeout(uint8_t timeout) {
                default_timeout_ = timeout;
            }

            /// Get the default timeout. (Default: 5)
            uint8_t get_default_timeout() const {
                return default_timeout_;
            }

            /// returns the length of one tick.
            std::chrono::milliseconds get_tick_length() const {
                return tick_length_ms_;
            }

        private:
            void process_tasks()
            {
                time_type current_time = clock_type::now();
                std::vector<identifier_type> finished_tasks;

                for (const auto& task : tasks_)
                {
                    if (task.second.first < current_time)
                    {
                        (task.second.second)();
                        finished_tasks.push_back(task.first);
                        CROW_LOG_DEBUG << "task_timer called: " << this <<
                                          ' ' << task.first;
                    }
                }

                for (const auto& task : finished_tasks)
                    tasks_.erase(task);

                // If no task is currently scheduled, reset the issued ids back
                // to 0.
                if (tasks_.empty()) highest_id_ = 0;
            }

            void tick_handler(const error_code& ec)
            {
                if (ec) return;

                process_tasks();

                timer_.expires_after(tick_length_ms_);
                timer_.async_wait(
                  std::bind(&task_timer::tick_handler, this, std::placeholders::_1));
            }

        private:
            asio::io_context& io_context_;
            asio::basic_waitable_timer<clock_type> timer_;
            std::map<identifier_type, std::pair<time_type, task_type>> tasks_;

            // A continuously increasing number to be issued to threads to
            // identify them. If no tasks are scheduled, it will be reset to 0.
            identifier_type highest_id_{0};
            std::chrono::milliseconds tick_length_ms_;
            uint8_t default_timeout_{5};

        };
    } // namespace detail
} // namespace crow

#ifdef CROW_USE_BOOST
#include <boost/asio.hpp>
#ifdef CROW_ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif
#else
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#include <asio.hpp>
#ifdef CROW_ENABLE_SSL
#include <asio/ssl.hpp>
#endif
#endif


namespace crow
{
#ifdef CROW_USE_BOOST
    namespace asio = boost::asio;
    using error_code = boost::system::error_code;
#else
    using error_code = asio::error_code;
#endif
    using tcp = asio::ip::tcp;
    using stream_protocol = asio::local::stream_protocol;

    struct TCPAcceptor
    {
        using endpoint = tcp::endpoint;
        tcp::acceptor acceptor_;
        TCPAcceptor(asio::io_context& io_context):
          acceptor_(io_context) {}

        int16_t port() const
        {
            return acceptor_.local_endpoint().port();
        }
        std::string address() const
        {
            return acceptor_.local_endpoint().address().to_string();
        }
        std::string url_display(bool ssl_used) const
        {
            return (ssl_used ? "https://" : "http://") + acceptor_.local_endpoint().address().to_string() + ":" + std::to_string(acceptor_.local_endpoint().port());
        }
        tcp::acceptor& raw_acceptor()
        {
            return acceptor_;
        }
        endpoint local_endpoint() const
        {
            return acceptor_.local_endpoint();
        }
        inline static tcp::acceptor::reuse_address reuse_address_option() { return tcp::acceptor::reuse_address(true); }
    };

    struct UnixSocketAcceptor
    {
        using endpoint = stream_protocol::endpoint;
        stream_protocol::acceptor acceptor_;
        UnixSocketAcceptor(asio::io_context& io_context):
          acceptor_(io_context) {}

        int16_t port() const
        {
            return 0;
        }
        std::string address() const
        {
            return acceptor_.local_endpoint().path();
        }
        std::string url_display(bool) const
        {
            return acceptor_.local_endpoint().path();
        }
        stream_protocol::acceptor& raw_acceptor()
        {
            return acceptor_;
        }
        endpoint local_endpoint() const
        {
            return acceptor_.local_endpoint();
        }
        inline static stream_protocol::acceptor::reuse_address reuse_address_option()
        {
            // reuse addr must be false (https://github.com/chriskohlhoff/asio/issues/622)
            return stream_protocol::acceptor::reuse_address(false);
        }
    };
} // namespace crow

//#define CROW_JSON_NO_ERROR_CHECK
//#define CROW_JSON_USE_MAP

#include <string>
#ifdef CROW_JSON_USE_MAP
#include <map>
#else
#include <unordered_map>
#endif
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <cmath>
#include <cfloat>


using std::isinf;
using std::isnan;


namespace crow // NOTE: Already documented in "crow/app.h"
{
    namespace mustache
    {
        class template_t;
    }

    namespace json
    {
        static inline char to_hex(char c)
        {
            c = c & 0xf;
            if (c < 10)
                return '0' + c;
            return 'a' + c - 10;
        }

        inline void escape(const std::string& str, std::string& ret)
        {
            ret.reserve(ret.size() + str.size() + str.size() / 4);
            for (auto c : str)
            {
                switch (c)
                {
                    case '"': ret += "\\\""; break;
                    case '\\': ret += "\\\\"; break;
                    case '\n': ret += "\\n"; break;
                    case '\b': ret += "\\b"; break;
                    case '\f': ret += "\\f"; break;
                    case '\r': ret += "\\r"; break;
                    case '\t': ret += "\\t"; break;
                    default:
                        if (c >= 0 && c < 0x20)
                        {
                            ret += "\\u00";
                            ret += to_hex(c / 16);
                            ret += to_hex(c % 16);
                        }
                        else
                            ret += c;
                        break;
                }
            }
        }
        inline std::string escape(const std::string& str)
        {
            std::string ret;
            escape(str, ret);
            return ret;
        }

        enum class type : char
        {
            Null,
            False,
            True,
            Number,
            String,
            List,
            Object,
            Function
        };

        inline const char* get_type_str(type t)
        {
            switch (t)
            {
                case type::Number: return "Number";
                case type::False: return "False";
                case type::True: return "True";
                case type::List: return "List";
                case type::String: return "String";
                case type::Object: return "Object";
                case type::Function: return "Function";
                default: return "Unknown";
            }
        }

        enum class num_type : char
        {
            Signed_integer,
            Unsigned_integer,
            Floating_point,
            Null,
            Double_precision_floating_point
        };

        class rvalue;
        rvalue load(const char* data, size_t size);

        namespace detail
        {
            /// A read string implementation with comparison functionality.
            struct r_string
            {
                r_string(){}
                r_string(char* s, char* e):
                  s_(s), e_(e){}
                ~r_string()
                {
                    if (owned_)
                        delete[] s_;
                }

                r_string(const r_string& r)
                {
                    *this = r;
                }

                r_string(r_string&& r)
                {
                    *this = r;
                }

                r_string& operator=(r_string&& r)
                {
                    s_ = r.s_;
                    e_ = r.e_;
                    owned_ = r.owned_;
                    if (r.owned_)
                        r.owned_ = 0;
                    return *this;
                }

                r_string& operator=(const r_string& r)
                {
                    s_ = r.s_;
                    e_ = r.e_;
                    owned_ = 0;
                    return *this;
                }

                operator std::string() const
                {
                    return std::string(s_, e_);
                }


                const char* begin() const { return s_; }
                const char* end() const { return e_; }
                size_t size() const { return end() - begin(); }

                using iterator = const char*;
                using const_iterator = const char*;

                char* s_;         ///< Start.
                mutable char* e_; ///< End.
                uint8_t owned_{0};
                friend std::ostream& operator<<(std::ostream& os, const r_string& s)
                {
                    os << static_cast<std::string>(s);
                    return os;
                }

            private:
                void force(char* s, uint32_t length)
                {
                    s_ = s;
                    e_ = s_ + length;
                    owned_ = 1;
                }
                friend rvalue crow::json::load(const char* data, size_t size);

                friend bool operator==(const r_string& l, const r_string& r);
                friend bool operator==(const std::string& l, const r_string& r);
                friend bool operator==(const r_string& l, const std::string& r);

                template<typename T, typename U>
                inline static bool equals(const T& l, const U& r)
                {
                    if (l.size() != r.size())
                        return false;

                    for (size_t i = 0; i < l.size(); i++)
                    {
                        if (*(l.begin() + i) != *(r.begin() + i))
                            return false;
                    }

                    return true;
                }
            };

            inline bool operator<(const r_string& l, const r_string& r)
            {
                return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
            }

            inline bool operator<(const r_string& l, const std::string& r)
            {
                return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
            }

            inline bool operator<(const std::string& l, const r_string& r)
            {
                return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
            }

            inline bool operator>(const r_string& l, const r_string& r)
            {
                return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
            }

            inline bool operator>(const r_string& l, const std::string& r)
            {
                return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
            }

            inline bool operator>(const std::string& l, const r_string& r)
            {
                return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
            }

            inline bool operator==(const r_string& l, const r_string& r)
            {
                return r_string::equals(l, r);
            }

            inline bool operator==(const r_string& l, const std::string& r)
            {
                return r_string::equals(l, r);
            }

            inline bool operator==(const std::string& l, const r_string& r)
            {
                return r_string::equals(l, r);
            }

            inline bool operator!=(const r_string& l, const r_string& r)
            {
                return !(l == r);
            }

            inline bool operator!=(const r_string& l, const std::string& r)
            {
                return !(l == r);
            }

            inline bool operator!=(const std::string& l, const r_string& r)
            {
                return !(l == r);
            }
        } // namespace detail

        /// JSON read value.

        ///
        /// Value can mean any json value, including a JSON object.
        /// Read means this class is used to primarily read strings into a JSON value.
        class rvalue
        {
            static const int cached_bit = 2;
            static const int error_bit = 4;

        public:
            rvalue() noexcept:
              option_{error_bit}
            {
            }
            rvalue(type t) noexcept:
              lsize_{}, lremain_{}, t_{t}
            {
            }
            rvalue(type t, char* s, char* e) noexcept:
              start_{s}, end_{e}, t_{t}
            {
                determine_num_type();
            }

            rvalue(const rvalue& r):
              start_(r.start_), end_(r.end_), key_(r.key_), t_(r.t_), nt_(r.nt_), option_(r.option_)
            {
                copy_l(r);
            }

            rvalue(rvalue&& r) noexcept
            {
                *this = std::move(r);
            }

            rvalue& operator=(const rvalue& r)
            {
                start_ = r.start_;
                end_ = r.end_;
                key_ = r.key_;
                t_ = r.t_;
                nt_ = r.nt_;
                option_ = r.option_;
                copy_l(r);
                return *this;
            }
            rvalue& operator=(rvalue&& r) noexcept
            {
                start_ = r.start_;
                end_ = r.end_;
                key_ = std::move(r.key_);
                l_ = std::move(r.l_);
                lsize_ = r.lsize_;
                lremain_ = r.lremain_;
                t_ = r.t_;
                nt_ = r.nt_;
                option_ = r.option_;
                return *this;
            }

            explicit operator bool() const noexcept
            {
                return (option_ & error_bit) == 0;
            }

            explicit operator int64_t() const
            {
                return i();
            }

            explicit operator uint64_t() const
            {
                return u();
            }

            explicit operator int() const
            {
                return static_cast<int>(i());
            }

            /// Return any json value (not object or list) as a string.
            explicit operator std::string() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() == type::Object || t() == type::List)
                    throw std::runtime_error("json type container");
#endif
                switch (t())
                {
                    case type::String:
                        return std::string(s());
                    case type::Null:
                        return std::string("null");
                    case type::True:
                        return std::string("true");
                    case type::False:
                        return std::string("false");
                    default:
                        return std::string(start_, end_ - start_);
                }
            }

            /// The type of the JSON value.
            type t() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (option_ & error_bit)
                {
                    throw std::runtime_error("invalid json object");
                }
#endif
                return t_;
            }

            /// The number type of the JSON value.
            num_type nt() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (option_ & error_bit)
                {
                    throw std::runtime_error("invalid json object");
                }
#endif
                return nt_;
            }

            /// The integer value.
            int64_t i() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                switch (t())
                {
                    case type::Number:
                    case type::String:
                        return utility::lexical_cast<int64_t>(start_, end_ - start_);
                    default:
                        const std::string msg = "expected number, got: " + std::string(get_type_str(t()));
                        throw std::runtime_error(msg);
                }
#endif
                return utility::lexical_cast<int64_t>(start_, end_ - start_);
            }

            /// The unsigned integer value.
            uint64_t u() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                switch (t())
                {
                    case type::Number:
                    case type::String:
                        return utility::lexical_cast<uint64_t>(start_, end_ - start_);
                    default:
                        throw std::runtime_error(std::string("expected number, got: ") + get_type_str(t()));
                }
#endif
                return utility::lexical_cast<uint64_t>(start_, end_ - start_);
            }

            /// The double precision floating-point number value.
            double d() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::Number)
                    throw std::runtime_error("value is not number");
#endif
                return utility::lexical_cast<double>(start_, end_ - start_);
            }

            /// The boolean value.
            bool b() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::True && t() != type::False)
                    throw std::runtime_error("value is not boolean");
#endif
                return t() == type::True;
            }

            /// The string value.
            detail::r_string s() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::String)
                    throw std::runtime_error("value is not string");
#endif
                unescape();
                return detail::r_string{start_, end_};
            }

            /// The list or object value
            std::vector<rvalue> lo() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::Object && t() != type::List)
                    throw std::runtime_error("value is not a container");
#endif
                std::vector<rvalue> ret;
                ret.reserve(lsize_);
                for (uint32_t i = 0; i < lsize_; i++)
                {
                    ret.emplace_back(l_[i]);
                }
                return ret;
            }

            /// Convert escaped string character to their original form ("\\n" -> '\n').
            void unescape() const
            {
                if (*(start_ - 1))
                {
                    char* head = start_;
                    char* tail = start_;
                    while (head != end_)
                    {
                        if (*head == '\\')
                        {
                            switch (*++head)
                            {
                                case '"': *tail++ = '"'; break;
                                case '\\': *tail++ = '\\'; break;
                                case '/': *tail++ = '/'; break;
                                case 'b': *tail++ = '\b'; break;
                                case 'f': *tail++ = '\f'; break;
                                case 'n': *tail++ = '\n'; break;
                                case 'r': *tail++ = '\r'; break;
                                case 't': *tail++ = '\t'; break;
                                case 'u':
                                {
                                    auto from_hex = [](char c) {
                                        if (c >= 'a')
                                            return c - 'a' + 10;
                                        if (c >= 'A')
                                            return c - 'A' + 10;
                                        return c - '0';
                                    };
                                    unsigned int code =
                                      (from_hex(head[1]) << 12) +
                                      (from_hex(head[2]) << 8) +
                                      (from_hex(head[3]) << 4) +
                                      from_hex(head[4]);
                                    if (code >= 0x800)
                                    {
                                        *tail++ = 0xE0 | (code >> 12);
                                        *tail++ = 0x80 | ((code >> 6) & 0x3F);
                                        *tail++ = 0x80 | (code & 0x3F);
                                    }
                                    else if (code >= 0x80)
                                    {
                                        *tail++ = 0xC0 | (code >> 6);
                                        *tail++ = 0x80 | (code & 0x3F);
                                    }
                                    else
                                    {
                                        *tail++ = code;
                                    }
                                    head += 4;
                                }
                                break;
                            }
                        }
                        else
                            *tail++ = *head;
                        head++;
                    }
                    end_ = tail;
                    *end_ = 0;
                    *(start_ - 1) = 0;
                }
            }

            /// Check if the json object has the passed string as a key.
            bool has(const char* str) const
            {
                return has(std::string(str));
            }

            bool has(const std::string& str) const
            {
                struct Pred
                {
                    bool operator()(const rvalue& l, const rvalue& r) const
                    {
                        return l.key_ < r.key_;
                    }
                    bool operator()(const rvalue& l, const std::string& r) const
                    {
                        return l.key_ < r;
                    }
                    bool operator()(const std::string& l, const rvalue& r) const
                    {
                        return l < r.key_;
                    }
                };
                if (!is_cached())
                {
                    std::sort(begin(), end(), Pred());
                    set_cached();
                }
                auto it = lower_bound(begin(), end(), str, Pred());
                return it != end() && it->key_ == str;
            }

            int count(const std::string& str) const
            {
                return has(str) ? 1 : 0;
            }

            rvalue* begin() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::Object && t() != type::List)
                    throw std::runtime_error("value is not a container");
#endif
                return l_.get();
            }
            rvalue* end() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::Object && t() != type::List)
                    throw std::runtime_error("value is not a container");
#endif
                return l_.get() + lsize_;
            }

            const detail::r_string& key() const
            {
                return key_;
            }

            size_t size() const
            {
                if (t() == type::String)
                    return s().size();
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::Object && t() != type::List)
                    throw std::runtime_error("value is not a container");
#endif
                return lsize_;
            }

            const rvalue& operator[](int index) const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::List)
                    throw std::runtime_error("value is not a list");
                if (index >= static_cast<int>(lsize_) || index < 0)
                    throw std::runtime_error("list out of bound");
#endif
                return l_[index];
            }

            const rvalue& operator[](size_t index) const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::List)
                    throw std::runtime_error("value is not a list");
                if (index >= lsize_)
                    throw std::runtime_error("list out of bound");
#endif
                return l_[index];
            }

            const rvalue& operator[](const char* str) const
            {
                return this->operator[](std::string(str));
            }

            const rvalue& operator[](const std::string& str) const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::Object)
                    throw std::runtime_error("value is not an object");
#endif
                struct Pred
                {
                    bool operator()(const rvalue& l, const rvalue& r) const
                    {
                        return l.key_ < r.key_;
                    }
                    bool operator()(const rvalue& l, const std::string& r) const
                    {
                        return l.key_ < r;
                    }
                    bool operator()(const std::string& l, const rvalue& r) const
                    {
                        return l < r.key_;
                    }
                };
                if (!is_cached())
                {
                    std::sort(begin(), end(), Pred());
                    set_cached();
                }
                auto it = lower_bound(begin(), end(), str, Pred());
                if (it != end() && it->key_ == str)
                    return *it;
#ifndef CROW_JSON_NO_ERROR_CHECK
                throw std::runtime_error("cannot find key: " + str);
#else
                static rvalue nullValue;
                return nullValue;
#endif
            }

            void set_error()
            {
                option_ |= error_bit;
            }

            bool error() const
            {
                return (option_ & error_bit) != 0;
            }

            std::vector<std::string> keys() const
            {
#ifndef CROW_JSON_NO_ERROR_CHECK
                if (t() != type::Object)
                    throw std::runtime_error("value is not an object");
#endif
                std::vector<std::string> ret;
                ret.reserve(lsize_);
                for (uint32_t i = 0; i < lsize_; i++)
                {
                    ret.emplace_back(std::string(l_[i].key()));
                }
                return ret;
            }

        private:
            bool is_cached() const
            {
                return (option_ & cached_bit) != 0;
            }
            void set_cached() const
            {
                option_ |= cached_bit;
            }
            void copy_l(const rvalue& r)
            {
                if (r.t() != type::Object && r.t() != type::List)
                    return;
                lsize_ = r.lsize_;
                lremain_ = 0;
                l_.reset(new rvalue[lsize_]);
                std::copy(r.begin(), r.end(), begin());
            }

            void emplace_back(rvalue&& v)
            {
                if (!lremain_)
                {
                    int new_size = lsize_ + lsize_;
                    if (new_size - lsize_ > 60000)
                        new_size = lsize_ + 60000;
                    if (new_size < 4)
                        new_size = 4;
                    rvalue* p = new rvalue[new_size];
                    rvalue* p2 = p;
                    for (auto& x : *this)
                        *p2++ = std::move(x);
                    l_.reset(p);
                    lremain_ = new_size - lsize_;
                }
                l_[lsize_++] = std::move(v);
                lremain_--;
            }

            /// Determines num_type from the string.
            void determine_num_type()
            {
                if (t_ != type::Number)
                {
                    nt_ = num_type::Null;
                    return;
                }

                const std::size_t len = end_ - start_;
                const bool has_minus = std::memchr(start_, '-', len) != nullptr;
                const bool has_e = std::memchr(start_, 'e', len) != nullptr || std::memchr(start_, 'E', len) != nullptr;
                const bool has_dec_sep = std::memchr(start_, '.', len) != nullptr;
                if (has_dec_sep || has_e)
                    nt_ = num_type::Floating_point;
                else if (has_minus)
                    nt_ = num_type::Signed_integer;
                else
                    nt_ = num_type::Unsigned_integer;
            }

            mutable char* start_;
            mutable char* end_;
            detail::r_string key_;
            std::unique_ptr<rvalue[]> l_;
            uint32_t lsize_;
            uint16_t lremain_;
            type t_;
            num_type nt_{num_type::Null};
            mutable uint8_t option_{0};

            friend rvalue load_nocopy_internal(char* data, size_t size);
            friend rvalue load(const char* data, size_t size);
            friend std::ostream& operator<<(std::ostream& os, const rvalue& r)
            {
                switch (r.t_)
                {

                    case type::Null: os << "null"; break;
                    case type::False: os << "false"; break;
                    case type::True: os << "true"; break;
                    case type::Number:
                    {
                        switch (r.nt())
                        {
                            case num_type::Floating_point: os << r.d(); break;
                            case num_type::Double_precision_floating_point: os << r.d(); break;
                            case num_type::Signed_integer: os << r.i(); break;
                            case num_type::Unsigned_integer: os << r.u(); break;
                            case num_type::Null: throw std::runtime_error("Number with num_type Null");
                        }
                    }
                    break;
                    case type::String: os << '"' << r.s() << '"'; break;
                    case type::List:
                    {
                        os << '[';
                        bool first = true;
                        for (auto& x : r)
                        {
                            if (!first)
                                os << ',';
                            first = false;
                            os << x;
                        }
                        os << ']';
                    }
                    break;
                    case type::Object:
                    {
                        os << '{';
                        bool first = true;
                        for (auto& x : r)
                        {
                            if (!first)
                                os << ',';
                            os << '"' << escape(x.key_) << "\":";
                            first = false;
                            os << x;
                        }
                        os << '}';
                    }
                    break;
                    case type::Function: os << "custom function"; break;
                }
                return os;
            }
        };
        namespace detail
        {
        }

        inline bool operator==(const rvalue& l, const std::string& r)
        {
            return l.s() == r;
        }

        inline bool operator==(const std::string& l, const rvalue& r)
        {
            return l == r.s();
        }

        inline bool operator!=(const rvalue& l, const std::string& r)
        {
            return l.s() != r;
        }

        inline bool operator!=(const std::string& l, const rvalue& r)
        {
            return l != r.s();
        }

        inline bool operator==(const rvalue& l, const int& r)
        {
          return l.i() == r;
        }

        inline bool operator==(const int& l, const rvalue& r)
        {
          return l == r.i();
        }

        inline bool operator!=(const rvalue& l, const int& r)
        {
          return l.i() != r;
        }

        inline bool operator!=(const int& l, const rvalue& r)
        {
          return l != r.i();
        }


        inline rvalue load_nocopy_internal(char* data, size_t size)
        {
            // Defend against excessive recursion
            static constexpr unsigned max_depth = 10000;

            //static const char* escaped = "\"\\/\b\f\n\r\t";
            struct Parser
            {
                Parser(char* data_, size_t /*size*/):
                  data(data_)
                {
                }

                bool consume(char c)
                {
                    if (CROW_UNLIKELY(*data != c))
                        return false;
                    data++;
                    return true;
                }

                void ws_skip()
                {
                    while (*data == ' ' || *data == '\t' || *data == '\r' || *data == '\n')
                        ++data;
                }

                rvalue decode_string()
                {
                    if (CROW_UNLIKELY(!consume('"')))
                        return {};
                    char* start = data;
                    uint8_t has_escaping = 0;
                    while (1)
                    {
                        if (CROW_LIKELY(*data != '"' && *data != '\\' && *data != '\0'))
                        {
                            data++;
                        }
                        else if (*data == '"')
                        {
                            *data = 0;
                            *(start - 1) = has_escaping;
                            data++;
                            return {type::String, start, data - 1};
                        }
                        else if (*data == '\\')
                        {
                            has_escaping = 1;
                            data++;
                            switch (*data)
                            {
                                case 'u':
                                {
                                    auto check = [](char c) {
                                        return ('0' <= c && c <= '9') ||
                                               ('a' <= c && c <= 'f') ||
                                               ('A' <= c && c <= 'F');
                                    };
                                    if (!(check(*(data + 1)) &&
                                          check(*(data + 2)) &&
                                          check(*(data + 3)) &&
                                          check(*(data + 4))))
                                        return {};
                                }
                                    data += 5;
                                    break;
                                case '"':
                                case '\\':
                                case '/':
                                case 'b':
                                case 'f':
                                case 'n':
                                case 'r':
                                case 't':
                                    data++;
                                    break;
                                default:
                                    return {};
                            }
                        }
                        else
                            return {};
                    }
                    return {};
                }

                rvalue decode_list(unsigned depth)
                {
                    rvalue ret(type::List);
                    if (CROW_UNLIKELY(!consume('[')) || CROW_UNLIKELY(depth > max_depth))
                    {
                        ret.set_error();
                        return ret;
                    }
                    ws_skip();
                    if (CROW_UNLIKELY(*data == ']'))
                    {
                        data++;
                        return ret;
                    }

                    while (1)
                    {
                        auto v = decode_value(depth + 1);
                        if (CROW_UNLIKELY(!v))
                        {
                            ret.set_error();
                            break;
                        }
                        ws_skip();
                        ret.emplace_back(std::move(v));
                        if (*data == ']')
                        {
                            data++;
                            break;
                        }
                        if (CROW_UNLIKELY(!consume(',')))
                        {
                            ret.set_error();
                            break;
                        }
                        ws_skip();
                    }
                    return ret;
                }

                rvalue decode_number()
                {
                    char* start = data;

                    enum NumberParsingState
                    {
                        Minus,
                        AfterMinus,
                        ZeroFirst,
                        Digits,
                        DigitsAfterPoints,
                        E,
                        DigitsAfterE,
                        Invalid,
                    } state{Minus};
                    while (CROW_LIKELY(state != Invalid))
                    {
                        switch (*data)
                        {
                            case '0':
                                state = static_cast<NumberParsingState>("\2\2\7\3\4\6\6"[state]);
                                /*if (state == NumberParsingState::Minus || state == NumberParsingState::AfterMinus)
                                {
                                    state = NumberParsingState::ZeroFirst;
                                }
                                else if (state == NumberParsingState::Digits ||
                                    state == NumberParsingState::DigitsAfterE ||
                                    state == NumberParsingState::DigitsAfterPoints)
                                {
                                    // ok; pass
                                }
                                else if (state == NumberParsingState::E)
                                {
                                    state = NumberParsingState::DigitsAfterE;
                                }
                                else
                                    return {};*/
                                break;
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                                state = static_cast<NumberParsingState>("\3\3\7\3\4\6\6"[state]);
                                while (*(data + 1) >= '0' && *(data + 1) <= '9')
                                    data++;
                                /*if (state == NumberParsingState::Minus || state == NumberParsingState::AfterMinus)
                                {
                                    state = NumberParsingState::Digits;
                                }
                                else if (state == NumberParsingState::Digits ||
                                    state == NumberParsingState::DigitsAfterE ||
                                    state == NumberParsingState::DigitsAfterPoints)
                                {
                                    // ok; pass
                                }
                                else if (state == NumberParsingState::E)
                                {
                                    state = NumberParsingState::DigitsAfterE;
                                }
                                else
                                    return {};*/
                                break;
                            case '.':
                                state = static_cast<NumberParsingState>("\7\7\4\4\7\7\7"[state]);
                                /*
                                if (state == NumberParsingState::Digits || state == NumberParsingState::ZeroFirst)
                                {
                                    state = NumberParsingState::DigitsAfterPoints;
                                }
                                else
                                    return {};
                                */
                                break;
                            case '-':
                                state = static_cast<NumberParsingState>("\1\7\7\7\7\6\7"[state]);
                                /*if (state == NumberParsingState::Minus)
                                {
                                    state = NumberParsingState::AfterMinus;
                                }
                                else if (state == NumberParsingState::E)
                                {
                                    state = NumberParsingState::DigitsAfterE;
                                }
                                else
                                    return {};*/
                                break;
                            case '+':
                                state = static_cast<NumberParsingState>("\7\7\7\7\7\6\7"[state]);
                                /*if (state == NumberParsingState::E)
                                {
                                    state = NumberParsingState::DigitsAfterE;
                                }
                                else
                                    return {};*/
                                break;
                            case 'e':
                            case 'E':
                                state = static_cast<NumberParsingState>("\7\7\7\5\5\7\7"[state]);
                                /*if (state == NumberParsingState::Digits ||
                                    state == NumberParsingState::DigitsAfterPoints)
                                {
                                    state = NumberParsingState::E;
                                }
                                else
                                    return {};*/
                                break;
                            default:
                                if (CROW_LIKELY(state == NumberParsingState::ZeroFirst ||
                                                state == NumberParsingState::Digits ||
                                                state == NumberParsingState::DigitsAfterPoints ||
                                                state == NumberParsingState::DigitsAfterE))
                                    return {type::Number, start, data};
                                else
                                    return {};
                        }
                        data++;
                    }

                    return {};
                }


                rvalue decode_value(unsigned depth)
                {
                    switch (*data)
                    {
                        case '[':
                            return decode_list(depth + 1);
                        case '{':
                            return decode_object(depth + 1);
                        case '"':
                            return decode_string();
                        case 't':
                            if ( //e-data >= 4 &&
                              data[1] == 'r' &&
                              data[2] == 'u' &&
                              data[3] == 'e')
                            {
                                data += 4;
                                return {type::True};
                            }
                            else
                                return {};
                        case 'f':
                            if ( //e-data >= 5 &&
                              data[1] == 'a' &&
                              data[2] == 'l' &&
                              data[3] == 's' &&
                              data[4] == 'e')
                            {
                                data += 5;
                                return {type::False};
                            }
                            else
                                return {};
                        case 'n':
                            if ( //e-data >= 4 &&
                              data[1] == 'u' &&
                              data[2] == 'l' &&
                              data[3] == 'l')
                            {
                                data += 4;
                                return {type::Null};
                            }
                            else
                                return {};
                        //case '1': case '2': case '3':
                        //case '4': case '5': case '6':
                        //case '7': case '8': case '9':
                        //case '0': case '-':
                        default:
                            return decode_number();
                    }
                    return {};
                }

                rvalue decode_object(unsigned depth)
                {
                    rvalue ret(type::Object);
                    if (CROW_UNLIKELY(!consume('{')) || CROW_UNLIKELY(depth > max_depth))
                    {
                        ret.set_error();
                        return ret;
                    }

                    ws_skip();

                    if (CROW_UNLIKELY(*data == '}'))
                    {
                        data++;
                        return ret;
                    }

                    while (1)
                    {
                        auto t = decode_string();
                        if (CROW_UNLIKELY(!t))
                        {
                            ret.set_error();
                            break;
                        }

                        ws_skip();
                        if (CROW_UNLIKELY(!consume(':')))
                        {
                            ret.set_error();
                            break;
                        }

                        // TODO(ipkn) caching key to speed up (flyweight?)
                        // I have no idea how flyweight could apply here, but maybe some speedup can happen if we stopped checking type since decode_string returns a string anyway
                        auto key = t.s();

                        ws_skip();
                        auto v = decode_value(depth + 1);
                        if (CROW_UNLIKELY(!v))
                        {
                            ret.set_error();
                            break;
                        }
                        ws_skip();

                        v.key_ = std::move(key);
                        ret.emplace_back(std::move(v));
                        if (CROW_UNLIKELY(*data == '}'))
                        {
                            data++;
                            break;
                        }
                        if (CROW_UNLIKELY(!consume(',')))
                        {
                            ret.set_error();
                            break;
                        }
                        ws_skip();
                    }
                    return ret;
                }

                rvalue parse()
                {
                    ws_skip();
                    auto ret = decode_value(0); // or decode object?
                    ws_skip();
                    if (ret && *data != '\0')
                        ret.set_error();
                    return ret;
                }

                char* data;
            };
            return Parser(data, size).parse();
        }
        inline rvalue load(const char* data, size_t size)
        {
            char* s = new char[size + 1];
            memcpy(s, data, size);
            s[size] = 0;
            auto ret = load_nocopy_internal(s, size);
            if (ret)
                ret.key_.force(s, size);
            else
                delete[] s;
            return ret;
        }

        inline rvalue load(const char* data)
        {
            return load(data, strlen(data));
        }

        inline rvalue load(const std::string& str)
        {
            return load(str.data(), str.size());
        }

        struct wvalue_reader;

        /// JSON write value.

        ///
        /// Value can mean any json value, including a JSON object.<br>
        /// Write means this class is used to primarily assemble JSON objects using keys and values and export those into a string.
        class wvalue : public returnable
        {
            friend class crow::mustache::template_t;
            friend struct wvalue_reader;

        public:
            using object =
#ifdef CROW_JSON_USE_MAP
              std::map<std::string, wvalue>;
#else
              std::unordered_map<std::string, wvalue>;
#endif

            using list = std::vector<wvalue>;

            type t() const { return t_; }

            /// Create an empty json value (outputs "{}" instead of a "null" string)
            static crow::json::wvalue empty_object() { return crow::json::wvalue::object(); }

        private:
            type t_{type::Null};         ///< The type of the value.
            num_type nt{num_type::Null}; ///< The specific type of the number if \ref t_ is a number.
            union number
            {
                double d;
                int64_t si;
                uint64_t ui;

            public:
                constexpr number() noexcept:
                  ui() {} /* default constructor initializes unsigned integer. */
                constexpr number(std::uint64_t value) noexcept:
                  ui(value) {}
                constexpr number(std::int64_t value) noexcept:
                  si(value) {}
                explicit constexpr number(double value) noexcept:
                  d(value) {}
                explicit constexpr number(float value) noexcept:
                  d(value) {}
            } num;                                      ///< Value if type is a number.
            std::string s;                              ///< Value if type is a string.
            std::unique_ptr<list> l;                    ///< Value if type is a list.
            std::unique_ptr<object> o;                  ///< Value if type is a JSON object.
            std::function<std::string(std::string&)> f; ///< Value if type is a function (C++ lambda)

        public:
            wvalue():
              returnable("application/json") {}

            wvalue(std::nullptr_t):
              returnable("application/json"), t_(type::Null) {}

            wvalue(bool value):
              returnable("application/json"), t_(value ? type::True : type::False) {}

            wvalue(std::uint8_t value):
              returnable("application/json"), t_(type::Number), nt(num_type::Unsigned_integer), num(static_cast<std::uint64_t>(value)) {}
            wvalue(std::uint16_t value):
              returnable("application/json"), t_(type::Number), nt(num_type::Unsigned_integer), num(static_cast<std::uint64_t>(value)) {}
            wvalue(std::uint32_t value):
              returnable("application/json"), t_(type::Number), nt(num_type::Unsigned_integer), num(static_cast<std::uint64_t>(value)) {}
            wvalue(std::uint64_t value):
              returnable("application/json"), t_(type::Number), nt(num_type::Unsigned_integer), num(static_cast<std::uint64_t>(value)) {}

            wvalue(std::int8_t value):
              returnable("application/json"), t_(type::Number), nt(num_type::Signed_integer), num(static_cast<std::int64_t>(value)) {}
            wvalue(std::int16_t value):
              returnable("application/json"), t_(type::Number), nt(num_type::Signed_integer), num(static_cast<std::int64_t>(value)) {}
            wvalue(std::int32_t value):
              returnable("application/json"), t_(type::Number), nt(num_type::Signed_integer), num(static_cast<std::int64_t>(value)) {}
            wvalue(std::int64_t value):
              returnable("application/json"), t_(type::Number), nt(num_type::Signed_integer), num(static_cast<std::int64_t>(value)) {}

            wvalue(float value):
              returnable("application/json"), t_(type::Number), nt(num_type::Floating_point), num(static_cast<double>(value)) {}
            wvalue(double value):
              returnable("application/json"), t_(type::Number), nt(num_type::Double_precision_floating_point), num(static_cast<double>(value)) {}

            wvalue(char const* value):
              returnable("application/json"), t_(type::String), s(value) {}

            wvalue(std::string const& value):
              returnable("application/json"), t_(type::String), s(value) {}
            wvalue(std::string&& value):
              returnable("application/json"), t_(type::String), s(std::move(value)) {}

            wvalue(std::initializer_list<std::pair<std::string const, wvalue>> initializer_list):
              returnable("application/json"), t_(type::Object), o(new object(initializer_list)) {}

            wvalue(object const& value):
              returnable("application/json"), t_(type::Object), o(new object(value)) {}
            wvalue(object&& value):
              returnable("application/json"), t_(type::Object), o(new object(std::move(value))) {}

            wvalue(const list& r):
              returnable("application/json")
            {
                t_ = type::List;
                l = std::unique_ptr<list>(new list{});
                l->reserve(r.size());
                for (auto it = r.begin(); it != r.end(); ++it)
                    l->emplace_back(*it);
            }
            wvalue(list& r):
              returnable("application/json")
            {
                t_ = type::List;
                l = std::unique_ptr<list>(new list{});
                l->reserve(r.size());
                for (auto it = r.begin(); it != r.end(); ++it)
                    l->emplace_back(*it);
            }

            /// Create a write value from a read value (useful for editing JSON strings).
            wvalue(const rvalue& r):
              returnable("application/json")
            {
                t_ = r.t();
                switch (r.t())
                {
                    case type::Null:
                    case type::False:
                    case type::True:
                    case type::Function:
                        return;
                    case type::Number:
                        nt = r.nt();
                        if (nt == num_type::Floating_point || nt == num_type::Double_precision_floating_point)
                            num.d = r.d();
                        else if (nt == num_type::Signed_integer)
                            num.si = r.i();
                        else
                            num.ui = r.u();
                        return;
                    case type::String:
                        s = r.s();
                        return;
                    case type::List:
                        l = std::unique_ptr<list>(new list{});
                        l->reserve(r.size());
                        for (auto it = r.begin(); it != r.end(); ++it)
                            l->emplace_back(*it);
                        return;
                    case type::Object:
                        o = std::unique_ptr<object>(new object{});
                        for (auto it = r.begin(); it != r.end(); ++it)
                            o->emplace(it->key(), *it);
                        return;
                }
            }

            wvalue(const wvalue& r):
              returnable("application/json")
            {
                t_ = r.t();
                switch (r.t())
                {
                    case type::Null:
                    case type::False:
                    case type::True:
                        return;
                    case type::Number:
                        nt = r.nt;
                        if (nt == num_type::Floating_point || nt == num_type::Double_precision_floating_point)
                            num.d = r.num.d;
                        else if (nt == num_type::Signed_integer)
                            num.si = r.num.si;
                        else
                            num.ui = r.num.ui;
                        return;
                    case type::String:
                        s = r.s;
                        return;
                    case type::List:
                        l = std::unique_ptr<list>(new list{});
                        l->reserve(r.size());
                        for (auto it = r.l->begin(); it != r.l->end(); ++it)
                            l->emplace_back(*it);
                        return;
                    case type::Object:
                        o = std::unique_ptr<object>(new object{});
                        o->insert(r.o->begin(), r.o->end());
                        return;
                    case type::Function:
                        f = r.f;
                }
            }

            wvalue(wvalue&& r):
              returnable("application/json")
            {
                *this = std::move(r);
            }

            wvalue& operator=(wvalue&& r)
            {
                t_ = r.t_;
                nt = r.nt;
                num = r.num;
                s = std::move(r.s);
                l = std::move(r.l);
                o = std::move(r.o);
                return *this;
            }

            /// Used for compatibility, same as \ref reset()
            void clear()
            {
                reset();
            }

            void reset()
            {
                t_ = type::Null;
                l.reset();
                o.reset();
            }

            wvalue& operator=(std::nullptr_t)
            {
                reset();
                return *this;
            }
            wvalue& operator=(bool value)
            {
                reset();
                if (value)
                    t_ = type::True;
                else
                    t_ = type::False;
                return *this;
            }

            wvalue& operator=(float value)
            {
                reset();
                t_ = type::Number;
                num.d = value;
                nt = num_type::Floating_point;
                return *this;
            }

            wvalue& operator=(double value)
            {
                reset();
                t_ = type::Number;
                num.d = value;
                nt = num_type::Double_precision_floating_point;
                return *this;
            }

            wvalue& operator=(unsigned short value)
            {
                reset();
                t_ = type::Number;
                num.ui = value;
                nt = num_type::Unsigned_integer;
                return *this;
            }

            wvalue& operator=(short value)
            {
                reset();
                t_ = type::Number;
                num.si = value;
                nt = num_type::Signed_integer;
                return *this;
            }

            wvalue& operator=(long long value)
            {
                reset();
                t_ = type::Number;
                num.si = value;
                nt = num_type::Signed_integer;
                return *this;
            }

            wvalue& operator=(long value)
            {
                reset();
                t_ = type::Number;
                num.si = value;
                nt = num_type::Signed_integer;
                return *this;
            }

            wvalue& operator=(int value)
            {
                reset();
                t_ = type::Number;
                num.si = value;
                nt = num_type::Signed_integer;
                return *this;
            }

            wvalue& operator=(unsigned long long value)
            {
                reset();
                t_ = type::Number;
                num.ui = value;
                nt = num_type::Unsigned_integer;
                return *this;
            }

            wvalue& operator=(unsigned long value)
            {
                reset();
                t_ = type::Number;
                num.ui = value;
                nt = num_type::Unsigned_integer;
                return *this;
            }

            wvalue& operator=(unsigned int value)
            {
                reset();
                t_ = type::Number;
                num.ui = value;
                nt = num_type::Unsigned_integer;
                return *this;
            }

            wvalue& operator=(const char* str)
            {
                reset();
                t_ = type::String;
                s = str;
                return *this;
            }

            wvalue& operator=(const std::string& str)
            {
                reset();
                t_ = type::String;
                s = str;
                return *this;
            }

            wvalue& operator=(list&& v)
            {
                if (t_ != type::List)
                    reset();
                t_ = type::List;
                if (!l)
                    l = std::unique_ptr<list>(new list{});
                l->clear();
                l->resize(v.size());
                size_t idx = 0;
                for (auto& x : v)
                {
                    (*l)[idx++] = std::move(x);
                }
                return *this;
            }

            template<typename T>
            wvalue& operator=(const std::vector<T>& v)
            {
                if (t_ != type::List)
                    reset();
                t_ = type::List;
                if (!l)
                    l = std::unique_ptr<list>(new list{});
                l->clear();
                l->resize(v.size());
                size_t idx = 0;
                for (auto& x : v)
                {
                    (*l)[idx++] = x;
                }
                return *this;
            }

            wvalue& operator=(std::initializer_list<std::pair<std::string const, wvalue>> initializer_list)
            {
                if (t_ != type::Object)
                {
                    reset();
                    t_ = type::Object;
                    o = std::unique_ptr<object>(new object(initializer_list));
                }
                else
                {
#if defined(__APPLE__) || defined(__MACH__) || defined(__FreeBSD__) || defined(__ANDROID__) || defined(_LIBCPP_VERSION)
                    o = std::unique_ptr<object>(new object(initializer_list));
#else
                    (*o) = initializer_list;
#endif
                }
                return *this;
            }

            wvalue& operator=(object const& value)
            {
                if (t_ != type::Object)
                {
                    reset();
                    t_ = type::Object;
                    o = std::unique_ptr<object>(new object(value));
                }
                else
                {
#if defined(__APPLE__) || defined(__MACH__) || defined(__FreeBSD__) || defined(__ANDROID__) || defined(_LIBCPP_VERSION)
                    o = std::unique_ptr<object>(new object(value));
#else
                    (*o) = value;
#endif
                }
                return *this;
            }

            wvalue& operator=(object&& value)
            {
                if (t_ != type::Object)
                {
                    reset();
                    t_ = type::Object;
                    o = std::unique_ptr<object>(new object(std::move(value)));
                }
                else
                {
                    (*o) = std::move(value);
                }
                return *this;
            }

            wvalue& operator=(std::function<std::string(std::string&)>&& func)
            {
                reset();
                t_ = type::Function;
                f = std::move(func);
                return *this;
            }

            wvalue& operator[](unsigned index)
            {
                if (t_ != type::List)
                    reset();
                t_ = type::List;
                if (!l)
                    l = std::unique_ptr<list>(new list{});
                if (l->size() < index + 1)
                    l->resize(index + 1);
                return (*l)[index];
            }

            const wvalue& operator[](unsigned index) const
            {
                return const_cast<wvalue*>(this)->operator[](index);
            }

            int count(const std::string& str) const
            {
                if (t_ != type::Object)
                    return 0;
                if (!o)
                    return 0;
                return o->count(str);
            }

            wvalue& operator[](const std::string& str)
            {
                if (t_ != type::Object)
                    reset();
                t_ = type::Object;
                if (!o)
                    o = std::unique_ptr<object>(new object{});
                return (*o)[str];
            }

            const wvalue& operator[](const std::string& str) const
            {
                return const_cast<wvalue*>(this)->operator[](str);
            }

            std::vector<std::string> keys() const
            {
                if (t_ != type::Object)
                    return {};
                std::vector<std::string> result;
                for (auto& kv : *o)
                {
                    result.push_back(kv.first);
                }
                return result;
            }

            std::string execute(std::string txt = "") const //Not using reference because it cannot be used with a default rvalue
            {
                if (t_ != type::Function)
                    return "";
                return f(txt);
            }

            /// If the wvalue is a list, it returns the length of the list, otherwise it returns 1.
            std::size_t size() const
            {
                if (t_ != type::List)
                    return 1;
                return l->size();
            }

            /// Returns an estimated size of the value in bytes.
            size_t estimate_length() const
            {
                switch (t_)
                {
                    case type::Null: return 4;
                    case type::False: return 5;
                    case type::True: return 4;
                    case type::Number: return 30;
                    case type::String: return 2 + s.size() + s.size() / 2;
                    case type::List:
                    {
                        size_t sum{};
                        if (l)
                        {
                            for (auto& x : *l)
                            {
                                sum += 1;
                                sum += x.estimate_length();
                            }
                        }
                        return sum + 2;
                    }
                    case type::Object:
                    {
                        size_t sum{};
                        if (o)
                        {
                            for (auto& kv : *o)
                            {
                                sum += 2;
                                sum += 2 + kv.first.size() + kv.first.size() / 2;
                                sum += kv.second.estimate_length();
                            }
                        }
                        return sum + 2;
                    }
                    case type::Function:
                        return 0;
                }
                return 1;
            }

        private:
            inline void dump_string(const std::string& str, std::string& out) const
            {
                out.push_back('"');
                escape(str, out);
                out.push_back('"');
            }

            inline void dump_indentation_part(std::string& out, const int indent, const char separator, const int indent_level) const
            {
                out.push_back('\n');
                out.append(indent_level * indent, separator);
            }


            inline void dump_internal(const wvalue& v, std::string& out, const int indent, const char separator, const int indent_level = 0) const
            {
                switch (v.t_)
                {
                    case type::Null: out += "null"; break;
                    case type::False: out += "false"; break;
                    case type::True: out += "true"; break;
                    case type::Number:
                    {
                        if (v.nt == num_type::Floating_point || v.nt == num_type::Double_precision_floating_point)
                        {
                            if (isnan(v.num.d) || isinf(v.num.d))
                            {
                                out += "null";
                                CROW_LOG_WARNING << "Invalid JSON value detected (" << v.num.d << "), value set to null";
                                break;
                            }
                            enum
                            {
                                start,
                                decp, // Decimal point
                                zero
                            } f_state;
                            char outbuf[128];
                            if (v.nt == num_type::Double_precision_floating_point)
                            {
#ifdef _MSC_VER
                                sprintf_s(outbuf, sizeof(outbuf), "%.*g", DECIMAL_DIG, v.num.d);
#else
                                snprintf(outbuf, sizeof(outbuf), "%.*g", DECIMAL_DIG, v.num.d);
#endif
                            }
                            else
                            {
#ifdef _MSC_VER
                                sprintf_s(outbuf, sizeof(outbuf), "%f", v.num.d);
#else
                                snprintf(outbuf, sizeof(outbuf), "%f", v.num.d);
#endif
                            }
                            char* p = &outbuf[0];
                            char* pos_first_trailing_0 = nullptr;
                            f_state = start;
                            while (*p != '\0')
                            {
                                //std::cout << *p << std::endl;
                                char ch = *p;
                                switch (f_state)
                                {
                                    case start: // Loop and lookahead until a decimal point is found
                                        if (ch == '.')
                                        {
                                            char fch = *(p + 1);
                                            // if the first character is 0, leave it be (this is so that "1.00000" becomes "1.0" and not "1.")
                                            if (fch != '\0' && fch == '0') p++;
                                            f_state = decp;
                                        }
                                        p++;
                                        break;
                                    case decp: // Loop until a 0 is found, if found, record its position
                                        if (ch == '0')
                                        {
                                            f_state = zero;
                                            pos_first_trailing_0 = p;
                                        }
                                        p++;
                                        break;
                                    case zero: // if a non 0 is found (e.g. 1.00004) remove the earlier recorded 0 position and look for more trailing 0s
                                        if (ch != '0')
                                        {
                                            pos_first_trailing_0 = nullptr;
                                            f_state = decp;
                                        }
                                        p++;
                                        break;
                                }
                            }
                            if (pos_first_trailing_0 != nullptr) // if any trailing 0s are found, terminate the string where they begin
                                *pos_first_trailing_0 = '\0';
                            out += outbuf;
                        }
                        else if (v.nt == num_type::Signed_integer)
                        {
                            out += std::to_string(v.num.si);
                        }
                        else
                        {
                            out += std::to_string(v.num.ui);
                        }
                    }
                    break;
                    case type::String: dump_string(v.s, out); break;
                    case type::List:
                    {
                        out.push_back('[');

                        if (indent >= 0)
                        {
                            dump_indentation_part(out, indent, separator, indent_level + 1);
                        }

                        if (v.l)
                        {
                            bool first = true;
                            for (auto& x : *v.l)
                            {
                                if (!first)
                                {
                                    out.push_back(',');

                                    if (indent >= 0)
                                    {
                                        dump_indentation_part(out, indent, separator, indent_level + 1);
                                    }
                                }
                                first = false;
                                dump_internal(x, out, indent, separator, indent_level + 1);
                            }
                        }

                        if (indent >= 0)
                        {
                            dump_indentation_part(out, indent, separator, indent_level);
                        }

                        out.push_back(']');
                    }
                    break;
                    case type::Object:
                    {
                        out.push_back('{');

                        if (indent >= 0)
                        {
                            dump_indentation_part(out, indent, separator, indent_level + 1);
                        }

                        if (v.o)
                        {
                            bool first = true;
                            for (auto& kv : *v.o)
                            {
                                if (!first)
                                {
                                    out.push_back(',');
                                    if (indent >= 0)
                                    {
                                        dump_indentation_part(out, indent, separator, indent_level + 1);
                                    }
                                }
                                first = false;
                                dump_string(kv.first, out);
                                out.push_back(':');

                                if (indent >= 0)
                                {
                                    out.push_back(' ');
                                }

                                dump_internal(kv.second, out, indent, separator, indent_level + 1);
                            }
                        }

                        if (indent >= 0)
                        {
                            dump_indentation_part(out, indent, separator, indent_level);
                        }

                        out.push_back('}');
                    }
                    break;

                    case type::Function:
                        out += "custom function";
                        break;
                }
            }

        public:
            std::string dump(const int indent, const char separator = ' ') const
            {
                std::string ret;
                ret.reserve(estimate_length());
                dump_internal(*this, ret, indent, separator);
                return ret;
            }

            std::string dump() const override
            {
                static constexpr int DontIndent = -1;

                return dump(DontIndent);
            }
        };

        // Used for accessing the internals of a wvalue
        struct wvalue_reader
        {
            int64_t get(int64_t fallback)
            {
                if (ref.t() != type::Number || ref.nt == num_type::Floating_point ||
                    ref.nt == num_type::Double_precision_floating_point)
                    return fallback;
                return ref.num.si;
            }

            double get(double fallback)
            {
                if (ref.t() != type::Number || ref.nt != num_type::Floating_point ||
                    ref.nt == num_type::Double_precision_floating_point)
                    return fallback;
                return ref.num.d;
            }

            bool get(bool fallback)
            {
                if (ref.t() == type::True) return true;
                if (ref.t() == type::False) return false;
                return fallback;
            }

            std::string get(const std::string& fallback)
            {
                if (ref.t() != type::String) return fallback;
                return ref.s;
            }

            const wvalue& ref;
        };

        //std::vector<asio::const_buffer> dump_ref(wvalue& v)
        //{
        //}
    } // namespace json
} // namespace crow

/**
 * \file crow/mustache.h
 * \brief This file includes the definition of the crow::mustache
 * namespace and its members.
 */

#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <functional>

namespace crow // NOTE: Already documented in "crow/app.h"
{
    /**
     * \namespace crow::mustache
     * \brief In this namespace is defined most of the functions and
     * classes related to template rendering.
     *
     * If you are here you might want to read these functions and
     * classes:
     *
     * - \ref template_t
     * - \ref load_text
     * - \ref load_text_unsafe
     * - \ref load
     * - \ref load_unsafe
     *
     * As name suggest, crow uses [mustache](https://en.wikipedia.org/wiki/Mustache_(template_system))
     * as main template rendering system.
     *
     * You may be interested in taking a look at the [Templating guide
     * page](https://crowcpp.org/master/guides/templating/).
     */
    namespace mustache
    {
        using context = json::wvalue;

        template_t load(const std::string& filename);

        /**
         * \class invalid_template_exception
         * \brief Represents compilation error of an template. Throwed
         * specially at mustache compile time.
         */
        class invalid_template_exception : public std::exception
        {
        public:
            invalid_template_exception(const std::string& msg_):
              msg("crow::mustache error: " + msg_)
            {}
            virtual const char* what() const throw() override
            {
                return msg.c_str();
            }
            std::string msg;
        };

        /**
         * \struct rendered_template
         * \brief Returned object after call the
         * \ref template_t::render() method. Its intended to be
         * returned during a **rule declaration**.
         *
         * \see \ref CROW_ROUTE
         * \see \ref CROW_BP_ROUTE
         */
        struct rendered_template : returnable
        {
            rendered_template():
              returnable("text/html") {}

            rendered_template(std::string& body):
              returnable("text/html"), body_(std::move(body)) {}

            std::string body_;

            std::string dump() const override
            {
                return body_;
            }
        };

        /**
         * \enum ActionType
         * \brief Used in \ref Action to represent different parsing
         * behaviors.
         *
         * \see \ref Action
         */
        enum class ActionType
        {
            Ignore,
            Tag,
            UnescapeTag,
            OpenBlock,
            CloseBlock,
            ElseBlock,
            Partial,
        };

        /**
         * \struct Action
         * \brief Used during mustache template compilation to
         * represent parsing actions.
         *
         * \see \ref compile
         * \see \ref template_t
         */
        struct Action
        {
            bool has_end_match;
            char tag_char;
            int start;
            int end;
            int pos;
            ActionType t;

            Action(char tag_char_, ActionType t_, size_t start_, size_t end_, size_t pos_ = 0):
              has_end_match(false), tag_char(tag_char_), start(static_cast<int>(start_)), end(static_cast<int>(end_)), pos(static_cast<int>(pos_)), t(t_)
            {
            }

            bool missing_end_pair() const {
                switch (t)
                {
                    case ActionType::Ignore:
                    case ActionType::Tag:
                    case ActionType::UnescapeTag:
                    case ActionType::CloseBlock:
                    case ActionType::Partial:
                        return false;

                    // requires a match
                    case ActionType::OpenBlock:
                    case ActionType::ElseBlock:
                        return !has_end_match;

                    default:
                        throw std::logic_error("invalid type");
                }
            }
        };

        /**
         * \class template_t
         * \brief Compiled mustache template object.
         *
         * \warning Use \ref compile instead.
         */
        class template_t
        {
        public:
            template_t(std::string body):
              body_(std::move(body))
            {
                // {{ {{# {{/ {{^ {{! {{> {{=
                parse();
            }

        private:
            std::string tag_name(const Action& action) const
            {
                return body_.substr(action.start, action.end - action.start);
            }
            auto find_context(const std::string& name, const std::vector<const context*>& stack, bool shouldUseOnlyFirstStackValue = false) const -> std::pair<bool, const context&>
            {
                if (name == ".")
                {
                    return {true, *stack.back()};
                }
                static json::wvalue empty_str;
                empty_str = "";

                int dotPosition = name.find(".");
                if (dotPosition == static_cast<int>(name.npos))
                {
                    for (auto it = stack.rbegin(); it != stack.rend(); ++it)
                    {
                        if ((*it)->t() == json::type::Object)
                        {
                            if ((*it)->count(name))
                                return {true, (**it)[name]};
                        }
                    }
                }
                else
                {
                    std::vector<int> dotPositions;
                    dotPositions.push_back(-1);
                    while (dotPosition != static_cast<int>(name.npos))
                    {
                        dotPositions.push_back(dotPosition);
                        dotPosition = name.find(".", dotPosition + 1);
                    }
                    dotPositions.push_back(name.size());
                    std::vector<std::string> names;
                    names.reserve(dotPositions.size() - 1);
                    for (int i = 1; i < static_cast<int>(dotPositions.size()); i++)
                        names.emplace_back(name.substr(dotPositions[i - 1] + 1, dotPositions[i] - dotPositions[i - 1] - 1));

                    for (auto it = stack.rbegin(); it != stack.rend(); ++it)
                    {
                        const context* view = *it;
                        bool found = true;
                        for (auto jt = names.begin(); jt != names.end(); ++jt)
                        {
                            if (view->t() == json::type::Object &&
                                view->count(*jt))
                            {
                                view = &(*view)[*jt];
                            }
                            else
                            {
                                if (shouldUseOnlyFirstStackValue)
                                {
                                    return {false, empty_str};
                                }
                                found = false;
                                break;
                            }
                        }
                        if (found)
                            return {true, *view};
                    }
                }

                return {false, empty_str};
            }

            void escape(const std::string& in, std::string& out) const
            {
                out.reserve(out.size() + in.size());
                for (auto it = in.begin(); it != in.end(); ++it)
                {
                    switch (*it)
                    {
                        case '&': out += "&amp;"; break;
                        case '<': out += "&lt;"; break;
                        case '>': out += "&gt;"; break;
                        case '"': out += "&quot;"; break;
                        case '\'': out += "&#39;"; break;
                        case '/': out += "&#x2F;"; break;
                        case '`': out += "&#x60;"; break;
                        case '=': out += "&#x3D;"; break;
                        default: out += *it; break;
                    }
                }
            }

            bool isTagInsideObjectBlock(const int& current, const std::vector<const context*>& stack) const
            {
                int openedBlock = 0;
                for (int i = current; i > 0; --i)
                {
                    auto& action = actions_[i - 1];

                    if (action.t == ActionType::OpenBlock)
                    {
                        if (openedBlock == 0 && (*stack.rbegin())->t() == json::type::Object)
                        {
                            return true;
                        }
                        --openedBlock;
                    }
                    else if (action.t == ActionType::CloseBlock)
                    {
                        ++openedBlock;
                    }
                }

                return false;
            }

            void render_internal(int actionBegin, int actionEnd, std::vector<const context*>& stack, std::string& out, int indent) const
            {
                int current = actionBegin;

                if (indent)
                    out.insert(out.size(), indent, ' ');

                while (current < actionEnd)
                {
                    auto& fragment = fragments_[current];
                    auto& action = actions_[current];
                    render_fragment(fragment, indent, out);
                    switch (action.t)
                    {
                        case ActionType::Ignore:
                            // do nothing
                            break;
                        case ActionType::Partial:
                        {
                            std::string partial_name = tag_name(action);
                            auto partial_templ = load(partial_name);
                            int partial_indent = action.pos;
                            partial_templ.render_internal(0, partial_templ.fragments_.size() - 1, stack, out, partial_indent ? indent + partial_indent : 0);
                        }
                        break;
                        case ActionType::UnescapeTag:
                        case ActionType::Tag:
                        {
                            bool shouldUseOnlyFirstStackValue = false;
                            if (isTagInsideObjectBlock(current, stack))
                            {
                                shouldUseOnlyFirstStackValue = true;
                            }
                            auto optional_ctx = find_context(tag_name(action), stack, shouldUseOnlyFirstStackValue);
                            auto& ctx = optional_ctx.second;
                            switch (ctx.t())
                            {
                                case json::type::False:
                                case json::type::True:
                                case json::type::Number:
                                    out += ctx.dump();
                                    break;
                                case json::type::String:
                                    if (action.t == ActionType::Tag)
                                        escape(ctx.s, out);
                                    else
                                        out += ctx.s;
                                    break;
                                case json::type::Function:
                                {
                                    std::string execute_result = ctx.execute();
                                    while (execute_result.find("{{") != std::string::npos)
                                    {
                                        template_t result_plug(execute_result);
                                        execute_result = result_plug.render_string(*(stack[0]));
                                    }

                                    if (action.t == ActionType::Tag)
                                        escape(execute_result, out);
                                    else
                                        out += execute_result;
                                }
                                break;
                                default:
                                    throw std::runtime_error("not implemented tag type" + utility::lexical_cast<std::string>(static_cast<int>(ctx.t())));
                            }
                        }
                        break;
                        case ActionType::ElseBlock:
                        {
                            static context nullContext;
                            auto optional_ctx = find_context(tag_name(action), stack);
                            if (!optional_ctx.first)
                            {
                                stack.emplace_back(&nullContext);
                                break;
                            }

                            auto& ctx = optional_ctx.second;
                            switch (ctx.t())
                            {
                                case json::type::List:
                                    if (ctx.l && !ctx.l->empty())
                                        current = action.pos;
                                    else
                                        stack.emplace_back(&nullContext);
                                    break;
                                case json::type::False:
                                case json::type::Null:
                                    stack.emplace_back(&nullContext);
                                    break;
                                default:
                                    current = action.pos;
                                    break;
                            }
                            break;
                        }
                        case ActionType::OpenBlock:
                        {
                            auto optional_ctx = find_context(tag_name(action), stack);
                            if (!optional_ctx.first)
                            {
                                current = action.pos;
                                break;
                            }

                            auto& ctx = optional_ctx.second;
                            switch (ctx.t())
                            {
                                case json::type::List:
                                    if (ctx.l)
                                        for (auto it = ctx.l->begin(); it != ctx.l->end(); ++it)
                                        {
                                            stack.push_back(&*it);
                                            render_internal(current + 1, action.pos, stack, out, indent);
                                            stack.pop_back();
                                        }
                                    current = action.pos;
                                    break;
                                case json::type::Number:
                                case json::type::String:
                                case json::type::Object:
                                case json::type::True:
                                    stack.push_back(&ctx);
                                    break;
                                case json::type::False:
                                case json::type::Null:
                                    current = action.pos;
                                    break;
                                default:
                                    throw std::runtime_error("{{#: not implemented context type: " + utility::lexical_cast<std::string>(static_cast<int>(ctx.t())));
                                    break;
                            }
                            break;
                        }
                        case ActionType::CloseBlock:
                            stack.pop_back();
                            break;
                        default:
                            throw std::runtime_error("not implemented " + utility::lexical_cast<std::string>(static_cast<int>(action.t)));
                    }
                    current++;
                }
                auto& fragment = fragments_[actionEnd];
                render_fragment(fragment, indent, out);
            }
            void render_fragment(const std::pair<int, int> fragment, int indent, std::string& out) const
            {
                if (indent)
                {
                    for (int i = fragment.first; i < fragment.second; i++)
                    {
                        out += body_[i];
                        if (body_[i] == '\n' && i + 1 != static_cast<int>(body_.size()))
                            out.insert(out.size(), indent, ' ');
                    }
                }
                else
                    out.insert(out.size(), body_, fragment.first, fragment.second - fragment.first);
            }

        public:
            /// Output a returnable template from this mustache template
            rendered_template render() const
            {
                context empty_ctx;
                std::vector<const context*> stack;
                stack.emplace_back(&empty_ctx);

                std::string ret;
                render_internal(0, fragments_.size() - 1, stack, ret, 0);
                return rendered_template(ret);
            }

            /// Apply the values from the context provided and output a returnable template from this mustache template
            rendered_template render(const context& ctx) const
            {
                std::vector<const context*> stack;
                stack.emplace_back(&ctx);

                std::string ret;
                render_internal(0, fragments_.size() - 1, stack, ret, 0);
                return rendered_template(ret);
            }

            /// Apply the values from the context provided and output a returnable template from this mustache template
            rendered_template render(const context&& ctx) const
            {
                return render(ctx);
            }

            /// Output a returnable template from this mustache template
            std::string render_string() const
            {
                context empty_ctx;
                std::vector<const context*> stack;
                stack.emplace_back(&empty_ctx);

                std::string ret;
                render_internal(0, fragments_.size() - 1, stack, ret, 0);
                return ret;
            }

            /// Apply the values from the context provided and output a returnable template from this mustache template
            std::string render_string(const context& ctx) const
            {
                std::vector<const context*> stack;
                stack.emplace_back(&ctx);

                std::string ret;
                render_internal(0, fragments_.size() - 1, stack, ret, 0);
                return ret;
            }

        private:
            void parse()
            {
                std::string tag_open = "{{";
                std::string tag_close = "}}";

                std::vector<int> blockPositions;

                size_t current = 0;
                while (1)
                {
                    size_t idx = body_.find(tag_open, current);
                    if (idx == body_.npos)
                    {
                        fragments_.emplace_back(static_cast<int>(current), static_cast<int>(body_.size()));
                        actions_.emplace_back('!', ActionType::Ignore, 0, 0);
                        break;
                    }
                    fragments_.emplace_back(static_cast<int>(current), static_cast<int>(idx));

                    idx += tag_open.size();
                    size_t endIdx = body_.find(tag_close, idx);
                    if (endIdx == idx)
                    {
                        throw invalid_template_exception("empty tag is not allowed");
                    }
                    if (endIdx == body_.npos)
                    {
                        // error, no matching tag
                        throw invalid_template_exception("not matched opening tag");
                    }
                    current = endIdx + tag_close.size();
                    char tag_char = body_[idx];
                    switch (tag_char)
                    {
                        case '#':
                            idx++;
                            while (body_[idx] == ' ')
                                idx++;
                            while (body_[endIdx - 1] == ' ')
                                endIdx--;
                            blockPositions.emplace_back(static_cast<int>(actions_.size()));
                            actions_.emplace_back(tag_char, ActionType::OpenBlock, idx, endIdx);
                            break;
                        case '/':
                            idx++;
                            while (body_[idx] == ' ')
                                idx++;
                            while (body_[endIdx - 1] == ' ')
                                endIdx--;
                            {
                                if (blockPositions.empty())
                                {
                                    throw invalid_template_exception(
                                             std::string("unexpected closing tag: ")
                                             + body_.substr(idx, endIdx - idx)
                                             );
                                }
                                auto& matched = actions_[blockPositions.back()];
                                if (body_.compare(idx, endIdx - idx,
                                                  body_, matched.start, matched.end - matched.start) != 0)
                                {
                                     throw invalid_template_exception(
                                             std::string("not matched {{")
                                             + matched.tag_char
                                             + "{{/ pair: "
                                             + body_.substr(matched.start, matched.end - matched.start) + ", "
                                             + body_.substr(idx, endIdx - idx)
                                             );
                                }
                                matched.pos = static_cast<int>(actions_.size());
                                matched.has_end_match = true;
                            }
                            actions_.emplace_back(tag_char, ActionType::CloseBlock, idx, endIdx, blockPositions.back());
                            blockPositions.pop_back();
                            break;
                        case '^':
                            idx++;
                            while (body_[idx] == ' ')
                                idx++;
                            while (body_[endIdx - 1] == ' ')
                                endIdx--;
                            blockPositions.emplace_back(static_cast<int>(actions_.size()));
                            actions_.emplace_back(tag_char, ActionType::ElseBlock, idx, endIdx);
                            break;
                        case '!':
                            // do nothing action
                            actions_.emplace_back(tag_char, ActionType::Ignore, idx + 1, endIdx);
                            break;
                        case '>': // partial
                            idx++;
                            while (body_[idx] == ' ')
                                idx++;
                            while (body_[endIdx - 1] == ' ')
                                endIdx--;
                            actions_.emplace_back(tag_char, ActionType::Partial, idx, endIdx);
                            break;
                        case '{':
                            if (tag_open != "{{" || tag_close != "}}")
                                throw invalid_template_exception("cannot use triple mustache when delimiter changed");

                            idx++;
                            if (body_[endIdx + 2] != '}')
                            {
                                throw invalid_template_exception("{{{: }}} not matched");
                            }
                            while (body_[idx] == ' ')
                                idx++;
                            while (body_[endIdx - 1] == ' ')
                                endIdx--;
                            actions_.emplace_back(tag_char, ActionType::UnescapeTag, idx, endIdx);
                            current++;
                            break;
                        case '&':
                            idx++;
                            while (body_[idx] == ' ')
                                idx++;
                            while (body_[endIdx - 1] == ' ')
                                endIdx--;
                            actions_.emplace_back(tag_char, ActionType::UnescapeTag, idx, endIdx);
                            break;
                        case '=':
                            // tag itself is no-op
                            idx++;
                            actions_.emplace_back(tag_char, ActionType::Ignore, idx, endIdx);
                            endIdx--;
                            if (body_[endIdx] != '=')
                                throw invalid_template_exception("{{=: not matching = tag: " + body_.substr(idx, endIdx - idx));
                            endIdx--;
                            while (body_[idx] == ' ')
                                idx++;
                            while (body_[endIdx] == ' ')
                                endIdx--;
                            endIdx++;
                            {
                                bool succeeded = false;
                                for (size_t i = idx; i < endIdx; i++)
                                {
                                    if (body_[i] == ' ')
                                    {
                                        tag_open = body_.substr(idx, i - idx);
                                        while (body_[i] == ' ')
                                            i++;
                                        tag_close = body_.substr(i, endIdx - i);
                                        if (tag_open.empty())
                                            throw invalid_template_exception("{{=: empty open tag");
                                        if (tag_close.empty())
                                            throw invalid_template_exception("{{=: empty close tag");

                                        if (tag_close.find(" ") != tag_close.npos)
                                            throw invalid_template_exception("{{=: invalid open/close tag: " + tag_open + " " + tag_close);
                                        succeeded = true;
                                        break;
                                    }
                                }
                                if (!succeeded)
                                    throw invalid_template_exception("{{=: cannot find space between new open/close tags");
                            }
                            break;
                        default:
                            // normal tag case;
                            while (body_[idx] == ' ')
                                idx++;
                            while (body_[endIdx - 1] == ' ')
                                endIdx--;
                            actions_.emplace_back(tag_char, ActionType::Tag, idx, endIdx);
                            break;
                    }
                }

                // ensure no unmatched tags
                for (int i = 0; i < static_cast<int>(actions_.size()); i++)
                {
                    if (actions_[i].missing_end_pair())
                    {
                        throw invalid_template_exception(
                                std::string("open tag has no matching end tag {{")
                                + actions_[i].tag_char
                                + " {{/ pair: "
                                + body_.substr(actions_[i].start, actions_[i].end - actions_[i].start)
                                );
                    }
                }

                // removing standalones
                for (int i = static_cast<int>(actions_.size()) - 2; i >= 0; i--)
                {
                    if (actions_[i].t == ActionType::Tag || actions_[i].t == ActionType::UnescapeTag)
                        continue;
                    auto& fragment_before = fragments_[i];
                    auto& fragment_after = fragments_[i + 1];
                    bool is_last_action = i == static_cast<int>(actions_.size()) - 2;
                    bool all_space_before = true;
                    int j, k;
                    for (j = fragment_before.second - 1; j >= fragment_before.first; j--)
                    {
                        if (body_[j] != ' ')
                        {
                            all_space_before = false;
                            break;
                        }
                    }
                    if (all_space_before && i > 0)
                        continue;
                    if (!all_space_before && body_[j] != '\n')
                        continue;
                    bool all_space_after = true;
                    for (k = fragment_after.first; k < static_cast<int>(body_.size()) && k < fragment_after.second; k++)
                    {
                        if (body_[k] != ' ')
                        {
                            all_space_after = false;
                            break;
                        }
                    }
                    if (all_space_after && !is_last_action)
                        continue;
                    if (!all_space_after &&
                        !(
                          body_[k] == '\n' ||
                          (body_[k] == '\r' &&
                           k + 1 < static_cast<int>(body_.size()) &&
                           body_[k + 1] == '\n')))
                        continue;
                    if (actions_[i].t == ActionType::Partial)
                    {
                        actions_[i].pos = fragment_before.second - j - 1;
                    }
                    fragment_before.second = j + 1;
                    if (!all_space_after)
                    {
                        if (body_[k] == '\n')
                            k++;
                        else
                            k += 2;
                        fragment_after.first = k;
                    }
                }
            }

            std::vector<std::pair<int, int>> fragments_;
            std::vector<Action> actions_;
            std::string body_;
        };

        /// \brief The function that compiles a source into a mustache
        /// template.
        inline template_t compile(const std::string& body)
        {
            return template_t(body);
        }

        namespace detail
        {
            inline std::string& get_template_base_directory_ref()
            {
                static std::string template_base_directory = "templates";
                return template_base_directory;
            }

            /// A base directory not related to any blueprint
            inline std::string& get_global_template_base_directory_ref()
            {
                static std::string template_base_directory = "templates";
                return template_base_directory;
            }
        } // namespace detail

        /// \brief The default way that \ref load, \ref load_unsafe,
        /// \ref load_text and \ref load_text_unsafe use to read the
        /// contents of a file.
        inline std::string default_loader(const std::string& filename)
        {
            std::string path = detail::get_template_base_directory_ref();
            std::ifstream inf(utility::join_path(path, filename));
            if (!inf)
            {
                CROW_LOG_WARNING << "Template \"" << filename << "\" not found.";
                return {};
            }
            return {std::istreambuf_iterator<char>(inf), std::istreambuf_iterator<char>()};
        }

        namespace detail
        {
            inline std::function<std::string(std::string)>& get_loader_ref()
            {
                static std::function<std::string(std::string)> loader = default_loader;
                return loader;
            }
        } // namespace detail

        /// \brief Defines the templates directory path at **route
        /// level**. By default is `templates/`.
        inline void set_base(const std::string& path)
        {
            auto& base = detail::get_template_base_directory_ref();
            base = path;
            if (base.back() != '\\' &&
                base.back() != '/')
            {
                base += '/';
            }
        }

        /// \brief Defines the templates directory path at **global
        /// level**. By default is `templates/`.
        inline void set_global_base(const std::string& path)
        {
            auto& base = detail::get_global_template_base_directory_ref();
            base = path;
            if (base.back() != '\\' &&
                base.back() != '/')
            {
                base += '/';
            }
        }

        /// \brief Change the way that \ref load, \ref load_unsafe,
        /// \ref load_text and \ref load_text_unsafe reads a file.
        ///
        /// By default, the previously mentioned functions load files
        /// using \ref default_loader, that only reads a file and
        /// returns a std::string.
        inline void set_loader(std::function<std::string(std::string)> loader)
        {
            detail::get_loader_ref() = std::move(loader);
        }

        /// \brief Open, read and sanitize a file but returns a
        /// std::string without a previous rendering process.
        ///
        /// Except for the **sanitize process** this function does the
        /// almost the same thing that \ref load_text_unsafe.
        inline std::string load_text(const std::string& filename)
        {
            std::string filename_sanitized(filename);
            utility::sanitize_filename(filename_sanitized);
            return detail::get_loader_ref()(filename_sanitized);
        }

        /// \brief Open and read a file but returns a std::string
        /// without a previous rendering process.
        ///
        /// This function is more like a helper to reduce code like
        /// this...
        ///
        /// ```cpp
        /// std::ifstream file("home.html");
        /// return std::string({std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()});
        /// ```
        ///
        /// ... Into this...
        ///
        /// ```cpp
        /// return load("home.html");
        /// ```
        ///
        /// \warning Usually \ref load_text is more recommended to use
        /// instead because it may prevent some [XSS Attacks](https://en.wikipedia.org/wiki/Cross-site_scripting).
        /// **Never blindly trust your users!**
        inline std::string load_text_unsafe(const std::string& filename)
        {
            return detail::get_loader_ref()(filename);
        }

        /// \brief Open, read and renders a file using a mustache
        /// compiler. It also sanitize the input before compilation.
        inline template_t load(const std::string& filename)
        {
            std::string filename_sanitized(filename);
            utility::sanitize_filename(filename_sanitized);
            return compile(detail::get_loader_ref()(filename_sanitized));
        }

        /// \brief Open, read and renders a file using a mustache
        /// compiler. But it **do not** sanitize the input before
        /// compilation.
        ///
        /// \warning Usually \ref load is more recommended to use
        /// instead because it may prevent some [XSS Attacks](https://en.wikipedia.org/wiki/Cross-site_scripting).
        /// **Never blindly trust your users!**
        inline template_t load_unsafe(const std::string& filename)
        {
            return compile(detail::get_loader_ref()(filename));
        }
    } // namespace mustache
} // namespace crow

#include <stdexcept>

namespace crow
{
    struct bad_request : public std::runtime_error
    {
        bad_request(const std::string& what_arg)
            : std::runtime_error(what_arg) {}

        bad_request(const char* what_arg)
            : std::runtime_error(what_arg) {}
    };
}
#ifdef CROW_ENABLE_COMPRESSION

#include <string>
#include <zlib.h>

// http://zlib.net/manual.html
namespace crow // NOTE: Already documented in "crow/app.h"
{
    namespace compression
    {
        // Values used in the 'windowBits' parameter for deflateInit2.
        enum algorithm
        {
            // 15 is the default value for deflate
            DEFLATE = 15,
            // windowBits can also be greater than 15 for optional gzip encoding.
            // Add 16 to windowBits to write a simple gzip header and trailer around the compressed data instead of a zlib wrapper.
            GZIP = 15 | 16,
        };

        inline std::string compress_string(std::string const& str, algorithm algo)
        {
            std::string compressed_str;
            z_stream stream{};
            // Initialize with the default values
            if (::deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, algo, 8, Z_DEFAULT_STRATEGY) == Z_OK)
            {
                char buffer[8192];

                stream.avail_in = str.size();
                // zlib does not take a const pointer. The data is not altered.
                stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(str.c_str()));

                int code = Z_OK;
                do
                {
                    stream.avail_out = sizeof(buffer);
                    stream.next_out = reinterpret_cast<Bytef*>(&buffer[0]);

                    code = ::deflate(&stream, Z_FINISH);
                    // Successful and non-fatal error code returned by deflate when used with Z_FINISH flush
                    if (code == Z_OK || code == Z_STREAM_END)
                    {
                        std::copy(&buffer[0], &buffer[sizeof(buffer) - stream.avail_out], std::back_inserter(compressed_str));
                    }

                } while (code == Z_OK);

                if (code != Z_STREAM_END)
                    compressed_str.clear();

                ::deflateEnd(&stream);
            }

            return compressed_str;
        }

        inline std::string decompress_string(std::string const& deflated_string)
        {
            std::string inflated_string;
            Bytef tmp[8192];

            z_stream zstream{};
            zstream.avail_in = deflated_string.size();
            // Nasty const_cast but zlib won't alter its contents
            zstream.next_in = const_cast<Bytef*>(reinterpret_cast<Bytef const*>(deflated_string.c_str()));
            // Initialize with automatic header detection, for gzip support
            if (::inflateInit2(&zstream, MAX_WBITS | 32) == Z_OK)
            {
                do
                {
                    zstream.avail_out = sizeof(tmp);
                    zstream.next_out = &tmp[0];

                    auto ret = ::inflate(&zstream, Z_NO_FLUSH);
                    if (ret == Z_OK || ret == Z_STREAM_END)
                    {
                        std::copy(&tmp[0], &tmp[sizeof(tmp) - zstream.avail_out], std::back_inserter(inflated_string));
                    }
                    else
                    {
                        // Something went wrong with inflate; make sure we return an empty string
                        inflated_string.clear();
                        break;
                    }

                } while (zstream.avail_out == 0);

                // Free zlib's internal memory
                ::inflateEnd(&zstream);
            }

            return inflated_string;
        }
    } // namespace compression
} // namespace crow

#endif


#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>

namespace crow
{
    const char cr = '\r';
    const char lf = '\n';
    const std::string crlf("\r\n");

    enum class HTTPMethod : char
    {
#ifndef DELETE
        DELETE = 0,
        GET,
        HEAD,
        POST,
        PUT,

        CONNECT,
        OPTIONS,
        TRACE,

        PATCH,
        PURGE,

        COPY,
        LOCK,
        MKCOL,
        MOVE,
        PROPFIND,
        PROPPATCH,
        SEARCH,
        UNLOCK,
        BIND,
        REBIND,
        UNBIND,
        ACL,

        REPORT,
        MKACTIVITY,
        CHECKOUT,
        MERGE,

        MSEARCH,
        NOTIFY,
        SUBSCRIBE,
        UNSUBSCRIBE,

        MKCALENDAR,

        LINK,
        UNLINK,

        SOURCE,
#endif

        Delete = 0,
        Get,
        Head,
        Post,
        Put,

        Connect,
        Options,
        Trace,

        Patch,
        Purge,

        Copy,
        Lock,
        MkCol,
        Move,
        Propfind,
        Proppatch,
        Search,
        Unlock,
        Bind,
        Rebind,
        Unbind,
        Acl,

        Report,
        MkActivity,
        Checkout,
        Merge,

        MSearch,
        Notify,
        Subscribe,
        Unsubscribe,

        MkCalendar,

        Link,
        Unlink,

        Source,


        InternalMethodCount,
        // should not add an item below this line: used for array count
    };

    constexpr const char* method_strings[] =
      {
        "DELETE",
        "GET",
        "HEAD",
        "POST",
        "PUT",

        "CONNECT",
        "OPTIONS",
        "TRACE",

        "PATCH",
        "PURGE",

        "COPY",
        "LOCK",
        "MKCOL",
        "MOVE",
        "PROPFIND",
        "PROPPATCH",
        "SEARCH",
        "UNLOCK",
        "BIND",
        "REBIND",
        "UNBIND",
        "ACL",

        "REPORT",
        "MKACTIVITY",
        "CHECKOUT",
        "MERGE",

        "M-SEARCH",
        "NOTIFY",
        "SUBSCRIBE",
        "UNSUBSCRIBE",

        "MKCALENDAR",

        "LINK",
        "UNLINK",

        "SOURCE"};


    inline std::string method_name(HTTPMethod method)
    {
        if (CROW_LIKELY(method < HTTPMethod::InternalMethodCount))
        {
            return method_strings[static_cast<unsigned int>(method)];
        }
        return "invalid";
    }

    // clang-format off

    enum status
    {
        CONTINUE                      = 100,
        SWITCHING_PROTOCOLS           = 101,

        OK                            = 200,
        CREATED                       = 201,
        ACCEPTED                      = 202,
        NON_AUTHORITATIVE_INFORMATION = 203,
        NO_CONTENT                    = 204,
        RESET_CONTENT                 = 205,
        PARTIAL_CONTENT               = 206,

        MULTIPLE_CHOICES              = 300,
        MOVED_PERMANENTLY             = 301,
        FOUND                         = 302,
        SEE_OTHER                     = 303,
        NOT_MODIFIED                  = 304,
        TEMPORARY_REDIRECT            = 307,
        PERMANENT_REDIRECT            = 308,

        BAD_REQUEST                   = 400,
        UNAUTHORIZED                  = 401,
        FORBIDDEN                     = 403,
        NOT_FOUND                     = 404,
        METHOD_NOT_ALLOWED            = 405,
        NOT_ACCEPTABLE                = 406,
        PROXY_AUTHENTICATION_REQUIRED = 407,
        CONFLICT                      = 409,
        GONE                          = 410,
        PAYLOAD_TOO_LARGE             = 413,
        UNSUPPORTED_MEDIA_TYPE        = 415,
        RANGE_NOT_SATISFIABLE         = 416,
        EXPECTATION_FAILED            = 417,
        PRECONDITION_REQUIRED         = 428,
        TOO_MANY_REQUESTS             = 429,
        UNAVAILABLE_FOR_LEGAL_REASONS = 451,

        INTERNAL_SERVER_ERROR         = 500,
        NOT_IMPLEMENTED               = 501,
        BAD_GATEWAY                   = 502,
        SERVICE_UNAVAILABLE           = 503,
        GATEWAY_TIMEOUT               = 504,
        VARIANT_ALSO_NEGOTIATES       = 506
    };

    // clang-format on

    enum class ParamType : char
    {
        INT,
        UINT,
        DOUBLE,
        STRING,
        PATH,

        MAX
    };

    /// @cond SKIP
    struct routing_params
    {
        std::vector<int64_t> int_params;
        std::vector<uint64_t> uint_params;
        std::vector<double> double_params;
        std::vector<std::string> string_params;

        void debug_print() const
        {
            std::cerr << "routing_params" << std::endl;
            for (auto i : int_params)
                std::cerr << i << ", ";
            std::cerr << std::endl;
            for (auto i : uint_params)
                std::cerr << i << ", ";
            std::cerr << std::endl;
            for (auto i : double_params)
                std::cerr << i << ", ";
            std::cerr << std::endl;
            for (auto& i : string_params)
                std::cerr << i << ", ";
            std::cerr << std::endl;
        }

        template<typename T>
        T get(unsigned) const;
    };

    template<>
    inline int64_t routing_params::get<int64_t>(unsigned index) const
    {
        return int_params[index];
    }

    template<>
    inline uint64_t routing_params::get<uint64_t>(unsigned index) const
    {
        return uint_params[index];
    }

    template<>
    inline double routing_params::get<double>(unsigned index) const
    {
        return double_params[index];
    }

    template<>
    inline std::string routing_params::get<std::string>(unsigned index) const
    {
        return string_params[index];
    }
    /// @endcond

    struct routing_handle_result
    {
        uint16_t rule_index;
        std::vector<uint16_t> blueprint_indices;
        routing_params r_params;
        HTTPMethod method;

        routing_handle_result() {}

        routing_handle_result(uint16_t rule_index_, std::vector<uint16_t> blueprint_indices_, routing_params r_params_):
          rule_index(rule_index_),
          blueprint_indices(blueprint_indices_),
          r_params(r_params_) {}

        routing_handle_result(uint16_t rule_index_, std::vector<uint16_t> blueprint_indices_, routing_params r_params_, HTTPMethod method_):
          rule_index(rule_index_),
          blueprint_indices(blueprint_indices_),
          r_params(r_params_),
          method(method_) {}
    };
} // namespace crow

// clang-format off
#ifndef CROW_MSVC_WORKAROUND
constexpr crow::HTTPMethod method_from_string(const char* str)
{
    return crow::black_magic::is_equ_p(str, "GET", 3)    ? crow::HTTPMethod::Get :
           crow::black_magic::is_equ_p(str, "DELETE", 6) ? crow::HTTPMethod::Delete :
           crow::black_magic::is_equ_p(str, "HEAD", 4)   ? crow::HTTPMethod::Head :
           crow::black_magic::is_equ_p(str, "POST", 4)   ? crow::HTTPMethod::Post :
           crow::black_magic::is_equ_p(str, "PUT", 3)    ? crow::HTTPMethod::Put :

           crow::black_magic::is_equ_p(str, "OPTIONS", 7) ? crow::HTTPMethod::Options :
           crow::black_magic::is_equ_p(str, "CONNECT", 7) ? crow::HTTPMethod::Connect :
           crow::black_magic::is_equ_p(str, "TRACE", 5)   ? crow::HTTPMethod::Trace :

           crow::black_magic::is_equ_p(str, "PATCH", 5)     ? crow::HTTPMethod::Patch :
           crow::black_magic::is_equ_p(str, "PURGE", 5)     ? crow::HTTPMethod::Purge :
           crow::black_magic::is_equ_p(str, "COPY", 4)      ? crow::HTTPMethod::Copy :
           crow::black_magic::is_equ_p(str, "LOCK", 4)      ? crow::HTTPMethod::Lock :
           crow::black_magic::is_equ_p(str, "MKCOL", 5)     ? crow::HTTPMethod::MkCol :
           crow::black_magic::is_equ_p(str, "MOVE", 4)      ? crow::HTTPMethod::Move :
           crow::black_magic::is_equ_p(str, "PROPFIND", 8)  ? crow::HTTPMethod::Propfind :
           crow::black_magic::is_equ_p(str, "PROPPATCH", 9) ? crow::HTTPMethod::Proppatch :
           crow::black_magic::is_equ_p(str, "SEARCH", 6)    ? crow::HTTPMethod::Search :
           crow::black_magic::is_equ_p(str, "UNLOCK", 6)    ? crow::HTTPMethod::Unlock :
           crow::black_magic::is_equ_p(str, "BIND", 4)      ? crow::HTTPMethod::Bind :
           crow::black_magic::is_equ_p(str, "REBIND", 6)    ? crow::HTTPMethod::Rebind :
           crow::black_magic::is_equ_p(str, "UNBIND", 6)    ? crow::HTTPMethod::Unbind :
           crow::black_magic::is_equ_p(str, "ACL", 3)       ? crow::HTTPMethod::Acl :

           crow::black_magic::is_equ_p(str, "REPORT", 6)      ? crow::HTTPMethod::Report :
           crow::black_magic::is_equ_p(str, "MKACTIVITY", 10) ? crow::HTTPMethod::MkActivity :
           crow::black_magic::is_equ_p(str, "CHECKOUT", 8)    ? crow::HTTPMethod::Checkout :
           crow::black_magic::is_equ_p(str, "MERGE", 5)       ? crow::HTTPMethod::Merge :

           crow::black_magic::is_equ_p(str, "MSEARCH", 7)      ? crow::HTTPMethod::MSearch :
           crow::black_magic::is_equ_p(str, "NOTIFY", 6)       ? crow::HTTPMethod::Notify :
           crow::black_magic::is_equ_p(str, "SUBSCRIBE", 9)    ? crow::HTTPMethod::Subscribe :
           crow::black_magic::is_equ_p(str, "UNSUBSCRIBE", 11) ? crow::HTTPMethod::Unsubscribe :

           crow::black_magic::is_equ_p(str, "MKCALENDAR", 10) ? crow::HTTPMethod::MkCalendar :

           crow::black_magic::is_equ_p(str, "LINK", 4)   ? crow::HTTPMethod::Link :
           crow::black_magic::is_equ_p(str, "UNLINK", 6) ? crow::HTTPMethod::Unlink :

           crow::black_magic::is_equ_p(str, "SOURCE", 6) ? crow::HTTPMethod::Source :
                                                           throw std::runtime_error("invalid http method");
}

constexpr crow::HTTPMethod operator""_method(const char* str, size_t /*len*/)
{
    return method_from_string( str );
}
#endif
// clang-format on

/* merged revision: 5b951d74bd66ec9d38448e0a85b1cf8b85d97db3 */
/* updated to     : e13b274770da9b82a1085dec29182acfea72e7a7 (beyond v2.9.5) */
/* commits not included:
 * 091ebb87783a58b249062540bbea07de2a11e9cf
 * 6132d1fefa03f769a3979355d1f5da0b8889cad2
 * 7ba312397c2a6c851a4b5efe6c1603b1e1bda6ff
 * d7675453a6c03180572f084e95eea0d02df39164
 * dff604db203986e532e5a679bafd0e7382c6bdd9 (Might be useful to actually add [upgrade requests with a body])
 * e01811e7f4894d7f0f7f4bd8492cccec6f6b4038 (related to above)
 * 05525c5fde1fc562481f6ae08fa7056185325daf (also related to above)
 * 350258965909f249f9c59823aac240313e0d0120 (cannot be implemented due to upgrade)
 */

// clang-format off
extern "C" {
#include <stddef.h>
#if defined(_WIN32) && !defined(__MINGW32__) && \
  (!defined(_MSC_VER) || _MSC_VER<1600) && !defined(__WINE__)
#include <BaseTsd.h>
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#elif (defined(__sun) || defined(__sun__)) && defined(__SunOS_5_9)
#include <sys/inttypes.h>
#else
#include <stdint.h>
#endif
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
}

namespace crow
{
/* Maximium header size allowed. If the macro is not defined
 * before including this header then the default is used. To
 * change the maximum header size, define the macro in the build
 * environment (e.g. -DHTTP_MAX_HEADER_SIZE=<value>). To remove
 * the effective limit on the size of the header, define the macro
 * to a very large number (e.g. -DCROW_HTTP_MAX_HEADER_SIZE=0x7fffffff)
 */
#ifndef CROW_HTTP_MAX_HEADER_SIZE
# define CROW_HTTP_MAX_HEADER_SIZE (80*1024)
#endif

typedef struct http_parser http_parser;
typedef struct http_parser_settings http_parser_settings;

/* Callbacks should return non-zero to indicate an error. The parser will
 * then halt execution.
 *
 * The one exception is on_headers_complete. In a HTTP_RESPONSE parser
 * returning '1' from on_headers_complete will tell the parser that it
 * should not expect a body. This is used when receiving a response to a
 * HEAD request which may contain 'Content-Length' or 'Transfer-Encoding:
 * chunked' headers that indicate the presence of a body.
 *
 * Returning `2` from on_headers_complete will tell parser that it should not
 * expect neither a body nor any futher responses on this connection. This is
 * useful for handling responses to a CONNECT request which may not contain
 * `Upgrade` or `Connection: upgrade` headers.
 *
 * http_data_cb does not return data chunks. It will be called arbitrarally
 * many times for each string. E.G. you might get 10 callbacks for "on_url"
 * each providing just a few characters more data.
 */
typedef int (*http_data_cb) (http_parser*, const char *at, size_t length);
typedef int (*http_cb) (http_parser*);


/* Flag values for http_parser.flags field */
enum http_connection_flags // This is basically 7 booleans placed into 1 integer. Uses 4 bytes instead of n bytes (7 currently).
  { F_CHUNKED               = 1 << 0 // 00000000 00000000 00000000 00000001
  , F_CONNECTION_KEEP_ALIVE = 1 << 1 // 00000000 00000000 00000000 00000010
  , F_CONNECTION_CLOSE      = 1 << 2 // 00000000 00000000 00000000 00000100
  , F_TRAILING              = 1 << 3 // 00000000 00000000 00000000 00001000
  , F_UPGRADE               = 1 << 4 // 00000000 00000000 00000000 00010000
  , F_SKIPBODY              = 1 << 5 // 00000000 00000000 00000000 00100000
  , F_CONTENTLENGTH         = 1 << 6 // 00000000 00000000 00000000 01000000
  };


/* Map for errno-related constants
 *
 * The provided argument should be a macro that takes 2 arguments.
 */
#define CROW_HTTP_ERRNO_MAP(CROW_XX)                                                    \
  /* No error */                                                                        \
  CROW_XX(OK, "success")                                                                \
                                                                                        \
  /* Callback-related errors */                                                         \
  CROW_XX(CB_message_begin, "the on_message_begin callback failed")                     \
  CROW_XX(CB_method, "the on_method callback failed")                                   \
  CROW_XX(CB_url, "the \"on_url\" callback failed")                                     \
  CROW_XX(CB_header_field, "the \"on_header_field\" callback failed")                   \
  CROW_XX(CB_header_value, "the \"on_header_value\" callback failed")                   \
  CROW_XX(CB_headers_complete, "the \"on_headers_complete\" callback failed")           \
  CROW_XX(CB_body, "the \"on_body\" callback failed")                                   \
  CROW_XX(CB_message_complete, "the \"on_message_complete\" callback failed")           \
  CROW_XX(CB_status, "the \"on_status\" callback failed")                               \
                                                                                        \
  /* Parsing-related errors */                                                          \
  CROW_XX(INVALID_EOF_STATE, "stream ended at an unexpected time")                      \
  CROW_XX(HEADER_OVERFLOW, "too many header bytes seen; overflow detected")             \
  CROW_XX(CLOSED_CONNECTION, "data received after completed connection: close message") \
  CROW_XX(INVALID_VERSION, "invalid HTTP version")                                      \
  CROW_XX(INVALID_STATUS, "invalid HTTP status code")                                   \
  CROW_XX(INVALID_METHOD, "invalid HTTP method")                                        \
  CROW_XX(INVALID_URL, "invalid URL")                                                   \
  CROW_XX(INVALID_HOST, "invalid host")                                                 \
  CROW_XX(INVALID_PORT, "invalid port")                                                 \
  CROW_XX(INVALID_PATH, "invalid path")                                                 \
  CROW_XX(INVALID_QUERY_STRING, "invalid query string")                                 \
  CROW_XX(INVALID_FRAGMENT, "invalid fragment")                                         \
  CROW_XX(LF_EXPECTED, "LF character expected")                                         \
  CROW_XX(INVALID_HEADER_TOKEN, "invalid character in header")                          \
  CROW_XX(INVALID_CONTENT_LENGTH, "invalid character in content-length header")         \
  CROW_XX(UNEXPECTED_CONTENT_LENGTH, "unexpected content-length header")                \
  CROW_XX(INVALID_CHUNK_SIZE, "invalid character in chunk size header")                 \
  CROW_XX(INVALID_CONSTANT, "invalid constant string")                                  \
  CROW_XX(INVALID_INTERNAL_STATE, "encountered unexpected internal state")              \
  CROW_XX(STRICT, "strict mode assertion failed")                                       \
  CROW_XX(UNKNOWN, "an unknown error occurred")                                         \
  CROW_XX(INVALID_TRANSFER_ENCODING, "request has invalid transfer-encoding")           \


/* Define CHPE_* values for each errno value above */
#define CROW_HTTP_ERRNO_GEN(n, s) CHPE_##n,
enum http_errno {
  CROW_HTTP_ERRNO_MAP(CROW_HTTP_ERRNO_GEN)
};
#undef CROW_HTTP_ERRNO_GEN


/* Get an http_errno value from an http_parser */
#define CROW_HTTP_PARSER_ERRNO(p) ((enum http_errno)(p)->http_errno)


    struct http_parser
    {
        /** PRIVATE **/
        unsigned int flags : 7;                  /* F_* values from 'flags' enum; semi-public */
        unsigned int state : 8;                  /* enum state from http_parser.c */
        unsigned int header_state : 7;           /* enum header_state from http_parser.c */
        unsigned int index : 5;                  /* index into current matcher */
        unsigned int uses_transfer_encoding : 1; /* Transfer-Encoding header is present */
        unsigned int allow_chunked_length : 1;   /* Allow headers with both `Content-Length` and `Transfer-Encoding: chunked` set */
        unsigned int lenient_http_headers : 1;

        uint32_t nread;          /* # bytes read in various scenarios */
        uint64_t content_length; /* # bytes in body. `(uint64_t) -1` (all bits one) if no Content-Length header. */
        unsigned long qs_point;

        /** READ-ONLY **/
        unsigned char http_major;
        unsigned char http_minor;
        unsigned int method : 8;       /* requests only */
        unsigned int http_errno : 7;

  /* 1 = Upgrade header was present and the parser has exited because of that.
   * 0 = No upgrade header present.
   * Should be checked when http_parser_execute() returns in addition to
   * error checking.
   */
        unsigned int upgrade : 1;

        /** PUBLIC **/
        void* data; /* A pointer to get hook to the "connection" or "socket" object */
    };


    struct http_parser_settings
    {
        http_cb on_message_begin;
        http_cb on_method;
        http_data_cb on_url;
        http_data_cb on_header_field;
        http_data_cb on_header_value;
        http_cb on_headers_complete;
        http_data_cb on_body;
        http_cb on_message_complete;
    };



// SOURCE (.c) CODE
static uint32_t max_header_size = CROW_HTTP_MAX_HEADER_SIZE;

#ifndef CROW_ULLONG_MAX
# define CROW_ULLONG_MAX ((uint64_t) -1) /* 2^64-1 */
#endif

#ifndef CROW_MIN
# define CROW_MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef CROW_ARRAY_SIZE
# define CROW_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef CROW_BIT_AT
# define CROW_BIT_AT(a, i)                                           \
  (!!((unsigned int) (a)[(unsigned int) (i) >> 3] &                  \
   (1 << ((unsigned int) (i) & 7))))
#endif

#define CROW_SET_ERRNO(e)                                            \
do {                                                                 \
  parser->nread = nread;                                             \
  parser->http_errno = (e);                                          \
} while(0)

/* Run the notify callback FOR, returning ER if it fails */
#define CROW_CALLBACK_NOTIFY_(FOR, ER)                               \
do {                                                                 \
  assert(CROW_HTTP_PARSER_ERRNO(parser) == CHPE_OK);                 \
                                                                     \
  if (CROW_LIKELY(settings->on_##FOR)) {                             \
    if (CROW_UNLIKELY(0 != settings->on_##FOR(parser))) {            \
      CROW_SET_ERRNO(CHPE_CB_##FOR);                                 \
    }                                                                \
                                                                     \
    /* We either errored above or got paused; get out */             \
    if (CROW_UNLIKELY(CROW_HTTP_PARSER_ERRNO(parser) != CHPE_OK)) {  \
      return (ER);                                                   \
    }                                                                \
  }                                                                  \
} while (0)

/* Run the notify callback FOR and consume the current byte */
#define CROW_CALLBACK_NOTIFY(FOR)            CROW_CALLBACK_NOTIFY_(FOR, p - data + 1)

/* Run the notify callback FOR and don't consume the current byte */
#define CROW_CALLBACK_NOTIFY_NOADVANCE(FOR)  CROW_CALLBACK_NOTIFY_(FOR, p - data)

/* Run data callback FOR with LEN bytes, returning ER if it fails */
#define CROW_CALLBACK_DATA_(FOR, LEN, ER)                            \
do {                                                                 \
  assert(CROW_HTTP_PARSER_ERRNO(parser) == CHPE_OK);                 \
                                                                     \
  if (FOR##_mark) {                                                  \
    if (CROW_LIKELY(settings->on_##FOR)) {                           \
      if (CROW_UNLIKELY(0 !=                                         \
          settings->on_##FOR(parser, FOR##_mark, (LEN)))) {          \
        CROW_SET_ERRNO(CHPE_CB_##FOR);                               \
      }                                                              \
                                                                     \
      /* We either errored above or got paused; get out */           \
      if (CROW_UNLIKELY(CROW_HTTP_PARSER_ERRNO(parser) != CHPE_OK)) {\
        return (ER);                                                 \
      }                                                              \
    }                                                                \
    FOR##_mark = NULL;                                               \
  }                                                                  \
} while (0)

/* Run the data callback FOR and consume the current byte */
#define CROW_CALLBACK_DATA(FOR)                                      \
    CROW_CALLBACK_DATA_(FOR, p - FOR##_mark, p - data + 1)

/* Run the data callback FOR and don't consume the current byte */
#define CROW_CALLBACK_DATA_NOADVANCE(FOR)                            \
    CROW_CALLBACK_DATA_(FOR, p - FOR##_mark, p - data)

/* Set the mark FOR; non-destructive if mark is already set */
#define CROW_MARK(FOR)                                               \
do {                                                                 \
  if (!FOR##_mark) {                                                 \
    FOR##_mark = p;                                                  \
  }                                                                  \
} while (0)

/* Don't allow the total size of the HTTP headers (including the status
 * line) to exceed max_header_size.  This check is here to protect
 * embedders against denial-of-service attacks where the attacker feeds
 * us a never-ending header that the embedder keeps buffering.
 *
 * This check is arguably the responsibility of embedders but we're doing
 * it on the embedder's behalf because most won't bother and this way we
 * make the web a little safer.  max_header_size is still far bigger
 * than any reasonable request or response so this should never affect
 * day-to-day operation.
 */
#define CROW_COUNT_HEADER_SIZE(V)                                    \
do {                                                                 \
  nread += (uint32_t)(V);                                            \
  if (CROW_UNLIKELY(nread > max_header_size)) {                      \
    CROW_SET_ERRNO(CHPE_HEADER_OVERFLOW);                            \
    goto error;                                                      \
  }                                                                  \
} while (0)
#define CROW_REEXECUTE()                                             \
  goto reexecute;                                                    \

#define CROW_PROXY_CONNECTION "proxy-connection"
#define CROW_CONNECTION "connection"
#define CROW_CONTENT_LENGTH "content-length"
#define CROW_TRANSFER_ENCODING "transfer-encoding"
#define CROW_UPGRADE "upgrade"
#define CROW_CHUNKED "chunked"
#define CROW_KEEP_ALIVE "keep-alive"
#define CROW_CLOSE "close"



    enum state
    {
        s_dead = 1 /* important that this is > 0 */

        ,
        s_start_req

        ,
        s_req_method,
        s_req_spaces_before_url,
        s_req_schema,
        s_req_schema_slash,
        s_req_schema_slash_slash,
        s_req_server_start,
        s_req_server,             // }
        s_req_server_with_at,     // |
        s_req_path,               // | The parser recognizes how to switch between these states,
        s_req_query_string_start, // | however it doesn't process them any differently.
        s_req_query_string,       // }
        s_req_http_start,
        s_req_http_H,
        s_req_http_HT,
        s_req_http_HTT,
        s_req_http_HTTP,
        s_req_http_I,
        s_req_http_IC,
        s_req_http_major,
        s_req_http_dot,
        s_req_http_minor,
        s_req_http_end,
        s_req_line_almost_done

        ,
        s_header_field_start,
        s_header_field,
        s_header_value_discard_ws,
        s_header_value_discard_ws_almost_done,
        s_header_value_discard_lws,
        s_header_value_start,
        s_header_value,
        s_header_value_lws

        ,
        s_header_almost_done

        ,
        s_chunk_size_start,
        s_chunk_size,
        s_chunk_parameters,
        s_chunk_size_almost_done

        ,
        s_headers_almost_done,
        s_headers_done

        /* Important: 's_headers_done' must be the last 'header' state. All
         * states beyond this must be 'body' states. It is used for overflow
         * checking. See the CROW_PARSING_HEADER() macro.
         */

        ,
        s_chunk_data,
        s_chunk_data_almost_done,
        s_chunk_data_done

        ,
        s_body_identity,
        s_body_identity_eof

        ,
        s_message_done
    };


#define CROW_PARSING_HEADER(state) (state <= s_headers_done)


enum header_states
  { h_general = 0
  , h_C
  , h_CO
  , h_CON

  , h_matching_connection
  , h_matching_proxy_connection
  , h_matching_content_length
  , h_matching_transfer_encoding
  , h_matching_upgrade

  , h_connection
  , h_content_length
  , h_content_length_num
  , h_content_length_ws
  , h_transfer_encoding
  , h_upgrade

  , h_matching_transfer_encoding_token_start
  , h_matching_transfer_encoding_chunked
  , h_matching_transfer_encoding_token

  , h_matching_connection_keep_alive
  , h_matching_connection_close

  , h_transfer_encoding_chunked
  , h_connection_keep_alive
  , h_connection_close
  };

enum http_host_state
  {
    s_http_host_dead = 1
  , s_http_userinfo_start
  , s_http_userinfo
  , s_http_host_start
  , s_http_host_v6_start
  , s_http_host
  , s_http_host_v6
  , s_http_host_v6_end
  , s_http_host_v6_zone_start
  , s_http_host_v6_zone
  , s_http_host_port_start
  , s_http_host_port
};

/* Macros for character classes; depends on strict-mode  */
#define CROW_LOWER(c)            (unsigned char)(c | 0x20)
#define CROW_IS_ALPHA(c)         (CROW_LOWER(c) >= 'a' && CROW_LOWER(c) <= 'z')
#define CROW_IS_NUM(c)           ((c) >= '0' && (c) <= '9')
#define CROW_IS_ALPHANUM(c)      (CROW_IS_ALPHA(c) || CROW_IS_NUM(c))
//#define CROW_IS_HEX(c)           (CROW_IS_NUM(c) || (CROW_LOWER(c) >= 'a' && CROW_LOWER(c) <= 'f'))
#define CROW_IS_MARK(c)          ((c) == '-' || (c) == '_' || (c) == '.' || \
  (c) == '!' || (c) == '~' || (c) == '*' || (c) == '\'' || (c) == '(' ||    \
  (c) == ')')
#define CROW_IS_USERINFO_CHAR(c) (CROW_IS_ALPHANUM(c) || CROW_IS_MARK(c) || (c) == '%' || \
  (c) == ';' || (c) == ':' || (c) == '&' || (c) == '=' || (c) == '+' ||                   \
  (c) == '$' || (c) == ',')

#define CROW_TOKEN(c)            (tokens[(unsigned char)c])
#define CROW_IS_URL_CHAR(c)      (CROW_BIT_AT(normal_url_char, (unsigned char)c))
//#define CROW_IS_HOST_CHAR(c)     (CROW_IS_ALPHANUM(c) || (c) == '.' || (c) == '-')

  /**
 * Verify that a char is a valid visible (printable) US-ASCII
 * character or %x80-FF
 **/
#define CROW_IS_HEADER_CHAR(ch)                                                     \
  (ch == cr || ch == lf || ch == 9 || ((unsigned char)ch > 31 && ch != 127))

#define CROW_start_state s_start_req

# define CROW_STRICT_CHECK(cond)                                     \
do {                                                                 \
  if (cond) {                                                        \
    CROW_SET_ERRNO(CHPE_STRICT);                                     \
    goto error;                                                      \
  }                                                                  \
} while (0)
#define CROW_NEW_MESSAGE() (CROW_start_state)

/* Our URL parser.
 *
 * This is designed to be shared by http_parser_execute() for URL validation,
 * hence it has a state transition + byte-for-byte interface. In addition, it
 * is meant to be embedded in http_parser_parse_url(), which does the dirty
 * work of turning state transitions URL components for its API.
 *
 * This function should only be invoked with non-space characters. It is
 * assumed that the caller cares about (and can detect) the transition between
 * URL and non-URL states by looking for these.
 */
inline enum state
parse_url_char(enum state s, const char ch, http_parser *parser, const char* url_mark, const char* p)
{
# define CROW_T(v) 0


static const uint8_t normal_url_char[32] = {
/*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
        0    |   0    |   0    |   0    |   0    |   0    |   0    |   0,
/*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
        0    |CROW_T(2)|  0    |   0    |CROW_T(16)| 0    |   0    |   0,
/*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
        0    |   0    |   0    |   0    |   0    |   0    |   0    |   0,
/*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
        0    |   0    |   0    |   0    |   0    |   0    |   0    |   0,
/*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
        0    |   2    |   4    |   0    |   16   |   32   |   64   |  128,
/*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |   0,
/*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  80  P    81  Q    82  R    83  S    84  CROW_T    85  U    86  V    87  W  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |   0, };

#undef CROW_T

  if (ch == ' ' || ch == '\r' || ch == '\n') {
    return s_dead;
  }
  if (ch == '\t' || ch == '\f') {
    return s_dead;
  }

  switch (s) {
    case s_req_spaces_before_url:
      /* Proxied requests are followed by scheme of an absolute URI (alpha).
       * All methods except CONNECT are followed by '/' or '*'.
       */

      if (ch == '/' || ch == '*') {
        return s_req_path;
      }

      if (CROW_IS_ALPHA(ch)) {
        return s_req_schema;
      }

      break;

    case s_req_schema:
      if (CROW_IS_ALPHA(ch)) {
        return s;
      }

      if (ch == ':') {
        return s_req_schema_slash;
      }

      break;

    case s_req_schema_slash:
      if (ch == '/') {
        return s_req_schema_slash_slash;
      }

      break;

    case s_req_schema_slash_slash:
      if (ch == '/') {
        return s_req_server_start;
      }

      break;

    case s_req_server_with_at:
      if (ch == '@') {
        return s_dead;
      }

    /* fall through */
    case s_req_server_start:
    case s_req_server:
      if (ch == '/') {
        return s_req_path;
      }

      if (ch == '?') {
          parser->qs_point = p - url_mark;
        return s_req_query_string_start;
      }

      if (ch == '@') {
        return s_req_server_with_at;
      }

      if (CROW_IS_USERINFO_CHAR(ch) || ch == '[' || ch == ']') {
        return s_req_server;
      }

      break;

    case s_req_path:
      if (CROW_IS_URL_CHAR(ch)) {
        return s;
      }
      else if (ch == '?')
      {
          parser->qs_point = p - url_mark;
          return s_req_query_string_start;
      }

      break;

    case s_req_query_string_start:
    case s_req_query_string:
      if (CROW_IS_URL_CHAR(ch)) {
        return s_req_query_string;
      }
      else if (ch == '?')
      {
          return s_req_query_string;
      }

      break;

    default:
      break;
  }

  /* We should never fall out of the switch above unless there's an error */
  return s_dead;
}

inline size_t http_parser_execute (http_parser *parser,
                            const http_parser_settings *settings,
                            const char *data,
                            size_t len)
{

/* Tokens as defined by rfc 2616. Also lowercases them.
 *        token       = 1*<any CHAR except CTLs or separators>
 *     separators     = "(" | ")" | "<" | ">" | "@"
 *                    | "," | ";" | ":" | "\" | <">
 *                    | "/" | "[" | "]" | "?" | "="
 *                    | "{" | "}" | SP  | HT
 */
static const char tokens[256] = {
/*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
        0,      '!',      0,      '#',     '$',     '%',     '&',    '\'',
/*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
        0,       0,      '*',     '+',      0,      '-',     '.',      0,
/*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
       '0',     '1',     '2',     '3',     '4',     '5',     '6',     '7',
/*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
       '8',     '9',      0,       0,       0,       0,       0,       0,
/*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
        0,      'a',     'b',     'c',     'd',     'e',     'f',     'g',
/*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
       'h',     'i',     'j',     'k',     'l',     'm',     'n',     'o',
/*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
       'p',     'q',     'r',     's',     't',     'u',     'v',     'w',
/*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
       'x',     'y',     'z',      0,       0,       0,      '^',     '_',
/*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
       '`',     'a',     'b',     'c',     'd',     'e',     'f',     'g',
/* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
       'h',     'i',     'j',     'k',     'l',     'm',     'n',     'o',
/* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
       'p',     'q',     'r',     's',     't',     'u',     'v',     'w',
/* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
       'x',     'y',     'z',      0,      '|',      0,      '~',       0 };


static const int8_t unhex[256] =
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  , 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1
  ,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  };



  char c, ch;
  int8_t unhex_val;
  const char *p = data;
  const char *header_field_mark = 0;
  const char *header_value_mark = 0;
  const char *url_mark = 0;
  const char *url_start_mark = 0;
  const char *body_mark = 0;
  const unsigned int lenient = parser->lenient_http_headers;
  const unsigned int allow_chunked_length = parser->allow_chunked_length;

  uint32_t nread = parser->nread;

  /* We're in an error state. Don't bother doing anything. */
  if (CROW_HTTP_PARSER_ERRNO(parser) != CHPE_OK) {
    return 0;
  }

  if (len == 0) {
    switch (parser->state) {
      case s_body_identity_eof:
        /* Use of CROW_CALLBACK_NOTIFY() here would erroneously return 1 byte read if we got paused. */
        CROW_CALLBACK_NOTIFY_NOADVANCE(message_complete);
        return 0;

      case s_dead:
      case s_start_req:
        return 0;

      default:
        CROW_SET_ERRNO(CHPE_INVALID_EOF_STATE);
        return 1;
    }
  }


  if (parser->state == s_header_field)
    header_field_mark = data;
  if (parser->state == s_header_value)
    header_value_mark = data;
  switch (parser->state) {
  case s_req_path:
  case s_req_schema:
  case s_req_schema_slash:
  case s_req_schema_slash_slash:
  case s_req_server_start:
  case s_req_server:
  case s_req_server_with_at:
  case s_req_query_string_start:
  case s_req_query_string:
    url_mark = data;
    break;
  default:
    break;
  }

  for (p=data; p != data + len; p++) {
    ch = *p;

    if (CROW_PARSING_HEADER(parser->state))
      CROW_COUNT_HEADER_SIZE(1);

reexecute:
    switch (parser->state) {

      case s_dead:
        /* this state is used after a 'Connection: close' message
         * the parser will error out if it reads another message
         */
        if (CROW_LIKELY(ch == cr || ch == lf))
          break;

        CROW_SET_ERRNO(CHPE_CLOSED_CONNECTION);
        goto error;

      case s_start_req:
      {
        if (ch == cr || ch == lf)
          break;
        parser->flags = 0;
        parser->uses_transfer_encoding = 0;
        parser->content_length = CROW_ULLONG_MAX;

        if (CROW_UNLIKELY(!CROW_IS_ALPHA(ch))) {
          CROW_SET_ERRNO(CHPE_INVALID_METHOD);
          goto error;
        }

        parser->method = 0;
        parser->index = 1;
        switch (ch) {
          case 'A': parser->method = (unsigned)HTTPMethod::Acl;                                                              break;
          case 'B': parser->method = (unsigned)HTTPMethod::Bind;                                                             break;
          case 'C': parser->method = (unsigned)HTTPMethod::Connect;   /* or COPY, CHECKOUT */                                break;
          case 'D': parser->method = (unsigned)HTTPMethod::Delete;                                                           break;
          case 'G': parser->method = (unsigned)HTTPMethod::Get;                                                              break;
          case 'H': parser->method = (unsigned)HTTPMethod::Head;                                                             break;
          case 'L': parser->method = (unsigned)HTTPMethod::Lock;      /* or LINK */                                          break;
          case 'M': parser->method = (unsigned)HTTPMethod::MkCol;     /* or MOVE, MKACTIVITY, MERGE, M-SEARCH, MKCALENDAR */ break;
          case 'N': parser->method = (unsigned)HTTPMethod::Notify;                                                           break;
          case 'O': parser->method = (unsigned)HTTPMethod::Options;                                                          break;
          case 'P': parser->method = (unsigned)HTTPMethod::Post;      /* or PROPFIND|PROPPATCH|PUT|PATCH|PURGE */            break;
          case 'R': parser->method = (unsigned)HTTPMethod::Report;    /* or REBIND */                                        break;
          case 'S': parser->method = (unsigned)HTTPMethod::Subscribe; /* or SEARCH, SOURCE */                                break;
          case 'T': parser->method = (unsigned)HTTPMethod::Trace;                                                            break;
          case 'U': parser->method = (unsigned)HTTPMethod::Unlock;    /* or UNSUBSCRIBE, UNBIND, UNLINK */                   break;
          default:
            CROW_SET_ERRNO(CHPE_INVALID_METHOD);
            goto error;
        }
        parser->state = s_req_method;

        CROW_CALLBACK_NOTIFY(message_begin);

        break;
      }

      case s_req_method:
      {
        const char *matcher;
        if (CROW_UNLIKELY(ch == '\0')) {
          CROW_SET_ERRNO(CHPE_INVALID_METHOD);
          goto error;
        }

        matcher = method_strings[parser->method];
        if (ch == ' ' && matcher[parser->index] == '\0') {
          parser->state = s_req_spaces_before_url;
        } else if (ch == matcher[parser->index]) {
          ; /* nada */
        } else if ((ch >= 'A' && ch <= 'Z') || ch == '-') {

          switch (parser->method << 16 | parser->index << 8 | ch) {
#define CROW_XX(meth, pos, ch, new_meth) \
            case ((unsigned)HTTPMethod::meth << 16 | pos << 8 | ch): \
              parser->method = (unsigned)HTTPMethod::new_meth; break;

            CROW_XX(Post,      1, 'U', Put)
            CROW_XX(Post,      1, 'A', Patch)
            CROW_XX(Post,      1, 'R', Propfind)
            CROW_XX(Put,       2, 'R', Purge)
            CROW_XX(Connect,   1, 'H', Checkout)
            CROW_XX(Connect,   2, 'P', Copy)
            CROW_XX(MkCol,     1, 'O', Move)
            CROW_XX(MkCol,     1, 'E', Merge)
            CROW_XX(MkCol,     1, '-', MSearch)
            CROW_XX(MkCol,     2, 'A', MkActivity)
            CROW_XX(MkCol,     3, 'A', MkCalendar)
            CROW_XX(Subscribe, 1, 'E', Search)
            CROW_XX(Subscribe, 1, 'O', Source)
            CROW_XX(Report,    2, 'B', Rebind)
            CROW_XX(Propfind,  4, 'P', Proppatch)
            CROW_XX(Lock,      1, 'I', Link)
            CROW_XX(Unlock,    2, 'S', Unsubscribe)
            CROW_XX(Unlock,    2, 'B', Unbind)
            CROW_XX(Unlock,    3, 'I', Unlink)
#undef CROW_XX
            default:
              CROW_SET_ERRNO(CHPE_INVALID_METHOD);
              goto error;
          }
        } else {
          CROW_SET_ERRNO(CHPE_INVALID_METHOD);
          goto error;
        }

        CROW_CALLBACK_NOTIFY_NOADVANCE(method);

        ++parser->index;
        break;
      }

      case s_req_spaces_before_url:
      {
        if (ch == ' ') break;

        CROW_MARK(url);
        CROW_MARK(url_start);
        if (parser->method == (unsigned)HTTPMethod::Connect) {
          parser->state = s_req_server_start;
        }

        parser->state = parse_url_char(static_cast<state>(parser->state), ch, parser, url_start_mark, p);
        if (CROW_UNLIKELY(parser->state == s_dead)) {
          CROW_SET_ERRNO(CHPE_INVALID_URL);
          goto error;
        }

        break;
      }

      case s_req_schema:
      case s_req_schema_slash:
      case s_req_schema_slash_slash:
      case s_req_server_start:
      {
        switch (ch) {
          /* No whitespace allowed here */
          case ' ':
          case cr:
          case lf:
            CROW_SET_ERRNO(CHPE_INVALID_URL);
            goto error;
          default:
            parser->state = parse_url_char(static_cast<state>(parser->state), ch, parser, url_start_mark, p);
            if (CROW_UNLIKELY(parser->state == s_dead)) {
              CROW_SET_ERRNO(CHPE_INVALID_URL);
              goto error;
            }
        }

        break;
      }

      case s_req_server:
      case s_req_server_with_at:
      case s_req_path:
      case s_req_query_string_start:
      case s_req_query_string:
      {
        switch (ch) {
          case ' ':
            parser->state = s_req_http_start;
            CROW_CALLBACK_DATA(url);
            break;
          case cr: // No space after URL means no HTTP version. Which means the request is using HTTP/0.9
          case lf:
            if (CROW_UNLIKELY(parser->method != (unsigned)HTTPMethod::Get)) // HTTP/0.9 doesn't define any method other than GET
            {
              parser->state = s_dead;
              CROW_SET_ERRNO(CHPE_INVALID_VERSION);
              goto error;
            }
            parser->http_major = 0;
            parser->http_minor = 9;
            parser->state = (ch == cr) ?
              s_req_line_almost_done :
              s_header_field_start;
            CROW_CALLBACK_DATA(url);
            break;
          default:
            parser->state = parse_url_char(static_cast<state>(parser->state), ch, parser, url_start_mark, p);
            if (CROW_UNLIKELY(parser->state == s_dead)) {
              CROW_SET_ERRNO(CHPE_INVALID_URL);
              goto error;
            }
        }
        break;
      }

      case s_req_http_start:
        switch (ch) {
          case ' ':
            break;
          case 'H':
            parser->state = s_req_http_H;
            break;
          case 'I':
            if (parser->method == (unsigned)HTTPMethod::Source) {
              parser->state = s_req_http_I;
              break;
            }
            /* fall through */
          default:
            CROW_SET_ERRNO(CHPE_INVALID_CONSTANT);
            goto error;
        }
        break;

      case s_req_http_H:
        CROW_STRICT_CHECK(ch != 'T');
        parser->state = s_req_http_HT;
        break;

      case s_req_http_HT:
        CROW_STRICT_CHECK(ch != 'T');
        parser->state = s_req_http_HTT;
        break;

      case s_req_http_HTT:
        CROW_STRICT_CHECK(ch != 'P');
        parser->state = s_req_http_HTTP;
        break;

      case s_req_http_I:
        CROW_STRICT_CHECK(ch != 'C');
        parser->state = s_req_http_IC;
        break;

      case s_req_http_IC:
        CROW_STRICT_CHECK(ch != 'E');
        parser->state = s_req_http_HTTP;  /* Treat "ICE" as "HTTP". */
        break;

      case s_req_http_HTTP:
        CROW_STRICT_CHECK(ch != '/');
        parser->state = s_req_http_major;
        break;

      /* dot */
      case s_req_http_major:
        if (CROW_UNLIKELY(!CROW_IS_NUM(ch))) {
          CROW_SET_ERRNO(CHPE_INVALID_VERSION);
          goto error;
        }

        parser->http_major = ch - '0';
        parser->state = s_req_http_dot;
        break;

      case s_req_http_dot:
      {
        if (CROW_UNLIKELY(ch != '.')) {
          CROW_SET_ERRNO(CHPE_INVALID_VERSION);
          goto error;
        }

        parser->state = s_req_http_minor;
        break;
      }

      /* minor HTTP version */
      case s_req_http_minor:
        if (CROW_UNLIKELY(!CROW_IS_NUM(ch))) {
          CROW_SET_ERRNO(CHPE_INVALID_VERSION);
          goto error;
        }

        parser->http_minor = ch - '0';
        parser->state = s_req_http_end;
        break;

      /* end of request line */
      case s_req_http_end:
      {
        if (ch == cr) {
          parser->state = s_req_line_almost_done;
          break;
        }

        if (ch == lf) {
          parser->state = s_header_field_start;
          break;
        }

        CROW_SET_ERRNO(CHPE_INVALID_VERSION);
        goto error;
        break;
      }

      /* end of request line */
      case s_req_line_almost_done:
      {
        if (CROW_UNLIKELY(ch != lf)) {
          CROW_SET_ERRNO(CHPE_LF_EXPECTED);
          goto error;
        }

        parser->state = s_header_field_start;
        break;
      }

      case s_header_field_start:
      {
        if (ch == cr) {
          parser->state = s_headers_almost_done;
          break;
        }

        if (ch == lf) {
          /* they might be just sending \n instead of \r\n so this would be
           * the second \n to denote the end of headers*/
          parser->state = s_headers_almost_done;
          CROW_REEXECUTE();
        }

        c = CROW_TOKEN(ch);

        if (CROW_UNLIKELY(!c)) {
          CROW_SET_ERRNO(CHPE_INVALID_HEADER_TOKEN);
          goto error;
        }

        CROW_MARK(header_field);

        parser->index = 0;
        parser->state = s_header_field;

        switch (c) {
          case 'c':
            parser->header_state = h_C;
            break;

          case 'p':
            parser->header_state = h_matching_proxy_connection;
            break;

          case 't':
            parser->header_state = h_matching_transfer_encoding;
            break;

          case 'u':
            parser->header_state = h_matching_upgrade;
            break;

          default:
            parser->header_state = h_general;
            break;
        }
        break;
      }

      case s_header_field:
      {
        const char* start = p;
        for (; p != data + len; p++) {
          ch = *p;
          c = CROW_TOKEN(ch);

          if (!c)
            break;

          switch (parser->header_state) {
            case h_general: {
              size_t left = data + len - p;
              const char* pe = p + CROW_MIN(left, max_header_size);
              while (p+1 < pe && CROW_TOKEN(p[1])) {
                p++;
              }
              break;
            }

            case h_C:
              parser->index++;
              parser->header_state = (c == 'o' ? h_CO : h_general);
              break;

            case h_CO:
              parser->index++;
              parser->header_state = (c == 'n' ? h_CON : h_general);
              break;

            case h_CON:
              parser->index++;
              switch (c) {
                case 'n':
                  parser->header_state = h_matching_connection;
                  break;
                case 't':
                  parser->header_state = h_matching_content_length;
                  break;
                default:
                  parser->header_state = h_general;
                  break;
              }
              break;

            /* connection */

            case h_matching_connection:
              parser->index++;
              if (parser->index > sizeof(CROW_CONNECTION)-1 || c != CROW_CONNECTION[parser->index]) {
                parser->header_state = h_general;
              } else if (parser->index == sizeof(CROW_CONNECTION)-2) {
                parser->header_state = h_connection;
              }
              break;

            /* proxy-connection */

            case h_matching_proxy_connection:
              parser->index++;
              if (parser->index > sizeof(CROW_PROXY_CONNECTION)-1 || c != CROW_PROXY_CONNECTION[parser->index]) {
                parser->header_state = h_general;
              } else if (parser->index == sizeof(CROW_PROXY_CONNECTION)-2) {
                parser->header_state = h_connection;
              }
              break;

            /* content-length */

            case h_matching_content_length:
              parser->index++;
              if (parser->index > sizeof(CROW_CONTENT_LENGTH)-1 || c != CROW_CONTENT_LENGTH[parser->index]) {
                parser->header_state = h_general;
              } else if (parser->index == sizeof(CROW_CONTENT_LENGTH)-2) {
                parser->header_state = h_content_length;
              }
              break;

            /* transfer-encoding */

            case h_matching_transfer_encoding:
              parser->index++;
              if (parser->index > sizeof(CROW_TRANSFER_ENCODING)-1 || c != CROW_TRANSFER_ENCODING[parser->index]) {
                parser->header_state = h_general;
              } else if (parser->index == sizeof(CROW_TRANSFER_ENCODING)-2) {
                parser->header_state = h_transfer_encoding;
                parser->uses_transfer_encoding = 1;
              }
              break;

            /* upgrade */

            case h_matching_upgrade:
              parser->index++;
              if (parser->index > sizeof(CROW_UPGRADE)-1 || c != CROW_UPGRADE[parser->index]) {
                parser->header_state = h_general;
              } else if (parser->index == sizeof(CROW_UPGRADE)-2) {
                parser->header_state = h_upgrade;
              }
              break;

            case h_connection:
            case h_content_length:
            case h_transfer_encoding:
            case h_upgrade:
              if (ch != ' ') parser->header_state = h_general;
              break;

            default:
              assert(0 && "Unknown header_state");
              break;
          }
        }

        if (p == data + len) {
          --p;
          CROW_COUNT_HEADER_SIZE(p - start);
          break;
        }

        CROW_COUNT_HEADER_SIZE(p - start);

        if (ch == ':') {
          parser->state = s_header_value_discard_ws;
          CROW_CALLBACK_DATA(header_field);
          break;
        }
/* RFC-7230 Sec 3.2.4 expressly forbids line-folding in header field-names.
        if (ch == cr) {
          parser->state = s_header_almost_done;
          CROW_CALLBACK_DATA(header_field);
          break;
        }

        if (ch == lf) {
          parser->state = s_header_field_start;
          CROW_CALLBACK_DATA(header_field);
          break;
        }
*/
        CROW_SET_ERRNO(CHPE_INVALID_HEADER_TOKEN);
        goto error;
      }

      case s_header_value_discard_ws:
        if (ch == ' ' || ch == '\t') break;

        if (ch == cr) {
          parser->state = s_header_value_discard_ws_almost_done;
          break;
        }

        if (ch == lf) {
          parser->state = s_header_value_discard_lws;
          break;
        }

        /* fall through */

      case s_header_value_start:
      {
        CROW_MARK(header_value);

        parser->state = s_header_value;
        parser->index = 0;

        c = CROW_LOWER(ch);

        switch (parser->header_state) {
          case h_upgrade:
            // Crow does not support HTTP/2 at the moment.
            // According to the RFC https://datatracker.ietf.org/doc/html/rfc7540#section-3.2
            // "A server that does not support HTTP/2 can respond to the request as though the Upgrade header field were absent"
            // => `F_UPGRADE` is not set if the header starts by "h2".
            // This prevents the parser from skipping the request body.
            if (ch != 'h' || p+1 == (data + len) || *(p+1) != '2') {
              parser->flags |= F_UPGRADE;
            }
            parser->header_state = h_general;
            break;

          case h_transfer_encoding:
            /* looking for 'Transfer-Encoding: chunked' */
            if ('c' == c) {
              parser->header_state = h_matching_transfer_encoding_chunked;
            } else {
              parser->header_state = h_matching_transfer_encoding_token;
            }
            break;

          /* Multi-value `Transfer-Encoding` header */
          case h_matching_transfer_encoding_token_start:
            break;

          case h_content_length:
            if (CROW_UNLIKELY(!CROW_IS_NUM(ch))) {
              CROW_SET_ERRNO(CHPE_INVALID_CONTENT_LENGTH);
              goto error;
            }

            if (parser->flags & F_CONTENTLENGTH) {
              CROW_SET_ERRNO(CHPE_UNEXPECTED_CONTENT_LENGTH);
              goto error;
            }
            parser->flags |= F_CONTENTLENGTH;
            parser->content_length = ch - '0';
            parser->header_state = h_content_length_num;
            break;

          /* when obsolete line folding is encountered for content length
           * continue to the s_header_value state */
          case h_content_length_ws:
            break;

          case h_connection:
            /* looking for 'Connection: keep-alive' */
            if (c == 'k') {
              parser->header_state = h_matching_connection_keep_alive;
            /* looking for 'Connection: close' */
            } else if (c == 'c') {
              parser->header_state = h_matching_connection_close;
            } else if (c == ' ' || c == '\t') {
              /* Skip lws */
            } else {
              parser->header_state = h_general;
            }
            break;

          default:
            parser->header_state = h_general;
            break;
        }
        break;
      }

      case s_header_value:
      {
        const char* start = p;
        enum header_states h_state = static_cast<header_states>(parser->header_state);
        for (; p != data + len; p++) {
          ch = *p;

          if (ch == cr) {
            parser->state = s_header_almost_done;
            parser->header_state = h_state;
            CROW_CALLBACK_DATA(header_value);
            break;
          }

          if (ch == lf) {
            parser->state = s_header_almost_done;
            CROW_COUNT_HEADER_SIZE(p - start);
            parser->header_state = h_state;
            CROW_CALLBACK_DATA_NOADVANCE(header_value);
            CROW_REEXECUTE();
          }

          if (!lenient && !CROW_IS_HEADER_CHAR(ch)) {
            CROW_SET_ERRNO(CHPE_INVALID_HEADER_TOKEN);
            goto error;
          }

          c = CROW_LOWER(ch);

          switch (h_state) {
            case h_general:
              {
                size_t left = data + len - p;
                const char* pe = p + CROW_MIN(left, max_header_size);

                for (; p != pe; p++) {
                  ch = *p;
                  if (ch == cr || ch == lf) {
                    --p;
                    break;
                  }
                  if (!lenient && !CROW_IS_HEADER_CHAR(ch)) {
                    CROW_SET_ERRNO(CHPE_INVALID_HEADER_TOKEN);
                    goto error;
                  }
                }
                if (p == data + len)
                  --p;
                break;
              }

            case h_connection:
            case h_transfer_encoding:
              assert(0 && "Shouldn't get here.");
              break;

            case h_content_length:
              if (ch == ' ') break;
              h_state = h_content_length_num;
              /* fall through */

            case h_content_length_num:
            {
              uint64_t t;

              if (ch == ' ') {
                h_state = h_content_length_ws;
                break;
              }

              if (CROW_UNLIKELY(!CROW_IS_NUM(ch))) {
                CROW_SET_ERRNO(CHPE_INVALID_CONTENT_LENGTH);
                parser->header_state = h_state;
                goto error;
              }

              t = parser->content_length;
              t *= 10;
              t += ch - '0';

              /* Overflow? Test against a conservative limit for simplicity. */
              if (CROW_UNLIKELY((CROW_ULLONG_MAX - 10) / 10 < parser->content_length)) {
                CROW_SET_ERRNO(CHPE_INVALID_CONTENT_LENGTH);
                parser->header_state = h_state;
                goto error;
              }

              parser->content_length = t;
              break;
            }

            case h_content_length_ws:
              if (ch == ' ') break;
              CROW_SET_ERRNO(CHPE_INVALID_CONTENT_LENGTH);
              parser->header_state = h_state;
              goto error;

            /* Transfer-Encoding: chunked */
            case h_matching_transfer_encoding_token_start:
              /* looking for 'Transfer-Encoding: chunked' */
              if ('c' == c) {
                h_state = h_matching_transfer_encoding_chunked;
              } else if (CROW_TOKEN(c)) {
                /* TODO(indutny): similar code below does this, but why?
                 * At the very least it seems to be inconsistent given that
                 * h_matching_transfer_encoding_token does not check for
                 * `STRICT_TOKEN`
                 */
                h_state = h_matching_transfer_encoding_token;
              } else if (c == ' ' || c == '\t') {
                /* Skip lws */
              } else {
                h_state = h_general;
              }
              break;

            case h_matching_transfer_encoding_chunked:
              parser->index++;
              if (parser->index > sizeof(CROW_CHUNKED)-1 || c != CROW_CHUNKED[parser->index]) {
                h_state = h_matching_transfer_encoding_token;
              } else if (parser->index == sizeof(CROW_CHUNKED)-2) {
                h_state = h_transfer_encoding_chunked;
              }
              break;

            case h_matching_transfer_encoding_token:
              if (ch == ',') {
                h_state = h_matching_transfer_encoding_token_start;
                parser->index = 0;
              }
              break;

            /* looking for 'Connection: keep-alive' */
            case h_matching_connection_keep_alive:
              parser->index++;
              if (parser->index > sizeof(CROW_KEEP_ALIVE)-1 || c != CROW_KEEP_ALIVE[parser->index]) {
                h_state = h_general;
              } else if (parser->index == sizeof(CROW_KEEP_ALIVE)-2) {
                h_state = h_connection_keep_alive;
              }
              break;

            /* looking for 'Connection: close' */
            case h_matching_connection_close:
              parser->index++;
              if (parser->index > sizeof(CROW_CLOSE)-1 || c != CROW_CLOSE[parser->index]) {
                h_state = h_general;
              } else if (parser->index == sizeof(CROW_CLOSE)-2) {
                h_state = h_connection_close;
              }
              break;

              // Edited from original (because of commits that werent included)
            case h_transfer_encoding_chunked:
              if (ch != ' ') h_state = h_matching_transfer_encoding_token;
              break;
            case h_connection_keep_alive:
            case h_connection_close:
              if (ch != ' ') h_state = h_general;
              break;

            default:
              parser->state = s_header_value;
              h_state = h_general;
              break;
          }
        }
        parser->header_state = h_state;


        if (p == data + len)
          --p;

        CROW_COUNT_HEADER_SIZE(p - start);
        break;
      }

      case s_header_almost_done:
      {
        if (CROW_UNLIKELY(ch != lf)) {
          CROW_SET_ERRNO(CHPE_LF_EXPECTED);
          goto error;
        }

        parser->state = s_header_value_lws;
        break;
      }

      case s_header_value_lws:
      {
        if (ch == ' ' || ch == '\t') {
          if (parser->header_state == h_content_length_num) {
              /* treat obsolete line folding as space */
              parser->header_state = h_content_length_ws;
          }
          parser->state = s_header_value_start;
          CROW_REEXECUTE();
        }

        /* finished the header */
        switch (parser->header_state) {
          case h_connection_keep_alive:
            parser->flags |= F_CONNECTION_KEEP_ALIVE;
            break;
          case h_connection_close:
            parser->flags |= F_CONNECTION_CLOSE;
            break;
          case h_transfer_encoding_chunked:
            parser->flags |= F_CHUNKED;
            break;
          default:
            break;
        }

        parser->state = s_header_field_start;
        CROW_REEXECUTE();
      }

      case s_header_value_discard_ws_almost_done:
      {
        CROW_STRICT_CHECK(ch != lf);
        parser->state = s_header_value_discard_lws;
        break;
      }

      case s_header_value_discard_lws:
      {
        if (ch == ' ' || ch == '\t') {
          parser->state = s_header_value_discard_ws;
          break;
        } else {
          /* header value was empty */
          CROW_MARK(header_value);
          parser->state = s_header_field_start;
          CROW_CALLBACK_DATA_NOADVANCE(header_value);
          CROW_REEXECUTE();
        }
      }

      case s_headers_almost_done:
      {
        CROW_STRICT_CHECK(ch != lf);

        if (parser->flags & F_TRAILING) {
          /* End of a chunked request */
          CROW_CALLBACK_NOTIFY(message_complete);
          break;
        }

        /* Cannot use transfer-encoding and a content-length header together
           per the HTTP specification. (RFC 7230 Section 3.3.3) */
        if ((parser->uses_transfer_encoding == 1) &&
            (parser->flags & F_CONTENTLENGTH)) {
          /* Allow it for lenient parsing as long as `Transfer-Encoding` is
           * not `chunked` or allow_length_with_encoding is set
           */
          if (parser->flags & F_CHUNKED) {
            if (!allow_chunked_length) {
              CROW_SET_ERRNO(CHPE_UNEXPECTED_CONTENT_LENGTH);
              goto error;
            }
          } else if (!lenient) {
            CROW_SET_ERRNO(CHPE_UNEXPECTED_CONTENT_LENGTH);
            goto error;
          }
        }

        parser->state = s_headers_done;

        /* Set this here so that on_headers_complete() callbacks can see it */
        parser->upgrade =
          (parser->flags & F_UPGRADE || parser->method == (unsigned)HTTPMethod::Connect);

        /* Here we call the headers_complete callback. This is somewhat
         * different than other callbacks because if the user returns 1, we
         * will interpret that as saying that this message has no body. This
         * is needed for the annoying case of recieving a response to a HEAD
         * request.
         *
         * We'd like to use CROW_CALLBACK_NOTIFY_NOADVANCE() here but we cannot, so
         * we have to simulate it by handling a change in errno below.
         */
        if (settings->on_headers_complete) {
          switch (settings->on_headers_complete(parser)) {
            case 0:
              break;

            case 2:
              parser->upgrade = 1;
              //break;

            /* fall through */
            case 1:
              parser->flags |= F_SKIPBODY;
              break;

            default:
              CROW_SET_ERRNO(CHPE_CB_headers_complete);
              parser->nread = nread;
              return p - data; /* Error */
          }
        }

        if (CROW_HTTP_PARSER_ERRNO(parser) != CHPE_OK) {
          parser->nread = nread;
          return p - data;
        }

        CROW_REEXECUTE();
      }

      case s_headers_done:
      {
        CROW_STRICT_CHECK(ch != lf);

        parser->nread = 0;
        nread = 0;

        /* Exit, the rest of the connect is in a different protocol. */
        if (parser->upgrade) {
          CROW_CALLBACK_NOTIFY(message_complete);
          parser->nread = nread;
          return (p - data) + 1;
        }

        if (parser->flags & F_SKIPBODY) {
          CROW_CALLBACK_NOTIFY(message_complete);
        } else if (parser->flags & F_CHUNKED) {
          /* chunked encoding - ignore Content-Length header,
           * prepare for a chunk */
            parser->state = s_chunk_size_start;
        }
        else if (parser->uses_transfer_encoding == 1)
        {
            if (!lenient)
            {
                /* RFC 7230 3.3.3 */

                /* If a Transfer-Encoding header field
             * is present in a request and the chunked transfer coding is not
             * the final encoding, the message body length cannot be determined
             * reliably; the server MUST respond with the 400 (Bad Request)
             * status code and then close the connection.
             */
                CROW_SET_ERRNO(CHPE_INVALID_TRANSFER_ENCODING);
                parser->nread = nread;
                return (p - data); /* Error */
            }
            else
            {
                /* RFC 7230 3.3.3 */

                /* If a Transfer-Encoding header field is present in a response and
             * the chunked transfer coding is not the final encoding, the
             * message body length is determined by reading the connection until
             * it is closed by the server.
             */
                parser->state = s_body_identity_eof;
            }
        }
        else
        {
            if (parser->content_length == 0)
            {
                /* Content-Length header given but zero: Content-Length: 0\r\n */
                CROW_CALLBACK_NOTIFY(message_complete);
            }
            else if (parser->content_length != CROW_ULLONG_MAX)
            {
                /* Content-Length header given and non-zero */
                parser->state = s_body_identity;
            }
            else
            {
                /* Assume content-length 0 - read the next */
                CROW_CALLBACK_NOTIFY(message_complete);
            }
        }

        break;
      }

      case s_body_identity:
      {
        uint64_t to_read = CROW_MIN(parser->content_length,
                               (uint64_t) ((data + len) - p));

        assert(parser->content_length != 0
            && parser->content_length != CROW_ULLONG_MAX);

        /* The difference between advancing content_length and p is because
         * the latter will automaticaly advance on the next loop iteration.
         * Further, if content_length ends up at 0, we want to see the last
         * byte again for our message complete callback.
         */
        CROW_MARK(body);
        parser->content_length -= to_read;
        p += to_read - 1;

        if (parser->content_length == 0) {
          parser->state = s_message_done;

          /* Mimic CROW_CALLBACK_DATA_NOADVANCE() but with one extra byte.
           *
           * The alternative to doing this is to wait for the next byte to
           * trigger the data callback, just as in every other case. The
           * problem with this is that this makes it difficult for the test
           * harness to distinguish between complete-on-EOF and
           * complete-on-length. It's not clear that this distinction is
           * important for applications, but let's keep it for now.
           */
          CROW_CALLBACK_DATA_(body, p - body_mark + 1, p - data);
          CROW_REEXECUTE();
        }

        break;
      }

      /* read until EOF */
      case s_body_identity_eof:
        CROW_MARK(body);
        p = data + len - 1;

        break;

      case s_message_done:
        CROW_CALLBACK_NOTIFY(message_complete);
        break;

      case s_chunk_size_start:
      {
        assert(nread == 1);
        assert(parser->flags & F_CHUNKED);

        unhex_val = unhex[static_cast<unsigned char>(ch)];
        if (CROW_UNLIKELY(unhex_val == -1)) {
          CROW_SET_ERRNO(CHPE_INVALID_CHUNK_SIZE);
          goto error;
        }

        parser->content_length = unhex_val;
        parser->state = s_chunk_size;
        break;
      }

      case s_chunk_size:
      {
        uint64_t t;

        assert(parser->flags & F_CHUNKED);

        if (ch == cr) {
          parser->state = s_chunk_size_almost_done;
          break;
        }

        unhex_val = unhex[static_cast<unsigned char>(ch)];

        if (unhex_val == -1) {
          if (ch == ';' || ch == ' ') {
            parser->state = s_chunk_parameters;
            break;
          }

          CROW_SET_ERRNO(CHPE_INVALID_CHUNK_SIZE);
          goto error;
        }

        t = parser->content_length;
        t *= 16;
        t += unhex_val;

        /* Overflow? Test against a conservative limit for simplicity. */
        if (CROW_UNLIKELY((CROW_ULLONG_MAX - 16) / 16 < parser->content_length)) {
          CROW_SET_ERRNO(CHPE_INVALID_CONTENT_LENGTH);
          goto error;
        }

        parser->content_length = t;
        break;
      }

      case s_chunk_parameters:
      {
        assert(parser->flags & F_CHUNKED);
        /* just ignore this shit. TODO check for overflow */
        if (ch == cr) {
          parser->state = s_chunk_size_almost_done;
          break;
        }
        break;
      }

      case s_chunk_size_almost_done:
      {
        assert(parser->flags & F_CHUNKED);
        CROW_STRICT_CHECK(ch != lf);

        parser->nread = 0;
        nread = 0;

        if (parser->content_length == 0) {
          parser->flags |= F_TRAILING;
          parser->state = s_header_field_start;
        } else {
          parser->state = s_chunk_data;
        }
        break;
      }

      case s_chunk_data:
      {
        uint64_t to_read = CROW_MIN(parser->content_length,
                               (uint64_t) ((data + len) - p));

        assert(parser->flags & F_CHUNKED);
        assert(parser->content_length != 0
            && parser->content_length != CROW_ULLONG_MAX);

        /* See the explanation in s_body_identity for why the content
         * length and data pointers are managed this way.
         */
        CROW_MARK(body);
        parser->content_length -= to_read;
        p += to_read - 1;

        if (parser->content_length == 0) {
          parser->state = s_chunk_data_almost_done;
        }

        break;
      }

      case s_chunk_data_almost_done:
        assert(parser->flags & F_CHUNKED);
        assert(parser->content_length == 0);
        CROW_STRICT_CHECK(ch != cr);
        parser->state = s_chunk_data_done;
        CROW_CALLBACK_DATA(body);
        break;

      case s_chunk_data_done:
        assert(parser->flags & F_CHUNKED);
        CROW_STRICT_CHECK(ch != lf);
        parser->nread = 0;
        nread = 0;
        parser->state = s_chunk_size_start;
        break;

      default:
        assert(0 && "unhandled state");
        CROW_SET_ERRNO(CHPE_INVALID_INTERNAL_STATE);
        goto error;
    }
  }

  /* Run callbacks for any marks that we have leftover after we ran out of
   * bytes. There should be at most one of these set, so it's OK to invoke
   * them in series (unset marks will not result in callbacks).
   *
   * We use the NOADVANCE() variety of callbacks here because 'p' has already
   * overflowed 'data' and this allows us to correct for the off-by-one that
   * we'd otherwise have (since CROW_CALLBACK_DATA() is meant to be run with a 'p'
   * value that's in-bounds).
   */

  assert(((header_field_mark ? 1 : 0) +
          (header_value_mark ? 1 : 0) +
          (url_mark ? 1 : 0)  +
          (body_mark ? 1 : 0)) <= 1);

  CROW_CALLBACK_DATA_NOADVANCE(header_field);
  CROW_CALLBACK_DATA_NOADVANCE(header_value);
  CROW_CALLBACK_DATA_NOADVANCE(url);
  CROW_CALLBACK_DATA_NOADVANCE(body);

  parser->nread = nread;
  return len;

error:
  if (CROW_HTTP_PARSER_ERRNO(parser) == CHPE_OK) {
    CROW_SET_ERRNO(CHPE_UNKNOWN);
  }

  parser->nread = nread;
  return (p - data);
}

inline void
  http_parser_init(http_parser* parser)
{
  void *data = parser->data; /* preserve application data */
  memset(parser, 0, sizeof(*parser));
  parser->data = data;
  parser->state = s_start_req;
  parser->http_errno = CHPE_OK;
}

/* Return a string name of the given error */
inline const char *
http_errno_name(enum http_errno err) {
/* Map errno values to strings for human-readable output */
#define CROW_HTTP_STRERROR_GEN(n, s) { "CHPE_" #n, s },
static struct {
  const char *name;
  const char *description;
} http_strerror_tab[] = {
  CROW_HTTP_ERRNO_MAP(CROW_HTTP_STRERROR_GEN)
};
#undef CROW_HTTP_STRERROR_GEN
  assert(((size_t) err) < CROW_ARRAY_SIZE(http_strerror_tab));
  return http_strerror_tab[err].name;
}

/* Return a string description of the given error */
inline const char *
http_errno_description(enum http_errno err) {
/* Map errno values to strings for human-readable output */
#define CROW_HTTP_STRERROR_GEN(n, s) { "CHPE_" #n, s },
static struct {
  const char *name;
  const char *description;
} http_strerror_tab[] = {
  CROW_HTTP_ERRNO_MAP(CROW_HTTP_STRERROR_GEN)
};
#undef CROW_HTTP_STRERROR_GEN
  assert(((size_t) err) < CROW_ARRAY_SIZE(http_strerror_tab));
  return http_strerror_tab[err].description;
}

/* Checks if this is the final chunk of the body. */
inline int
http_body_is_final(const struct http_parser *parser) {
    return parser->state == s_message_done;
}

/* Change the maximum header size provided at compile time. */
inline void
http_parser_set_max_header_size(uint32_t size) {
  max_header_size = size;
}

#undef CROW_HTTP_ERRNO_MAP
#undef CROW_SET_ERRNO
#undef CROW_CALLBACK_NOTIFY_
#undef CROW_CALLBACK_NOTIFY
#undef CROW_CALLBACK_NOTIFY_NOADVANCE
#undef CROW_CALLBACK_DATA_
#undef CROW_CALLBACK_DATA
#undef CROW_CALLBACK_DATA_NOADVANCE
#undef CROW_MARK
#undef CROW_PROXY_CONNECTION
#undef CROW_CONNECTION
#undef CROW_CONTENT_LENGTH
#undef CROW_TRANSFER_ENCODING
#undef CROW_UPGRADE
#undef CROW_CHUNKED
#undef CROW_KEEP_ALIVE
#undef CROW_CLOSE
#undef CROW_PARSING_HEADER
#undef CROW_LOWER
#undef CROW_IS_ALPHA
#undef CROW_IS_NUM
#undef CROW_IS_ALPHANUM
//#undef CROW_IS_HEX
#undef CROW_IS_MARK
#undef CROW_IS_USERINFO_CHAR
#undef CROW_TOKEN
#undef CROW_IS_URL_CHAR
//#undef CROW_IS_HOST_CHAR
#undef CROW_STRICT_CHECK

}

// clang-format on


#include <string_view>
#include <locale>
#include <unordered_map>


namespace crow
{
    /// Hashing function for ci_map (unordered_multimap).
    struct ci_hash
    {
        size_t operator()(const std::string_view key) const
        {
            std::size_t seed = 0;
            std::locale locale;

            for (auto c : key)
                hash_combine(seed, std::toupper(c, locale));

            return seed;
        }

    private:
        static inline void hash_combine(std::size_t& seed, char v)
        {
            std::hash<char> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

    /// Equals function for ci_map (unordered_multimap).
    struct ci_key_eq
    {
        bool operator()(const std::string_view l, const std::string_view r) const
        {
            return utility::string_equals(l, r);
        }
    };

    using ci_map = std::unordered_multimap<std::string, std::string, ci_hash, ci_key_eq>;
} // namespace crow


#ifdef CROW_USE_BOOST
#include <boost/asio.hpp>
#else
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#include <asio.hpp>
#endif


namespace crow // NOTE: Already documented in "crow/app.h"
{
#ifdef CROW_USE_BOOST
    namespace asio = boost::asio;
#endif

    /// Find and return the value associated with the key. (returns an empty string if nothing is found)
    template<typename T>
    inline const std::string& get_header_value(const T& headers, const std::string& key)
    {
        if (headers.count(key))
        {
            return headers.find(key)->second;
        }
        static std::string empty;
        return empty;
    }

    /// An HTTP request.
    struct request
    {
        HTTPMethod method;
        std::string raw_url;     ///< The full URL containing the `?` and URL parameters.
        std::string url;         ///< The endpoint without any parameters.
        query_string url_params; ///< The parameters associated with the request. (everything after the `?` in the URL)
        ci_map headers;
        std::string body;
        std::string remote_ip_address; ///< The IP address from which the request was sent.
        unsigned char http_ver_major, http_ver_minor;
        bool keep_alive,    ///< Whether or not the server should send a `connection: Keep-Alive` header to the client.
          close_connection, ///< Whether or not the server should shut down the TCP connection once a response is sent.
          upgrade;          ///< Whether or noth the server should change the HTTP connection to a different connection.

        void* middleware_context{};
        void* middleware_container{};
        asio::io_context* io_context{};

        /// Construct an empty request. (sets the method to `GET`)
        request():
          method(HTTPMethod::Get)
        {}

        /// Construct a request with all values assigned.
        request(HTTPMethod method_, std::string raw_url_, std::string url_, query_string url_params_, ci_map headers_, std::string body_, unsigned char http_major, unsigned char http_minor, bool has_keep_alive, bool has_close_connection, bool is_upgrade):
          method(method_), raw_url(std::move(raw_url_)), url(std::move(url_)), url_params(std::move(url_params_)), headers(std::move(headers_)), body(std::move(body_)), http_ver_major(http_major), http_ver_minor(http_minor), keep_alive(has_keep_alive), close_connection(has_close_connection), upgrade(is_upgrade)
        {}

        void add_header(std::string key, std::string value)
        {
            headers.emplace(std::move(key), std::move(value));
        }

        const std::string& get_header_value(const std::string& key) const
        {
            return crow::get_header_value(headers, key);
        }

        bool check_version(unsigned char major, unsigned char minor) const
        {
            return http_ver_major == major && http_ver_minor == minor;
        }

        /// Get the body as parameters in QS format.

        ///
        /// This is meant to be used with requests of type "application/x-www-form-urlencoded"
        const query_string get_body_params() const
        {
            return query_string(body, false);
        }

        /// Send data to whoever made this request with a completion handler and return immediately.
        template<typename CompletionHandler>
        void post(CompletionHandler handler)
        {
            asio::post(io_context, handler);
        }

        /// Send data to whoever made this request with a completion handler.
        template<typename CompletionHandler>
        void dispatch(CompletionHandler handler)
        {
            asio::dispatch(io_context, handler);
        }
    };
} // namespace crow

#include <array>

namespace crow // NOTE: Already documented in "crow/app.h"
{
#ifdef CROW_USE_BOOST
    namespace asio = boost::asio;
    using error_code = boost::system::error_code;
#else
    using error_code = asio::error_code;
#endif

    /**
     * \namespace crow::websocket
     * \brief Namespace that includes the \ref Connection class
     * and \ref connection struct. Useful for WebSockets connection.
     *
     * Used specially in crow/websocket.h, crow/app.h and crow/routing.h
     */
    namespace websocket
    {
        enum class WebSocketReadState
        {
            MiniHeader,
            Len16,
            Len64,
            Mask,
            Payload,
        };

        // Codes taken from https://www.rfc-editor.org/rfc/rfc6455#section-7.4.1
        enum CloseStatusCode : uint16_t {
            NormalClosure = 1000,
            EndpointGoingAway = 1001,
            ProtocolError = 1002,
            UnacceptableData = 1003,
            InconsistentData = 1007,
            PolicyViolated = 1008,
            MessageTooBig = 1009,
            ExtensionsNotNegotiated = 1010,
            UnexpectedCondition = 1011,

            // Reserved for applications only, should not send/receive these to/from clients
            NoStatusCodePresent = 1005,
            ClosedAbnormally = 1006,
            TLSHandshakeFailure = 1015,

            StartStatusCodesForLibraries = 3000,
            StartStatusCodesForPrivateUse = 4000,
            // Status code should be between 1000 and 4999 inclusive
            StartStatusCodes = NormalClosure,
            EndStatusCodes = 4999,
        };

        /// A base class for websocket connection.
        struct connection
        {
            virtual void send_binary(std::string msg) = 0;
            virtual void send_text(std::string msg) = 0;
            virtual void send_ping(std::string msg) = 0;
            virtual void send_pong(std::string msg) = 0;
            virtual void close(std::string const& msg = "quit", uint16_t status_code = CloseStatusCode::NormalClosure) = 0;
            virtual std::string get_remote_ip() = 0;
            virtual std::string get_subprotocol() const = 0;
            virtual ~connection() = default;

            void userdata(void* u) { userdata_ = u; }
            void* userdata() { return userdata_; }

        private:
            void* userdata_;
        };

        // Modified version of the illustration in RFC6455 Section-5.2
        //
        //
        //  0               1               2               3               -byte
        //  0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 -bit
        // +-+-+-+-+-------+-+-------------+-------------------------------+
        // |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
        // |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
        // |N|V|V|V|       |S|             |   (if payload len==126/127)   |
        // | |1|2|3|       |K|             |                               |
        // +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
        // |     Extended payload length continued, if payload len == 127  |
        // + - - - - - - - - - - - - - - - +-------------------------------+
        // |                               |Masking-key, if MASK set to 1  |
        // +-------------------------------+-------------------------------+
        // | Masking-key (continued)       |          Payload Data         |
        // +-------------------------------- - - - - - - - - - - - - - - - +
        // :                     Payload Data continued ...                :
        // + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
        // |                     Payload Data continued ...                |
        // +---------------------------------------------------------------+
        //

        /// A websocket connection.

        template<typename Adaptor, typename Handler>
        class Connection : public connection
        {
        public:
            /// Constructor for a connection.

            ///
            /// Requires a request with an "Upgrade: websocket" header.<br>
            /// Automatically handles the handshake.
            Connection(const crow::request& req, Adaptor&& adaptor, Handler* handler,
                       uint64_t max_payload, const std::vector<std::string>& subprotocols,
                       std::function<void(crow::websocket::connection&)> open_handler,
                       std::function<void(crow::websocket::connection&, const std::string&, bool)> message_handler,
                       std::function<void(crow::websocket::connection&, const std::string&, uint16_t)> close_handler,
                       std::function<void(crow::websocket::connection&, const std::string&)> error_handler,
                       std::function<bool(const crow::request&, void**)> accept_handler,
                       bool mirror_protocols):
              adaptor_(std::move(adaptor)),
              handler_(handler),
              max_payload_bytes_(max_payload),
              open_handler_(std::move(open_handler)),
              message_handler_(std::move(message_handler)),
              close_handler_(std::move(close_handler)),
              error_handler_(std::move(error_handler)),
              accept_handler_(std::move(accept_handler))
            {
                if (!utility::string_equals(req.get_header_value("upgrade"), "websocket"))
                {
                    adaptor_.close();
                    handler_->remove_websocket(this);
                    delete this;
                    return;
                }

                std::string requested_subprotocols_header = req.get_header_value("Sec-WebSocket-Protocol");
                if (!subprotocols.empty() || !requested_subprotocols_header.empty())
                {
                    auto requested_subprotocols = utility::split(requested_subprotocols_header, ", ");
                    auto subprotocol = utility::find_first_of(subprotocols.begin(), subprotocols.end(), requested_subprotocols.begin(), requested_subprotocols.end());
                    if (subprotocol != subprotocols.end())
                    {
                        subprotocol_ = *subprotocol;
                    }
                }

                if (mirror_protocols & !requested_subprotocols_header.empty())
                {
                    subprotocol_ = requested_subprotocols_header;
                }

                if (accept_handler_)
                {
                    void* ud = nullptr;
                    if (!accept_handler_(req, &ud))
                    {
                        adaptor_.close();
                        handler_->remove_websocket(this);
                        delete this;
                        return;
                    }
                    userdata(ud);
                }

                // Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
                // Sec-WebSocket-Version: 13
                std::string magic = req.get_header_value("Sec-WebSocket-Key") + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
                sha1::SHA1 s;
                s.processBytes(magic.data(), magic.size());
                uint8_t digest[20];
                s.getDigestBytes(digest);

                start(crow::utility::base64encode((unsigned char*)digest, 20));
            }

            ~Connection() noexcept override
            {
                // Do not modify anchor_ here since writing shared_ptr is not atomic.
                auto watch = std::weak_ptr<void>{anchor_};

                // Wait until all unhandled asynchronous operations to join.
                // As the deletion occurs inside 'check_destroy()', which already locks
                //  anchor, use count can be 1 on valid deletion context.
                while (watch.use_count() > 2) // 1 for 'check_destroy() routine', 1 for 'this->anchor_'
                {
                    std::this_thread::yield();
                }
            }

            template<typename Callable>
            struct WeakWrappedMessage
            {
                Callable callable;
                std::weak_ptr<void> watch;

                void operator()()
                {
                    if (auto anchor = watch.lock())
                    {
                        std::move(callable)();
                    }
                }
            };

            /// Send data through the socket.
            template<typename CompletionHandler>
            void dispatch(CompletionHandler&& handler)
            {
                asio::dispatch(adaptor_.get_io_context(),
                               WeakWrappedMessage<typename std::decay<CompletionHandler>::type>{
                                 std::forward<CompletionHandler>(handler), anchor_});
            }

            /// Send data through the socket and return immediately.
            template<typename CompletionHandler>
            void post(CompletionHandler&& handler)
            {
                asio::post(adaptor_.get_io_context(),
                           WeakWrappedMessage<typename std::decay<CompletionHandler>::type>{
                             std::forward<CompletionHandler>(handler), anchor_});
            }

            /// Send a "Ping" message.

            ///
            /// Usually invoked to check if the other point is still online.
            void send_ping(std::string msg) override
            {
                send_data(0x9, std::move(msg));
            }

            /// Send a "Pong" message.

            ///
            /// Usually automatically invoked as a response to a "Ping" message.
            void send_pong(std::string msg) override
            {
                send_data(0xA, std::move(msg));
            }

            /// Send a binary encoded message.
            void send_binary(std::string msg) override
            {
                send_data(0x2, std::move(msg));
            }

            /// Send a plaintext message.
            void send_text(std::string msg) override
            {
                send_data(0x1, std::move(msg));
            }

            /// Send a close signal.

            ///
            /// Sets a flag to destroy the object once the message is sent.
            void close(std::string const& msg, uint16_t status_code) override
            {
                dispatch([this, msg, status_code]() mutable {
                    has_sent_close_ = true;
                    if (has_recv_close_ && !is_close_handler_called_)
                    {
                        is_close_handler_called_ = true;
                        if (close_handler_)
                            close_handler_(*this, msg, status_code);
                    }
                    auto header = build_header(0x8, msg.size() + 2);
                    char status_buf[2];
                    *(uint16_t*)(status_buf) = htons(status_code);

                    write_buffers_.emplace_back(std::move(header));
                    write_buffers_.emplace_back(std::string(status_buf, 2));
                    write_buffers_.emplace_back(msg);
                    do_write();
                });
            }

            std::string get_remote_ip() override
            {
                return adaptor_.address();
            }

            void set_max_payload_size(uint64_t payload)
            {
                max_payload_bytes_ = payload;
            }

            /// Returns the matching client/server subprotocol, empty string if none matched. 
            std::string get_subprotocol() const override
            {
                return subprotocol_;
            }

        protected:
            /// Generate the websocket headers using an opcode and the message size (in bytes).
            std::string build_header(int opcode, size_t size)
            {
                char buf[2 + 8] = "\x80\x00";
                buf[0] += opcode;
                if (size < 126)
                {
                    buf[1] += static_cast<char>(size);
                    return {buf, buf + 2};
                }
                else if (size < 0x10000)
                {
                    buf[1] += 126;
                    *(uint16_t*)(buf + 2) = htons(static_cast<uint16_t>(size));
                    return {buf, buf + 4};
                }
                else
                {
                    buf[1] += 127;
                    *reinterpret_cast<uint64_t*>(buf + 2) = ((1 == htonl(1)) ? static_cast<uint64_t>(size) : (static_cast<uint64_t>(htonl((size)&0xFFFFFFFF)) << 32) | htonl(static_cast<uint64_t>(size) >> 32));
                    return {buf, buf + 10};
                }
            }

            /// Send the HTTP upgrade response.

            ///
            /// Finishes the handshake process, then starts reading messages from the socket.
            void start(std::string&& hello)
            {
                static const std::string header =
                  "HTTP/1.1 101 Switching Protocols\r\n"
                  "Upgrade: websocket\r\n"
                  "Connection: Upgrade\r\n"
                  "Sec-WebSocket-Accept: ";
                write_buffers_.emplace_back(header);
                write_buffers_.emplace_back(std::move(hello));
                write_buffers_.emplace_back(crlf);
                if (!subprotocol_.empty())
                {
                    write_buffers_.emplace_back("Sec-WebSocket-Protocol: ");
                    write_buffers_.emplace_back(subprotocol_);
                    write_buffers_.emplace_back(crlf);
                }
                write_buffers_.emplace_back(crlf);
                do_write();
                if (open_handler_)
                    open_handler_(*this);
                do_read();
            }

            /// Read a websocket message.

            ///
            /// Involves:<br>
            /// Handling headers (opcodes, size).<br>
            /// Unmasking the payload.<br>
            /// Reading the actual payload.<br>
            void do_read()
            {
                if (has_sent_close_ && has_recv_close_)
                {
                    close_connection_ = true;
                    adaptor_.shutdown_readwrite();
                    adaptor_.close();
                    check_destroy();
                    return;
                }

                is_reading = true;
                switch (state_)
                {
                    case WebSocketReadState::MiniHeader:
                    {
                        mini_header_ = 0;
                        //asio::async_read(adaptor_.socket(), asio::buffer(&mini_header_, 1),
                        adaptor_.socket().async_read_some(
                          asio::buffer(&mini_header_, 2),
                          [this](const error_code& ec, std::size_t
#ifdef CROW_ENABLE_DEBUG
                                                               bytes_transferred
#endif
                          )

                          {
                              is_reading = false;
                              mini_header_ = ntohs(mini_header_);
#ifdef CROW_ENABLE_DEBUG

                              if (!ec && bytes_transferred != 2)
                              {
                                  throw std::runtime_error("WebSocket:MiniHeader:async_read fail:asio bug?");
                              }
#endif

                              if (!ec)
                              {
                                  if ((mini_header_ & 0x80) == 0x80)
                                      has_mask_ = true;
                                  else //if the websocket specification is enforced and the message isn't masked, terminate the connection
                                  {
#ifndef CROW_ENFORCE_WS_SPEC
                                      has_mask_ = false;
#else
                                      close_connection_ = true;
                                      adaptor_.shutdown_readwrite();
                                      adaptor_.close();
                                      if (error_handler_)
                                          error_handler_(*this, "Client connection not masked.");
                                      check_destroy(CloseStatusCode::UnacceptableData);
#endif
                                  }

                                  if ((mini_header_ & 0x7f) == 127)
                                  {
                                      state_ = WebSocketReadState::Len64;
                                  }
                                  else if ((mini_header_ & 0x7f) == 126)
                                  {
                                      state_ = WebSocketReadState::Len16;
                                  }
                                  else
                                  {
                                      remaining_length_ = mini_header_ & 0x7f;
                                      state_ = WebSocketReadState::Mask;
                                  }
                                  do_read();
                              }
                              else
                              {
                                  close_connection_ = true;
                                  adaptor_.shutdown_readwrite();
                                  adaptor_.close();
                                  if (error_handler_)
                                      error_handler_(*this, ec.message());
                                  check_destroy();
                              }
                          });
                    }
                    break;
                    case WebSocketReadState::Len16:
                    {
                        remaining_length_ = 0;
                        remaining_length16_ = 0;
                        asio::async_read(
                          adaptor_.socket(), asio::buffer(&remaining_length16_, 2),
                          [this](const error_code& ec, std::size_t
#ifdef CROW_ENABLE_DEBUG
                                                               bytes_transferred
#endif
                          ) {
                              is_reading = false;
                              remaining_length16_ = ntohs(remaining_length16_);
                              remaining_length_ = remaining_length16_;
#ifdef CROW_ENABLE_DEBUG
                              if (!ec && bytes_transferred != 2)
                              {
                                  throw std::runtime_error("WebSocket:Len16:async_read fail:asio bug?");
                              }
#endif

                              if (!ec)
                              {
                                  state_ = WebSocketReadState::Mask;
                                  do_read();
                              }
                              else
                              {
                                  close_connection_ = true;
                                  adaptor_.shutdown_readwrite();
                                  adaptor_.close();
                                  if (error_handler_)
                                      error_handler_(*this, ec.message());
                                  check_destroy();
                              }
                          });
                    }
                    break;
                    case WebSocketReadState::Len64:
                    {
                        asio::async_read(
                          adaptor_.socket(), asio::buffer(&remaining_length_, 8),
                          [this](const error_code& ec, std::size_t
#ifdef CROW_ENABLE_DEBUG
                                                               bytes_transferred
#endif
                          ) {
                              is_reading = false;
                              remaining_length_ = ((1 == ntohl(1)) ? (remaining_length_) : (static_cast<uint64_t>(ntohl((remaining_length_)&0xFFFFFFFF)) << 32) | ntohl((remaining_length_) >> 32));
#ifdef CROW_ENABLE_DEBUG
                              if (!ec && bytes_transferred != 8)
                              {
                                  throw std::runtime_error("WebSocket:Len16:async_read fail:asio bug?");
                              }
#endif

                              if (!ec)
                              {
                                  state_ = WebSocketReadState::Mask;
                                  do_read();
                              }
                              else
                              {
                                  close_connection_ = true;
                                  adaptor_.shutdown_readwrite();
                                  adaptor_.close();
                                  if (error_handler_)
                                      error_handler_(*this, ec.message());
                                  check_destroy();
                              }
                          });
                    }
                    break;
                    case WebSocketReadState::Mask:
                        if (remaining_length_ > max_payload_bytes_)
                        {
                            close_connection_ = true;
                            adaptor_.close();
                            if (error_handler_)
                                error_handler_(*this, "Message length exceeds maximum payload.");
                            check_destroy(MessageTooBig);
                        }
                        else if (has_mask_)
                        {
                            asio::async_read(
                              adaptor_.socket(), asio::buffer((char*)&mask_, 4),
                              [this](const error_code& ec, std::size_t
#ifdef CROW_ENABLE_DEBUG
                                                                   bytes_transferred
#endif
                              ) {
                                  is_reading = false;
#ifdef CROW_ENABLE_DEBUG
                                  if (!ec && bytes_transferred != 4)
                                  {
                                      throw std::runtime_error("WebSocket:Mask:async_read fail:asio bug?");
                                  }
#endif

                                  if (!ec)
                                  {
                                      state_ = WebSocketReadState::Payload;
                                      do_read();
                                  }
                                  else
                                  {
                                      close_connection_ = true;
                                      if (error_handler_)
                                          error_handler_(*this, ec.message());
                                      adaptor_.shutdown_readwrite();
                                      adaptor_.close();
                                      check_destroy();
                                  }
                              });
                        }
                        else
                        {
                            state_ = WebSocketReadState::Payload;
                            do_read();
                        }
                        break;
                    case WebSocketReadState::Payload:
                    {
                        auto to_read = static_cast<std::uint64_t>(buffer_.size());
                        if (remaining_length_ < to_read)
                            to_read = remaining_length_;
                        adaptor_.socket().async_read_some(
                          asio::buffer(buffer_, static_cast<std::size_t>(to_read)),
                          [this](const error_code& ec, std::size_t bytes_transferred) {
                              is_reading = false;

                              if (!ec)
                              {
                                  fragment_.insert(fragment_.end(), buffer_.begin(), buffer_.begin() + bytes_transferred);
                                  remaining_length_ -= bytes_transferred;
                                  if (remaining_length_ == 0)
                                  {
                                      if (handle_fragment())
                                      {
                                          state_ = WebSocketReadState::MiniHeader;
                                          do_read();
                                      }
                                  }
                                  else
                                      do_read();
                              }
                              else
                              {
                                  close_connection_ = true;
                                  if (error_handler_)
                                      error_handler_(*this, ec.message());
                                  adaptor_.shutdown_readwrite();
                                  adaptor_.close();
                                  check_destroy();
                              }
                          });
                    }
                    break;
                }
            }

            /// Check if the FIN bit is set.
            bool is_FIN()
            {
                return mini_header_ & 0x8000;
            }

            /// Extract the opcode from the header.
            int opcode()
            {
                return (mini_header_ & 0x0f00) >> 8;
            }

            /// Process the payload fragment.

            ///
            /// Unmasks the fragment, checks the opcode, merges fragments into 1 message body, and calls the appropriate handler.
            bool handle_fragment()
            {
                if (has_mask_)
                {
                    for (decltype(fragment_.length()) i = 0; i < fragment_.length(); i++)
                    {
                        fragment_[i] ^= ((char*)&mask_)[i % 4];
                    }
                }
                switch (opcode())
                {
                    case 0: // Continuation
                    {
                        message_ += fragment_;
                        if (is_FIN())
                        {
                            if (message_handler_)
                                message_handler_(*this, message_, is_binary_);
                            message_.clear();
                        }
                    }
                    break;
                    case 1: // Text
                    {
                        is_binary_ = false;
                        message_ += fragment_;
                        if (is_FIN())
                        {
                            if (message_handler_)
                                message_handler_(*this, message_, is_binary_);
                            message_.clear();
                        }
                    }
                    break;
                    case 2: // Binary
                    {
                        is_binary_ = true;
                        message_ += fragment_;
                        if (is_FIN())
                        {
                            if (message_handler_)
                                message_handler_(*this, message_, is_binary_);
                            message_.clear();
                        }
                    }
                    break;
                    case 0x8: // Close
                    {
                        has_recv_close_ = true;


                        uint16_t status_code = NoStatusCodePresent;
                        std::string::size_type message_start = 2;
                        if (fragment_.size() >= 2)
                        {
                            status_code = ntohs(((uint16_t*)fragment_.data())[0]);
                        } else {
                            // no message will crash substr
                            message_start = 0;
                        }

                        if (!has_sent_close_)
                        {
                            close(fragment_.substr(message_start), status_code);
                        }
                        else
                        {

                            close_connection_ = true;
                            if (!is_close_handler_called_)
                            {
                                if (close_handler_)
                                    close_handler_(*this, fragment_.substr(message_start), status_code);
                                is_close_handler_called_ = true;
                            }
                            adaptor_.shutdown_readwrite();
                            adaptor_.close();

                            // Close handler must have been called at this point so code does not matter
                            check_destroy();
                            return false;
                        }
                    }
                    break;
                    case 0x9: // Ping
                    {
                        send_pong(fragment_);
                    }
                    break;
                    case 0xA: // Pong
                    {
                        pong_received_ = true;
                    }
                    break;
                }

                fragment_.clear();
                return true;
            }

            /// Send the buffers' data through the socket.

            ///
            /// Also destroys the object if the Close flag is set.
            void do_write()
            {
                if (sending_buffers_.empty())
                {
                    sending_buffers_.swap(write_buffers_);
                    std::vector<asio::const_buffer> buffers;
                    buffers.reserve(sending_buffers_.size());
                    for (auto& s : sending_buffers_)
                    {
                        buffers.emplace_back(asio::buffer(s));
                    }
                    auto watch = std::weak_ptr<void>{anchor_};
                    asio::async_write(
                      adaptor_.socket(), buffers,
                      [&, watch](const error_code& ec, std::size_t /*bytes_transferred*/) {
                          if (!ec && !close_connection_)
                          {
                              sending_buffers_.clear();
                              if (!write_buffers_.empty())
                                  do_write();
                              if (has_sent_close_)
                                  close_connection_ = true;
                          }
                          else
                          {
                              auto anchor = watch.lock();
                              if (anchor == nullptr) { return; }

                              sending_buffers_.clear();
                              close_connection_ = true;
                              check_destroy();
                          }
                      });
                }
            }

            /// Destroy the Connection.
            void check_destroy(websocket::CloseStatusCode code = CloseStatusCode::ClosedAbnormally)
            {
                // Note that if the close handler was not yet called at this point we did not receive a close packet (or send one)
                // and thus we use ClosedAbnormally unless instructed otherwise
                if (!is_close_handler_called_)
                    if (close_handler_)
                        close_handler_(*this, "uncleanly", code);
                handler_->remove_websocket(this);
                if (sending_buffers_.empty() && !is_reading)
                    delete this;
            }


            struct SendMessageType
            {
                std::string payload;
                Connection* self;
                int opcode;

                void operator()()
                {
                    self->send_data_impl(this);
                }
            };

            void send_data_impl(SendMessageType* s)
            {
                auto header = build_header(s->opcode, s->payload.size());
                write_buffers_.emplace_back(std::move(header));
                write_buffers_.emplace_back(std::move(s->payload));
                do_write();
            }

            void send_data(int opcode, std::string&& msg)
            {
                SendMessageType event_arg{
                  std::move(msg),
                  this,
                  opcode};

                post(std::move(event_arg));
            }

        private:
            Adaptor adaptor_;
            Handler* handler_;

            std::vector<std::string> sending_buffers_;
            std::vector<std::string> write_buffers_;

            std::array<char, 4096> buffer_;
            bool is_binary_;
            std::string message_;
            std::string fragment_;
            WebSocketReadState state_{WebSocketReadState::MiniHeader};
            uint16_t remaining_length16_{0};
            uint64_t remaining_length_{0};
            uint64_t max_payload_bytes_{UINT64_MAX};
            std::string subprotocol_;
            bool close_connection_{false};
            bool is_reading{false};
            bool has_mask_{false};
            uint32_t mask_;
            uint16_t mini_header_;
            bool has_sent_close_{false};
            bool has_recv_close_{false};
            bool error_occurred_{false};
            bool pong_received_{false};
            bool is_close_handler_called_{false};

            std::shared_ptr<void> anchor_ = std::make_shared<int>(); // Value is just for placeholding

            std::function<void(crow::websocket::connection&)> open_handler_;
            std::function<void(crow::websocket::connection&, const std::string&, bool)> message_handler_;
            std::function<void(crow::websocket::connection&, const std::string&, uint16_t status_code)> close_handler_;
            std::function<void(crow::websocket::connection&, const std::string&)> error_handler_;
            std::function<bool(const crow::request&, void**)> accept_handler_;
        };
    } // namespace websocket
} // namespace crow


#include <string>
#include <unordered_map>
#include <algorithm>


namespace crow
{
    /// A wrapper for `nodejs/http-parser`.

    ///
    /// Used to generate a \ref crow.request from the TCP socket buffer.
    template<typename Handler>
    struct HTTPParser : public http_parser
    {
        static int on_message_begin(http_parser*)
        {
            return 0;
        }
        static int on_method(http_parser* self_)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);
            self->req.method = static_cast<HTTPMethod>(self->method);

            return 0;
        }
        static int on_url(http_parser* self_, const char* at, size_t length)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);
            self->req.raw_url.insert(self->req.raw_url.end(), at, at + length);
            self->req.url_params = query_string(self->req.raw_url);
            self->req.url = self->req.raw_url.substr(0, self->qs_point != 0 ? self->qs_point : std::string::npos);

            self->process_url();

            return 0;
        }
        static int on_header_field(http_parser* self_, const char* at, size_t length)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);
            switch (self->header_building_state)
            {
                case 0:
                    if (!self->header_value.empty())
                    {
                        self->req.headers.emplace(std::move(self->header_field), std::move(self->header_value));
                    }
                    self->header_field.assign(at, at + length);
                    self->header_building_state = 1;
                    break;
                case 1:
                    self->header_field.insert(self->header_field.end(), at, at + length);
                    break;
            }
            return 0;
        }
        static int on_header_value(http_parser* self_, const char* at, size_t length)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);
            switch (self->header_building_state)
            {
                case 0:
                    self->header_value.insert(self->header_value.end(), at, at + length);
                    break;
                case 1:
                    self->header_building_state = 0;
                    self->header_value.assign(at, at + length);
                    break;
            }
            return 0;
        }
        static int on_headers_complete(http_parser* self_)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);
            if (!self->header_field.empty())
            {
                self->req.headers.emplace(std::move(self->header_field), std::move(self->header_value));
            }

            self->set_connection_parameters();

            self->process_header();
            return 0;
        }
        static int on_body(http_parser* self_, const char* at, size_t length)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);
            self->req.body.insert(self->req.body.end(), at, at + length);
            return 0;
        }
        static int on_message_complete(http_parser* self_)
        {
            HTTPParser* self = static_cast<HTTPParser*>(self_);

            self->message_complete = true;
            self->process_message();
            return 0;
        }
        HTTPParser(Handler* handler):
          http_parser(),
          handler_(handler)
        {
            http_parser_init(this);
        }

        // return false on error
        /// Parse a buffer into the different sections of an HTTP request.
        bool feed(const char* buffer, int length)
        {
            if (message_complete)
                return true;

            const static http_parser_settings settings_{
              on_message_begin,
              on_method,
              on_url,
              on_header_field,
              on_header_value,
              on_headers_complete,
              on_body,
              on_message_complete,
            };

            int nparsed = http_parser_execute(this, &settings_, buffer, length);
            if (http_errno != CHPE_OK)
            {
                return false;
            }
            return nparsed == length;
        }

        bool done()
        {
            return feed(nullptr, 0);
        }

        void clear()
        {
            req = crow::request();
            header_field.clear();
            header_value.clear();
            header_building_state = 0;
            qs_point = 0;
            message_complete = false;
            state = CROW_NEW_MESSAGE();
        }

        inline void process_url()
        {
            handler_->handle_url();
        }

        inline void process_header()
        {
            handler_->handle_header();
        }

        inline void process_message()
        {
            handler_->handle();
        }

        inline void set_connection_parameters()
        {
            req.http_ver_major = http_major;
            req.http_ver_minor = http_minor;

            //NOTE(EDev): it seems that the problem is with crow's policy on closing the connection for HTTP_VERSION < 1.0, the behaviour for that in crow is "don't close the connection, but don't send a keep-alive either"

            // HTTP1.1 = always send keep_alive, HTTP1.0 = only send if header exists, HTTP?.? = never send
            req.keep_alive = (http_major == 1 && http_minor == 0) ?
                               ((flags & F_CONNECTION_KEEP_ALIVE) ? true : false) :
                               ((http_major == 1 && http_minor == 1) ? true : false);

            // HTTP1.1 = only close if close header exists, HTTP1.0 = always close unless keep_alive header exists, HTTP?.?= never close
            req.close_connection = (http_major == 1 && http_minor == 0) ?
                                     ((flags & F_CONNECTION_KEEP_ALIVE) ? false : true) :
                                     ((http_major == 1 && http_minor == 1) ? ((flags & F_CONNECTION_CLOSE) ? true : false) : false);
            req.upgrade = static_cast<bool>(upgrade);
        }

        /// The final request that this parser outputs.
        ///
        /// Data parsed is put directly into this object as soon as the related callback returns. (e.g. the request will have the cooorect method as soon as on_method() returns)
        request req;

    private:
        int header_building_state = 0;
        bool message_complete = false;
        std::string header_field;
        std::string header_value;

        Handler* handler_; ///< This is currently an HTTP connection object (\ref crow.Connection).
    };
} // namespace crow

#undef CROW_NEW_MESSAGE
#undef CROW_start_state


#include <string>
#include <vector>
#include <sstream>


namespace crow
{

    /// Encapsulates anything related to processing and organizing `multipart/xyz` messages
    namespace multipart
    {

        const std::string dd = "--";

        /// The first part in a section, contains metadata about the part
        struct header
        {
            std::string value;                                   ///< The first part of the header, usually `Content-Type` or `Content-Disposition`
            std::unordered_map<std::string, std::string> params; ///< The parameters of the header, come after the `value`

            operator int() const { return std::stoi(value); }    ///< Returns \ref value as integer
            operator double() const { return std::stod(value); } ///< Returns \ref value as double
        };

        /// Multipart header map (key is header key).
        using mph_map = std::unordered_multimap<std::string, header, ci_hash, ci_key_eq>;

        /// Find and return the value object associated with the key. (returns an empty class if nothing is found)
        template<typename O, typename T>
        inline const O& get_header_value_object(const T& headers, const std::string& key)
        {
            if (headers.count(key))
            {
                return headers.find(key)->second;
            }
            static O empty;
            return empty;
        }

        /// Same as \ref get_header_value_object() but for \ref multipart.header
        template<typename T>
        inline const header& get_header_object(const T& headers, const std::string& key)
        {
            return get_header_value_object<header>(headers, key);
        }

        ///One part of the multipart message

        ///
        /// It is usually separated from other sections by a `boundary`
        struct part
        {
            mph_map headers;  ///< (optional) The first part before the data, Contains information regarding the type of data and encoding
            std::string body; ///< The actual data in the part

            operator int() const { return std::stoi(body); }    ///< Returns \ref body as integer
            operator double() const { return std::stod(body); } ///< Returns \ref body as double

            const header& get_header_object(const std::string& key) const
            {
                return multipart::get_header_object(headers, key);
            }
        };

        /// Multipart map (key is the name parameter).
        using mp_map = std::unordered_multimap<std::string, part, ci_hash, ci_key_eq>;

        /// The parsed multipart request/response
        struct message : public returnable
        {
            ci_map headers;          ///< The request/response headers
            std::string boundary;    ///< The text boundary that separates different `parts`
            std::vector<part> parts; ///< The individual parts of the message
            mp_map part_map;         ///< The individual parts of the message, organized in a map with the `name` header parameter being the key

            const std::string& get_header_value(const std::string& key) const
            {
                return crow::get_header_value(headers, key);
            }

            part get_part_by_name(const std::string& name)
            {
                mp_map::iterator result = part_map.find(name);
                if (result != part_map.end())
                    return result->second;
                else
                    return {};
            }

            /// Represent all parts as a string (**does not include message headers**)
            std::string dump() const override
            {
                std::stringstream str;
                std::string delimiter = dd + boundary;

                for (unsigned i = 0; i < parts.size(); i++)
                {
                    str << delimiter << crlf;
                    str << dump(i);
                }
                str << delimiter << dd << crlf;
                return str.str();
            }

            /// Represent an individual part as a string
            std::string dump(int part_) const
            {
                std::stringstream str;
                part item = parts[part_];
                for (auto& item_h : item.headers)
                {
                    str << item_h.first << ": " << item_h.second.value;
                    for (auto& it : item_h.second.params)
                    {
                        str << "; " << it.first << '=' << pad(it.second);
                    }
                    str << crlf;
                }
                str << crlf;
                str << item.body << crlf;
                return str.str();
            }

            /// Default constructor using default values
            message(const ci_map& headers_, const std::string& boundary_, const std::vector<part>& sections):
              returnable("multipart/form-data; boundary=CROW-BOUNDARY"), headers(headers_), boundary(boundary_), parts(sections)
            {
                if (!boundary.empty())
                    content_type = "multipart/form-data; boundary=" + boundary;
                for (auto& item : parts)
                {
                    part_map.emplace(
                      (get_header_object(item.headers, "Content-Disposition").params.find("name")->second),
                      item);
                }
            }

            /// Create a multipart message from a request data
            explicit message(const request& req):
              returnable("multipart/form-data; boundary=CROW-BOUNDARY"),
              headers(req.headers),
              boundary(get_boundary(get_header_value("Content-Type")))
            {
                if (!boundary.empty())
                {
                    content_type = "multipart/form-data; boundary=" + boundary;
                    parse_body(req.body);
                }
                else
                {
                    throw bad_request("Empty boundary in multipart message");
                }
            }

        private:
            std::string get_boundary(const std::string& header) const
            {
                constexpr char boundary_text[] = "boundary=";
                size_t found = header.find(boundary_text);
                if (found != std::string::npos)
                {
                    std::string to_return(header.substr(found + strlen(boundary_text)));
                    if (to_return[0] == '\"')
                    {
                        to_return = to_return.substr(1, to_return.length() - 2);
                    }
                    return to_return;
                }
                return std::string();
            }

            void parse_body(std::string body)
            {
                std::string delimiter = dd + boundary;

                // TODO(EDev): Exit on error
                while (body != (crlf))
                {
                    size_t found = body.find(delimiter);
                    if (found == std::string::npos)
                    {
                        // did not find delimiter; probably an ill-formed body; throw to indicate the issue to user
                        throw bad_request("Unable to find delimiter in multipart message. Probably ill-formed body");
                    }
                    std::string section = body.substr(0, found);

                    // +2 is the CRLF.
                    // We don't check it and delete it so that the same delimiter can be used for The last delimiter (--delimiter--CRLF).
                    body.erase(0, found + delimiter.length() + 2);
                    if (!section.empty())
                    {
                        part parsed_section(parse_section(section));
                        part_map.emplace(
                          (get_header_object(parsed_section.headers, "Content-Disposition").params.find("name")->second),
                          parsed_section);
                        parts.push_back(std::move(parsed_section));
                    }
                }
            }

            part parse_section(std::string& section)
            {
                struct part to_return;

                size_t found = section.find(crlf + crlf);
                std::string head_line = section.substr(0, found + 2);
                section.erase(0, found + 4);

                parse_section_head(head_line, to_return);
                to_return.body = section.substr(0, section.length() - 2);
                return to_return;
            }

            void parse_section_head(std::string& lines, part& part)
            {
                while (!lines.empty())
                {
                    header to_add;

                    const size_t found_crlf = lines.find(crlf);
                    std::string line = lines.substr(0, found_crlf);
                    std::string key;
                    lines.erase(0, found_crlf + 2);
                    // Add the header if available
                    if (!line.empty())
                    {
                        const size_t found_semicolon = line.find("; ");
                        std::string header = line.substr(0, found_semicolon);
                        if (found_semicolon != std::string::npos)
                            line.erase(0, found_semicolon + 2);
                        else
                            line = std::string();

                        size_t header_split = header.find(": ");
                        key = header.substr(0, header_split);

                        to_add.value = header.substr(header_split + 2);
                    }

                    // Add the parameters
                    while (!line.empty())
                    {
                        const size_t found_semicolon = line.find("; ");
                        std::string param = line.substr(0, found_semicolon);
                        if (found_semicolon != std::string::npos)
                            line.erase(0, found_semicolon + 2);
                        else
                            line = std::string();

                        size_t param_split = param.find('=');

                        std::string value = param.substr(param_split + 1);

                        to_add.params.emplace(param.substr(0, param_split), trim(value));
                    }
                    part.headers.emplace(key, to_add);
                }
            }

            inline std::string trim(std::string& string, const char& excess = '"') const
            {
                if (string.length() > 1 && string[0] == excess && string[string.length() - 1] == excess)
                    return string.substr(1, string.length() - 2);
                return string;
            }

            inline std::string pad(std::string& string, const char& padding = '"') const
            {
                return (padding + string + padding);
            }
        };
    } // namespace multipart
} // namespace crow


#include <charconv>
#include <string>
#include <vector>
#include <string_view>
#include <sstream>

// for crow::multipart::dd

namespace crow
{

    /// Encapsulates anything related to processing and organizing `multipart/xyz` messages
    namespace multipart
    {
        /// The first part in a section, contains metadata about the part
        struct header_view
        {
            std::string_view value;                                        ///< The first part of the header, usually `Content-Type` or `Content-Disposition`
            std::unordered_map<std::string_view, std::string_view> params; ///< The parameters of the header, come after the `value`

            /// Returns \ref value as integer
            operator int() const
            {
                int result = 0;
                std::from_chars(value.data(), value.data() + value.size(), result);
                return result;
            }

            /// Returns \ref value as double
            operator double() const
            {
                // There's no std::from_chars for floating-point types in a lot of STLs
                return std::stod(static_cast<std::string>(value));
            }
        };

        /// Multipart header map (key is header key).
        using mph_view_map = std::unordered_multimap<std::string_view, header_view, ci_hash, ci_key_eq>;

        /// Finds and returns the header with the specified key. (returns an empty header if nothing is found)
        inline const header_view& get_header_object(const mph_view_map& headers, const std::string_view key)
        {
            const auto header = headers.find(key);
            if (header != headers.cend())
            {
                return header->second;
            }

            static header_view empty;
            return empty;
        }

        /// String padded with the specified padding (double quotes by default)
        struct padded
        {
            std::string_view value;   ///< String to pad
            const char padding = '"'; ///< Padding to use

            /// Outputs padded value to the stream
            friend std::ostream& operator<<(std::ostream& stream, const padded value_)
            {
                return stream << value_.padding << value_.value << value_.padding;
            }
        };

        ///One part of the multipart message

        ///
        /// It is usually separated from other sections by a `boundary`
        struct part_view
        {
            mph_view_map headers;  ///< (optional) The first part before the data, Contains information regarding the type of data and encoding
            std::string_view body; ///< The actual data in the part

            /// Returns \ref body as integer
            operator int() const
            {
                int result = 0;
                std::from_chars(body.data(), body.data() + body.size(), result);
                return result;
            }

            /// Returns \ref body as double
            operator double() const
            {
                // There's no std::from_chars for floating-point types in a lot of STLs
                return std::stod(static_cast<std::string>(body));
            }

            const header_view& get_header_object(const std::string_view key) const
            {
                return multipart::get_header_object(headers, key);
            }

            friend std::ostream& operator<<(std::ostream& stream, const part_view& part)
            {
                for (const auto& [header_key, header_value] : part.headers)
                {
                    stream << header_key << ": " << header_value.value;
                    for (const auto& [param_key, param_value] : header_value.params)
                    {
                        stream << "; " << param_key << '=' << padded{param_value};
                    }
                    stream << crlf;
                }
                stream << crlf;
                stream << part.body << crlf;
                return stream;
            }
        };

        /// Multipart map (key is the name parameter).
        using mp_view_map = std::unordered_multimap<std::string_view, part_view, ci_hash, ci_key_eq>;

        /// The parsed multipart request/response
        struct message_view
        {
            std::reference_wrapper<const ci_map> headers; ///< The request/response headers
            std::string boundary;                         ///< The text boundary that separates different `parts`
            std::vector<part_view> parts;                 ///< The individual parts of the message
            mp_view_map part_map;                         ///< The individual parts of the message, organized in a map with the `name` header parameter being the key

            const std::string& get_header_value(const std::string& key) const
            {
                return crow::get_header_value(headers.get(), key);
            }

            part_view get_part_by_name(const std::string_view name)
            {
                mp_view_map::iterator result = part_map.find(name);
                if (result != part_map.end())
                    return result->second;
                else
                    return {};
            }

            friend std::ostream& operator<<(std::ostream& stream, const message_view message)
            {
                std::string delimiter = dd + message.boundary;

                for (const part_view& part : message.parts)
                {
                    stream << delimiter << crlf;
                    stream << part;
                }
                stream << delimiter << dd << crlf;

                return stream;
            }

            /// Represent all parts as a string (**does not include message headers**)
            std::string dump() const
            {
                std::ostringstream str;
                str << *this;
                return std::move(str).str();
            }

            /// Represent an individual part as a string
            std::string dump(int part_) const
            {
                std::ostringstream str;
                str << parts.at(part_);
                return std::move(str).str();
            }

            /// Default constructor using default values
            message_view(const ci_map& headers_, const std::string& boundary_, const std::vector<part_view>& sections):
              headers(headers_), boundary(boundary_), parts(sections)
            {
                for (const part_view& item : parts)
                {
                    part_map.emplace(
                      (get_header_object(item.headers, "Content-Disposition").params.find("name")->second),
                      item);
                }
            }

            /// Create a multipart message from a request data
            explicit message_view(const request& req):
              headers(req.headers),
              boundary(get_boundary(get_header_value("Content-Type")))
            {
                parse_body(req.body);
            }

        private:
            std::string_view get_boundary(const std::string_view header) const
            {
                constexpr std::string_view boundary_text = "boundary=";
                const size_t found = header.find(boundary_text);
                if (found == std::string_view::npos)
                {
                    return std::string_view();
                }

                const std::string_view to_return = header.substr(found + boundary_text.size());
                if (to_return[0] == '\"')
                {
                    return to_return.substr(1, to_return.length() - 2);
                }
                return to_return;
            }

            void parse_body(std::string_view body)
            {
                const std::string delimiter = dd + boundary;

                // TODO(EDev): Exit on error
                while (body != (crlf))
                {
                    const size_t found = body.find(delimiter);
                    if (found == std::string_view::npos)
                    {
                        // did not find delimiter; probably an ill-formed body; ignore the rest
                        break;
                    }

                    const std::string_view section = body.substr(0, found);

                    // +2 is the CRLF.
                    // We don't check it and delete it so that the same delimiter can be used for The last delimiter (--delimiter--CRLF).
                    body = body.substr(found + delimiter.length() + 2);
                    if (!section.empty())
                    {
                        part_view parsed_section = parse_section(section);
                        part_map.emplace(
                          (get_header_object(parsed_section.headers, "Content-Disposition").params.find("name")->second),
                          parsed_section);
                        parts.push_back(std::move(parsed_section));
                    }
                }
            }

            part_view parse_section(std::string_view section)
            {
                constexpr static std::string_view crlf2 = "\r\n\r\n";

                const size_t found = section.find(crlf2);
                const std::string_view head_line = section.substr(0, found + 2);
                section = section.substr(found + 4);

                return part_view{
                  parse_section_head(head_line),
                  section.substr(0, section.length() - 2),
                };
            }

            mph_view_map parse_section_head(std::string_view lines)
            {
                mph_view_map result;

                while (!lines.empty())
                {
                    header_view to_add;

                    const size_t found_crlf = lines.find(crlf);
                    std::string_view line = lines.substr(0, found_crlf);
                    std::string_view key;
                    lines = lines.substr(found_crlf + 2);
                    // Add the header if available
                    if (!line.empty())
                    {
                        const size_t found_semicolon = line.find("; ");
                        std::string_view header = line.substr(0, found_semicolon);
                        if (found_semicolon != std::string_view::npos)
                            line = line.substr(found_semicolon + 2);
                        else
                            line = std::string_view();

                        const size_t header_split = header.find(": ");
                        key = header.substr(0, header_split);

                        to_add.value = header.substr(header_split + 2);
                    }

                    // Add the parameters
                    while (!line.empty())
                    {
                        const size_t found_semicolon = line.find("; ");
                        std::string_view param = line.substr(0, found_semicolon);
                        if (found_semicolon != std::string_view::npos)
                            line = line.substr(found_semicolon + 2);
                        else
                            line = std::string_view();

                        const size_t param_split = param.find('=');

                        const std::string_view value = param.substr(param_split + 1);

                        to_add.params.emplace(param.substr(0, param_split), trim(value));
                    }
                    result.emplace(key, to_add);
                }

                return result;
            }

            inline std::string_view trim(const std::string_view string, const char excess = '"') const
            {
                if (string.length() > 1 && string[0] == excess && string[string.length() - 1] == excess)
                    return string.substr(1, string.length() - 2);
                return string;
            }
        };
    } // namespace multipart
} // namespace crow

#include <string>
#include <unordered_map>
#include <ios>
#include <fstream>
#include <sstream>
// S_ISREG is not defined for windows
// This defines it like suggested in https://stackoverflow.com/a/62371749
#if defined(_MSC_VER)
#define _CRT_INTERNAL_NONSTDC_NAMES 1
#endif
#include <sys/stat.h>
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif



namespace crow
{
    template<typename Adaptor, typename Handler, typename... Middlewares>
    class Connection;

    class Router;

    /// HTTP response
    struct response
    {
        template<typename Adaptor, typename Handler, typename... Middlewares>
        friend class crow::Connection;

        friend class Router;

        int code{200};    ///< The Status code for the response.
        std::string body; ///< The actual payload containing the response data.
        ci_map headers;   ///< HTTP headers.

#ifdef CROW_ENABLE_COMPRESSION
        bool compressed = true; ///< If compression is enabled and this is false, the individual response will not be compressed.
#endif
        bool skip_body = false;            ///< Whether this is a response to a HEAD request.
        bool manual_length_header = false; ///< Whether Crow should automatically add a "Content-Length" header.

        /// Set the value of an existing header in the response.
        void set_header(std::string key, std::string value)
        {
            headers.erase(key);
            headers.emplace(std::move(key), std::move(value));
        }

        /// Add a new header to the response.
        void add_header(std::string key, std::string value)
        {
            headers.emplace(std::move(key), std::move(value));
        }

        const std::string& get_header_value(const std::string& key)
        {
            return crow::get_header_value(headers, key);
        }

        // naive validation of a mime-type string
        static bool validate_mime_type(const std::string& candidate) noexcept
        {
            // Here we simply check that the candidate type starts with
            // a valid parent type, and has at least one character afterwards.
            std::array<std::string, 10> valid_parent_types = {
              "application/", "audio/", "font/", "example/",
              "image/", "message/", "model/", "multipart/",
              "text/", "video/"};
            for (const std::string& parent : valid_parent_types)
            {
                // ensure the candidate is *longer* than the parent,
                // to avoid unnecessary string comparison and to
                // reject zero-length subtypes.
                if (candidate.size() <= parent.size())
                {
                    continue;
                }
                // strncmp is used rather than substr to avoid allocation,
                // but a string_view approach would be better if Crow
                // migrates to C++17.
                if (strncmp(parent.c_str(), candidate.c_str(), parent.size()) == 0)
                {
                    return true;
                }
            }
            return false;
        }

        // Find the mime type from the content type either by lookup,
        // or by the content type itself, if it is a valid a mime type.
        // Defaults to text/plain.
        static std::string get_mime_type(const std::string& contentType)
        {
            const auto mimeTypeIterator = mime_types.find(contentType);
            if (mimeTypeIterator != mime_types.end())
            {
                return mimeTypeIterator->second;
            }
            else if (validate_mime_type(contentType))
            {
                return contentType;
            }
            else
            {
                CROW_LOG_WARNING << "Unable to interpret mime type for content type '" << contentType << "'. Defaulting to text/plain.";
                return "text/plain";
            }
        }


        // clang-format off
        response() {}
        explicit response(int code_) : code(code_) {}
        response(std::string body_) : body(std::move(body_)) {}
        response(int code_, std::string body_) : code(code_), body(std::move(body_)) {}
        // clang-format on
        response(returnable&& value)
        {
            body = value.dump();
            set_header("Content-Type", value.content_type);
        }
        response(returnable& value)
        {
            body = value.dump();
            set_header("Content-Type", value.content_type);
        }
        response(int code_, returnable& value):
          code(code_)
        {
            body = value.dump();
            set_header("Content-Type", value.content_type);
        }
        response(int code_, returnable&& value):
          code(code_), body(value.dump())
        {
            set_header("Content-Type", std::move(value.content_type));
        }

        response(response&& r)
        {
            *this = std::move(r);
        }

        response(std::string contentType, std::string body_):
          body(std::move(body_))
        {
            set_header("Content-Type", get_mime_type(contentType));
        }

        response(int code_, std::string contentType, std::string body_):
          code(code_), body(std::move(body_))
        {
            set_header("Content-Type", get_mime_type(contentType));
        }

        response& operator=(const response& r) = delete;

        response& operator=(response&& r) noexcept
        {
            body = std::move(r.body);
            code = r.code;
            headers = std::move(r.headers);
            completed_ = r.completed_;
            file_info = std::move(r.file_info);
            return *this;
        }

        /// Check if the response has completed (whether response.end() has been called)
        bool is_completed() const noexcept
        {
            return completed_;
        }

        void clear()
        {
            body.clear();
            code = 200;
            headers.clear();
            completed_ = false;
            file_info = static_file_info{};
        }

        /// Return a "Temporary Redirect" response.

        ///
        /// Location can either be a route or a full URL.
        void redirect(const std::string& location)
        {
            code = 307;
            set_header("Location", location);
        }

        /// Return a "Permanent Redirect" response.

        ///
        /// Location can either be a route or a full URL.
        void redirect_perm(const std::string& location)
        {
            code = 308;
            set_header("Location", location);
        }

        /// Return a "Found (Moved Temporarily)" response.

        ///
        /// Location can either be a route or a full URL.
        void moved(const std::string& location)
        {
            code = 302;
            set_header("Location", location);
        }

        /// Return a "Moved Permanently" response.

        ///
        /// Location can either be a route or a full URL.
        void moved_perm(const std::string& location)
        {
            code = 301;
            set_header("Location", location);
        }

        void write(const std::string& body_part)
        {
            body += body_part;
        }

        /// Set the response completion flag and call the handler (to send the response).
        void end()
        {
            if (!completed_)
            {
                completed_ = true;
                if (skip_body)
                {
                    set_header("Content-Length", std::to_string(body.size()));
                    body = "";
                    manual_length_header = true;
                }
                if (complete_request_handler_)
                {
                    complete_request_handler_();
                    manual_length_header = false;
                    skip_body = false;
                }
            }
        }

        /// Same as end() except it adds a body part right before ending.
        void end(const std::string& body_part)
        {
            body += body_part;
            end();
        }

        /// Check if the connection is still alive (usually by checking the socket status).
        bool is_alive()
        {
            return is_alive_helper_ && is_alive_helper_();
        }

        /// Check whether the response has a static file defined.
        bool is_static_type()
        {
            return file_info.path.size();
        }

        /// This constains metadata (coming from the `stat` command) related to any static files associated with this response.

        ///
        /// Either a static file or a string body can be returned as 1 response.
        struct static_file_info
        {
            std::string path = "";
            struct stat statbuf;
            int statResult;
        };

        /// Return a static file as the response body
        void set_static_file_info(std::string path)
        {
            utility::sanitize_filename(path);
            set_static_file_info_unsafe(path);
        }

        /// Return a static file as the response body without sanitizing the path (use set_static_file_info instead)
        void set_static_file_info_unsafe(std::string path)
        {
            file_info.path = path;
            file_info.statResult = stat(file_info.path.c_str(), &file_info.statbuf);
#ifdef CROW_ENABLE_COMPRESSION
            compressed = false;
#endif
            if (file_info.statResult == 0 && S_ISREG(file_info.statbuf.st_mode))
            {
                std::size_t last_dot = path.find_last_of('.');
                std::string extension = path.substr(last_dot + 1);
                code = 200;
                this->add_header("Content-Length", std::to_string(file_info.statbuf.st_size));

                if (!extension.empty())
                {
                    this->add_header("Content-Type", get_mime_type(extension));
                }
            }
            else
            {
                code = 404;
                file_info.path.clear();
            }
        }

    private:
        bool completed_{};
        std::function<void()> complete_request_handler_;
        std::function<bool()> is_alive_helper_;
        static_file_info file_info;
    };
} // namespace crow


namespace crow
{

    struct UTF8
    {
        struct context
        {};

        void before_handle(request& /*req*/, response& /*res*/, context& /*ctx*/)
        {}

        void after_handle(request& /*req*/, response& res, context& /*ctx*/)
        {
            if (get_header_value(res.headers, "Content-Type").empty())
            {
                res.set_header("Content-Type", "text/plain; charset=utf-8");
            }
        }
    };

} // namespace crow

#include <iomanip>
#include <memory>

namespace crow
{
    // Any middleware requires following 3 members:

    // struct context;
    //      storing data for the middleware; can be read from another middleware or handlers

    // before_handle
    //      called before handling the request.
    //      if res.end() is called, the operation is halted.
    //      (still call after_handle of this middleware)
    //      2 signatures:
    //      void before_handle(request& req, response& res, context& ctx)
    //          if you only need to access this middlewares context.
    //      template <typename AllContext>
    //      void before_handle(request& req, response& res, context& ctx, AllContext& all_ctx)
    //          you can access another middlewares' context by calling `all_ctx.template get<MW>()'
    //          ctx == all_ctx.template get<CurrentMiddleware>()

    // after_handle
    //      called after handling the request.
    //      void after_handle(request& req, response& res, context& ctx)
    //      template <typename AllContext>
    //      void after_handle(request& req, response& res, context& ctx, AllContext& all_ctx)

    struct CookieParser
    {
        // Cookie stores key, value and attributes
        struct Cookie
        {
            enum class SameSitePolicy
            {
                Strict,
                Lax,
                None
            };

            template<typename U>
            Cookie(const std::string& key, U&& value):
              Cookie()
            {
                key_ = key;
                value_ = std::forward<U>(value);
            }

            Cookie(const std::string& key):
              Cookie(key, "") {}

            // format cookie to HTTP header format
            std::string dump() const
            {
                const static char* HTTP_DATE_FORMAT = "%a, %d %b %Y %H:%M:%S GMT";

                std::stringstream ss;
                ss << key_ << '=';
                ss << (value_.empty() ? "\"\"" : value_);
                dumpString(ss, !domain_.empty(), "Domain=", domain_);
                dumpString(ss, !path_.empty(), "Path=", path_);
                dumpString(ss, secure_, "Secure");
                dumpString(ss, httponly_, "HttpOnly");
                if (expires_at_)
                {
                    ss << DIVIDER << "Expires="
                       << std::put_time(expires_at_.get(), HTTP_DATE_FORMAT);
                }
                if (max_age_)
                {
                    ss << DIVIDER << "Max-Age=" << *max_age_;
                }
                if (same_site_)
                {
                    ss << DIVIDER << "SameSite=";
                    switch (*same_site_)
                    {
                        case SameSitePolicy::Strict:
                            ss << "Strict";
                            break;
                        case SameSitePolicy::Lax:
                            ss << "Lax";
                            break;
                        case SameSitePolicy::None:
                            ss << "None";
                            break;
                    }
                }
                return ss.str();
            }

            const std::string& name()
            {
                return key_;
            }

            template<typename U>
            Cookie& value(U&& value)
            {
                value_ = std::forward<U>(value);
                return *this;
            }

            // Expires attribute
            Cookie& expires(const std::tm& time)
            {
                expires_at_ = std::unique_ptr<std::tm>(new std::tm(time));
                return *this;
            }

            // Max-Age attribute
            Cookie& max_age(long long seconds)
            {
                max_age_ = std::unique_ptr<long long>(new long long(seconds));
                return *this;
            }

            // Domain attribute
            Cookie& domain(const std::string& name)
            {
                domain_ = name;
                return *this;
            }

            // Path attribute
            Cookie& path(const std::string& path)
            {
                path_ = path;
                return *this;
            }

            // Secured attribute
            Cookie& secure()
            {
                secure_ = true;
                return *this;
            }

            // HttpOnly attribute
            Cookie& httponly()
            {
                httponly_ = true;
                return *this;
            }

            // SameSite attribute
            Cookie& same_site(SameSitePolicy ssp)
            {
                same_site_ = std::unique_ptr<SameSitePolicy>(new SameSitePolicy(ssp));
                return *this;
            }

            Cookie(const Cookie& c):
              key_(c.key_),
              value_(c.value_),
              domain_(c.domain_),
              path_(c.path_),
              secure_(c.secure_),
              httponly_(c.httponly_)
            {
                if (c.max_age_)
                    max_age_ = std::unique_ptr<long long>(new long long(*c.max_age_));

                if (c.expires_at_)
                    expires_at_ = std::unique_ptr<std::tm>(new std::tm(*c.expires_at_));

                if (c.same_site_)
                    same_site_ = std::unique_ptr<SameSitePolicy>(new SameSitePolicy(*c.same_site_));
            }

        private:
            Cookie() = default;

            static void dumpString(std::stringstream& ss, bool cond, const char* prefix,
                                   const std::string& value = "")
            {
                if (cond)
                {
                    ss << DIVIDER << prefix << value;
                }
            }

        private:
            std::string key_;
            std::string value_;
            std::unique_ptr<long long> max_age_{};
            std::string domain_ = "";
            std::string path_ = "";
            bool secure_ = false;
            bool httponly_ = false;
            std::unique_ptr<std::tm> expires_at_{};
            std::unique_ptr<SameSitePolicy> same_site_{};

            static constexpr const char* DIVIDER = "; ";
        };


        struct context
        {
            std::unordered_map<std::string, std::string> jar;

            std::string get_cookie(const std::string& key) const
            {
                auto cookie = jar.find(key);
                if (cookie != jar.end())
                    return cookie->second;
                return {};
            }

            template<typename U>
            Cookie& set_cookie(const std::string& key, U&& value)
            {
                cookies_to_add.emplace_back(key, std::forward<U>(value));
                return cookies_to_add.back();
            }

            Cookie& set_cookie(Cookie cookie)
            {
                cookies_to_add.push_back(std::move(cookie));
                return cookies_to_add.back();
            }

        private:
            friend struct CookieParser;
            std::vector<Cookie> cookies_to_add;
        };

        void before_handle(request& req, response& res, context& ctx)
        {
            // TODO(dranikpg): remove copies, use string_view with c++17
            int count = req.headers.count("Cookie");
            if (!count)
                return;
            if (count > 1)
            {
                res.code = 400;
                res.end();
                return;
            }
            std::string cookies = req.get_header_value("Cookie");
            size_t pos = 0;
            while (pos < cookies.size())
            {
                size_t pos_equal = cookies.find('=', pos);
                if (pos_equal == cookies.npos)
                    break;
                std::string name = cookies.substr(pos, pos_equal - pos);
                name = utility::trim(name);
                pos = pos_equal + 1;
                if (pos == cookies.size())
                    break;

                size_t pos_semicolon = cookies.find(';', pos);
                std::string value = cookies.substr(pos, pos_semicolon - pos);

                value = utility::trim(value);
                if (value[0] == '"' && value[value.size() - 1] == '"')
                {
                    value = value.substr(1, value.size() - 2);
                }

                ctx.jar.emplace(std::move(name), std::move(value));

                pos = pos_semicolon;
                if (pos == cookies.npos)
                    break;
                pos++;
            }
        }

        void after_handle(request& /*req*/, response& res, context& ctx)
        {
            for (const auto& cookie : ctx.cookies_to_add)
            {
                res.add_header("Set-Cookie", cookie.dump());
            }
        }
    };

    /*
    App<CookieParser, AnotherJarMW> app;
    A B C
    A::context
        int aa;

    ctx1 : public A::context
    ctx2 : public ctx1, public B::context
    ctx3 : public ctx2, public C::context

    C depends on A

    C::handle
        context.aaa

    App::context : private CookieParser::context, ...
    {
        jar

    }

    SimpleApp
    */
} // namespace crow



#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>

#include <memory>
#include <string>
#include <cstdio>
#include <mutex>

#include <fstream>
#include <sstream>

#include <type_traits>
#include <functional>
#include <chrono>

#include <variant>

namespace
{
    // convert all integer values to int64_t
    template<typename T>
    using wrap_integral_t = typename std::conditional<
      std::is_integral<T>::value && !std::is_same<bool, T>::value
        // except for uint64_t because that could lead to overflow on conversion
        && !std::is_same<uint64_t, T>::value,
      int64_t, T>::type;

    // convert char[]/char* to std::string
    template<typename T>
    using wrap_char_t = typename std::conditional<
      std::is_same<typename std::decay<T>::type, char*>::value,
      std::string, T>::type;

    // Upgrade to correct type for multi_variant use
    template<typename T>
    using wrap_mv_t = wrap_char_t<wrap_integral_t<T>>;
} // namespace

namespace crow
{
    namespace session
    {

        using multi_value_types = black_magic::S<bool, int64_t, double, std::string>;

        /// A multi_value is a safe variant wrapper with json conversion support
        struct multi_value
        {
            json::wvalue json() const
            {
                // clang-format off
                return std::visit([](auto arg) {
                    return json::wvalue(arg);
                }, v_);
                // clang-format on
            }

            static multi_value from_json(const json::rvalue&);

            std::string string() const
            {
                // clang-format off
                return std::visit([](auto arg) {
                    if constexpr (std::is_same_v<decltype(arg), std::string>)
                        return arg;
                    else
                        return std::to_string(arg);
                }, v_);
                // clang-format on
            }

            template<typename T, typename RT = wrap_mv_t<T>>
            RT get(const T& fallback)
            {
                if (const RT* val = std::get_if<RT>(&v_)) return *val;
                return fallback;
            }

            template<typename T, typename RT = wrap_mv_t<T>>
            void set(T val)
            {
                v_ = RT(std::move(val));
            }

            typename multi_value_types::rebind<std::variant> v_;
        };

        inline multi_value multi_value::from_json(const json::rvalue& rv)
        {
            using namespace json;
            switch (rv.t())
            {
                case type::Number:
                {
                    if (rv.nt() == num_type::Floating_point || rv.nt() == num_type::Double_precision_floating_point)
                        return multi_value{rv.d()};
                    else if (rv.nt() == num_type::Unsigned_integer)
                        return multi_value{int64_t(rv.u())};
                    else
                        return multi_value{rv.i()};
                }
                case type::False: return multi_value{false};
                case type::True: return multi_value{true};
                case type::String: return multi_value{std::string(rv)};
                default: return multi_value{false};
            }
        }

        /// Expiration tracker keeps track of soonest-to-expire keys
        struct ExpirationTracker
        {
            using DataPair = std::pair<uint64_t /*time*/, std::string /*key*/>;

            /// Add key with time to tracker.
            /// If the key is already present, it will be updated
            void add(std::string key, uint64_t time)
            {
                auto it = times_.find(key);
                if (it != times_.end()) remove(key);
                times_[key] = time;
                queue_.insert({time, std::move(key)});
            }

            void remove(const std::string& key)
            {
                auto it = times_.find(key);
                if (it != times_.end())
                {
                    queue_.erase({it->second, key});
                    times_.erase(it);
                }
            }

            /// Get expiration time of soonest-to-expire entry
            uint64_t peek_first() const
            {
                if (queue_.empty()) return std::numeric_limits<uint64_t>::max();
                return queue_.begin()->first;
            }

            std::string pop_first()
            {
                auto it = times_.find(queue_.begin()->second);
                auto key = it->first;
                times_.erase(it);
                queue_.erase(queue_.begin());
                return key;
            }

            using iterator = typename std::set<DataPair>::const_iterator;

            iterator begin() const { return queue_.cbegin(); }

            iterator end() const { return queue_.cend(); }

        private:
            std::set<DataPair> queue_;
            std::unordered_map<std::string, uint64_t> times_;
        };

        /// CachedSessions are shared across requests
        struct CachedSession
        {
            std::string session_id;
            std::string requested_session_id; // session hasn't been created yet, but a key was requested

            std::unordered_map<std::string, multi_value> entries;
            std::unordered_set<std::string> dirty; // values that were changed after last load

            void* store_data;
            bool requested_refresh;

            // number of references held - used for correctly destroying the cache.
            // No need to be atomic, all SessionMiddleware accesses are synchronized
            int referrers;
            std::recursive_mutex mutex;
        };
    } // namespace session

    // SessionMiddleware allows storing securely and easily small snippets of user information
    template<typename Store>
    struct SessionMiddleware
    {
        using lock = std::scoped_lock<std::mutex>;
        using rc_lock = std::scoped_lock<std::recursive_mutex>;

        struct context
        {
            // Get a mutex for locking this session
            std::recursive_mutex& mutex()
            {
                check_node();
                return node->mutex;
            }

            // Check whether this session is already present
            bool exists() { return bool(node); }

            // Get a value by key or fallback if it doesn't exist or is of another type
            template<typename F>
            auto get(const std::string& key, const F& fallback = F())
              // This trick lets the multi_value deduce the return type from the fallback
              // which allows both:
              //   context.get<std::string>("key")
              //   context.get("key", "") -> char[] is transformed into string by multivalue
              // to return a string
              -> decltype(std::declval<session::multi_value>().get<F>(std::declval<F>()))
            {
                if (!node) return fallback;
                rc_lock l(node->mutex);

                auto it = node->entries.find(key);
                if (it != node->entries.end()) return it->second.get<F>(fallback);
                return fallback;
            }

            // Set a value by key
            template<typename T>
            void set(const std::string& key, T value)
            {
                check_node();
                rc_lock l(node->mutex);

                node->dirty.insert(key);
                node->entries[key].set(std::move(value));
            }

            bool contains(const std::string& key)
            {
                if (!node) return false;
                return node->entries.find(key) != node->entries.end();
            }

            // Atomically mutate a value with a function
            template<typename Func>
            void apply(const std::string& key, const Func& f)
            {
                using traits = utility::function_traits<Func>;
                using arg = typename std::decay<typename traits::template arg<0>>::type;
                using retv = typename std::decay<typename traits::result_type>::type;
                check_node();
                rc_lock l(node->mutex);
                node->dirty.insert(key);
                node->entries[key].set<retv>(f(node->entries[key].get(arg{})));
            }

            // Remove a value from the session
            void remove(const std::string& key)
            {
                if (!node) return;
                rc_lock l(node->mutex);
                node->dirty.insert(key);
                node->entries.erase(key);
            }

            // Format value by key as a string
            std::string string(const std::string& key)
            {
                if (!node) return "";
                rc_lock l(node->mutex);

                auto it = node->entries.find(key);
                if (it != node->entries.end()) return it->second.string();
                return "";
            }

            // Get a list of keys present in session
            std::vector<std::string> keys()
            {
                if (!node) return {};
                rc_lock l(node->mutex);

                std::vector<std::string> out;
                for (const auto& p : node->entries)
                    out.push_back(p.first);
                return out;
            }

            // Delay expiration by issuing another cookie with an updated expiration time
            // and notifying the store
            void refresh_expiration()
            {
                if (!node) return;
                node->requested_refresh = true;
            }

        private:
            friend struct SessionMiddleware;

            void check_node()
            {
                if (!node) node = std::make_shared<session::CachedSession>();
            }

            std::shared_ptr<session::CachedSession> node;
        };

        template<typename... Ts>
        SessionMiddleware(
          CookieParser::Cookie cookie,
          int id_length,
          Ts... ts):
          id_length_(id_length),
          cookie_(cookie),
          store_(std::forward<Ts>(ts)...), mutex_(new std::mutex{})
        {}

        template<typename... Ts>
        SessionMiddleware(Ts... ts):
          SessionMiddleware(
            CookieParser::Cookie("session").path("/").max_age(/*month*/ 30 * 24 * 60 * 60),
            /*id_length */ 20, // around 10^34 possible combinations, but small enough to fit into SSO
            std::forward<Ts>(ts)...)
        {}

        template<typename AllContext>
        void before_handle(request& /*req*/, response& /*res*/, context& ctx, AllContext& all_ctx)
        {
            lock l(*mutex_);

            auto& cookies = all_ctx.template get<CookieParser>();
            auto session_id = load_id(cookies);
            if (session_id == "") return;

            // search entry in cache
            auto it = cache_.find(session_id);
            if (it != cache_.end())
            {
                it->second->referrers++;
                ctx.node = it->second;
                return;
            }

            // check this is a valid entry before loading
            if (!store_.contains(session_id)) return;

            auto node = std::make_shared<session::CachedSession>();
            node->session_id = session_id;
            node->referrers = 1;

            try
            {
                store_.load(*node);
            }
            catch (...)
            {
                CROW_LOG_ERROR << "Exception occurred during session load";
                return;
            }

            ctx.node = node;
            cache_[session_id] = node;
        }

        template<typename AllContext>
        void after_handle(request& /*req*/, response& /*res*/, context& ctx, AllContext& all_ctx)
        {
            lock l(*mutex_);
            if (!ctx.node || --ctx.node->referrers > 0) return;
            ctx.node->requested_refresh |= ctx.node->session_id == "";

            // generate new id
            if (ctx.node->session_id == "")
            {
                // check for requested id
                ctx.node->session_id = std::move(ctx.node->requested_session_id);
                if (ctx.node->session_id == "")
                {
                    ctx.node->session_id = utility::random_alphanum(id_length_);
                }
            }
            else
            {
                cache_.erase(ctx.node->session_id);
            }

            if (ctx.node->requested_refresh)
            {
                auto& cookies = all_ctx.template get<CookieParser>();
                store_id(cookies, ctx.node->session_id);
            }

            try
            {
                store_.save(*ctx.node);
            }
            catch (...)
            {
                CROW_LOG_ERROR << "Exception occurred during session save";
                return;
            }
        }

    private:
        std::string next_id()
        {
            std::string id;
            do
            {
                id = utility::random_alphanum(id_length_);
            } while (store_.contains(id));
            return id;
        }

        std::string load_id(const CookieParser::context& cookies)
        {
            return cookies.get_cookie(cookie_.name());
        }

        void store_id(CookieParser::context& cookies, const std::string& session_id)
        {
            cookie_.value(session_id);
            cookies.set_cookie(cookie_);
        }

    private:
        int id_length_;

        // prototype for cookie
        CookieParser::Cookie cookie_;

        Store store_;

        // mutexes are immovable
        std::unique_ptr<std::mutex> mutex_;
        std::unordered_map<std::string, std::shared_ptr<session::CachedSession>> cache_;
    };

    /// InMemoryStore stores all entries in memory
    struct InMemoryStore
    {
        // Load a value into the session cache.
        // A load is always followed by a save, no loads happen consecutively
        void load(session::CachedSession& cn)
        {
            // load & stores happen sequentially, so moving is safe
            cn.entries = std::move(entries[cn.session_id]);
        }

        // Persist session data
        void save(session::CachedSession& cn)
        {
            entries[cn.session_id] = std::move(cn.entries);
            // cn.dirty is a list of changed keys since the last load
        }

        bool contains(const std::string& key)
        {
            return entries.count(key) > 0;
        }

        std::unordered_map<std::string, std::unordered_map<std::string, session::multi_value>> entries;
    };

    // FileStore stores all data as json files in a folder.
    // Files are deleted after expiration. Expiration refreshes are automatically picked up.
    struct FileStore
    {
        FileStore(const std::string& folder, uint64_t expiration_seconds = /*month*/ 30 * 24 * 60 * 60):
          path_(folder), expiration_seconds_(expiration_seconds)
        {
            std::ifstream ifs(get_filename(".expirations", false));

            auto current_ts = chrono_time();
            std::string key;
            uint64_t time;
            while (ifs >> key >> time)
            {
                if (current_ts > time)
                {
                    evict(key);
                }
                else if (contains(key))
                {
                    expirations_.add(key, time);
                }
            }
        }

        ~FileStore()
        {
            std::ofstream ofs(get_filename(".expirations", false), std::ios::trunc);
            for (const auto& p : expirations_)
                ofs << p.second << " " << p.first << "\n";
        }

        // Delete expired entries
        // At most 3 to prevent freezes
        void handle_expired()
        {
            int deleted = 0;
            auto current_ts = chrono_time();
            while (current_ts > expirations_.peek_first() && deleted < 3)
            {
                evict(expirations_.pop_first());
                deleted++;
            }
        }

        void load(session::CachedSession& cn)
        {
            handle_expired();

            std::ifstream file(get_filename(cn.session_id));

            std::stringstream buffer;
            buffer << file.rdbuf() << std::endl;

            for (const auto& p : json::load(buffer.str()))
                cn.entries[p.key()] = session::multi_value::from_json(p);
        }

        void save(session::CachedSession& cn)
        {
            if (cn.requested_refresh)
                expirations_.add(cn.session_id, chrono_time() + expiration_seconds_);
            if (cn.dirty.empty()) return;

            std::ofstream file(get_filename(cn.session_id));
            json::wvalue jw;
            for (const auto& p : cn.entries)
                jw[p.first] = p.second.json();
            file << jw.dump() << std::flush;
        }

        std::string get_filename(const std::string& key, bool suffix = true)
        {
            return utility::join_path(path_, key + (suffix ? ".json" : ""));
        }

        bool contains(const std::string& key)
        {
            std::ifstream file(get_filename(key));
            return file.good();
        }

        void evict(const std::string& key)
        {
            std::remove(get_filename(key).c_str());
        }

        uint64_t chrono_time() const
        {
            return std::chrono::duration_cast<std::chrono::seconds>(
                     std::chrono::steady_clock::now().time_since_epoch())
              .count();
        }

        std::string path_;
        uint64_t expiration_seconds_;
        session::ExpirationTracker expirations_;
    };

} // namespace crow



namespace crow
{
    namespace detail
    {


        template<typename... Middlewares>
        struct partial_context : public black_magic::pop_back<Middlewares...>::template rebind<partial_context>, public black_magic::last_element_type<Middlewares...>::type::context
        {
            using parent_context = typename black_magic::pop_back<Middlewares...>::template rebind<::crow::detail::partial_context>;
            template<int N>
            using partial = typename std::conditional<N == sizeof...(Middlewares) - 1, partial_context, typename parent_context::template partial<N>>::type;

            template<typename T>
            typename T::context& get()
            {
                return static_cast<typename T::context&>(*this);
            }
        };



        template<>
        struct partial_context<>
        {
            template<int>
            using partial = partial_context;
        };


        template<typename... Middlewares>
        struct context : private partial_context<Middlewares...>
        //struct context : private Middlewares::context... // simple but less type-safe
        {
            template<typename CallCriteria, int N, typename Context, typename Container>
            friend typename std::enable_if<(N == 0)>::type after_handlers_call_helper(const CallCriteria& cc, Container& middlewares, Context& ctx, request& req, response& res);
            template<typename CallCriteria, int N, typename Context, typename Container>
            friend typename std::enable_if<(N > 0)>::type after_handlers_call_helper(const CallCriteria& cc, Container& middlewares, Context& ctx, request& req, response& res);

            template<typename CallCriteria, int N, typename Context, typename Container>
            friend typename std::enable_if<(N < std::tuple_size<typename std::remove_reference<Container>::type>::value), bool>::type
              middleware_call_helper(const CallCriteria& cc, Container& middlewares, request& req, response& res, Context& ctx);

            template<typename T>
            typename T::context& get()
            {
                return static_cast<typename T::context&>(*this);
            }

            template<int N>
            using partial = typename partial_context<Middlewares...>::template partial<N>;
        };
    } // namespace detail
} // namespace crow



#include <tuple>
#include <type_traits>
#include <iostream>
#include <utility>

namespace crow // NOTE: Already documented in "crow/app.h"
{

    /// Local middleware should extend ILocalMiddleware
    struct ILocalMiddleware
    {
        using call_global = std::false_type;
    };

    namespace detail
    {
        template<typename MW>
        struct check_before_handle_arity_3_const
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) const = &T::before_handle>
            struct get
            {};
        };

        template<typename MW>
        struct check_before_handle_arity_3
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) = &T::before_handle>
            struct get
            {};
        };

        template<typename MW>
        struct check_after_handle_arity_3_const
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) const = &T::after_handle>
            struct get
            {};
        };

        template<typename MW>
        struct check_after_handle_arity_3
        {
            template<typename T, void (T::*)(request&, response&, typename MW::context&) = &T::after_handle>
            struct get
            {};
        };

        template<typename MW>
        struct check_global_call_false
        {
            template<typename T, typename std::enable_if<T::call_global::value == false, bool>::type = true>
            struct get
            {};
        };

        template<typename T>
        struct is_before_handle_arity_3_impl
        {
            template<typename C>
            static std::true_type f(typename check_before_handle_arity_3_const<T>::template get<C>*);

            template<typename C>
            static std::true_type f(typename check_before_handle_arity_3<T>::template get<C>*);

            template<typename C>
            static std::false_type f(...);

        public:
            static const bool value = decltype(f<T>(nullptr))::value;
        };

        template<typename T>
        struct is_after_handle_arity_3_impl
        {
            template<typename C>
            static std::true_type f(typename check_after_handle_arity_3_const<T>::template get<C>*);

            template<typename C>
            static std::true_type f(typename check_after_handle_arity_3<T>::template get<C>*);

            template<typename C>
            static std::false_type f(...);

        public:
            static constexpr bool value = decltype(f<T>(nullptr))::value;
        };

        template<typename MW>
        struct is_middleware_global
        {
            template<typename C>
            static std::false_type f(typename check_global_call_false<MW>::template get<C>*);

            template<typename C>
            static std::true_type f(...);

            static const bool value = decltype(f<MW>(nullptr))::value;
        };

        template<typename MW, typename Context, typename ParentContext>
        typename std::enable_if<!is_before_handle_arity_3_impl<MW>::value>::type
          before_handler_call(MW& mw, request& req, response& res, Context& ctx, ParentContext& /*parent_ctx*/)
        {
            mw.before_handle(req, res, ctx.template get<MW>(), ctx);
        }

        template<typename MW, typename Context, typename ParentContext>
        typename std::enable_if<is_before_handle_arity_3_impl<MW>::value>::type
          before_handler_call(MW& mw, request& req, response& res, Context& ctx, ParentContext& /*parent_ctx*/)
        {
            mw.before_handle(req, res, ctx.template get<MW>());
        }

        template<typename MW, typename Context, typename ParentContext>
        typename std::enable_if<!is_after_handle_arity_3_impl<MW>::value>::type
          after_handler_call(MW& mw, request& req, response& res, Context& ctx, ParentContext& /*parent_ctx*/)
        {
            mw.after_handle(req, res, ctx.template get<MW>(), ctx);
        }

        template<typename MW, typename Context, typename ParentContext>
        typename std::enable_if<is_after_handle_arity_3_impl<MW>::value>::type
          after_handler_call(MW& mw, request& req, response& res, Context& ctx, ParentContext& /*parent_ctx*/)
        {
            mw.after_handle(req, res, ctx.template get<MW>());
        }


        template<typename CallCriteria,
                 int N, typename Context, typename Container>
        typename std::enable_if<(N < std::tuple_size<typename std::remove_reference<Container>::type>::value), bool>::type
          middleware_call_helper(const CallCriteria& cc, Container& middlewares, request& req, response& res, Context& ctx)
        {

            using CurrentMW = typename std::tuple_element<N, typename std::remove_reference<Container>::type>::type;

            if (!cc.template enabled<CurrentMW>(N))
            {
                return middleware_call_helper<CallCriteria, N + 1, Context, Container>(cc, middlewares, req, res, ctx);
            }

            using parent_context_t = typename Context::template partial<N - 1>;
            before_handler_call<CurrentMW, Context, parent_context_t>(std::get<N>(middlewares), req, res, ctx, static_cast<parent_context_t&>(ctx));
            if (res.is_completed())
            {
                after_handler_call<CurrentMW, Context, parent_context_t>(std::get<N>(middlewares), req, res, ctx, static_cast<parent_context_t&>(ctx));
                return true;
            }

            if (middleware_call_helper<CallCriteria, N + 1, Context, Container>(cc, middlewares, req, res, ctx))
            {
                after_handler_call<CurrentMW, Context, parent_context_t>(std::get<N>(middlewares), req, res, ctx, static_cast<parent_context_t&>(ctx));
                return true;
            }

            return false;
        }

        template<typename CallCriteria, int N, typename Context, typename Container>
        typename std::enable_if<(N >= std::tuple_size<typename std::remove_reference<Container>::type>::value), bool>::type
          middleware_call_helper(const CallCriteria& /*cc*/, Container& /*middlewares*/, request& /*req*/, response& /*res*/, Context& /*ctx*/)
        {
            return false;
        }

        template<typename CallCriteria, int N, typename Context, typename Container>
        typename std::enable_if<(N < 0)>::type
          after_handlers_call_helper(const CallCriteria& /*cc*/, Container& /*middlewares*/, Context& /*context*/, request& /*req*/, response& /*res*/)
        {
        }

        template<typename CallCriteria, int N, typename Context, typename Container>
        typename std::enable_if<(N == 0)>::type after_handlers_call_helper(const CallCriteria& cc, Container& middlewares, Context& ctx, request& req, response& res)
        {
            using parent_context_t = typename Context::template partial<N - 1>;
            using CurrentMW = typename std::tuple_element<N, typename std::remove_reference<Container>::type>::type;
            if (cc.template enabled<CurrentMW>(N))
            {
                after_handler_call<CurrentMW, Context, parent_context_t>(std::get<N>(middlewares), req, res, ctx, static_cast<parent_context_t&>(ctx));
            }
        }

        template<typename CallCriteria, int N, typename Context, typename Container>
        typename std::enable_if<(N > 0)>::type after_handlers_call_helper(const CallCriteria& cc, Container& middlewares, Context& ctx, request& req, response& res)
        {
            using parent_context_t = typename Context::template partial<N - 1>;
            using CurrentMW = typename std::tuple_element<N, typename std::remove_reference<Container>::type>::type;
            if (cc.template enabled<CurrentMW>(N))
            {
                after_handler_call<CurrentMW, Context, parent_context_t>(std::get<N>(middlewares), req, res, ctx, static_cast<parent_context_t&>(ctx));
            }
            after_handlers_call_helper<CallCriteria, N - 1, Context, Container>(cc, middlewares, ctx, req, res);
        }

        // A CallCriteria that accepts only global middleware
        struct middleware_call_criteria_only_global
        {
            template<typename MW>
            constexpr bool enabled(int) const
            {
                return is_middleware_global<MW>::value;
            }
        };

        template<typename F, typename... Args>
        typename std::enable_if<black_magic::CallHelper<F, black_magic::S<Args...>>::value, void>::type
          wrapped_handler_call(crow::request& /*req*/, crow::response& res, const F& f, Args&&... args)
        {
            static_assert(!std::is_same<void, decltype(f(std::declval<Args>()...))>::value,
                          "Handler function cannot have void return type; valid return types: string, int, crow::response, crow::returnable");

            res = crow::response(f(std::forward<Args>(args)...));
            res.end();
        }

        template<typename F, typename... Args>
        typename std::enable_if<
          !black_magic::CallHelper<F, black_magic::S<Args...>>::value &&
            black_magic::CallHelper<F, black_magic::S<crow::request&, Args...>>::value,
          void>::type
          wrapped_handler_call(crow::request& req, crow::response& res, const F& f, Args&&... args)
        {
            static_assert(!std::is_same<void, decltype(f(std::declval<crow::request>(), std::declval<Args>()...))>::value,
                          "Handler function cannot have void return type; valid return types: string, int, crow::response, crow::returnable");

            res = crow::response(f(req, std::forward<Args>(args)...));
            res.end();
        }

        template<typename F, typename... Args>
        typename std::enable_if<
          !black_magic::CallHelper<F, black_magic::S<Args...>>::value &&
            !black_magic::CallHelper<F, black_magic::S<crow::request&, Args...>>::value &&
            black_magic::CallHelper<F, black_magic::S<crow::response&, Args...>>::value,
          void>::type
          wrapped_handler_call(crow::request& /*req*/, crow::response& res, const F& f, Args&&... args)
        {
            static_assert(std::is_same<void, decltype(f(std::declval<crow::response&>(), std::declval<Args>()...))>::value,
                          "Handler function with response argument should have void return type");

            f(res, std::forward<Args>(args)...);
        }

        template<typename F, typename... Args>
        typename std::enable_if<
          !black_magic::CallHelper<F, black_magic::S<Args...>>::value &&
            !black_magic::CallHelper<F, black_magic::S<crow::request&, Args...>>::value &&
            !black_magic::CallHelper<F, black_magic::S<crow::response&, Args...>>::value &&
            black_magic::CallHelper<F, black_magic::S<const crow::request&, crow::response&, Args...>>::value,
          void>::type
          wrapped_handler_call(crow::request& req, crow::response& res, const F& f, Args&&... args)
        {
            static_assert(std::is_same<void, decltype(f(std::declval<crow::request&>(), std::declval<crow::response&>(), std::declval<Args>()...))>::value,
                          "Handler function with response argument should have void return type");

            f(req, res, std::forward<Args>(args)...);
        }

        // wrapped_handler_call transparently wraps a handler call behind (req, res, args...)
        template<typename F, typename... Args>
        typename std::enable_if<
          !black_magic::CallHelper<F, black_magic::S<Args...>>::value &&
            !black_magic::CallHelper<F, black_magic::S<crow::request&, Args...>>::value &&
            !black_magic::CallHelper<F, black_magic::S<crow::response&, Args...>>::value &&
            !black_magic::CallHelper<F, black_magic::S<const crow::request&, crow::response&, Args...>>::value,
          void>::type
          wrapped_handler_call(crow::request& req, crow::response& res, const F& f, Args&&... args)
        {
            static_assert(std::is_same<void, decltype(f(std::declval<crow::request&>(), std::declval<crow::response&>(), std::declval<Args>()...))>::value,
                          "Handler function with response argument should have void return type");

            f(req, res, std::forward<Args>(args)...);
        }

        template<bool Reversed>
        struct middleware_call_criteria_dynamic
        {};

        template<>
        struct middleware_call_criteria_dynamic<false>
        {
            middleware_call_criteria_dynamic(const std::vector<int>& indices_):
              indices(indices_), slider(0) {}

            template<typename>
            bool enabled(int mw_index) const
            {
                if (slider < int(indices.size()) && indices[slider] == mw_index)
                {
                    slider++;
                    return true;
                }
                return false;
            }

        private:
            const std::vector<int>& indices;
            mutable int slider;
        };

        template<>
        struct middleware_call_criteria_dynamic<true>
        {
            middleware_call_criteria_dynamic(const std::vector<int>& indices_):
              indices(indices_), slider(int(indices_.size()) - 1) {}

            template<typename>
            bool enabled(int mw_index) const
            {
                if (slider >= 0 && indices[slider] == mw_index)
                {
                    slider--;
                    return true;
                }
                return false;
            }

        private:
            const std::vector<int>& indices;
            mutable int slider;
        };

    } // namespace detail
} // namespace crow


#include <cstdint>
#include <utility>
#include <tuple>
#include <unordered_map>
#include <memory>
#include <vector>
#include <algorithm>
#include <type_traits>


namespace crow // NOTE: Already documented in "crow/app.h"
{

    constexpr const uint16_t INVALID_BP_ID{((uint16_t)-1)};

    namespace detail
    {
        /// Typesafe wrapper for storing lists of middleware as their indices in the App
        struct middleware_indices
        {
            template<typename App>
            void push()
            {}

            template<typename App, typename MW, typename... Middlewares>
            void push()
            {
                using MwContainer = typename App::mw_container_t;
                static_assert(black_magic::has_type<MW, MwContainer>::value, "Middleware must be present in app");
                static_assert(std::is_base_of<crow::ILocalMiddleware, MW>::value, "Middleware must extend ILocalMiddleware");
                int idx = black_magic::tuple_index<MW, MwContainer>::value;
                indices_.push_back(idx);
                push<App, Middlewares...>();
            }

            void merge_front(const detail::middleware_indices& other)
            {
                indices_.insert(indices_.begin(), other.indices_.cbegin(), other.indices_.cend());
            }

            void merge_back(const detail::middleware_indices& other)
            {
                indices_.insert(indices_.end(), other.indices_.cbegin(), other.indices_.cend());
            }

            void pop_back(const detail::middleware_indices& other)
            {
                indices_.resize(indices_.size() - other.indices_.size());
            }

            bool empty() const
            {
                return indices_.empty();
            }

            // Sorts indices and filters out duplicates to allow fast lookups with traversal
            void pack()
            {
                std::sort(indices_.begin(), indices_.end());
                indices_.erase(std::unique(indices_.begin(), indices_.end()), indices_.end());
            }

            const std::vector<int>& indices()
            {
                return indices_;
            }

        private:
            std::vector<int> indices_;
        };
    } // namespace detail

    /// A base class for all rules.

    ///
    /// Used to provide a common interface for code dealing with different types of rules.<br>
    /// A Rule provides a URL, allowed HTTP methods, and handlers.
    class BaseRule
    {
    public:
        BaseRule(std::string rule):
          rule_(std::move(rule))
        {}

        virtual ~BaseRule()
        {}

        virtual void validate() = 0;

        void set_added()
        {
            added_ = true;
        }

        bool is_added()
        {
            return added_;
        }

        std::unique_ptr<BaseRule> upgrade()
        {
            if (rule_to_upgrade_)
                return std::move(rule_to_upgrade_);
            return {};
        }

        virtual void handle(request&, response&, const routing_params&) = 0;
        virtual void handle_upgrade(const request&, response& res, SocketAdaptor&&)
        {
            res = response(404);
            res.end();
        }
        virtual void handle_upgrade(const request&, response& res, UnixSocketAdaptor&&)
        {
            res = response(404);
            res.end();
        }
#ifdef CROW_ENABLE_SSL
        virtual void handle_upgrade(const request&, response& res, SSLAdaptor&&)
        {
            res = response(404);
            res.end();
        }
#endif

        uint32_t get_methods()
        {
            return methods_;
        }

        template<typename F>
        void foreach_method(F f)
        {
            for (uint32_t method = 0, method_bit = 1; method < static_cast<uint32_t>(HTTPMethod::InternalMethodCount); method++, method_bit <<= 1)
            {
                if (methods_ & method_bit)
                    f(method);
            }
        }

        std::string custom_templates_base;

        const std::string& rule() { return rule_; }

    protected:
        uint32_t methods_{1 << static_cast<int>(HTTPMethod::Get)};

        std::string rule_;
        std::string name_;
        bool added_{false};

        std::unique_ptr<BaseRule> rule_to_upgrade_;

        detail::middleware_indices mw_indices_;

        friend class Router;
        friend class Blueprint;
        template<typename T>
        friend struct RuleParameterTraits;
    };


    namespace detail
    {
        namespace routing_handler_call_helper
        {
            template<typename T, int Pos>
            struct call_pair
            {
                using type = T;
                static const int pos = Pos;
            };

            template<typename H1>
            struct call_params
            {
                H1& handler;
                const routing_params& params;
                request& req;
                response& res;
            };

            template<typename F, int NInt, int NUint, int NDouble, int NString, typename S1, typename S2>
            struct call
            {};

            template<typename F, int NInt, int NUint, int NDouble, int NString, typename... Args1, typename... Args2>
            struct call<F, NInt, NUint, NDouble, NString, black_magic::S<int64_t, Args1...>, black_magic::S<Args2...>>
            {
                void operator()(F cparams)
                {
                    using pushed = typename black_magic::S<Args2...>::template push_back<call_pair<int64_t, NInt>>;
                    call<F, NInt + 1, NUint, NDouble, NString, black_magic::S<Args1...>, pushed>()(cparams);
                }
            };

            template<typename F, int NInt, int NUint, int NDouble, int NString, typename... Args1, typename... Args2>
            struct call<F, NInt, NUint, NDouble, NString, black_magic::S<uint64_t, Args1...>, black_magic::S<Args2...>>
            {
                void operator()(F cparams)
                {
                    using pushed = typename black_magic::S<Args2...>::template push_back<call_pair<uint64_t, NUint>>;
                    call<F, NInt, NUint + 1, NDouble, NString, black_magic::S<Args1...>, pushed>()(cparams);
                }
            };

            template<typename F, int NInt, int NUint, int NDouble, int NString, typename... Args1, typename... Args2>
            struct call<F, NInt, NUint, NDouble, NString, black_magic::S<double, Args1...>, black_magic::S<Args2...>>
            {
                void operator()(F cparams)
                {
                    using pushed = typename black_magic::S<Args2...>::template push_back<call_pair<double, NDouble>>;
                    call<F, NInt, NUint, NDouble + 1, NString, black_magic::S<Args1...>, pushed>()(cparams);
                }
            };

            template<typename F, int NInt, int NUint, int NDouble, int NString, typename... Args1, typename... Args2>
            struct call<F, NInt, NUint, NDouble, NString, black_magic::S<std::string, Args1...>, black_magic::S<Args2...>>
            {
                void operator()(F cparams)
                {
                    using pushed = typename black_magic::S<Args2...>::template push_back<call_pair<std::string, NString>>;
                    call<F, NInt, NUint, NDouble, NString + 1, black_magic::S<Args1...>, pushed>()(cparams);
                }
            };

            template<typename F, int NInt, int NUint, int NDouble, int NString, typename... Args1>
            struct call<F, NInt, NUint, NDouble, NString, black_magic::S<>, black_magic::S<Args1...>>
            {
                void operator()(F cparams)
                {
                    cparams.handler(
                      cparams.req,
                      cparams.res,
                      cparams.params.template get<typename Args1::type>(Args1::pos)...);
                }
            };

            template<typename Func, typename... ArgsWrapped>
            struct Wrapped
            {
                template<typename... Args>
                void set_(Func f, typename std::enable_if<!std::is_same<typename std::tuple_element<0, std::tuple<Args..., void>>::type, const request&>::value, int>::type = 0)
                {
                    handler_ = ([f = std::move(f)](const request&, response& res, Args... args) {
                        res = response(f(args...));
                        res.end();
                    });
                }

                template<typename Req, typename... Args>
                struct req_handler_wrapper
                {
                    req_handler_wrapper(Func fun):
                      f(std::move(fun))
                    {
                    }

                    void operator()(const request& req, response& res, Args... args)
                    {
                        res = response(f(req, args...));
                        res.end();
                    }

                    Func f;
                };

                template<typename... Args>
                void set_(Func f, typename std::enable_if<
                                    std::is_same<typename std::tuple_element<0, std::tuple<Args..., void>>::type, const request&>::value &&
                                      !std::is_same<typename std::tuple_element<1, std::tuple<Args..., void, void>>::type, response&>::value,
                                    int>::type = 0)
                {
                    handler_ = req_handler_wrapper<Args...>(std::move(f));
                    /*handler_ = (
                        [f = std::move(f)]
                        (const request& req, response& res, Args... args){
                             res = response(f(req, args...));
                             res.end();
                        });*/
                }

                template<typename... Args>
                void set_(Func f, typename std::enable_if<
                                    std::is_same<typename std::tuple_element<0, std::tuple<Args..., void>>::type, const request&>::value &&
                                      std::is_same<typename std::tuple_element<1, std::tuple<Args..., void, void>>::type, response&>::value,
                                    int>::type = 0)
                {
                    handler_ = std::move(f);
                }

                template<typename... Args>
                struct handler_type_helper
                {
                    using type = std::function<void(const crow::request&, crow::response&, Args...)>;
                    using args_type = black_magic::S<typename black_magic::promote_t<Args>...>;
                };

                template<typename... Args>
                struct handler_type_helper<const request&, Args...>
                {
                    using type = std::function<void(const crow::request&, crow::response&, Args...)>;
                    using args_type = black_magic::S<typename black_magic::promote_t<Args>...>;
                };

                template<typename... Args>
                struct handler_type_helper<const request&, response&, Args...>
                {
                    using type = std::function<void(const crow::request&, crow::response&, Args...)>;
                    using args_type = black_magic::S<typename black_magic::promote_t<Args>...>;
                };

                typename handler_type_helper<ArgsWrapped...>::type handler_;

                void operator()(request& req, response& res, const routing_params& params)
                {
                    detail::routing_handler_call_helper::call<
                      detail::routing_handler_call_helper::call_params<
                        decltype(handler_)>,
                      0, 0, 0, 0,
                      typename handler_type_helper<ArgsWrapped...>::args_type,
                      black_magic::S<>>()(
                      detail::routing_handler_call_helper::call_params<
                        decltype(handler_)>{handler_, params, req, res});
                }
            };

        } // namespace routing_handler_call_helper
    }     // namespace detail


    class CatchallRule
    {
    public:
        /// @cond SKIP
        CatchallRule() {}

        template<typename Func>
        typename std::enable_if<black_magic::CallHelper<Func, black_magic::S<>>::value, void>::type
          operator()(Func&& f)
        {
            static_assert(!std::is_same<void, decltype(f())>::value,
                          "Handler function cannot have void return type; valid return types: string, int, crow::response, crow::returnable");

            handler_ = ([f = std::move(f)](const request&, response& res) {
                res = response(f());
                res.end();
            });
        }

        template<typename Func>
        typename std::enable_if<
          !black_magic::CallHelper<Func, black_magic::S<>>::value &&
            black_magic::CallHelper<Func, black_magic::S<crow::request>>::value,
          void>::type
          operator()(Func&& f)
        {
            static_assert(!std::is_same<void, decltype(f(std::declval<crow::request>()))>::value,
                          "Handler function cannot have void return type; valid return types: string, int, crow::response, crow::returnable");

            handler_ = ([f = std::move(f)](const request& req, response& res) {
                res = response(f(req));
                res.end();
            });
        }

        template<typename Func>
        typename std::enable_if<
          !black_magic::CallHelper<Func, black_magic::S<>>::value &&
            !black_magic::CallHelper<Func, black_magic::S<crow::request>>::value &&
            black_magic::CallHelper<Func, black_magic::S<crow::response&>>::value,
          void>::type
          operator()(Func&& f)
        {
            static_assert(std::is_same<void, decltype(f(std::declval<crow::response&>()))>::value,
                          "Handler function with response argument should have void return type");
            handler_ = ([f = std::move(f)](const request&, response& res) {
                f(res);
            });
        }

        template<typename Func>
        typename std::enable_if<
          !black_magic::CallHelper<Func, black_magic::S<>>::value &&
            !black_magic::CallHelper<Func, black_magic::S<crow::request>>::value &&
            !black_magic::CallHelper<Func, black_magic::S<crow::response&>>::value,
          void>::type
          operator()(Func&& f)
        {
            static_assert(std::is_same<void, decltype(f(std::declval<crow::request>(), std::declval<crow::response&>()))>::value,
                          "Handler function with response argument should have void return type");

            handler_ = std::move(f);
        }
        /// @endcond
        bool has_handler()
        {
            return (handler_ != nullptr);
        }

    protected:
        friend class Router;

    private:
        std::function<void(const crow::request&, crow::response&)> handler_;
    };


    /// A rule dealing with websockets.

    ///
    /// Provides the interface for the user to put in the necessary handlers for a websocket to work.
    template<typename App>
    class WebSocketRule : public BaseRule
    {
        using self_t = WebSocketRule;

    public:
        WebSocketRule(std::string rule, App* app):
          BaseRule(std::move(rule)),
          app_(app),
          max_payload_(UINT64_MAX)
        {}

        void validate() override
        {}

        void handle(request&, response& res, const routing_params&) override
        {
            res = response(404);
            res.end();
        }

        void handle_upgrade(const request& req, response&, SocketAdaptor&& adaptor) override
        {
            max_payload_ = max_payload_override_ ? max_payload_ : app_->websocket_max_payload();
            new crow::websocket::Connection<SocketAdaptor, App>(req, std::move(adaptor), app_, max_payload_, subprotocols_, open_handler_, message_handler_, close_handler_, error_handler_, accept_handler_, mirror_protocols_);
        }
        void handle_upgrade(const request& req, response&, UnixSocketAdaptor&& adaptor) override
        {
            max_payload_ = max_payload_override_ ? max_payload_ : app_->websocket_max_payload();
            new crow::websocket::Connection<UnixSocketAdaptor, App>(req, std::move(adaptor), app_, max_payload_, subprotocols_, open_handler_, message_handler_, close_handler_, error_handler_, accept_handler_, mirror_protocols_);
        }
#ifdef CROW_ENABLE_SSL
        void handle_upgrade(const request& req, response&, SSLAdaptor&& adaptor) override
        {
            new crow::websocket::Connection<SSLAdaptor, App>(req, std::move(adaptor), app_, max_payload_, subprotocols_, open_handler_, message_handler_, close_handler_, error_handler_, accept_handler_, mirror_protocols_);
        }
#endif

        /// Override the global payload limit for this single WebSocket rule
        self_t& max_payload(uint64_t max_payload)
        {
            max_payload_ = max_payload;
            max_payload_override_ = true;
            return *this;
        }

        self_t& subprotocols(const std::vector<std::string>& subprotocols)
        {
            subprotocols_ = subprotocols;
            return *this;
        }

        template<typename Func>
        self_t& onopen(Func f)
        {
            open_handler_ = f;
            return *this;
        }

        template<typename Func>
        self_t& onmessage(Func f)
        {
            message_handler_ = f;
            return *this;
        }

        template<typename Func>
        self_t& onclose(Func f)
        {
            close_handler_ = f;
            return *this;
        }

        template<typename Func>
        self_t& onerror(Func f)
        {
            error_handler_ = f;
            return *this;
        }

        template<typename Func>
        self_t& onaccept(Func f)
        {
            accept_handler_ = f;
            return *this;
        }

        self_t& mirrorprotocols(bool mirror_protocols = true)
        {
            mirror_protocols_ = mirror_protocols;
            return *this;
        }

    protected:
        App* app_;
        std::function<void(crow::websocket::connection&)> open_handler_;
        std::function<void(crow::websocket::connection&, const std::string&, bool)> message_handler_;
        std::function<void(crow::websocket::connection&, const std::string&, uint16_t)> close_handler_;
        std::function<void(crow::websocket::connection&, const std::string&)> error_handler_;
        std::function<bool(const crow::request&, void**)> accept_handler_;
        bool mirror_protocols_ = false;
        uint64_t max_payload_;
        bool max_payload_override_ = false;
        std::vector<std::string> subprotocols_;
    };

    /// Allows the user to assign parameters using functions.

    ///
    /// `rule.name("name").methods(HTTPMethod::POST)`
    template<typename T>
    struct RuleParameterTraits
    {
        using self_t = T;

        template<typename App>
        WebSocketRule<App>& websocket(App* app)
        {
            auto p = new WebSocketRule<App>(static_cast<self_t*>(this)->rule_, app);
            static_cast<self_t*>(this)->rule_to_upgrade_.reset(p);
            return *p;
        }

        self_t& name(std::string name) noexcept
        {
            static_cast<self_t*>(this)->name_ = std::move(name);
            return static_cast<self_t&>(*this);
        }

        self_t& methods(HTTPMethod method)
        {
            static_cast<self_t*>(this)->methods_ = 1 << static_cast<int>(method);
            return static_cast<self_t&>(*this);
        }

        template<typename... MethodArgs>
        self_t& methods(HTTPMethod method, MethodArgs... args_method)
        {
            methods(args_method...);
            static_cast<self_t*>(this)->methods_ |= 1 << static_cast<int>(method);
            return static_cast<self_t&>(*this);
        }

        /// Enable local middleware for this handler
        template<typename App, typename... Middlewares>
        self_t& middlewares()
        {
            static_cast<self_t*>(this)->mw_indices_.template push<App, Middlewares...>();
            return static_cast<self_t&>(*this);
        }
    };

    /// A rule that can change its parameters during runtime.
    class DynamicRule : public BaseRule, public RuleParameterTraits<DynamicRule>
    {
    public:
        DynamicRule(std::string rule):
          BaseRule(std::move(rule))
        {}

        void validate() override
        {
            if (!erased_handler_)
            {
                throw std::runtime_error(name_ + (!name_.empty() ? ": " : "") + "no handler for url " + rule_);
            }
        }

        void handle(request& req, response& res, const routing_params& params) override
        {
            if (!custom_templates_base.empty())
                mustache::set_base(custom_templates_base);
            else if (mustache::detail::get_template_base_directory_ref() != "templates")
                mustache::set_base("templates");
            erased_handler_(req, res, params);
        }

        template<typename Func>
        void operator()(Func f)
        {
#ifdef CROW_MSVC_WORKAROUND
            using function_t = utility::function_traits<decltype(&Func::operator())>;
#else
            using function_t = utility::function_traits<Func>;
#endif
            erased_handler_ = wrap(std::move(f), black_magic::gen_seq<function_t::arity>());
        }

        // enable_if Arg1 == request && Arg2 == response
        // enable_if Arg1 == request && Arg2 != resposne
        // enable_if Arg1 != request
#ifdef CROW_MSVC_WORKAROUND
        template<typename Func, size_t... Indices>
#else
        template<typename Func, unsigned... Indices>
#endif
        std::function<void(request&, response&, const routing_params&)>
          wrap(Func f, black_magic::seq<Indices...>)
        {
#ifdef CROW_MSVC_WORKAROUND
            using function_t = utility::function_traits<decltype(&Func::operator())>;
#else
            using function_t = utility::function_traits<Func>;
#endif
            if (!black_magic::is_parameter_tag_compatible(
                  black_magic::get_parameter_tag_runtime(rule_.c_str()),
                  black_magic::compute_parameter_tag_from_args_list<
                    typename function_t::template arg<Indices>...>::value))
            {
                throw std::runtime_error("route_dynamic: Handler type is mismatched with URL parameters: " + rule_);
            }
            auto ret = detail::routing_handler_call_helper::Wrapped<Func, typename function_t::template arg<Indices>...>();
            ret.template set_<
              typename function_t::template arg<Indices>...>(std::move(f));
            return ret;
        }

        template<typename Func>
        void operator()(std::string name, Func&& f)
        {
            name_ = std::move(name);
            (*this).template operator()<Func>(std::forward(f));
        }

    private:
        std::function<void(request&, response&, const routing_params&)> erased_handler_;
    };

    /// Default rule created when CROW_ROUTE is called.
    template<typename... Args>
    class TaggedRule : public BaseRule, public RuleParameterTraits<TaggedRule<Args...>>
    {
    public:
        using self_t = TaggedRule<Args...>;

        TaggedRule(std::string rule):
          BaseRule(std::move(rule))
        {}

        void validate() override
        {
            if (rule_.at(0) != '/')
                throw std::runtime_error("Internal error: Routes must start with a '/'");

            if (!handler_)
            {
                throw std::runtime_error(name_ + (!name_.empty() ? ": " : "") + "no handler for url " + rule_);
            }
        }

        template<typename Func>
        void operator()(Func&& f)
        {
            handler_ = ([f = std::move(f)](request& req, response& res, Args... args) {
                detail::wrapped_handler_call(req, res, f, std::forward<Args>(args)...);
            });
        }

        template<typename Func>
        void operator()(std::string name, Func&& f)
        {
            name_ = std::move(name);
            (*this).template operator()<Func>(std::forward(f));
        }

        void handle(request& req, response& res, const routing_params& params) override
        {
            if (!custom_templates_base.empty())
                mustache::set_base(custom_templates_base);
            else if (mustache::detail::get_template_base_directory_ref() != mustache::detail::get_global_template_base_directory_ref())
                mustache::set_base(mustache::detail::get_global_template_base_directory_ref());

            detail::routing_handler_call_helper::call<
              detail::routing_handler_call_helper::call_params<decltype(handler_)>,
              0, 0, 0, 0,
              black_magic::S<Args...>,
              black_magic::S<>>()(
              detail::routing_handler_call_helper::call_params<decltype(handler_)>{handler_, params, req, res});
        }

    private:
        std::function<void(crow::request&, crow::response&, Args...)> handler_;
    };

    const int RULE_SPECIAL_REDIRECT_SLASH = 1;


    /// A search tree.
    class Trie
    {
    public:
        struct Node
        {
            uint16_t rule_index{};
            // Assign the index to the maximum 32 unsigned integer value by default so that any other number (specifically 0) is a valid BP id.
            uint16_t blueprint_index{INVALID_BP_ID};
            std::string key;
            ParamType param = ParamType::MAX; // MAX = No param.
            std::vector<Node> children;

            bool IsSimpleNode() const
            {
                return !rule_index &&
                       blueprint_index == INVALID_BP_ID &&
                       children.size() < 2 &&
                       param == ParamType::MAX &&
                       std::all_of(std::begin(children), std::end(children), [](const Node& x) {
                           return x.param == ParamType::MAX;
                       });
            }

            Node& add_child_node()
            {
                children.emplace_back();
                return children.back();
            }
        };


        Trie()
        {}

        /// Check whether or not the trie is empty.
        bool is_empty()
        {
            return head_.children.empty();
        }

        void optimize()
        {
            for (auto& child : head_.children)
            {
                optimizeNode(child);
            }
        }


    private:
        void optimizeNode(Node& node)
        {
            if (node.children.empty())
                return;
            if (node.IsSimpleNode())
            {
                auto children_temp = std::move(node.children);
                auto& child_temp = children_temp[0];
                node.key += child_temp.key;
                node.rule_index = child_temp.rule_index;
                node.blueprint_index = child_temp.blueprint_index;
                node.children = std::move(child_temp.children);
                optimizeNode(node);
            }
            else
            {
                for (auto& child : node.children)
                {
                    optimizeNode(child);
                }
            }
        }

        void debug_node_print(const Node& node, size_t level)
        {
            if (node.param != ParamType::MAX)
            {
                switch (node.param)
                {
                    case ParamType::INT:
                        CROW_LOG_DEBUG << std::string(3 * level, ' ') << "└➝ "
                                       << "<int>";
                        break;
                    case ParamType::UINT:
                        CROW_LOG_DEBUG << std::string(3 * level, ' ') << "└➝ "
                                       << "<uint>";
                        break;
                    case ParamType::DOUBLE:
                        CROW_LOG_DEBUG << std::string(3 * level, ' ') << "└➝ "
                                       << "<double>";
                        break;
                    case ParamType::STRING:
                        CROW_LOG_DEBUG << std::string(3 * level, ' ') << "└➝ "
                                       << "<string>";
                        break;
                    case ParamType::PATH:
                        CROW_LOG_DEBUG << std::string(3 * level, ' ') << "└➝ "
                                       << "<path>";
                        break;
                    default:
                        CROW_LOG_DEBUG << std::string(3 * level, ' ') << "└➝ "
                                       << "<ERROR>";
                        break;
                }
            }
            else
                CROW_LOG_DEBUG << std::string(3 * level, ' ') << "└➝ " << node.key;

            for (const auto& child : node.children)
            {
                debug_node_print(child, level + 1);
            }
        }

    public:
        void debug_print()
        {
            CROW_LOG_DEBUG << "└➙ ROOT";
            for (const auto& child : head_.children)
                debug_node_print(child, 1);
        }

        void validate()
        {
            if (!head_.IsSimpleNode())
                throw std::runtime_error("Internal error: Trie header should be simple!");
            optimize();
        }

        //Rule_index, Blueprint_index, routing_params
        routing_handle_result find(const std::string& req_url, const Node& node, unsigned pos = 0, routing_params* params = nullptr, std::vector<uint16_t>* blueprints = nullptr) const
        {
            //start params as an empty struct
            routing_params empty;
            if (params == nullptr)
                params = &empty;
            //same for blueprint vector
            std::vector<uint16_t> MT;
            if (blueprints == nullptr)
                blueprints = &MT;

            uint16_t found{};               //The rule index to be found
            std::vector<uint16_t> found_BP; //The Blueprint indices to be found
            routing_params match_params;    //supposedly the final matched parameters

            auto update_found = [&found, &found_BP, &match_params](routing_handle_result& ret) {
                found_BP = std::move(ret.blueprint_indices);
                if (ret.rule_index && (!found || found > ret.rule_index))
                {
                    found = ret.rule_index;
                    match_params = std::move(ret.r_params);
                }
            };

            //if the function was called on a node at the end of the string (the last recursion), return the nodes rule index, and whatever params were passed to the function
            if (pos == req_url.size())
            {
                found_BP = std::move(*blueprints);
                return routing_handle_result{node.rule_index, *blueprints, *params};
            }

            bool found_fragment = false;

            for (const auto& child : node.children)
            {
                if (child.param != ParamType::MAX)
                {
                    if (child.param == ParamType::INT)
                    {
                        char c = req_url[pos];
                        if ((c >= '0' && c <= '9') || c == '+' || c == '-')
                        {
                            char* eptr;
                            errno = 0;
                            long long int value = strtoll(req_url.data() + pos, &eptr, 10);
                            if (errno != ERANGE && eptr != req_url.data() + pos)
                            {
                                found_fragment = true;
                                params->int_params.push_back(value);
                                if (child.blueprint_index != INVALID_BP_ID) blueprints->push_back(child.blueprint_index);
                                auto ret = find(req_url, child, eptr - req_url.data(), params, blueprints);
                                update_found(ret);
                                params->int_params.pop_back();
                                if (!blueprints->empty()) blueprints->pop_back();
                            }
                        }
                    }

                    else if (child.param == ParamType::UINT)
                    {
                        char c = req_url[pos];
                        if ((c >= '0' && c <= '9') || c == '+')
                        {
                            char* eptr;
                            errno = 0;
                            unsigned long long int value = strtoull(req_url.data() + pos, &eptr, 10);
                            if (errno != ERANGE && eptr != req_url.data() + pos)
                            {
                                found_fragment = true;
                                params->uint_params.push_back(value);
                                if (child.blueprint_index != INVALID_BP_ID) blueprints->push_back(child.blueprint_index);
                                auto ret = find(req_url, child, eptr - req_url.data(), params, blueprints);
                                update_found(ret);
                                params->uint_params.pop_back();
                                if (!blueprints->empty()) blueprints->pop_back();
                            }
                        }
                    }

                    else if (child.param == ParamType::DOUBLE)
                    {
                        char c = req_url[pos];
                        if ((c >= '0' && c <= '9') || c == '+' || c == '-' || c == '.')
                        {
                            char* eptr;
                            errno = 0;
                            double value = strtod(req_url.data() + pos, &eptr);
                            if (errno != ERANGE && eptr != req_url.data() + pos)
                            {
                                found_fragment = true;
                                params->double_params.push_back(value);
                                if (child.blueprint_index != INVALID_BP_ID) blueprints->push_back(child.blueprint_index);
                                auto ret = find(req_url, child, eptr - req_url.data(), params, blueprints);
                                update_found(ret);
                                params->double_params.pop_back();
                                if (!blueprints->empty()) blueprints->pop_back();
                            }
                        }
                    }

                    else if (child.param == ParamType::STRING)
                    {
                        size_t epos = pos;
                        for (; epos < req_url.size(); epos++)
                        {
                            if (req_url[epos] == '/')
                                break;
                        }

                        if (epos != pos)
                        {
                            found_fragment = true;
                            params->string_params.push_back(req_url.substr(pos, epos - pos));
                            if (child.blueprint_index != INVALID_BP_ID) blueprints->push_back(child.blueprint_index);
                            auto ret = find(req_url, child, epos, params, blueprints);
                            update_found(ret);
                            params->string_params.pop_back();
                            if (!blueprints->empty()) blueprints->pop_back();
                        }
                    }

                    else if (child.param == ParamType::PATH)
                    {
                        size_t epos = req_url.size();

                        if (epos != pos)
                        {
                            found_fragment = true;
                            params->string_params.push_back(req_url.substr(pos, epos - pos));
                            if (child.blueprint_index != INVALID_BP_ID) blueprints->push_back(child.blueprint_index);
                            auto ret = find(req_url, child, epos, params, blueprints);
                            update_found(ret);
                            params->string_params.pop_back();
                            if (!blueprints->empty()) blueprints->pop_back();
                        }
                    }
                }

                else
                {
                    const std::string& fragment = child.key;
                    if (req_url.compare(pos, fragment.size(), fragment) == 0)
                    {
                        found_fragment = true;
                        if (child.blueprint_index != INVALID_BP_ID) blueprints->push_back(child.blueprint_index);
                        auto ret = find(req_url, child, pos + fragment.size(), params, blueprints);
                        update_found(ret);
                        if (!blueprints->empty()) blueprints->pop_back();
                    }
                }
            }

            if (!found_fragment)
                found_BP = std::move(*blueprints);

            return routing_handle_result{found, found_BP, match_params}; //Called after all the recursions have been done
        }

        routing_handle_result find(const std::string& req_url) const
        {
            return find(req_url, head_);
        }

        //This functions assumes any blueprint info passed is valid
        void add(const std::string& url, uint16_t rule_index, unsigned bp_prefix_length = 0, uint16_t blueprint_index = INVALID_BP_ID)
        {
            auto idx = &head_;

            bool has_blueprint = bp_prefix_length != 0 && blueprint_index != INVALID_BP_ID;

            for (unsigned i = 0; i < url.size(); i++)
            {
                char c = url[i];
                if (c == '<')
                {
                    static struct ParamTraits
                    {
                        ParamType type;
                        std::string name;
                    } paramTraits[] =
                      {
                        {ParamType::INT, "<int>"},
                        {ParamType::UINT, "<uint>"},
                        {ParamType::DOUBLE, "<float>"},
                        {ParamType::DOUBLE, "<double>"},
                        {ParamType::STRING, "<str>"},
                        {ParamType::STRING, "<string>"},
                        {ParamType::PATH, "<path>"},
                      };

                    for (const auto& x : paramTraits)
                    {
                        if (url.compare(i, x.name.size(), x.name) == 0)
                        {
                            bool found = false;
                            for (auto& child : idx->children)
                            {
                                if (child.param == x.type)
                                {
                                    idx = &child;
                                    i += x.name.size();
                                    found = true;
                                    break;
                                }
                            }
                            if (found)
                                break;

                            auto new_node_idx = &idx->add_child_node();
                            new_node_idx->param = x.type;
                            idx = new_node_idx;
                            i += x.name.size();
                            break;
                        }
                    }

                    i--;
                }
                else
                {
                    //This part assumes the tree is unoptimized (every node has a max 1 character key)
                    bool piece_found = false;
                    for (auto& child : idx->children)
                    {
                        if (child.key[0] == c)
                        {
                            idx = &child;
                            piece_found = true;
                            break;
                        }
                    }
                    if (!piece_found)
                    {
                        auto new_node_idx = &idx->add_child_node();
                        new_node_idx->key = c;
                        //The assumption here is that you'd only need to add a blueprint index if the tree didn't have the BP prefix.
                        if (has_blueprint && i == bp_prefix_length)
                            new_node_idx->blueprint_index = blueprint_index;
                        idx = new_node_idx;
                    }
                }
            }

            //check if the last node already has a value (exact url already in Trie)
            if (idx->rule_index)
                throw std::runtime_error("handler already exists for " + url);
            idx->rule_index = rule_index;
        }

    private:
        Node head_;
    };

    /// A blueprint can be considered a smaller section of a Crow app, specifically where the router is concerned.

    ///
    /// You can use blueprints to assign a common prefix to rules' prefix, set custom static and template folders, and set a custom catchall route.
    /// You can also assign nest blueprints for maximum Compartmentalization.
    class Blueprint
    {
    public:
        Blueprint(const std::string& prefix)
          : prefix_(prefix),
            static_dir_(prefix),
            templates_dir_(prefix)
            {}

        Blueprint(const std::string& prefix, const std::string& static_dir):
          prefix_(prefix), static_dir_(static_dir){}

        Blueprint(const std::string& prefix, const std::string& static_dir, const std::string& templates_dir):
          prefix_(prefix), static_dir_(static_dir), templates_dir_(templates_dir){}

        /*
        Blueprint(Blueprint& other)
        {
            prefix_ = std::move(other.prefix_);
            all_rules_ = std::move(other.all_rules_);
        }

        Blueprint(const Blueprint& other)
        {
            prefix_ = other.prefix_;
            all_rules_ = other.all_rules_;
        }
*/
        Blueprint(Blueprint&& value)
        {
            *this = std::move(value);
        }

        Blueprint& operator=(const Blueprint& value) = delete;

        Blueprint& operator=(Blueprint&& value) noexcept
        {
            prefix_ = std::move(value.prefix_);
            static_dir_ = std::move(value.static_dir_);
            templates_dir_ = std::move(value.templates_dir_);
            all_rules_ = std::move(value.all_rules_);
            catchall_rule_ = std::move(value.catchall_rule_);
            blueprints_ = std::move(value.blueprints_);
            mw_indices_ = std::move(value.mw_indices_);
            return *this;
        }

        bool operator==(const Blueprint& value)
        {
            return value.prefix() == prefix_;
        }

        bool operator!=(const Blueprint& value)
        {
            return value.prefix() != prefix_;
        }

        std::string prefix() const
        {
            return prefix_;
        }

        std::string static_dir() const
        {
            return static_dir_;
        }

        void set_added()
        {
            added_ = true;
        }

        bool is_added()
        {
            return added_;
        }

        DynamicRule& new_rule_dynamic(const std::string& rule)
        {
            std::string new_rule = '/' + prefix_ + rule;
            auto ruleObject = new DynamicRule(std::move(new_rule));
            ruleObject->custom_templates_base = templates_dir_;
            all_rules_.emplace_back(ruleObject);

            return *ruleObject;
        }

        template<uint64_t N>
        typename black_magic::arguments<N>::type::template rebind<TaggedRule>& new_rule_tagged(const std::string& rule)
        {
            std::string new_rule = '/' + prefix_ + rule;
            using RuleT = typename black_magic::arguments<N>::type::template rebind<TaggedRule>;

            auto ruleObject = new RuleT(std::move(new_rule));
            ruleObject->custom_templates_base = templates_dir_;
            all_rules_.emplace_back(ruleObject);

            return *ruleObject;
        }

        void register_blueprint(Blueprint& blueprint)
        {
            if (blueprints_.empty() || std::find(blueprints_.begin(), blueprints_.end(), &blueprint) == blueprints_.end())
            {
                apply_blueprint(blueprint);
                blueprints_.emplace_back(&blueprint);
            }
            else
                throw std::runtime_error("blueprint \"" + blueprint.prefix_ + "\" already exists in blueprint \"" + prefix_ + '\"');
        }


        CatchallRule& catchall_rule()
        {
            return catchall_rule_;
        }

        template<typename App, typename... Middlewares>
        void middlewares()
        {
            mw_indices_.push<App, Middlewares...>();
        }

    private:
        void apply_blueprint(Blueprint& blueprint)
        {

            blueprint.prefix_ = prefix_ + '/' + blueprint.prefix_;
            blueprint.static_dir_ = static_dir_ + '/' + blueprint.static_dir_;
            blueprint.templates_dir_ = templates_dir_ + '/' + blueprint.templates_dir_;
            for (auto& rule : blueprint.all_rules_)
            {
                std::string new_rule = '/' + prefix_ + rule->rule_;
                rule->rule_ = new_rule;
            }
            for (Blueprint* bp_child : blueprint.blueprints_)
            {
                Blueprint& bp_ref = *bp_child;
                apply_blueprint(bp_ref);
            }
        }

        std::string prefix_;
        std::string static_dir_;
        std::string templates_dir_;
        std::vector<std::unique_ptr<BaseRule>> all_rules_;
        CatchallRule catchall_rule_;
        std::vector<Blueprint*> blueprints_;
        detail::middleware_indices mw_indices_;
        bool added_{false};

        friend class Router;
    };

    /// Handles matching requests to existing rules and upgrade requests.
    class Router
    {
    public:
        bool using_ssl;

        Router() : using_ssl(false)
        {}

        DynamicRule& new_rule_dynamic(const std::string& rule)
        {
            auto ruleObject = new DynamicRule(rule);
            all_rules_.emplace_back(ruleObject);

            return *ruleObject;
        }

        template<uint64_t N>
        typename black_magic::arguments<N>::type::template rebind<TaggedRule>& new_rule_tagged(const std::string& rule)
        {
            using RuleT = typename black_magic::arguments<N>::type::template rebind<TaggedRule>;

            auto ruleObject = new RuleT(rule);
            all_rules_.emplace_back(ruleObject);

            return *ruleObject;
        }

        CatchallRule& catchall_rule()
        {
            return catchall_rule_;
        }

        void internal_add_rule_object(const std::string& rule, BaseRule* ruleObject)
        {
            internal_add_rule_object(rule, ruleObject, INVALID_BP_ID, blueprints_);
        }

        void internal_add_rule_object(const std::string& rule, BaseRule* ruleObject, const uint16_t& BP_index, std::vector<Blueprint*>& blueprints)
        {
            bool has_trailing_slash = false;
            std::string rule_without_trailing_slash;
            if (rule.size() > 1 && rule.back() == '/')
            {
                has_trailing_slash = true;
                rule_without_trailing_slash = rule;
                rule_without_trailing_slash.pop_back();
            }

            ruleObject->mw_indices_.pack();

            ruleObject->foreach_method([&](int method) {
                per_methods_[method].rules.emplace_back(ruleObject);
                per_methods_[method].trie.add(rule, per_methods_[method].rules.size() - 1, BP_index != INVALID_BP_ID ? blueprints[BP_index]->prefix().length() : 0, BP_index);

                // directory case:
                //   request to '/about' url matches '/about/' rule
                if (has_trailing_slash)
                {
                    per_methods_[method].trie.add(rule_without_trailing_slash, RULE_SPECIAL_REDIRECT_SLASH, BP_index != INVALID_BP_ID ? blueprints[BP_index]->prefix().length() : 0, BP_index);
                }
            });

            ruleObject->set_added();
        }

        void register_blueprint(Blueprint& blueprint)
        {
            if (std::find(blueprints_.begin(), blueprints_.end(), &blueprint) == blueprints_.end())
            {
                blueprints_.emplace_back(&blueprint);
            }
            else
                throw std::runtime_error("blueprint \"" + blueprint.prefix_ + "\" already exists in router");
        }

        void get_recursive_child_methods(Blueprint* blueprint, std::vector<HTTPMethod>& methods)
        {
            //we only need to deal with children if the blueprint has absolutely no methods (meaning its index won't be added to the trie)
            if (blueprint->static_dir_.empty() && blueprint->all_rules_.empty())
            {
                for (Blueprint* bp : blueprint->blueprints_)
                {
                    get_recursive_child_methods(bp, methods);
                }
            }
            else if (!blueprint->static_dir_.empty())
                methods.emplace_back(HTTPMethod::Get);
            for (auto& rule : blueprint->all_rules_)
            {
                rule->foreach_method([&methods](unsigned method) {
                    HTTPMethod method_final = static_cast<HTTPMethod>(method);
                    if (std::find(methods.begin(), methods.end(), method_final) == methods.end())
                        methods.emplace_back(method_final);
                });
            }
        }

        void validate_bp()
        {
            //Take all the routes from the registered blueprints and add them to `all_rules_` to be processed.
            detail::middleware_indices blueprint_mw;
            validate_bp(blueprints_, blueprint_mw);
        }

        void validate_bp(std::vector<Blueprint*> blueprints, detail::middleware_indices& current_mw)
        {
            for (unsigned i = 0; i < blueprints.size(); i++)
            {
                Blueprint* blueprint = blueprints[i];

                if (blueprint->is_added()) continue;

                if (blueprint->static_dir_ == "" && blueprint->all_rules_.empty())
                {
                    std::vector<HTTPMethod> methods;
                    get_recursive_child_methods(blueprint, methods);
                    for (HTTPMethod x : methods)
                    {
                        int method_index = static_cast<int>(x);
                        per_methods_[method_index].trie.add(blueprint->prefix(), 0, blueprint->prefix().length(), method_index);
                    }
                }

                current_mw.merge_back(blueprint->mw_indices_);
                for (auto& rule : blueprint->all_rules_)
                {
                    if (rule && !rule->is_added())
                    {
                        auto upgraded = rule->upgrade();
                        if (upgraded)
                            rule = std::move(upgraded);
                        rule->validate();
                        rule->mw_indices_.merge_front(current_mw);
                        internal_add_rule_object(rule->rule(), rule.get(), i, blueprints);
                    }
                }
                validate_bp(blueprint->blueprints_, current_mw);
                current_mw.pop_back(blueprint->mw_indices_);
                blueprint->set_added();
            }
        }

        void validate()
        {
            for (auto& rule : all_rules_)
            {
                if (rule && !rule->is_added())
                {
                    auto upgraded = rule->upgrade();
                    if (upgraded)
                        rule = std::move(upgraded);
                    rule->validate();
                    internal_add_rule_object(rule->rule(), rule.get());
                }
            }
            for (auto& per_method : per_methods_)
            {
                per_method.trie.validate();
            }
        }

        // TODO maybe add actual_method
        template<typename Adaptor>
        void handle_upgrade(const request& req, response& res, Adaptor&& adaptor)
        {
            if (req.method >= HTTPMethod::InternalMethodCount)
                return;

            auto& per_method = per_methods_[static_cast<int>(req.method)];
            auto& rules = per_method.rules;
            unsigned rule_index = per_method.trie.find(req.url).rule_index;

            if (!rule_index)
            {
                for (auto& method : per_methods_)
                {
                    if (method.trie.find(req.url).rule_index)
                    {
                        CROW_LOG_DEBUG << "Cannot match method " << req.url << " " << method_name(req.method);
                        res = response(405);
                        res.end();
                        return;
                    }
                }

                CROW_LOG_INFO << "Cannot match rules " << req.url;
                res = response(404);
                res.end();
                return;
            }

            if (rule_index >= rules.size())
                throw std::runtime_error("Trie internal structure corrupted!");

            if (rule_index == RULE_SPECIAL_REDIRECT_SLASH)
            {
                CROW_LOG_INFO << "Redirecting to a url with trailing slash: " << req.url;
                res = response(301);
                res.add_header("Location", req.url + "/");
                res.end();
                return;
            }

            CROW_LOG_DEBUG << "Matched rule (upgrade) '" << rules[rule_index]->rule_ << "' " << static_cast<uint32_t>(req.method) << " / " << rules[rule_index]->get_methods();

            try
            {
                rules[rule_index]->handle_upgrade(req, res, std::move(adaptor));
            }
            catch (...)
            {
                exception_handler_(res);
                res.end();
                return;
            }
        }

        void get_found_bp(std::vector<uint16_t>& bp_i, std::vector<Blueprint*>& blueprints, std::vector<Blueprint*>& found_bps, uint16_t index = 0)
        {
            // This statement makes 3 assertions:
            // 1. The index is above 0.
            // 2. The index does not lie outside the given blueprint list.
            // 3. The next blueprint we're adding has a prefix that starts the same as the already added blueprint + a slash (the rest is irrelevant).
            //
            // This is done to prevent a blueprint that has a prefix of "bp_prefix2" to be assumed as a child of one that has "bp_prefix".
            //
            // If any of the assertions is untrue, we delete the last item added, and continue using the blueprint list of the blueprint found before, the topmost being the router's list
            auto verify_prefix = [&bp_i, &index, &blueprints, &found_bps]() {
                return index > 0 &&
                       bp_i[index] < blueprints.size() &&
                       blueprints[bp_i[index]]->prefix().substr(0, found_bps[index - 1]->prefix().length() + 1).compare(std::string(found_bps[index - 1]->prefix() + '/')) == 0;
            };
            if (index < bp_i.size())
            {

                if (verify_prefix())
                {
                    found_bps.push_back(blueprints[bp_i[index]]);
                    get_found_bp(bp_i, found_bps.back()->blueprints_, found_bps, ++index);
                }
                else
                {
                    if (found_bps.size() < 2)
                    {
                        found_bps.clear();
                        found_bps.push_back(blueprints_[bp_i[index]]);
                    }
                    else
                    {
                        found_bps.pop_back();
                        Blueprint* last_element = found_bps.back();
                        found_bps.push_back(last_element->blueprints_[bp_i[index]]);
                    }
                    get_found_bp(bp_i, found_bps.back()->blueprints_, found_bps, ++index);
                }
            }
        }

        /// Is used to handle errors, you insert the error code, found route, request, and response. and it'll either call the appropriate catchall route (considering the blueprint system) and send you a status string (which is mainly used for debug messages), or just set the response code to the proper error code.
        std::string get_error(unsigned short code, routing_handle_result& found, const request& req, response& res)
        {
            res.code = code;
            std::vector<Blueprint*> bps_found;
            get_found_bp(found.blueprint_indices, blueprints_, bps_found);
            for (int i = bps_found.size() - 1; i > 0; i--)
            {
                std::vector<uint16_t> bpi = found.blueprint_indices;
                if (bps_found[i]->catchall_rule().has_handler())
                {
                    try
                    {
                        bps_found[i]->catchall_rule().handler_(req, res);
                    }
                    catch (...)
                    {
                        exception_handler_(res);
                    }
#ifdef CROW_ENABLE_DEBUG
                    return std::string("Redirected to Blueprint \"" + bps_found[i]->prefix() + "\" Catchall rule");
#else
                    return std::string();
#endif
                }
            }
            if (catchall_rule_.has_handler())
            {
                try
                {
                    catchall_rule_.handler_(req, res);
                }
                catch (...)
                {
                    exception_handler_(res);
                }
#ifdef CROW_ENABLE_DEBUG
                return std::string("Redirected to global Catchall rule");
#else
                return std::string();
#endif
            }
            return std::string();
        }

        std::unique_ptr<routing_handle_result> handle_initial(request& req, response& res)
        {
            HTTPMethod method_actual = req.method;

            std::unique_ptr<routing_handle_result> found{
              new routing_handle_result(
                0,
                std::vector<uint16_t>(),
                routing_params(),
                HTTPMethod::InternalMethodCount)}; // This is always returned to avoid a null pointer dereference.

            // NOTE(EDev): This most likely will never run since the parser should handle this situation and close the connection before it gets here.
            if (CROW_UNLIKELY(req.method >= HTTPMethod::InternalMethodCount))
                return found;
            else if (req.method == HTTPMethod::Head)
            {
                *found = per_methods_[static_cast<int>(method_actual)].trie.find(req.url);
                // support HEAD requests using GET if not defined as method for the requested URL
                if (!found->rule_index)
                {
                    method_actual = HTTPMethod::Get;
                    *found = per_methods_[static_cast<int>(method_actual)].trie.find(req.url);
                    if (!found->rule_index) // If a route is still not found, return a 404 without executing the rest of the HEAD specific code.
                    {
                        CROW_LOG_DEBUG << "Cannot match rules " << req.url;
                        res = response(404); //TODO(EDev): Should this redirect to catchall?
                        res.end();
                        return found;
                    }
                }

                res.skip_body = true;
                found->method = method_actual;
                return found;
            }
            else if (req.method == HTTPMethod::Options)
            {
                std::string allow = "OPTIONS, HEAD";

                if (req.url == "/*")
                {
                    for (int i = 0; i < static_cast<int>(HTTPMethod::InternalMethodCount); i++)
                    {
                        if (static_cast<int>(HTTPMethod::Head) == i)
                            continue; // HEAD is always allowed

                        if (!per_methods_[i].trie.is_empty())
                        {
                            allow.append(", ");
                            allow.append(method_name(static_cast<HTTPMethod>(i)));
                        }
                    }
#ifdef CROW_RETURNS_OK_ON_HTTP_OPTIONS_REQUEST
                    res = response(crow::status::OK);
#else
                    res = response(crow::status::NO_CONTENT);
#endif

                    res.set_header("Allow", allow);
                    res.end();
                    found->method = method_actual;
                    return found;
                }
                else
                {
                    bool rules_matched = false;
                    for (int i = 0; i < static_cast<int>(HTTPMethod::InternalMethodCount); i++)
                    {
                        if (per_methods_[i].trie.find(req.url).rule_index)
                        {
                            rules_matched = true;

                            if (static_cast<int>(HTTPMethod::Head) == i)
                                continue; // HEAD is always allowed

                            allow.append(", ");
                            allow.append(method_name(static_cast<HTTPMethod>(i)));
                        }
                    }
                    if (rules_matched)
                    {
#ifdef CROW_RETURNS_OK_ON_HTTP_OPTIONS_REQUEST
                        res = response(crow::status::OK);
#else
                        res = response(crow::status::NO_CONTENT);
#endif
                        res.set_header("Allow", allow);
                        res.end();
                        found->method = method_actual;
                        return found;
                    }
                    else
                    {
                        CROW_LOG_DEBUG << "Cannot match rules " << req.url;
                        res = response(404); //TODO(EDev): Should this redirect to catchall?
                        res.end();
                        return found;
                    }
                }
            }
            else // Every request that isn't a HEAD or OPTIONS request
            {
                *found = per_methods_[static_cast<int>(method_actual)].trie.find(req.url);
                // TODO(EDev): maybe ending the else here would allow the requests coming from above (after removing the return statement) to be checked on whether they actually point to a route
                if (!found->rule_index)
                {
                    for (auto& per_method : per_methods_)
                    {
                        if (per_method.trie.find(req.url).rule_index) //Route found, but in another method
                        {
                            const std::string error_message(get_error(405, *found, req, res));
                            CROW_LOG_DEBUG << "Cannot match method " << req.url << " " << method_name(method_actual) << ". " << error_message;
                            res.end();
                            return found;
                        }
                    }
                    //Route does not exist anywhere

                    const std::string error_message(get_error(404, *found, req, res));
                    CROW_LOG_DEBUG << "Cannot match rules " << req.url << ". " << error_message;
                    res.end();
                    return found;
                }

                found->method = method_actual;
                return found;
            }
        }

        template<typename App>
        void handle(request& req, response& res, routing_handle_result found)
        {
            HTTPMethod method_actual = found.method;
            auto& rules = per_methods_[static_cast<int>(method_actual)].rules;
            unsigned rule_index = found.rule_index;

            if (rule_index >= rules.size())
                throw std::runtime_error("Trie internal structure corrupted!");

            if (rule_index == RULE_SPECIAL_REDIRECT_SLASH)
            {
                CROW_LOG_INFO << "Redirecting to a url with trailing slash: " << req.url;
                res = response(301);
                res.add_header("Location", req.url + "/");
                res.end();
                return;
            }

            CROW_LOG_DEBUG << "Matched rule '" << rules[rule_index]->rule_ << "' " << static_cast<uint32_t>(req.method) << " / " << rules[rule_index]->get_methods();

            try
            {
                BaseRule& rule = *rules[rule_index];
                handle_rule<App>(rule, req, res, found.r_params);
            }
            catch (...)
            {
                exception_handler_(res);
                res.end();
                return;
            }
        }

        template<typename App>
        typename std::enable_if<std::tuple_size<typename App::mw_container_t>::value != 0, void>::type
          handle_rule(BaseRule& rule, crow::request& req, crow::response& res, const crow::routing_params& rp)
        {
            if (!rule.mw_indices_.empty())
            {
                auto& ctx = *reinterpret_cast<typename App::context_t*>(req.middleware_context);
                auto& container = *reinterpret_cast<typename App::mw_container_t*>(req.middleware_container);
                detail::middleware_call_criteria_dynamic<false> crit_fwd(rule.mw_indices_.indices());

                auto glob_completion_handler = std::move(res.complete_request_handler_);
                res.complete_request_handler_ = [] {};

                detail::middleware_call_helper<decltype(crit_fwd),
                                               0, typename App::context_t, typename App::mw_container_t>(crit_fwd, container, req, res, ctx);

                if (res.completed_)
                {
                    glob_completion_handler();
                    return;
                }

                res.complete_request_handler_ = [&rule, &ctx, &container, &req, &res, glob_completion_handler] {
                    detail::middleware_call_criteria_dynamic<true> crit_bwd(rule.mw_indices_.indices());

                    detail::after_handlers_call_helper<
                      decltype(crit_bwd),
                      std::tuple_size<typename App::mw_container_t>::value - 1,
                      typename App::context_t,
                      typename App::mw_container_t>(crit_bwd, container, ctx, req, res);
                    glob_completion_handler();
                };
            }
            rule.handle(req, res, rp);
        }

        template<typename App>
        typename std::enable_if<std::tuple_size<typename App::mw_container_t>::value == 0, void>::type
          handle_rule(BaseRule& rule, crow::request& req, crow::response& res, const crow::routing_params& rp)
        {
            rule.handle(req, res, rp);
        }

        void debug_print()
        {
            for (int i = 0; i < static_cast<int>(HTTPMethod::InternalMethodCount); i++)
            {
                Trie& trie_ = per_methods_[i].trie;
                if (!trie_.is_empty())
                {
                    CROW_LOG_DEBUG << method_name(static_cast<HTTPMethod>(i));
                    trie_.debug_print();
                }
            }
        }

        std::vector<Blueprint*>& blueprints()
        {
            return blueprints_;
        }

        std::function<void(crow::response&)>& exception_handler()
        {
            return exception_handler_;
        }

        static void default_exception_handler(response& res)
        {
            // any uncaught exceptions become 500s
            res = response(500);

            try
            {
                throw;
            }
            catch (const bad_request& e)
            {
                res = response (400);
                res.body = e.what();
            }
            catch (const std::exception& e)
            {
                CROW_LOG_ERROR << "An uncaught exception occurred: " << e.what();
            }
            catch (...)
            {
                CROW_LOG_ERROR << "An uncaught exception occurred. The type was unknown so no information was available.";
            }
        }

    private:
        CatchallRule catchall_rule_;

        struct PerMethod
        {
            std::vector<BaseRule*> rules;
            Trie trie;

            // rule index 0, 1 has special meaning; preallocate it to avoid duplication.
            PerMethod():
              rules(2) {}
        };
        std::array<PerMethod, static_cast<int>(HTTPMethod::InternalMethodCount)> per_methods_;
        std::vector<std::unique_ptr<BaseRule>> all_rules_;
        std::vector<Blueprint*> blueprints_;
        std::function<void(crow::response&)> exception_handler_ = &default_exception_handler;
    };
} // namespace crow


namespace crow
{
    struct CORSHandler;

    /// Used for tuning CORS policies
    struct CORSRules
    {
        friend struct crow::CORSHandler;

        /// Set Access-Control-Allow-Origin. Default is "*"
        CORSRules& origin(const std::string& origin)
        {
            origin_ = origin;
            return *this;
        }

        /// Set Access-Control-Allow-Methods. Default is "*"
        CORSRules& methods(crow::HTTPMethod method)
        {
            add_list_item(methods_, crow::method_name(method));
            return *this;
        }

        /// Set Access-Control-Allow-Methods. Default is "*"
        template<typename... Methods>
        CORSRules& methods(crow::HTTPMethod method, Methods... method_list)
        {
            add_list_item(methods_, crow::method_name(method));
            methods(method_list...);
            return *this;
        }

        /// Set Access-Control-Allow-Headers. Default is "*"
        CORSRules& headers(const std::string& header)
        {
            add_list_item(headers_, header);
            return *this;
        }

        /// Set Access-Control-Allow-Headers. Default is "*"
        template<typename... Headers>
        CORSRules& headers(const std::string& header, Headers... header_list)
        {
            add_list_item(headers_, header);
            headers(header_list...);
            return *this;
        }

        /// Set Access-Control-Expose-Headers. Default is none
        CORSRules& expose(const std::string& header)
        {
            add_list_item(exposed_headers_, header);
            return *this;
        }

        /// Set Access-Control-Expose-Headers. Default is none
        template<typename... Headers>
        CORSRules& expose(const std::string& header, Headers... header_list)
        {
            add_list_item(exposed_headers_, header);
            expose(header_list...);
            return *this;
        }

        /// Set Access-Control-Max-Age. Default is none
        CORSRules& max_age(int max_age)
        {
            max_age_ = std::to_string(max_age);
            return *this;
        }

        /// Enable Access-Control-Allow-Credentials
        CORSRules& allow_credentials()
        {
            allow_credentials_ = true;
            return *this;
        }

        /// Ignore CORS and don't send any headers
        void ignore()
        {
            ignore_ = true;
        }

        /// Handle CORS on specific prefix path
        CORSRules& prefix(const std::string& prefix);

        /// Handle CORS for specific blueprint
        CORSRules& blueprint(const Blueprint& bp);

        /// Global CORS policy
        CORSRules& global();

    private:
        CORSRules() = delete;
        CORSRules(CORSHandler* handler):
          handler_(handler) {}

        /// build comma separated list
        void add_list_item(std::string& list, const std::string& val)
        {
            if (list == "*") list = "";
            if (list.size() > 0) list += ", ";
            list += val;
        }

        /// Set header `key` to `value` if it is not set
        void set_header_no_override(const std::string& key, const std::string& value, crow::response& res)
        {
            if (value.size() == 0) return;
            if (!get_header_value(res.headers, key).empty()) return;
            res.add_header(key, value);
        }

        /// Set response headers
        void apply(const request& req, response& res)
        {
            if (ignore_) return;

            set_header_no_override("Access-Control-Allow-Methods", methods_, res);
            set_header_no_override("Access-Control-Allow-Headers", headers_, res);
            set_header_no_override("Access-Control-Expose-Headers", exposed_headers_, res);
            set_header_no_override("Access-Control-Max-Age", max_age_, res);

            bool origin_set = false;

            if (req.method != HTTPMethod::Options)
            {
                if (allow_credentials_)
                {
                    set_header_no_override("Access-Control-Allow-Credentials", "true", res);
                    if (origin_ == "*")
                    {
                        set_header_no_override("Access-Control-Allow-Origin", req.get_header_value("Origin"), res);
                        origin_set = true;
                    }
                }
            }

            if( !origin_set){
                set_header_no_override("Access-Control-Allow-Origin", origin_, res);
            }
        }

        bool ignore_ = false;
        // TODO: support multiple origins that are dynamically selected
        std::string origin_ = "*";
        std::string methods_ = "*";
        std::string headers_ = "*";
        std::string exposed_headers_;
        std::string max_age_;
        bool allow_credentials_ = false;

        CORSHandler* handler_;
    };

    /// CORSHandler is a global middleware for setting CORS headers.

    ///
    /// By default, it sets Access-Control-Allow-Origin/Methods/Headers to "*".
    /// The default behaviour can be changed with the `global()` cors rule.
    /// Additional rules for prexies can be added with `prefix()`.
    struct CORSHandler
    {
        struct context
        {};

        void before_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
        {}

        void after_handle(crow::request& req, crow::response& res, context& /*ctx*/)
        {
            auto& rule = find_rule(req.url);
            rule.apply(req, res);
        }

        /// Handle CORS on a specific prefix path
        CORSRules& prefix(const std::string& prefix)
        {
            rules.emplace_back(prefix, CORSRules(this));
            return rules.back().second;
        }

        /// Handle CORS for a specific blueprint
        CORSRules& blueprint(const Blueprint& bp)
        {
            rules.emplace_back(bp.prefix(), CORSRules(this));
            return rules.back().second;
        }

        /// Get the global CORS policy
        CORSRules& global()
        {
            return default_;
        }

    private:
        CORSRules& find_rule(const std::string& path)
        {
            // TODO: use a trie in case of many rules
            for (auto& rule : rules)
            {
                // Check if path starts with a rules prefix
                if (path.rfind(rule.first, 0) == 0)
                {
                    return rule.second;
                }
            }
            return default_;
        }

        std::vector<std::pair<std::string, CORSRules>> rules;
        CORSRules default_ = CORSRules(this);
    };

    inline CORSRules& CORSRules::prefix(const std::string& prefix)
    {
        return handler_->prefix(prefix);
    }

    inline CORSRules& CORSRules::blueprint(const Blueprint& bp)
    {
        return handler_->blueprint(bp);
    }

    inline CORSRules& CORSRules::global()
    {
        return handler_->global();
    }

} // namespace crow


#ifdef CROW_USE_BOOST
#include <boost/asio.hpp>
#else
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#include <asio.hpp>
#endif

#include <algorithm>
#include <atomic>
#include <chrono>
#include <memory>
#include <vector>


namespace crow
{
#ifdef CROW_USE_BOOST
    namespace asio = boost::asio;
    using error_code = boost::system::error_code;
#else
    using error_code = asio::error_code;
#endif
    using tcp = asio::ip::tcp;

#ifdef CROW_ENABLE_DEBUG
    static std::atomic<int> connectionCount;
#endif

    /// An HTTP connection.
    template<typename Adaptor, typename Handler, typename... Middlewares>
    class Connection : public std::enable_shared_from_this<Connection<Adaptor, Handler, Middlewares...>>
    {
        friend struct crow::response;

    public:
        Connection(
          asio::io_context& io_context,
          Handler* handler,
          const std::string& server_name,
          std::tuple<Middlewares...>* middlewares,
          std::function<std::string()>& get_cached_date_str_f,
          detail::task_timer& task_timer,
          typename Adaptor::context* adaptor_ctx_,
          std::atomic<unsigned int>& queue_length):
          adaptor_(io_context, adaptor_ctx_),
          handler_(handler),
          parser_(this),
          req_(parser_.req),
          server_name_(server_name),
          middlewares_(middlewares),
          get_cached_date_str(get_cached_date_str_f),
          task_timer_(task_timer),
          res_stream_threshold_(handler->stream_threshold()),
          queue_length_(queue_length)
        {
            queue_length_++;
#ifdef CROW_ENABLE_DEBUG
            connectionCount++;
            CROW_LOG_DEBUG << "Connection (" << this << ") allocated, total: " << connectionCount;
#endif
        }

        ~Connection()
        {
            queue_length_--;
#ifdef CROW_ENABLE_DEBUG
            connectionCount--;
            CROW_LOG_DEBUG << "Connection (" << this << ") freed, total: " << connectionCount;
#endif
        }

        /// The TCP socket on top of which the connection is established.
        decltype(std::declval<Adaptor>().raw_socket())& socket()
        {
            return adaptor_.raw_socket();
        }

        void start()
        {
            auto self = this->shared_from_this();
            adaptor_.start([self](const error_code& ec) {
                if (!ec)
                {
                    self->start_deadline();
                    self->parser_.clear();

                    self->do_read();
                }
                else
                {
                    CROW_LOG_ERROR << "Could not start adaptor: " << ec.message();
                }
            });
        }

        void handle_url()
        {
            routing_handle_result_ = handler_->handle_initial(req_, res);
            // if no route is found for the request method, return the response without parsing or processing anything further.
            if (!routing_handle_result_->rule_index)
            {
                parser_.done();
                need_to_call_after_handlers_ = true;
                complete_request();
            }
        }

        void handle_header()
        {
            // HTTP 1.1 Expect: 100-continue
            if (req_.http_ver_major == 1 && req_.http_ver_minor == 1 && get_header_value(req_.headers, "expect") == "100-continue")
            {
                continue_requested = true;
                buffers_.clear();
                static std::string expect_100_continue = "HTTP/1.1 100 Continue\r\n\r\n";
                buffers_.emplace_back(expect_100_continue.data(), expect_100_continue.size());
                do_write_sync(buffers_);
            }
        }

        void handle()
        {
            // TODO(EDev): cancel_deadline_timer should be looked into, it might be a good idea to add it to handle_url() and then restart the timer once everything passes
            cancel_deadline_timer();
            bool is_invalid_request = false;
            add_keep_alive_ = false;

            // Create context
            ctx_ = detail::context<Middlewares...>();
            req_.middleware_context = static_cast<void*>(&ctx_);
            req_.middleware_container = static_cast<void*>(middlewares_);
            req_.io_context = &adaptor_.get_io_context();
            req_.remote_ip_address = adaptor_.address();
            add_keep_alive_ = req_.keep_alive;
            close_connection_ = req_.close_connection;

            if (req_.check_version(1, 1)) // HTTP/1.1
            {
                if (!req_.headers.count("host"))
                {
                    is_invalid_request = true;
                    res = response(400);
                }
                else if (req_.upgrade)
                {
                    // h2 or h2c headers
                    if (req_.get_header_value("upgrade").find("h2")==0)
                    {
                        // TODO(ipkn): HTTP/2
                        // currently, ignore upgrade header
                    }
                    else
                    {

                        detail::middleware_call_helper<detail::middleware_call_criteria_only_global,
                                                       0, decltype(ctx_), decltype(*middlewares_)>({}, *middlewares_, req_, res, ctx_);
                        close_connection_ = true;
                        handler_->handle_upgrade(req_, res, std::move(adaptor_));
                        return;
                    }
                }
            }

            CROW_LOG_INFO << "Request: " << utility::lexical_cast<std::string>(adaptor_.remote_endpoint()) << " " << this << " HTTP/" << (char)(req_.http_ver_major + '0') << "." << (char)(req_.http_ver_minor + '0') << ' ' << method_name(req_.method) << " " << req_.url;


            need_to_call_after_handlers_ = false;
            if (!is_invalid_request)
            {
                res.complete_request_handler_ = nullptr;
                auto self = this->shared_from_this();
                res.is_alive_helper_ = [self]() -> bool {
                    return self->adaptor_.is_open();
                };

                detail::middleware_call_helper<detail::middleware_call_criteria_only_global,
                                               0, decltype(ctx_), decltype(*middlewares_)>({}, *middlewares_, req_, res, ctx_);

                if (!res.completed_)
                {
                    res.complete_request_handler_ = [self] {
                        self->complete_request();
                    };
                    need_to_call_after_handlers_ = true;
                    handler_->handle(req_, res, routing_handle_result_);
                    if (add_keep_alive_)
                        res.set_header("connection", "Keep-Alive");
                }
                else
                {
                    complete_request();
                }
            }
            else
            {
                complete_request();
            }
        }

        /// Call the after handle middleware and send the write the response to the connection.
        void complete_request()
        {
            CROW_LOG_INFO << "Response: " << this << ' ' << req_.raw_url << ' ' << res.code << ' ' << close_connection_;
            res.is_alive_helper_ = nullptr;

            if (need_to_call_after_handlers_)
            {
                need_to_call_after_handlers_ = false;

                // call all after_handler of middlewares
                detail::after_handlers_call_helper<
                  detail::middleware_call_criteria_only_global,
                  (static_cast<int>(sizeof...(Middlewares)) - 1),
                  decltype(ctx_),
                  decltype(*middlewares_)>({}, *middlewares_, ctx_, req_, res);
            }
#ifdef CROW_ENABLE_COMPRESSION
            if (!res.body.empty() && handler_->compression_used())
            {
                std::string accept_encoding = req_.get_header_value("Accept-Encoding");
                if (!accept_encoding.empty() && res.compressed)
                {
                    switch (handler_->compression_algorithm())
                    {
                        case compression::DEFLATE:
                            if (accept_encoding.find("deflate") != std::string::npos)
                            {
                                res.body = compression::compress_string(res.body, compression::algorithm::DEFLATE);
                                res.set_header("Content-Encoding", "deflate");
                            }
                            break;
                        case compression::GZIP:
                            if (accept_encoding.find("gzip") != std::string::npos)
                            {
                                res.body = compression::compress_string(res.body, compression::algorithm::GZIP);
                                res.set_header("Content-Encoding", "gzip");
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
#endif

            prepare_buffers();

            if (res.is_static_type())
            {
                do_write_static();
            }
            else
            {
                do_write_general();
            }
        }

    private:
        void prepare_buffers()
        {
            res.complete_request_handler_ = nullptr;
            res.is_alive_helper_ = nullptr;

            if (!adaptor_.is_open())
            {
                //CROW_LOG_DEBUG << this << " delete (socket is closed) " << is_reading << ' ' << is_writing;
                //delete this;
                return;
            }
            // TODO(EDev): HTTP version in status codes should be dynamic
            // Keep in sync with common.h/status
            static std::unordered_map<int, std::string> statusCodes = {
              {status::CONTINUE, "HTTP/1.1 100 Continue\r\n"},
              {status::SWITCHING_PROTOCOLS, "HTTP/1.1 101 Switching Protocols\r\n"},

              {status::OK, "HTTP/1.1 200 OK\r\n"},
              {status::CREATED, "HTTP/1.1 201 Created\r\n"},
              {status::ACCEPTED, "HTTP/1.1 202 Accepted\r\n"},
              {status::NON_AUTHORITATIVE_INFORMATION, "HTTP/1.1 203 Non-Authoritative Information\r\n"},
              {status::NO_CONTENT, "HTTP/1.1 204 No Content\r\n"},
              {status::RESET_CONTENT, "HTTP/1.1 205 Reset Content\r\n"},
              {status::PARTIAL_CONTENT, "HTTP/1.1 206 Partial Content\r\n"},

              {status::MULTIPLE_CHOICES, "HTTP/1.1 300 Multiple Choices\r\n"},
              {status::MOVED_PERMANENTLY, "HTTP/1.1 301 Moved Permanently\r\n"},
              {status::FOUND, "HTTP/1.1 302 Found\r\n"},
              {status::SEE_OTHER, "HTTP/1.1 303 See Other\r\n"},
              {status::NOT_MODIFIED, "HTTP/1.1 304 Not Modified\r\n"},
              {status::TEMPORARY_REDIRECT, "HTTP/1.1 307 Temporary Redirect\r\n"},
              {status::PERMANENT_REDIRECT, "HTTP/1.1 308 Permanent Redirect\r\n"},

              {status::BAD_REQUEST, "HTTP/1.1 400 Bad Request\r\n"},
              {status::UNAUTHORIZED, "HTTP/1.1 401 Unauthorized\r\n"},
              {status::FORBIDDEN, "HTTP/1.1 403 Forbidden\r\n"},
              {status::NOT_FOUND, "HTTP/1.1 404 Not Found\r\n"},
              {status::METHOD_NOT_ALLOWED, "HTTP/1.1 405 Method Not Allowed\r\n"},
              {status::NOT_ACCEPTABLE, "HTTP/1.1 406 Not Acceptable\r\n"},
              {status::PROXY_AUTHENTICATION_REQUIRED, "HTTP/1.1 407 Proxy Authentication Required\r\n"},
              {status::CONFLICT, "HTTP/1.1 409 Conflict\r\n"},
              {status::GONE, "HTTP/1.1 410 Gone\r\n"},
              {status::PAYLOAD_TOO_LARGE, "HTTP/1.1 413 Payload Too Large\r\n"},
              {status::UNSUPPORTED_MEDIA_TYPE, "HTTP/1.1 415 Unsupported Media Type\r\n"},
              {status::RANGE_NOT_SATISFIABLE, "HTTP/1.1 416 Range Not Satisfiable\r\n"},
              {status::EXPECTATION_FAILED, "HTTP/1.1 417 Expectation Failed\r\n"},
              {status::PRECONDITION_REQUIRED, "HTTP/1.1 428 Precondition Required\r\n"},
              {status::TOO_MANY_REQUESTS, "HTTP/1.1 429 Too Many Requests\r\n"},
              {status::UNAVAILABLE_FOR_LEGAL_REASONS, "HTTP/1.1 451 Unavailable For Legal Reasons\r\n"},

              {status::INTERNAL_SERVER_ERROR, "HTTP/1.1 500 Internal Server Error\r\n"},
              {status::NOT_IMPLEMENTED, "HTTP/1.1 501 Not Implemented\r\n"},
              {status::BAD_GATEWAY, "HTTP/1.1 502 Bad Gateway\r\n"},
              {status::SERVICE_UNAVAILABLE, "HTTP/1.1 503 Service Unavailable\r\n"},
              {status::GATEWAY_TIMEOUT, "HTTP/1.1 504 Gateway Timeout\r\n"},
              {status::VARIANT_ALSO_NEGOTIATES, "HTTP/1.1 506 Variant Also Negotiates\r\n"},
            };

            static const std::string seperator = ": ";

            buffers_.clear();
            buffers_.reserve(4 * (res.headers.size() + 5) + 3);

            if (!statusCodes.count(res.code))
            {
                CROW_LOG_WARNING << this << " status code "
                                 << "(" << res.code << ")"
                                 << " not defined, returning 500 instead";
                res.code = 500;
            }

            auto& status = statusCodes.find(res.code)->second;
            buffers_.emplace_back(status.data(), status.size());

            if (res.code >= 400 && res.body.empty())
                res.body = statusCodes[res.code].substr(9);

            for (auto& kv : res.headers)
            {
                buffers_.emplace_back(kv.first.data(), kv.first.size());
                buffers_.emplace_back(seperator.data(), seperator.size());
                buffers_.emplace_back(kv.second.data(), kv.second.size());
                buffers_.emplace_back(crlf.data(), crlf.size());
            }

            if (!res.manual_length_header && !res.headers.count("content-length"))
            {
                content_length_ = std::to_string(res.body.size());
                static std::string content_length_tag = "Content-Length: ";
                buffers_.emplace_back(content_length_tag.data(), content_length_tag.size());
                buffers_.emplace_back(content_length_.data(), content_length_.size());
                buffers_.emplace_back(crlf.data(), crlf.size());
            }
            if (!res.headers.count("server") && !server_name_.empty())
            {
                static std::string server_tag = "Server: ";
                buffers_.emplace_back(server_tag.data(), server_tag.size());
                buffers_.emplace_back(server_name_.data(), server_name_.size());
                buffers_.emplace_back(crlf.data(), crlf.size());
            }
            if (!res.headers.count("date"))
            {
                static std::string date_tag = "Date: ";
                date_str_ = get_cached_date_str();
                buffers_.emplace_back(date_tag.data(), date_tag.size());
                buffers_.emplace_back(date_str_.data(), date_str_.size());
                buffers_.emplace_back(crlf.data(), crlf.size());
            }
            if (add_keep_alive_)
            {
                static std::string keep_alive_tag = "Connection: Keep-Alive";
                buffers_.emplace_back(keep_alive_tag.data(), keep_alive_tag.size());
                buffers_.emplace_back(crlf.data(), crlf.size());
            }

            buffers_.emplace_back(crlf.data(), crlf.size());
        }

        void do_write_static()
        {
            asio::write(adaptor_.socket(), buffers_);

            if (res.file_info.statResult == 0)
            {
                std::ifstream is(res.file_info.path.c_str(), std::ios::in | std::ios::binary);
                std::vector<asio::const_buffer> buffers{1};
                char buf[16384];
                is.read(buf, sizeof(buf));
                while (is.gcount() > 0)
                {
                    buffers[0] = asio::buffer(buf, is.gcount());
                    do_write_sync(buffers);
                    is.read(buf, sizeof(buf));
                }
            }
            if (close_connection_)
            {
                adaptor_.shutdown_readwrite();
                adaptor_.close();
                CROW_LOG_DEBUG << this << " from write (static)";
            }

            res.end();
            res.clear();
            buffers_.clear();
            parser_.clear();
        }

        void do_write_general()
        {
            if (res.body.length() < res_stream_threshold_)
            {
                res_body_copy_.swap(res.body);
                buffers_.emplace_back(res_body_copy_.data(), res_body_copy_.size());

                do_write_sync(buffers_);

                if (need_to_start_read_after_complete_)
                {
                    need_to_start_read_after_complete_ = false;
                    start_deadline();
                    do_read();
                }
            }
            else
            {
                asio::write(adaptor_.socket(), buffers_); // Write the response start / headers
                cancel_deadline_timer();
                if (res.body.length() > 0)
                {
                    std::vector<asio::const_buffer> buffers{1};
                    const uint8_t* data = reinterpret_cast<const uint8_t*>(res.body.data());
                    size_t length = res.body.length();
                    for (size_t transferred = 0; transferred < length;)
                    {
                        size_t to_transfer = CROW_MIN(16384UL, length - transferred);
                        buffers[0] = asio::const_buffer(data + transferred, to_transfer);
                        do_write_sync(buffers);
                        transferred += to_transfer;
                    }
                }
                if (close_connection_)
                {
                    adaptor_.shutdown_readwrite();
                    adaptor_.close();
                    CROW_LOG_DEBUG << this << " from write (res_stream)";
                }

                res.end();
                res.clear();
                buffers_.clear();
                parser_.clear();
            }
        }

        void do_read()
        {
            auto self = this->shared_from_this();
            adaptor_.socket().async_read_some(
              asio::buffer(buffer_),
              [self](const error_code& ec, std::size_t bytes_transferred) {
                  bool error_while_reading = true;
                  if (!ec)
                  {
                      bool ret = self->parser_.feed(self->buffer_.data(), bytes_transferred);
                      if (ret && self->adaptor_.is_open())
                      {
                          error_while_reading = false;
                      }
                  }

                  if (error_while_reading)
                  {
                      self->cancel_deadline_timer();
                      self->parser_.done();
                      self->adaptor_.shutdown_read();
                      self->adaptor_.close();
                      CROW_LOG_DEBUG << self << " from read(1) with description: \"" << http_errno_description(static_cast<http_errno>(self->parser_.http_errno)) << '\"';
                  }
                  else if (self->close_connection_)
                  {
                      self->cancel_deadline_timer();
                      self->parser_.done();
                      // adaptor will close after write
                  }
                  else if (!self->need_to_call_after_handlers_)
                  {
                      self->start_deadline();
                      self->do_read();
                  }
                  else
                  {
                      // res will be completed later by user
                      self->need_to_start_read_after_complete_ = true;
                  }
              });
        }

        void do_write()
        {
            auto self = this->shared_from_this();
            asio::async_write(
              adaptor_.socket(), buffers_,
              [self](const error_code& ec, std::size_t /*bytes_transferred*/) {
                  self->res.clear();
                  self->res_body_copy_.clear();
                  if (!self->continue_requested)
                  {
                      self->parser_.clear();
                  }
                  else
                  {
                      self->continue_requested = false;
                  }

                  if (!ec)
                  {
                      if (self->close_connection_)
                      {
                          self->adaptor_.shutdown_write();
                          self->adaptor_.close();
                          CROW_LOG_DEBUG << self << " from write(1)";
                      }
                  }
                  else
                  {
                      CROW_LOG_DEBUG << self << " from write(2)";
                  }
              });
        }

        inline void do_write_sync(std::vector<asio::const_buffer>& buffers)
        {
            error_code ec;
            asio::write(adaptor_.socket(), buffers, ec);

            this->res.clear();
            this->res_body_copy_.clear();
            if (this->continue_requested)
            {
                this->continue_requested = false;
            }
            else
            {
                this->parser_.clear();
            }

            if (ec)
            {
                CROW_LOG_ERROR << ec << " - happened while sending buffers";
                CROW_LOG_DEBUG << this << " from write (sync)(2)";
            }
        }

        void cancel_deadline_timer()
        {
            CROW_LOG_DEBUG << this << " timer cancelled: " << &task_timer_ << ' ' << task_id_;
            task_timer_.cancel(task_id_);
        }

        void start_deadline(/*int timeout = 5*/)
        {
            cancel_deadline_timer();

            auto self = this->shared_from_this();
            task_id_ = task_timer_.schedule([self] {
                if (!self->adaptor_.is_open())
                {
                    return;
                }
                self->adaptor_.shutdown_readwrite();
                self->adaptor_.close();
            });
            CROW_LOG_DEBUG << this << " timer added: " << &task_timer_ << ' ' << task_id_;
        }

    private:
        Adaptor adaptor_;
        Handler* handler_;

        std::array<char, 4096> buffer_;

        HTTPParser<Connection> parser_;
        std::unique_ptr<routing_handle_result> routing_handle_result_;
        request& req_;
        response res;

        bool close_connection_ = false;

        const std::string& server_name_;
        std::vector<asio::const_buffer> buffers_;

        std::string content_length_;
        std::string date_str_;
        std::string res_body_copy_;

        detail::task_timer::identifier_type task_id_{};

        bool continue_requested{};
        bool need_to_call_after_handlers_{};
        bool need_to_start_read_after_complete_{};
        bool add_keep_alive_{};

        std::tuple<Middlewares...>* middlewares_;
        detail::context<Middlewares...> ctx_;

        std::function<std::string()>& get_cached_date_str;
        detail::task_timer& task_timer_;

        size_t res_stream_threshold_;

        std::atomic<unsigned int>& queue_length_;
    };

} // namespace crow


#ifdef CROW_USE_BOOST
#include <boost/asio.hpp>
#ifdef CROW_ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif
#else
#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#include <asio.hpp>
#ifdef CROW_ENABLE_SSL
#include <asio/ssl.hpp>
#endif
#endif

#include <atomic>
#include <chrono>
#include <cstdint>
#include <future>
#include <memory>
#include <vector>



namespace crow // NOTE: Already documented in "crow/app.h"
{
#ifdef CROW_USE_BOOST
    namespace asio = boost::asio;
    using error_code = boost::system::error_code;
#else
    using error_code = asio::error_code;
#endif
    using tcp = asio::ip::tcp;
    using stream_protocol = asio::local::stream_protocol;

    template<typename Handler, typename Acceptor = TCPAcceptor, typename Adaptor = SocketAdaptor, typename... Middlewares>
    class Server
    {
    public:
      Server(Handler* handler,
             typename Acceptor::endpoint endpoint, 
             std::string server_name = std::string("Crow/") + VERSION,
             std::tuple<Middlewares...>* middlewares = nullptr,
             unsigned int concurrency = 1,
             uint8_t timeout = 5,
             typename Adaptor::context* adaptor_ctx = nullptr):
          concurrency_(concurrency),
          task_queue_length_pool_(concurrency_ - 1),
          acceptor_(io_context_),
          signals_(io_context_),
          tick_timer_(io_context_),
          handler_(handler),
          timeout_(timeout),
          server_name_(server_name),
          middlewares_(middlewares),
          adaptor_ctx_(adaptor_ctx)
        {
            if (startup_failed_) {
                CROW_LOG_ERROR << "Startup failed; not running server.";
                return;
            }

            error_code ec;

            acceptor_.raw_acceptor().open(endpoint.protocol(), ec);
            if (ec) {
                CROW_LOG_ERROR << "Failed to open acceptor: " << ec.message();
                startup_failed_ = true;
                return;
            }

            acceptor_.raw_acceptor().set_option(Acceptor::reuse_address_option(), ec);
            if (ec) {
                CROW_LOG_ERROR << "Failed to set socket option: " << ec.message();
                startup_failed_ = true;
                return;
            }

            acceptor_.raw_acceptor().bind(endpoint, ec);
            if (ec) {
                CROW_LOG_ERROR << "Failed to bind to " << acceptor_.address()
                            << ":" << acceptor_.port() << " - " << ec.message();
                startup_failed_ = true;
                return;
            }

            acceptor_.raw_acceptor().listen(tcp::acceptor::max_listen_connections, ec);
            if (ec) {
                CROW_LOG_ERROR << "Failed to listen on port: " << ec.message();
                startup_failed_ = true;
                return;
            }


        }

        void set_tick_function(std::chrono::milliseconds d, std::function<void()> f)
        {
            tick_interval_ = d;
            tick_function_ = f;
        }

        void on_tick()
        {
            tick_function_();
            tick_timer_.expires_after(std::chrono::milliseconds(tick_interval_.count()));
            tick_timer_.async_wait([this](const error_code& ec) {
                if (ec)
                    return;
                on_tick();
            });
        }

        void run()
        {

            if (startup_failed_) {
                CROW_LOG_ERROR << "Server startup failed. Aborting run().";
                return;
            }

            uint16_t worker_thread_count = concurrency_ - 1;
            for (int i = 0; i < worker_thread_count; i++)
                io_context_pool_.emplace_back(new asio::io_context());
            get_cached_date_str_pool_.resize(worker_thread_count);
            task_timer_pool_.resize(worker_thread_count);

            std::vector<std::future<void>> v;
            std::atomic<int> init_count(0);
            for (uint16_t i = 0; i < worker_thread_count; i++)
                v.push_back(
                  std::async(
                    std::launch::async, [this, i, &init_count] {
                        // thread local date string get function
                        auto last = std::chrono::steady_clock::now();

                        std::string date_str;
                        auto update_date_str = [&] {
                            auto last_time_t = time(0);
                            tm my_tm;

#if defined(_MSC_VER) || defined(__MINGW32__)
                            gmtime_s(&my_tm, &last_time_t);
#else
                            gmtime_r(&last_time_t, &my_tm);
#endif
                            date_str.resize(100);
                            size_t date_str_sz = strftime(&date_str[0], 99, "%a, %d %b %Y %H:%M:%S GMT", &my_tm);
                            date_str.resize(date_str_sz);
                        };
                        update_date_str();
                        get_cached_date_str_pool_[i] = [&]() -> std::string {
                            if (std::chrono::steady_clock::now() - last >= std::chrono::seconds(1))
                            {
                                last = std::chrono::steady_clock::now();
                                update_date_str();
                            }
                            return date_str;
                        };

                        // initializing task timers
                        detail::task_timer task_timer(*io_context_pool_[i]);
                        task_timer.set_default_timeout(timeout_);
                        task_timer_pool_[i] = &task_timer;
                        task_queue_length_pool_[i] = 0;

                        init_count++;
                        while (1)
                        {
                            try
                            {
                                if (io_context_pool_[i]->run() == 0)
                                {
                                    // when io_service.run returns 0, there are no more works to do.
                                    break;
                                }
                            }
                            catch (std::exception& e)
                            {
                                CROW_LOG_ERROR << "Worker Crash: An uncaught exception occurred: " << e.what();
                            }
                        }
                    }));

            if (tick_function_ && tick_interval_.count() > 0)
            {
                tick_timer_.expires_after(std::chrono::milliseconds(tick_interval_.count()));
                tick_timer_.async_wait(
                  [this](const error_code& ec) {
                      if (ec)
                          return;
                      on_tick();
                  });
            }
            handler_->port(acceptor_.port());
            CROW_LOG_INFO << server_name_ 
                          << " server is running at " << acceptor_.url_display(handler_->ssl_used()) 
                          << " using " << concurrency_ << " threads";
            CROW_LOG_INFO << "Call `app.loglevel(crow::LogLevel::Warning)` to hide Info level logs.";

            signals_.async_wait(
              [&](const error_code& /*error*/, int /*signal_number*/) {
                  stop();
              });

            while (worker_thread_count != init_count)
                std::this_thread::yield();

            do_accept();

            std::thread(
              [this] {
                  notify_start();
                  io_context_.run();
                  CROW_LOG_INFO << "Exiting.";
              })
              .join();
        }

        void stop()
        {
            shutting_down_ = true; // Prevent the acceptor from taking new connections

            // Explicitly close the acceptor
            // else asio will throw an exception (linux only), when trying to start server again:
            // what():  bind: Address already in use
            if (acceptor_.raw_acceptor().is_open())
            {
                CROW_LOG_INFO << "Closing acceptor. " << &acceptor_;
                error_code ec;
                acceptor_.raw_acceptor().close(ec);
                if (ec)
                {
                    CROW_LOG_WARNING << "Failed to close acceptor: " << ec.message();
                }
            }

            for (auto& io_context : io_context_pool_)
            {
                if (io_context != nullptr)
                {
                    CROW_LOG_INFO << "Closing IO service " << &io_context;
                    io_context->stop(); // Close all io_services (and HTTP connections)
                }
            }

            CROW_LOG_INFO << "Closing main IO service (" << &io_context_ << ')';
            io_context_.stop(); // Close main io_service
        }

        
        uint16_t port() const {
            return acceptor_.local_endpoint().port();
        }

        /// Wait until the server has properly started or until timeout
        std::cv_status wait_for_start(std::chrono::steady_clock::time_point wait_until)
        {
            std::unique_lock<std::mutex> lock(start_mutex_);

            std::cv_status status = std::cv_status::no_timeout;
            while (!server_started_ && !startup_failed_ && status == std::cv_status::no_timeout)
                status = cv_started_.wait_until(lock, wait_until);
            return status;
        }


        void signal_clear()
        {
            signals_.clear();
        }

        void signal_add(int signal_number)
        {
            signals_.add(signal_number);
        }

    private:
        size_t pick_io_context_idx()
        {
            size_t min_queue_idx = 0;

            // TODO improve load balancing
            // size_t is used here to avoid the security issue https://codeql.github.com/codeql-query-help/cpp/cpp-comparison-with-wider-type/
            // even though the max value of this can be only uint16_t as concurrency is uint16_t.
            for (size_t i = 1; i < task_queue_length_pool_.size() && task_queue_length_pool_[min_queue_idx] > 0; i++)
            // No need to check other io_services if the current one has no tasks
            {
                if (task_queue_length_pool_[i] < task_queue_length_pool_[min_queue_idx])
                    min_queue_idx = i;
            }
            return min_queue_idx;
        }

        void do_accept()
        {
            if (!shutting_down_)
            {
                size_t context_idx = pick_io_context_idx();
                asio::io_context& ic = *io_context_pool_[context_idx];
                auto p = std::make_shared<Connection<Adaptor, Handler, Middlewares...>>(
                    ic, handler_, server_name_, middlewares_,
                    get_cached_date_str_pool_[context_idx], *task_timer_pool_[context_idx], adaptor_ctx_, task_queue_length_pool_[context_idx]);
                    
                CROW_LOG_DEBUG << &ic << " {" << context_idx << "} queue length: " << task_queue_length_pool_[context_idx];

                acceptor_.raw_acceptor().async_accept(
                  p->socket(),
                  [this, p, &ic, context_idx](error_code ec) {
                      if (!ec)
                      {
                          asio::post(ic,
                            [p] {
                                p->start();
                            });
                      }
                      do_accept();
                  });
            }
        }

        /// Notify anything using `wait_for_start()` to proceed
        void notify_start()
        {
            std::unique_lock<std::mutex> lock(start_mutex_);
            server_started_ = true;
            cv_started_.notify_all();
        }

    private:
        unsigned int concurrency_{2};
        std::vector<std::atomic<unsigned int>> task_queue_length_pool_;
        std::vector<std::unique_ptr<asio::io_context>> io_context_pool_;
        asio::io_context io_context_;
        std::vector<detail::task_timer*> task_timer_pool_;
        std::vector<std::function<std::string()>> get_cached_date_str_pool_;
        Acceptor acceptor_;
        bool shutting_down_ = false;
        bool server_started_{false};
        bool startup_failed_ = false;
        std::condition_variable cv_started_;
        std::mutex start_mutex_;
        asio::signal_set signals_;

        asio::basic_waitable_timer<std::chrono::high_resolution_clock> tick_timer_;

        Handler* handler_;
        std::uint8_t timeout_;
        std::string server_name_;
        bool use_unix_;

        std::chrono::milliseconds tick_interval_;
        std::function<void()> tick_function_;

        std::tuple<Middlewares...>* middlewares_;

        typename Adaptor::context* adaptor_ctx_;
    };
} // namespace crow

/**
 * \file crow/app.h
 * \brief This file includes the definition of the crow::Crow class,
 * the crow::App and crow::SimpleApp aliases, and some macros.
 *
 * In this file are defined:
 * - crow::Crow
 * - crow::App
 * - crow::SimpleApp
 * - \ref CROW_ROUTE
 * - \ref CROW_BP_ROUTE
 * - \ref CROW_WEBSOCKET_ROUTE
 * - \ref CROW_MIDDLEWARES
 * - \ref CROW_CATCHALL_ROUTE
 * - \ref CROW_BP_CATCHALL_ROUTE
 */


#include <chrono>
#include <string>
#include <functional>
#include <memory>
#include <future>
#include <cstdint>
#include <type_traits>
#include <thread>
#include <condition_variable>

#ifdef CROW_ENABLE_COMPRESSION
#endif // #ifdef CROW_ENABLE_COMPRESSION


#ifdef CROW_MSVC_WORKAROUND

#define CROW_ROUTE(app, url) app.route_dynamic(url) // See the documentation in the comment below.
#define CROW_BP_ROUTE(blueprint, url) blueprint.new_rule_dynamic(url) // See the documentation in the comment below.

#else // #ifdef CROW_MSVC_WORKAROUND

/**
 * \def CROW_ROUTE(app, url)
 * \brief Creates a route for app using a rule.
 *
 * It use crow::Crow::route_dynamic or crow::Crow::route to define
 * a rule for your application. It's usage is like this:
 *
 * ```cpp
 * auto app = crow::SimpleApp(); // or crow::App()
 * CROW_ROUTE(app, "/")
 * ([](){
 *     return "<h1>Hello, world!</h1>";
 * });
 * ```
 *
 * This is the recommended way to define routes in a crow application.
 * \see [Page of guide "Routes"](https://crowcpp.org/master/guides/routes/).
 */
#define CROW_ROUTE(app, url) app.template route<crow::black_magic::get_parameter_tag(url)>(url)

/**
 * \def CROW_BP_ROUTE(blueprint, url)
 * \brief Creates a route for a blueprint using a rule.
 *
 * It may use crow::Blueprint::new_rule_dynamic or
 * crow::Blueprint::new_rule_tagged to define a new rule for
 * an given blueprint. It's usage is similar
 * to CROW_ROUTE macro:
 *
 * ```cpp
 * crow::Blueprint my_bp();
 * CROW_BP_ROUTE(my_bp, "/")
 * ([](){
 *     return "<h1>Hello, world!</h1>";
 * });
 * ```
 *
 * This is the recommended way to define routes in a crow blueprint
 * because of its compile-time capabilities.
 *
 * \see [Page of the guide "Blueprints"](https://crowcpp.org/master/guides/blueprints/).
 */
#define CROW_BP_ROUTE(blueprint, url) blueprint.new_rule_tagged<crow::black_magic::get_parameter_tag(url)>(url)

/**
 * \def CROW_WEBSOCKET_ROUTE(app, url)
 * \brief Defines WebSocket route for app.
 *
 * It binds a WebSocket route to app. Easy solution to implement
 * WebSockets in your app. The usage syntax of this macro is
 * like this:
 *
 * ```cpp
 * auto app = crow::SimpleApp(); // or crow::App()
 * CROW_WEBSOCKET_ROUTE(app, "/ws")
 *     .onopen([&](crow::websocket::connection& conn){
 *                do_something();
 *            })
 *     .onclose([&](crow::websocket::connection& conn, const std::string& reason, uint16_t){
 *                 do_something();
 *             })
 *     .onmessage([&](crow::websocket::connection&, const std::string& data, bool is_binary){
 *                   if (is_binary)
 *                       do_something(data);
 *                   else
 *                       do_something_else(data);
 *               });
 * ```
 *
 * \see [Page of the guide "WebSockets"](https://crowcpp.org/master/guides/websockets/).
 */
#define CROW_WEBSOCKET_ROUTE(app, url) app.route<crow::black_magic::get_parameter_tag(url)>(url).websocket<std::remove_reference<decltype(app)>::type>(&app)

/**
 * \def CROW_MIDDLEWARES(app, ...)
 * \brief Enable a Middleware for an specific route in app
 * or blueprint.
 *
 * It defines the usage of a Middleware in one route. And it
 * can be used in both crow::SimpleApp (and crow::App) instances and
 * crow::Blueprint. Its usage syntax is like this:
 *
 * ```cpp
 * auto app = crow::SimpleApp(); // or crow::App()
 * CROW_ROUTE(app, "/with_middleware")
 * .CROW_MIDDLEWARES(app, LocalMiddleware) // Can be used more than one
 * ([]() {                                 // middleware.
 *     return "Hello world!";
 * });
 * ```
 *
 * \see [Page of the guide "Middlewares"](https://crowcpp.org/master/guides/middleware/).
 */
#define CROW_MIDDLEWARES(app, ...) template middlewares<typename std::remove_reference<decltype(app)>::type, __VA_ARGS__>()

#endif // #ifdef CROW_MSVC_WORKAROUND

/**
 * \def CROW_CATCHALL_ROUTE(app)
 * \brief Defines a custom catchall route for app using a
 * custom rule.
 *
 * It defines a handler when the client make a request for an
 * undefined route. Instead of just reply with a `404` status
 * code (default behavior), you can define a custom handler
 * using this macro.
 *
 * \see [Page of the guide "Routes" (Catchall routes)](https://crowcpp.org/master/guides/routes/#catchall-routes).
 */
#define CROW_CATCHALL_ROUTE(app) app.catchall_route()

/**
 * \def CROW_BP_CATCHALL_ROUTE(blueprint)
 * \brief Defines a custom catchall route for blueprint
 * using a custom rule.
 *
 * It defines a handler when the client make a request for an
 * undefined route in the blueprint.
 *
 * \see [Page of the guide "Blueprint" (Define a custom Catchall route)](https://crowcpp.org/master/guides/blueprints/#define-a-custom-catchall-route).
 */
#define CROW_BP_CATCHALL_ROUTE(blueprint) blueprint.catchall_rule()


/**
 * \namespace crow
 * \brief The main namespace of the library. In this namespace
 * is defined the most important classes and functions of the
 * library.
 *
 * Within this namespace, the Crow class, Router class, Connection
 * class, and other are defined.
 */
namespace crow
{
#ifdef CROW_ENABLE_SSL
    using ssl_context_t = asio::ssl::context;
#endif
    /**
     * \class Crow
     * \brief The main server application class.
     *
     * Use crow::SimpleApp or crow::App<Middleware1, Middleware2, etc...> instead of
     * directly instantiate this class.
     */
    template<typename... Middlewares>
    class Crow
    {
    public:
        /// \brief This is the crow application
        using self_t = Crow;

        /// \brief The HTTP server
        using server_t = Server<Crow, TCPAcceptor, SocketAdaptor, Middlewares...>;
        /// \brief An HTTP server that runs on unix domain socket
        using unix_server_t = Server<Crow, UnixSocketAcceptor, UnixSocketAdaptor, Middlewares...>;
#ifdef CROW_ENABLE_SSL
        /// \brief An HTTP server that runs on SSL with an SSLAdaptor
        using ssl_server_t = Server<Crow, TCPAcceptor, SSLAdaptor, Middlewares...>;
#endif
        Crow()
        {}

        /// \brief Construct Crow with a subset of middleware
        template<typename... Ts>
        Crow(Ts&&... ts):
          middlewares_(make_middleware_tuple(std::forward<Ts>(ts)...))
        {}

        /// \brief Process an Upgrade request
        ///
        /// Currently used to upgrade an HTTP connection to a WebSocket connection
        template<typename Adaptor>
        void handle_upgrade(const request& req, response& res, Adaptor&& adaptor)
        {
            router_.handle_upgrade(req, res, adaptor);
        }

        /// \brief Process only the method and URL of a request and provide a route (or an error response)
        std::unique_ptr<routing_handle_result> handle_initial(request& req, response& res)
        {
            return router_.handle_initial(req, res);
        }

        /// \brief Process the fully parsed request and generate a response for it
        void handle(request& req, response& res, std::unique_ptr<routing_handle_result>& found)
        {
            router_.handle<self_t>(req, res, *found);
        }

        /// \brief Process a fully parsed request from start to finish (primarily used for debugging)
        void handle_full(request& req, response& res)
        {
            auto found = handle_initial(req, res);
            if (found->rule_index)
                handle(req, res, found);
        }

        /// \brief Create a dynamic route using a rule (**Use CROW_ROUTE instead**)
        DynamicRule& route_dynamic(const std::string& rule)
        {
            return router_.new_rule_dynamic(rule);
        }

        /// \brief Create a route using a rule (**Use CROW_ROUTE instead**)
        template<uint64_t Tag>
        auto route(const std::string& rule)
          -> typename std::invoke_result<decltype(&Router::new_rule_tagged<Tag>), Router, const std::string&>::type
        {
            return router_.new_rule_tagged<Tag>(rule);
        }

        /// \brief Create a route for any requests without a proper route (**Use CROW_CATCHALL_ROUTE instead**)
        CatchallRule& catchall_route()
        {
            return router_.catchall_rule();
        }

        /// \brief Set the default max payload size for websockets
        self_t& websocket_max_payload(uint64_t max_payload)
        {
            max_payload_ = max_payload;
            return *this;
        }

        /// \brief Get the default max payload size for websockets
        uint64_t websocket_max_payload()
        {
            return max_payload_;
        }

        self_t& signal_clear()
        {
            signals_.clear();
            return *this;
        }

        self_t& signal_add(int signal_number)
        {
            signals_.push_back(signal_number);
            return *this;
        }

        std::vector<int> signals()
        {
            return signals_;
        }

        /// \brief Set the port that Crow will handle requests on
        self_t& port(std::uint16_t port)
        {
            port_ = port;
            return *this;
        }

        /// \brief Get the port that Crow will handle requests on
        std::uint16_t port() const
        {
            if (!server_started_)
            {
                return port_;
            }
#ifdef CROW_ENABLE_SSL
            if (ssl_used_)
            {
                return ssl_server_->port();
            }
            else
#endif
            {
                return server_->port();
            }
        }

        /// \brief Set the connection timeout in seconds (default is 5)
        self_t& timeout(std::uint8_t timeout)
        {
            timeout_ = timeout;
            return *this;
        }

        /// \brief Set the server name included in the 'Server' HTTP response header. If set to an empty string, the header will be omitted by default.
        self_t& server_name(std::string server_name)
        {
            server_name_ = server_name;
            return *this;
        }

        /// \brief The IP address that Crow will handle requests on (default is 0.0.0.0)
        self_t& bindaddr(std::string bindaddr)
        {
            bindaddr_ = bindaddr;
            return *this;
        }

        /// \brief Get the address that Crow will handle requests on
        std::string bindaddr()
        {
            return bindaddr_;
        }

        /// \brief Disable tcp/ip and use unix domain socket instead
        self_t& local_socket_path(std::string path)
        {
            bindaddr_ = path;
            use_unix_ = true;
            return *this;
        }

        /// \brief Get the unix domain socket path
        std::string local_socket_path()
        {
            return bindaddr_;
        }

        /// \brief Run the server on multiple threads using all available threads
        self_t& multithreaded()
        {
            return concurrency(std::thread::hardware_concurrency());
        }

        /// \brief Run the server on multiple threads using a specific number
        self_t& concurrency(unsigned int concurrency)
        {
            if (concurrency < 2) // Crow can have a minimum of 2 threads running
                concurrency = 2;
            concurrency_ = concurrency;
            return *this;
        }

        /// \brief Get the number of threads that server is using
        std::uint16_t concurrency() const
        {
            return concurrency_;
        }

        /// \brief Set the server's log level
        ///
        /// Possible values are:
        /// - crow::LogLevel::Debug       (0)
        /// - crow::LogLevel::Info        (1)
        /// - crow::LogLevel::Warning     (2)
        /// - crow::LogLevel::Error       (3)
        /// - crow::LogLevel::Critical    (4)
        self_t& loglevel(LogLevel level)
        {
            crow::logger::setLogLevel(level);
            return *this;
        }

        /// \brief Set the response body size (in bytes) beyond which Crow automatically streams responses (Default is 1MiB)
        ///
        /// Any streamed response is unaffected by Crow's timer, and therefore won't timeout before a response is fully sent.
        self_t& stream_threshold(size_t threshold)
        {
            res_stream_threshold_ = threshold;
            return *this;
        }

        /// \brief Get the response body size (in bytes) beyond which Crow automatically streams responses
        size_t& stream_threshold()
        {
            return res_stream_threshold_;
        }


        self_t& register_blueprint(Blueprint& blueprint)
        {
            router_.register_blueprint(blueprint);
            return *this;
        }

        /// \brief Set the function to call to handle uncaught exceptions generated in routes (Default generates error 500).
        ///
        /// The function must have the following signature: void(crow::response&).
        /// It must set the response passed in argument to the function, which will be sent back to the client.
        /// See Router::default_exception_handler() for the default implementation.
        template<typename Func>
        self_t& exception_handler(Func&& f)
        {
            router_.exception_handler() = std::forward<Func>(f);
            return *this;
        }

        std::function<void(crow::response&)>& exception_handler()
        {
            return router_.exception_handler();
        }

        /// \brief Set a custom duration and function to run on every tick
        template<typename Duration, typename Func>
        self_t& tick(Duration d, Func f)
        {
            tick_interval_ = std::chrono::duration_cast<std::chrono::milliseconds>(d);
            tick_function_ = f;
            return *this;
        }

#ifdef CROW_ENABLE_COMPRESSION

        self_t& use_compression(compression::algorithm algorithm)
        {
            comp_algorithm_ = algorithm;
            compression_used_ = true;
            return *this;
        }

        compression::algorithm compression_algorithm()
        {
            return comp_algorithm_;
        }

        bool compression_used() const
        {
            return compression_used_;
        }
#endif

        /// \brief Apply blueprints
        void add_blueprint()
        {
#if defined(__APPLE__) || defined(__MACH__)
            if (router_.blueprints().empty()) return;
#endif

            for (Blueprint* bp : router_.blueprints())
            {
                if (bp->static_dir().empty()) {
                    CROW_LOG_ERROR << "Blueprint " << bp->prefix() << " and its sub-blueprints ignored due to empty static directory.";
                    continue;
                }
                auto static_dir_ = crow::utility::normalize_path(bp->static_dir());

                bp->new_rule_tagged<crow::black_magic::get_parameter_tag(CROW_STATIC_ENDPOINT)>(CROW_STATIC_ENDPOINT)([static_dir_](crow::response& res, std::string file_path_partial) {
                    utility::sanitize_filename(file_path_partial);
                    res.set_static_file_info_unsafe(static_dir_ + file_path_partial);
                    res.end();
                });
            }

            router_.validate_bp();
        }

        /// \brief Go through the rules, upgrade them if possible, and add them to the list of rules
        void add_static_dir()
        {
            if (are_static_routes_added()) return;
            auto static_dir_ = crow::utility::normalize_path(CROW_STATIC_DIRECTORY);

            route<crow::black_magic::get_parameter_tag(CROW_STATIC_ENDPOINT)>(CROW_STATIC_ENDPOINT)([static_dir_](crow::response& res, std::string file_path_partial) {
                utility::sanitize_filename(file_path_partial);
                res.set_static_file_info_unsafe(static_dir_ + file_path_partial);
                res.end();
            });
            set_static_routes_added();
        }

        /// \brief A wrapper for `validate()` in the router
        void validate()
        {
            router_.validate();
        }

        /// \brief Run the server
        void run()
        {
#ifndef CROW_DISABLE_STATIC_DIR
            add_blueprint();
            add_static_dir();
#endif
            validate();

#ifdef CROW_ENABLE_SSL
            if (ssl_used_)
            {

                error_code ec;
                asio::ip::address addr = asio::ip::make_address(bindaddr_,ec);
                if (ec){
                    CROW_LOG_ERROR << ec.message() << " - Can not create valid ip address from string: \"" << bindaddr_ << "\"";
                    return;
                }
                tcp::endpoint endpoint(addr, port_);
                router_.using_ssl = true;
                ssl_server_ = std::move(std::unique_ptr<ssl_server_t>(new ssl_server_t(this, endpoint, server_name_, &middlewares_, concurrency_, timeout_, &ssl_context_)));
                ssl_server_->set_tick_function(tick_interval_, tick_function_);
                ssl_server_->signal_clear();
                for (auto snum : signals_)
                {
                    ssl_server_->signal_add(snum);
                }
                notify_server_start();
                ssl_server_->run();
            }
            else
#endif
            {
                if (use_unix_)
                {
                    UnixSocketAcceptor::endpoint endpoint(bindaddr_);
                    unix_server_ = std::move(std::unique_ptr<unix_server_t>(new unix_server_t(this, endpoint, server_name_, &middlewares_, concurrency_, timeout_, nullptr)));
                    unix_server_->set_tick_function(tick_interval_, tick_function_);
                    for (auto snum : signals_)
                    {
                        unix_server_->signal_add(snum);
                    }
                    notify_server_start();
                    unix_server_->run();
                }
                else
                {
                    error_code ec;
                    asio::ip::address addr = asio::ip::make_address(bindaddr_,ec);
                    if (ec){
                        CROW_LOG_ERROR << ec.message() << " - Can not create valid ip address from string: \"" << bindaddr_ << "\"";
                        return;
                    }
                    TCPAcceptor::endpoint endpoint(addr, port_);
                    server_ = std::move(std::unique_ptr<server_t>(new server_t(this, endpoint, server_name_, &middlewares_, concurrency_, timeout_, nullptr)));
                    server_->set_tick_function(tick_interval_, tick_function_);
                    for (auto snum : signals_)
                    {
                        server_->signal_add(snum);
                    }
                    notify_server_start();
                    server_->run();
                }
            }
        }

        /// \brief Non-blocking version of \ref run()
        ///
        /// The output from this method needs to be saved into a variable!
        /// Otherwise the call will be made on the same thread.
        std::future<void> run_async()
        {
            return std::async(std::launch::async, [&] {
                this->run();
            });
        }

        /// \brief Stop the server
        void stop()
        {
#ifdef CROW_ENABLE_SSL
            if (ssl_used_)
            {
                if (ssl_server_) { ssl_server_->stop(); }
            }
            else
#endif
            {
                close_websockets();
                if (server_) { server_->stop(); }
                if (unix_server_) { unix_server_->stop(); }
            }
        }

        void close_websockets()
        {
            for (auto websocket : websockets_)
            {
                CROW_LOG_INFO << "Quitting Websocket: " << websocket;
                websocket->close("Websocket Closed");
            }
        }


        void add_websocket(crow::websocket::connection* conn)
        {
            websockets_.push_back(conn);
        }

        void remove_websocket(crow::websocket::connection* conn)
        {
            websockets_.erase(std::remove(websockets_.begin(), websockets_.end(), conn), websockets_.end());
        }

        /// \brief Print the routing paths defined for each HTTP method
        void debug_print()
        {
            CROW_LOG_DEBUG << "Routing:";
            router_.debug_print();
        }


#ifdef CROW_ENABLE_SSL

        /// \brief Use certificate and key files for SSL
        self_t& ssl_file(const std::string& crt_filename, const std::string& key_filename)
        {
            ssl_used_ = true;
            ssl_context_.set_verify_mode(asio::ssl::verify_peer);
            ssl_context_.set_verify_mode(asio::ssl::verify_client_once);
            ssl_context_.use_certificate_file(crt_filename, ssl_context_t::pem);
            ssl_context_.use_private_key_file(key_filename, ssl_context_t::pem);
            ssl_context_.set_options(
              asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3);
            return *this;
        }

        /// \brief Use `.pem` file for SSL
        self_t& ssl_file(const std::string& pem_filename)
        {
            ssl_used_ = true;
            ssl_context_.set_verify_mode(asio::ssl::verify_peer);
            ssl_context_.set_verify_mode(asio::ssl::verify_client_once);
            ssl_context_.load_verify_file(pem_filename);
            ssl_context_.set_options(
              asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3);
            return *this;
        }

        /// \brief Use certificate chain and key files for SSL
        self_t& ssl_chainfile(const std::string& crt_filename, const std::string& key_filename)
        {
            ssl_used_ = true;
            ssl_context_.set_verify_mode(asio::ssl::verify_peer);
            ssl_context_.set_verify_mode(asio::ssl::verify_client_once);
            ssl_context_.use_certificate_chain_file(crt_filename);
            ssl_context_.use_private_key_file(key_filename, ssl_context_t::pem);
            ssl_context_.set_options(
              asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3);
            return *this;
        }

        self_t& ssl(asio::ssl::context&& ctx)
        {
            ssl_used_ = true;
            ssl_context_ = std::move(ctx);
            return *this;
        }

        bool ssl_used() const
        {
            return ssl_used_;
        }
#else

        template<typename T, typename... Remain>
        self_t& ssl_file(T&&, Remain&&...)
        {
            // We can't call .ssl() member function unless CROW_ENABLE_SSL is defined.
            static_assert(
              // make static_assert dependent to T; always false
              std::is_base_of<T, void>::value,
              "Define CROW_ENABLE_SSL to enable ssl support.");
            return *this;
        }

        template<typename T, typename... Remain>
        self_t& ssl_chainfile(T&&, Remain&&...)
        {
            // We can't call .ssl() member function unless CROW_ENABLE_SSL is defined.
            static_assert(
              // make static_assert dependent to T; always false
              std::is_base_of<T, void>::value,
              "Define CROW_ENABLE_SSL to enable ssl support.");
            return *this;
        }

        template<typename T>
        self_t& ssl(T&&)
        {
            // We can't call .ssl() member function unless CROW_ENABLE_SSL is defined.
            static_assert(
              // make static_assert dependent to T; always false
              std::is_base_of<T, void>::value,
              "Define CROW_ENABLE_SSL to enable ssl support.");
            return *this;
        }

        bool ssl_used() const
        {
            return false;
        }
#endif

        // middleware
        using context_t = detail::context<Middlewares...>;
        using mw_container_t = std::tuple<Middlewares...>;
        template<typename T>
        typename T::context& get_context(const request& req)
        {
            static_assert(black_magic::contains<T, Middlewares...>::value, "App doesn't have the specified middleware type.");
            auto& ctx = *reinterpret_cast<context_t*>(req.middleware_context);
            return ctx.template get<T>();
        }

        template<typename T>
        T& get_middleware()
        {
            return utility::get_element_by_type<T, Middlewares...>(middlewares_);
        }

        /// \brief Wait until the server has properly started
        std::cv_status wait_for_server_start(std::chrono::milliseconds wait_timeout = std::chrono::milliseconds(3000))
        {
            std::cv_status status = std::cv_status::no_timeout;
            auto wait_until = std::chrono::steady_clock::now() + wait_timeout;
            {
                std::unique_lock<std::mutex> lock(start_mutex_);
                while (!server_started_ && (status == std::cv_status::no_timeout))
                {
                    status = cv_started_.wait_until(lock, wait_until);
                }
            }
            if (status == std::cv_status::no_timeout)
            {
                if (server_) {
                    status = server_->wait_for_start(wait_until);
                } else if (unix_server_) {
                    status = unix_server_->wait_for_start(wait_until);
                }
#ifdef CROW_ENABLE_SSL
                else if (ssl_server_)
                {
                    status = ssl_server_->wait_for_start(wait_until);
                }
#endif
            }
            return status;
        }

    private:
        template<typename... Ts>
        std::tuple<Middlewares...> make_middleware_tuple(Ts&&... ts)
        {
            auto fwd = std::forward_as_tuple((ts)...);
            return std::make_tuple(
              std::forward<Middlewares>(
                black_magic::tuple_extract<Middlewares, decltype(fwd)>(fwd))...);
        }

        /// \brief Notify anything using \ref wait_for_server_start() to proceed
        void notify_server_start()
        {
            std::unique_lock<std::mutex> lock(start_mutex_);
            server_started_ = true;
            cv_started_.notify_all();
        }

        void set_static_routes_added() {
            static_routes_added_ = true;
        }

        bool are_static_routes_added() {
            return static_routes_added_;
        }

    private:
        std::uint8_t timeout_{5};
        uint16_t port_ = 80;
        unsigned int concurrency_ = 2;
        uint64_t max_payload_{UINT64_MAX};
        std::string server_name_ = std::string("Crow/") + VERSION;
        std::string bindaddr_ = "0.0.0.0";
        bool use_unix_ = false;
        size_t res_stream_threshold_ = 1048576;
        Router router_;
        bool static_routes_added_{false};

#ifdef CROW_ENABLE_COMPRESSION
        compression::algorithm comp_algorithm_;
        bool compression_used_{false};
#endif

        std::chrono::milliseconds tick_interval_;
        std::function<void()> tick_function_;

        std::tuple<Middlewares...> middlewares_;

#ifdef CROW_ENABLE_SSL
        std::unique_ptr<ssl_server_t> ssl_server_;
        bool ssl_used_{false};
        ssl_context_t ssl_context_{asio::ssl::context::sslv23};
#endif

        std::unique_ptr<server_t> server_;
        std::unique_ptr<unix_server_t> unix_server_;

        std::vector<int> signals_{SIGINT, SIGTERM};

        bool server_started_{false};
        std::condition_variable cv_started_;
        std::mutex start_mutex_;
        std::vector<crow::websocket::connection*> websockets_;
    };

    /// \brief Alias of Crow<Middlewares...>. Useful if you want
    /// a instance of an Crow application that require Middlewares
    template<typename... Middlewares>
    using App = Crow<Middlewares...>;

    /// \brief Alias of Crow<>. Useful if you want a instance of
    /// an Crow application that doesn't require of Middlewares
    using SimpleApp = Crow<>;
} // namespace crow

