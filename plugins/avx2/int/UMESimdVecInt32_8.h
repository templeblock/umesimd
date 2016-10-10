// The MIT License (MIT)
//
// Copyright (c) 2015 CERN
//
// Author: Przemyslaw Karpinski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
//  This piece of code was developed as part of ICE-DIP project at CERN.
//  "ICE-DIP is a European Industrial Doctorate project funded by the European Community's 
//  7th Framework programme Marie Curie Actions under grant PITN-GA-2012-316596".
//

#ifndef UME_SIMD_VEC_INT32_8_H_
#define UME_SIMD_VEC_INT32_8_H_

#include <type_traits>
#include <immintrin.h>

#include "../../../UMESimdInterface.h"

namespace UME {
namespace SIMD {

    template<>
    class SIMDVec_i<int32_t, 8> :
        public SIMDVecSignedInterface<
            SIMDVec_i<int32_t, 8>,
            SIMDVec_u<uint32_t, 8>,
            int32_t,
            8,
            uint32_t,
            SIMDVecMask<8>,
            SIMDSwizzle<8 >> ,
        public SIMDVecPackableInterface<
            SIMDVec_i<int32_t, 8>,
            SIMDVec_i<int32_t, 4 >>
    {
    public:
        friend class SIMDVec_u<uint32_t, 8>;
        friend class SIMDVec_f<float, 8>;
        friend class SIMDVec_f<double, 8>;

        friend class SIMDVec_i<int32_t, 16>;
    private:
        __m256i mVec;

        inline explicit SIMDVec_i(__m256i & x) { mVec = x; }
        inline explicit SIMDVec_i(const __m256i & x) { mVec = x; }
    public:

        constexpr static uint32_t length() { return 8; }
        constexpr static uint32_t alignment() { return 32; }

        // ZERO-CONSTR
        inline SIMDVec_i() {};

        // SET-CONSTR
        inline SIMDVec_i(int32_t i) {
            mVec = _mm256_set1_epi32(i);
        }
        // This constructor is used to force types other than SCALAR_TYPES
        // to be promoted to SCALAR_TYPE instead of SCALAR_TYPE*. This prevents
        // ambiguity between SET-CONSTR and LOAD-CONSTR.
        template<typename T>
        inline SIMDVec_i(
            T i, 
            typename std::enable_if< std::is_same<T, int>::value && 
                                    !std::is_same<T, int32_t>::value,
                                    void*>::type = nullptr)
        : SIMDVec_i(static_cast<int32_t>(i)) {}
        // LOAD-CONSTR
        inline explicit SIMDVec_i(int32_t const *p) { this->load(p); };
        // FULL-CONSTR
        inline SIMDVec_i(int32_t i0, int32_t i1, int32_t i2, int32_t i3,
                         int32_t i4, int32_t i5, int32_t i6, int32_t i7)
        {
            mVec = _mm256_setr_epi32(i0, i1, i2, i3, i4, i5, i6, i7);
        }
        // EXTRACT
        inline int32_t extract(uint32_t index) const {
            //return _mm256_extract_epi32(mVec, index); // TODO: this can be implemented in ICC
            alignas(32) int32_t raw[8];
            _mm256_store_si256((__m256i *)raw, mVec);
            return raw[index];
        }
        inline int32_t operator[] (uint32_t index) const {
            return extract(index);
        }

        // INSERT
        inline SIMDVec_i & insert(uint32_t index, int32_t value) {
            //UME_PERFORMANCE_UNOPTIMAL_WARNING()
            alignas(32) int32_t raw[8];
            _mm256_store_si256((__m256i *)raw, mVec);
            raw[index] = value;
            mVec = _mm256_load_si256((__m256i *)raw);
            return *this;
        }
        inline IntermediateIndex<SIMDVec_i, int32_t> operator[] (uint32_t index) {
            return IntermediateIndex<SIMDVec_i, int32_t>(index, static_cast<SIMDVec_i &>(*this));
        }

        // Override Mask Access operators
#if defined(USE_PARENTHESES_IN_MASK_ASSIGNMENT)
        inline IntermediateMask<SIMDVec_i, int32_t, SIMDVecMask<8>> operator() (SIMDVecMask<8> const & mask) {
            return IntermediateMask<SIMDVec_i, int32_t, SIMDVecMask<8>>(mask, static_cast<SIMDVec_i &>(*this));
        }
#else
        inline IntermediateMask<SIMDVec_i, int32_t, SIMDVecMask<8>> operator[] (SIMDVecMask<8> const & mask) {
            return IntermediateMask<SIMDVec_i, int32_t, SIMDVecMask<8>>(mask, static_cast<SIMDVec_i &>(*this));
        }
#endif

        // ASSIGNV
        inline SIMDVec_i & assign(SIMDVec_i const & b) {
            mVec = b.mVec;
            return *this;
        }
        inline SIMDVec_i & operator= (SIMDVec_i const & b) {
            return assign(b);
        }
        // MASSIGNV
        inline SIMDVec_i & assign(SIMDVecMask<8> const & mask, SIMDVec_i const & b) {
            mVec = _mm256_blendv_epi8(mVec, b.mVec, mask.mMask);
            return *this;
        }
        // ASSIGNS
        inline SIMDVec_i & assign(int32_t b) {
            mVec = _mm256_set1_epi32(b);
            return *this;
        }
        inline SIMDVec_i & operator= (int32_t b) {
            return assign(b);
        }
        // MASSIGNS
        inline SIMDVec_i & assign(SIMDVecMask<8> const & mask, int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }

        // PREFETCH0
        // PREFETCH1
        // PREFETCH2

        // LOAD
        inline SIMDVec_i & load(int32_t const * p) {
            mVec = _mm256_loadu_si256((__m256i*)p);
            return *this;
        }
        // MLOAD
        inline SIMDVec_i & load(SIMDVecMask<8> const & mask, int32_t const * p) {
            __m256i t0 = _mm256_loadu_si256((__m256i*)p);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // LOADA
        inline SIMDVec_i & loada(int32_t const * p) {
            mVec = _mm256_load_si256((__m256i *)p);
            return *this;
        }
        // MLOADA
        inline SIMDVec_i & loada(SIMDVecMask<8> const & mask, int32_t const * p) {
            __m256i t0 = _mm256_load_si256((__m256i*)p);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // SLOAD
        inline SIMDVec_i & sload(int32_t const * p) {
            mVec = _mm256_stream_load_si256((__m256i*)p);
            return *this;
        }
        // MSLOAD
        inline SIMDVec_i & sload(SIMDVecMask<8> const & mask, int32_t const * p) {
            __m256i t0 = _mm256_stream_load_si256((__m256i*)p);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // STORE
        inline int32_t * store(int32_t * p) const {
            _mm256_storeu_si256((__m256i*)p, mVec);
            return p;
        }
        // MSTORE
        inline int32_t * store(SIMDVecMask<8> const & mask, int32_t * p) const {
            __m256i t0 = _mm256_loadu_si256((__m256i*)p);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            _mm256_storeu_si256((__m256i*)p, t1);
            return p;
        }
        // STOREA
        inline int32_t * storea(int32_t * p) const {
            _mm256_store_si256((__m256i*)p, mVec);
            return p;
        }
        // MSTOREA
        inline int32_t * storea(SIMDVecMask<8> const & mask, int32_t * p) const {
            _mm256_maskstore_epi32((int*) p, mask.mMask, mVec);
            return p;
        }
        // SSTORE
        inline int32_t* sstore(int32_t* p) const {
            _mm256_stream_si256((__m256i*)p, mVec);
            return p;
        }
        // MSSTORE
        inline int32_t* sstore(SIMDVecMask<8> const & mask, int32_t* p) const {
            __m256i t0 = _mm256_stream_load_si256((__m256i*)p);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            _mm256_stream_si256((__m256i*)p, t1);
            return p;
        }
        // BLENDV
        inline SIMDVec_i blend(SIMDVecMask<8> const & mask, SIMDVec_i const & b) const {
            __m256i t0 = _mm256_blendv_epi8(mVec, b.mVec, mask.mMask);
            return SIMDVec_i(t0);
        }
        // BLENDS
        inline SIMDVec_i blend(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // SWIZZLE
        // SWIZZLEA

        // ADDV
        inline SIMDVec_i add(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_add_epi32(mVec, b.mVec);
            return SIMDVec_i(t0);
        }
        inline SIMDVec_i operator+ (SIMDVec_i const & b) const {
            return add(b);
        }
        // MADDV
        inline SIMDVec_i add(SIMDVecMask<8> const & mask, SIMDVec_i const & b) const {
            __m256i t0 = _mm256_add_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // ADDS
        inline SIMDVec_i add(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_add_epi32(mVec, t0);
            return SIMDVec_i(t1);
        }
        inline SIMDVec_i operator+ (int32_t b) const {
            return add(b);
        }
        // MADDS
        inline SIMDVec_i add(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_add_epi32(mVec, t0);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // ADDVA
        inline SIMDVec_i & adda(SIMDVec_i const & b) {
            mVec = _mm256_add_epi32(mVec, b.mVec);
            return *this;
        }
        inline SIMDVec_i & operator+= (SIMDVec_i const & b) {
            return adda(b);
        }
        // MADDVA
        inline SIMDVec_i & adda(SIMDVecMask<8> const & mask, SIMDVec_i const & b) {
            __m128i a_low = _mm256_extractf128_si256(mVec, 0);
            __m128i a_high = _mm256_extractf128_si256(mVec, 1);
            __m128i b_low = _mm256_extractf128_si256(b.mVec, 0);
            __m128i b_high = _mm256_extractf128_si256(b.mVec, 1);
            __m128i r_low = _mm_add_epi32(a_low, b_low);
            __m128i r_high = _mm_add_epi32(a_high, b_high);
            __m128i m_low = _mm256_extractf128_si256(mask.mMask, 0);
            __m128i m_high = _mm256_extractf128_si256(mask.mMask, 1);
            r_low = _mm_blendv_epi8(a_low, r_low, m_low);
            r_high = _mm_blendv_epi8(a_high, r_high, m_high);
            mVec = _mm256_insertf128_si256(mVec, r_low, 0);
            mVec = _mm256_insertf128_si256(mVec, r_high, 1);
            return *this;
        }
        // ADDSA
        inline SIMDVec_i & adda(int32_t b) {
            __m128i a_low = _mm256_extractf128_si256(mVec, 0);
            __m128i a_high = _mm256_extractf128_si256(mVec, 1);
            __m128i b_vec = _mm_set1_epi32(b);
            __m128i r_low = _mm_add_epi32(a_low, b_vec);
            __m128i r_high = _mm_add_epi32(a_high, b_vec);
            mVec = _mm256_insertf128_si256(mVec, r_low, 0);
            mVec = _mm256_insertf128_si256(mVec, r_high, 1);
            return *this;
        }
        inline SIMDVec_i & operator+= (int32_t b) {
            return adda(b);
        }
        // MADDSA
        inline SIMDVec_i & adda(SIMDVecMask<8> const & mask, int32_t b) {
            __m128i b_vec = _mm_set1_epi32(b);
            __m128i a_low = _mm256_extractf128_si256(mVec, 0);
            __m128i a_high = _mm256_extractf128_si256(mVec, 1);
            __m128i r_low = _mm_add_epi32(a_low, b_vec);
            __m128i r_high = _mm_add_epi32(a_high, b_vec);
            __m128i m_low = _mm256_extractf128_si256(mask.mMask, 0);
            __m128i m_high = _mm256_extractf128_si256(mask.mMask, 1);
            r_low = _mm_blendv_epi8(a_low, r_low, m_low);
            r_high = _mm_blendv_epi8(a_high, r_high, m_high);
            mVec = _mm256_insertf128_si256(mVec, r_low, 0);
            mVec = _mm256_insertf128_si256(mVec, r_high, 1);
            return *this;
        }
        // SADDV
        // MSADDV
        // SADDS
        // MSADDS
        // SADDVA
        // MSADDVA
        // SADDSA
        // MSADDSA
        // POSTINC
        inline SIMDVec_i postinc() {
            __m256i t0 = _mm256_set1_epi32(1);
            __m256i t1 = mVec;
            mVec = _mm256_add_epi32(mVec, t0);
            return SIMDVec_i(t1);
        }
        inline SIMDVec_i operator++ (int) {
            return postinc();
        }
        // MPOSTINC
        inline SIMDVec_i postinc(SIMDVecMask<8> const & mask) {
            __m256i t0 = _mm256_set1_epi32(1);
            __m256i t1 = mVec;
            __m256i t2 = _mm256_add_epi32(mVec, t0);
            mVec = _mm256_blendv_epi8(mVec, t2, mask.mMask);
            return SIMDVec_i(t1);
        }
        // PREFINC
        inline SIMDVec_i & prefinc() {
            __m256i t0 = _mm256_set1_epi32(1);
            mVec = _mm256_add_epi32(mVec, t0);
            return *this;
        }
        inline SIMDVec_i & operator++ () {
            return prefinc();
        }
        // MPREFINC
        inline SIMDVec_i & prefinc(SIMDVecMask<8> const & mask) {
            __m256i t0 = _mm256_set1_epi32(1);
            __m256i t1 = _mm256_add_epi32(mVec, t0);
            mVec = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return *this;
        }
        // SUBV
        inline SIMDVec_i sub(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_sub_epi32(mVec, b.mVec);
            return SIMDVec_i(t0);
        }
        inline SIMDVec_i operator- (SIMDVec_i const & b) const {
            return sub(b);
        }
        // MSUBV
        inline SIMDVec_i sub(SIMDVecMask<8> const & mask, SIMDVec_i const & b) const {
            __m256i t0 = _mm256_sub_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // SUBS
        inline SIMDVec_i sub(int32_t b) const {
            __m256i t0 = _mm256_sub_epi32(mVec, _mm256_set1_epi32(b));
            return SIMDVec_i(t0);
        }
        inline SIMDVec_i operator- (int32_t b) const {
            return sub(b);
        }
        // MSUBS
        inline SIMDVec_i sub(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_sub_epi32(mVec, _mm256_set1_epi32(b));
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // SUBVA
        inline SIMDVec_i & suba(SIMDVec_i const & b) {
            mVec = _mm256_sub_epi32(mVec, b.mVec);
            return *this;
        }
        inline SIMDVec_i & operator-= (SIMDVec_i const & b) {
            return suba(b);
        }
        // MSUBVA
        inline SIMDVec_i & suba(SIMDVecMask<8> const & mask, SIMDVec_i const & b) {
            __m256i t0 = _mm256_sub_epi32(mVec, b.mVec);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // SUBSA
        inline SIMDVec_i & suba(int32_t b) {
            mVec = _mm256_sub_epi32(mVec, _mm256_set1_epi32(b));
            return *this;
        }
        inline SIMDVec_i & operator-= (int32_t b) {
            return suba(b);
        }
        // MSUBSA
        inline SIMDVec_i & suba(SIMDVecMask<8> const & mask, int32_t b) {
            __m256i t0 = _mm256_sub_epi32(mVec, _mm256_set1_epi32(b));
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // SSUBV
        // MSSUBV
        // SSUBS
        // MSSUBS
        // SSUBVA
        // MSSUBVA
        // SSUBSA
        // MSSUBSA
        // SUBFROMV
        inline SIMDVec_i subfrom(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_sub_epi32(b.mVec, mVec);
            return SIMDVec_i(t0);
        }
        // MSUBFROMV
        inline SIMDVec_i subfrom(SIMDVecMask<8> const & mask, SIMDVec_i const & b) const {
            __m256i t0 = _mm256_sub_epi32(b.mVec, mVec);
            __m256i t1 = _mm256_blendv_epi8(b.mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // SUBFROMS
        inline SIMDVec_i subfrom(int32_t b) const {
            __m256i t0 = _mm256_sub_epi32(_mm256_set1_epi32(b), mVec);
            return SIMDVec_i(t0);
        }
        // MSUBFROMS
        inline SIMDVec_i subfrom(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_sub_epi32(t0, mVec);
            __m256i t2 = _mm256_blendv_epi8(t0, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // SUBFROMVA
        inline SIMDVec_i & subfroma(SIMDVec_i const & b) {
            mVec = _mm256_sub_epi32(b.mVec, mVec);
            return *this;
        }
        // MSUBFROMVA
        inline SIMDVec_i & subfroma(SIMDVecMask<8> const & mask, SIMDVec_i const & b) {
            __m256i t0 = _mm256_sub_epi32(b.mVec, mVec);
            mVec = _mm256_blendv_epi8(b.mVec, t0, mask.mMask);
            return *this;
        }
        // SUBFROMSA
        inline SIMDVec_i & subfroma(int32_t b) {
            mVec = _mm256_sub_epi32(_mm256_set1_epi32(b), mVec);
            return *this;
        }
        // MSUBFROMSA
        inline SIMDVec_i subfroma(SIMDVecMask<8> const & mask, int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_sub_epi32(t0, mVec);
            mVec = _mm256_blendv_epi8(t0, t1, mask.mMask);
            return *this;
        }
        // POSTDEC
        inline SIMDVec_i postdec() {
            __m256i t0 = _mm256_set1_epi32(1);
            __m256i t1 = mVec;
            mVec = _mm256_sub_epi32(mVec, t0);
            return SIMDVec_i(t1);
        }
        inline SIMDVec_i operator-- (int) {
            return postdec();
        }
        // MPOSTDEC
        inline SIMDVec_i postdec(SIMDVecMask<8> const & mask) {
            __m256i t0 = _mm256_set1_epi32(1);
            __m256i t1 = mVec;
            __m256i t2 = _mm256_sub_epi32(mVec, t0);
            mVec = _mm256_blendv_epi8(mVec, t2, mask.mMask);
            return SIMDVec_i(t1);
        }
        // PREFDEC
        inline SIMDVec_i & prefdec() {
            __m256i t0 = _mm256_set1_epi32(1);
            mVec = _mm256_sub_epi32(mVec, t0);
            return *this;
        }
        inline SIMDVec_i & operator-- () {
            return prefdec();
        }
        // MPREFDEC
        inline SIMDVec_i & prefdec(SIMDVecMask<8> const & mask) {
            __m256i t0 = _mm256_set1_epi32(1);
            __m256i t1 = _mm256_sub_epi32(mVec, t0);
            mVec = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return *this;
        }
        // MULV
        inline SIMDVec_i mul(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_mullo_epi32(mVec, b.mVec);
            return SIMDVec_i(t0);
        }
        inline SIMDVec_i operator* (SIMDVec_i const & b) const {
            return mul(b);
        }
        // MMULV
        inline SIMDVec_i mul(SIMDVecMask<8> const & mask, SIMDVec_i const & b) const {
            __m256i t0 = _mm256_mullo_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // MULS
        inline SIMDVec_i mul(int32_t b) const {
            __m256i t0 = _mm256_mullo_epi32(mVec, _mm256_set1_epi32(b));
            return SIMDVec_i(t0);
        }
        inline SIMDVec_i operator* (int32_t b) const {
            return mul(b);
        }
        // MMULS
        inline SIMDVec_i mul(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_mullo_epi32(mVec, _mm256_set1_epi32(b));
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // MULVA
        inline SIMDVec_i & mula(SIMDVec_i const & b) {
            mVec = _mm256_mullo_epi32(mVec, b.mVec);
            return *this;
        }
        inline SIMDVec_i & operator*= (SIMDVec_i const & b) {
            return mula(b);
        }
        // MMULVA
        inline SIMDVec_i & mula(SIMDVecMask<8> const & mask, SIMDVec_i const & b) {
            __m256i t0 = _mm256_mullo_epi32(mVec, b.mVec);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // MULSA
        inline SIMDVec_i & mula(int32_t b) {
            mVec = _mm256_mullo_epi32(mVec, _mm256_set1_epi32(b));
            return *this;
        }
        inline SIMDVec_i & operator*= (int32_t b) {
            return mula(b);
        }
        // MMULSA
        inline SIMDVec_i & mula(SIMDVecMask<8> const & mask, int32_t b) {
            __m256i t0 = _mm256_mullo_epi32(mVec, _mm256_set1_epi32(b));
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // DIVV
        inline SIMDVec_i operator/ (SIMDVec_i const & b) const {
            return div(b);
        }
        // MDIVV
        // DIVS
        inline SIMDVec_i operator/ (int32_t b) const {
            return div(b);
        }
        // MDIVS
        // DIVVA
        inline SIMDVec_i & operator/= (SIMDVec_i const & b) {
            return diva(b);
        }
        // MDIVVA
        // DIVSA
        inline SIMDVec_i & operator/= (int32_t b) {
            return diva(b);
        }
        // MDIVSA
        // RCP
        // MRCP
        // RCPS
        // MRCPS
        // RCPA
        // MRCPA
        // RCPSA
        // MRCPSA
        // CMPEQV
        inline SIMDVecMask<8> cmpeq(SIMDVec_i const & b) const {
            __m256i m0 = _mm256_cmpeq_epi32(mVec, b.mVec);
            return SIMDVecMask<8>(m0);
        }
        inline SIMDVecMask<8> operator==(SIMDVec_i const & b) const {
            return cmpeq(b);
        }
        // CMPEQS
        inline SIMDVecMask<8> cmpeq(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i m0 = _mm256_cmpeq_epi32(mVec, t0);
            return SIMDVecMask<8>(m0);
        }
        inline SIMDVecMask<8> operator== (int32_t b) const {
            return cmpeq(b);
        }
        // CMPNEV
        inline SIMDVecMask<8> cmpne(SIMDVec_i const & b) const {
            __m256i m0 = _mm256_cmpeq_epi32(mVec, b.mVec);
            __m256i t0 = _mm256_set1_epi32(0xFFFFFFFF);
            __m256i m1 = _mm256_xor_si256(m0, t0);
            return SIMDVecMask<8>(m1);
        }
        inline SIMDVecMask<8> operator!= (SIMDVec_i const & b) const {
            return cmpne(b);
        }
        // CMPNES
        inline SIMDVecMask<8> cmpne(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i m0 = _mm256_cmpeq_epi32(mVec, t0);
            __m256i t1 = _mm256_set1_epi32(0xFFFFFFFF);
            __m256i m1 = _mm256_xor_si256(m0, t1);
            return SIMDVecMask<8>(m1);
        }
        inline SIMDVecMask<8> operator!= (int32_t b) const {
            return cmpne(b);
        }
        // CMPGTV
        inline SIMDVecMask<8> cmpgt(SIMDVec_i const & b) const {;
            __m256i m0 = _mm256_cmpgt_epi32(mVec, b.mVec);
            return SIMDVecMask<8>(m0);
        }
        inline SIMDVecMask<8> operator> (SIMDVec_i const & b) const {
            return cmpgt(b);
        }
        // CMPGTS
        inline SIMDVecMask<8> cmpgt(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i m0 = _mm256_cmpgt_epi32(mVec, t0);
            return SIMDVecMask<8>(m0);
        }
        inline SIMDVecMask<8> operator> (int32_t b) const {
            return cmpgt(b);
        }
        // CMPLTV
        inline SIMDVecMask<8> cmplt(SIMDVec_i const & b) const {
            __m256i m0 = _mm256_cmpgt_epi32(b.mVec, mVec);
            return SIMDVecMask<8>(m0);
        }
        inline SIMDVecMask<8> operator< (SIMDVec_i const & b) const {
            return cmplt(b);
        }
        // CMPLTS
        inline SIMDVecMask<8> cmplt(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i m0 = _mm256_cmpgt_epi32(t0, mVec);
            return SIMDVecMask<8>(m0);
        }
        inline SIMDVecMask<8> operator< (int32_t b) const {
            return cmplt(b);
        }
        // CMPGEV
        inline SIMDVecMask<8> cmpge(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_max_epi32(mVec, b.mVec);
            __m256i m0 = _mm256_cmpeq_epi32(mVec, t0);
            return SIMDVecMask<8>(m0);
        }
        inline SIMDVecMask<8> operator>= (SIMDVec_i const & b) const {
            return cmpge(b);
        }
        // CMPGES
        inline SIMDVecMask<8> cmpge(int32_t b) const {
            __m256i t0 = _mm256_max_epi32(mVec, _mm256_set1_epi32(b));
            __m256i m0 = _mm256_cmpeq_epi32(mVec, t0);
            return SIMDVecMask<8>(m0);
        }
        inline SIMDVecMask<8> operator>= (int32_t b) const {
            return cmpge(b);
        }
        // CMPLEV
        inline SIMDVecMask<8> cmple(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_max_epi32(mVec, b.mVec);
            __m256i m0 = _mm256_cmpeq_epi32(b.mVec, t0);
            return SIMDVecMask<8>(m0);
        }
        inline SIMDVecMask<8> operator<= (SIMDVec_i const & b) const {
            return cmple(b);
        }
        // CMPLES
        inline SIMDVecMask<8> cmple(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_max_epi32(mVec,t0);
            __m256i m0 = _mm256_cmpeq_epi32(t0, t1);
            return SIMDVecMask<8>(m0);
        }
        inline SIMDVecMask<8> operator<= (int32_t b) const {
            return cmple(b);
        }
        // CMPEV
        inline bool cmpe(SIMDVec_i const & b) const {
            alignas(32) int32_t raw[8];
            __m256i m0 = _mm256_cmpeq_epi32(mVec, b.mVec);
            _mm256_store_si256((__m256i*)raw, m0);
            return (raw[0] != 0) && (raw[1] != 0) && (raw[2] != 0) && (raw[3] !=0) &&
                   (raw[4] != 0) && (raw[5] != 0) && (raw[6] != 0) && (raw[7] !=0);
        }
        // CMPES
        inline bool cmpe(int32_t b) const {
            alignas(32) int32_t raw[8];
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i m0 = _mm256_cmpeq_epi32(mVec, t0);
            _mm256_store_si256((__m256i*)raw, m0);
            return (raw[0] != 0) && (raw[1] != 0) && (raw[2] != 0) && (raw[3] !=0) &&
                   (raw[4] != 0) && (raw[5] != 0) && (raw[6] != 0) && (raw[7] !=0);
        }
        // UNIQUE
        inline bool unique() const {
            /* alignas(32) int32_t raw[8];
            _mm256_store_si256((__m256i *)raw, mVec);
            for (unsigned int i = 0; i < 7; i++) {
            for (unsigned int j = i + 1; j < 8; j++) {
            if (raw[i] == raw[j]) {
            return false;
            }
            }
            }*/
            return true;
        }
        // HADD
        inline int32_t hadd() const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_hadd_epi32(mVec, t0);
            __m256i t2 = _mm256_hadd_epi32(t1, t0);
            int32_t retval = _mm256_extract_epi32(t2, 0);
            retval += _mm256_extract_epi32(t2, 4);
            return retval;
        }
        // MHADD
        inline int32_t hadd(SIMDVecMask<8> const & mask) const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            __m256i t2 = _mm256_hadd_epi32(t1, t0);
            __m256i t3 = _mm256_hadd_epi32(t2, t0);
            int32_t retval = _mm256_extract_epi32(t3, 0);
            retval += _mm256_extract_epi32(t3, 4);
            return retval;
        }
        // HADDS
        inline int32_t hadd(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_hadd_epi32(mVec, t0);
            __m256i t2 = _mm256_hadd_epi32(t1, t0);
            int32_t retval = _mm256_extract_epi32(t2, 0);
            retval += _mm256_extract_epi32(t2, 4);
            return retval + b;
        }
        // MHADDS
        inline int32_t hadd(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            __m256i t2 = _mm256_hadd_epi32(t1, t0);
            __m256i t3 = _mm256_hadd_epi32(t2, t0);
            int32_t retval = _mm256_extract_epi32(t3, 0);
            retval += _mm256_extract_epi32(t3, 4);
            return retval + b;
        }
        // HMUL
        inline int32_t hmul() const {
            __m256i t0 = _mm256_set1_epi32(1);
            __m256i t1 = _mm256_permute2f128_si256(mVec, t0, 1);
            __m256i t2 = _mm256_mullo_epi32(mVec, t1);
            __m256i t3 = _mm256_shuffle_epi32(t2, 0xB);
            __m256i t4 = _mm256_mullo_epi32(t2, t3);
            __m256i t5 = _mm256_shuffle_epi32(t4, 0x1);
            __m256i t6 = _mm256_mullo_epi32(t5, t4);
            int32_t retval = _mm256_extract_epi32(t6, 0);
            return retval;
        }
        // MHMUL
        inline int32_t hmul(SIMDVecMask<8> const & mask) const {
            __m256i t0 = _mm256_set1_epi32(1);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            __m256i t2 = _mm256_permute2f128_si256(t1, t0, 1);
            __m256i t3 = _mm256_mullo_epi32(t1, t2);
            __m256i t4 = _mm256_shuffle_epi32(t3, 0xB);
            __m256i t5 = _mm256_mullo_epi32(t3, t4);
            __m256i t6 = _mm256_shuffle_epi32(t5, 0x1);
            __m256i t7 = _mm256_mullo_epi32(t6, t5);
            int32_t retval = _mm256_extract_epi32(t7, 0);
            return retval;
        }
        // HMULS
        inline int32_t hmul(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(1);
            __m256i t1 = _mm256_permute2f128_si256(mVec, t0, 1);
            __m256i t2 = _mm256_mullo_epi32(mVec, t1);
            __m256i t3 = _mm256_shuffle_epi32(t2, 0xB);
            __m256i t4 = _mm256_mullo_epi32(t2, t3);
            __m256i t5 = _mm256_shuffle_epi32(t4, 0x1);
            __m256i t6 = _mm256_mullo_epi32(t5, t4);
            int32_t retval = _mm256_extract_epi32(t6, 0);
            return retval * b;
        }
        // MHMULS
        inline int32_t hmul(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(1);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            __m256i t2 = _mm256_permute2f128_si256(t1, t0, 1);
            __m256i t3 = _mm256_mullo_epi32(t1, t2);
            __m256i t4 = _mm256_shuffle_epi32(t3, 0xB);
            __m256i t5 = _mm256_mullo_epi32(t3, t4);
            __m256i t6 = _mm256_shuffle_epi32(t5, 0x1);
            __m256i t7 = _mm256_mullo_epi32(t6, t5);
            int32_t retval = _mm256_extract_epi32(t7, 0);
            return retval * b;
        }

        // FMULADDV
        inline SIMDVec_i fmuladd(SIMDVec_i const & b, SIMDVec_i const & c) const {
            __m256i t0 = _mm256_mullo_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_add_epi32(t0, c.mVec);
            return SIMDVec_i(t1);
        }
        // MFMULADDV
        inline SIMDVec_i fmuladd(SIMDVecMask<8> const & mask, SIMDVec_i const & b, SIMDVec_i const & c) const {
            __m256i t0 = _mm256_mullo_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_add_epi32(t0, c.mVec);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // FMULSUBV
        inline SIMDVec_i fmulsub(SIMDVec_i const & b, SIMDVec_i const & c) const {
            __m256i t0 = _mm256_mullo_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_sub_epi32(t0, c.mVec);
            return SIMDVec_i(t1);
        }
        // MFMULSUBV
        inline SIMDVec_i fmulsub(SIMDVecMask<8> const & mask, SIMDVec_i const & b, SIMDVec_i const & c) const {
            __m256i t0 = _mm256_mullo_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_sub_epi32(t0, c.mVec);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // FADDMULV
        inline SIMDVec_i faddmul(SIMDVec_i const & b, SIMDVec_i const & c) const {
            __m256i t0 = _mm256_add_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_mullo_epi32(t0, c.mVec);
            return SIMDVec_i(t1);
        }
        // MFADDMULV
        inline SIMDVec_i faddmul(SIMDVecMask<8> const & mask, SIMDVec_i const & b, SIMDVec_i const & c) const {
            __m256i t0 = _mm256_add_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_mullo_epi32(t0, c.mVec);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // FSUBMULV
        inline SIMDVec_i fsubmul(SIMDVec_i const & b, SIMDVec_i const & c) const {
            __m256i t0 = _mm256_sub_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_mullo_epi32(t0, c.mVec);
            return SIMDVec_i(t1);
        }
        // MFSUBMULV
        inline SIMDVec_i fsubmul(SIMDVecMask<8> const & mask, SIMDVec_i const & b, SIMDVec_i const & c) const {
            __m256i t0 = _mm256_sub_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_mullo_epi32(t0, c.mVec);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }

        // MAXV
        inline SIMDVec_i max(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_max_epi32(mVec, b.mVec);
            return SIMDVec_i(t0);
        }
        // MMAXV
        inline SIMDVec_i max(SIMDVecMask<8> const & mask, SIMDVec_i const & b) const {
            __m256i t0 = _mm256_max_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // MAXS
        inline SIMDVec_i max(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_max_epi32(mVec, t0);
            return SIMDVec_i(t1);
        }
        // MMAXS
        inline SIMDVec_i max(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_max_epi32(mVec, t0);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // MAXVA
        inline SIMDVec_i & maxa(SIMDVec_i const & b) {
            mVec = _mm256_max_epi32(mVec, b.mVec);
            return *this;
        }
        // MMAXVA
        inline SIMDVec_i & maxa(SIMDVecMask<8> const & mask, SIMDVec_i const & b) {
            __m256i t0 = _mm256_max_epi32(mVec, b.mVec);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // MAXSA
        inline SIMDVec_i & maxa(int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            mVec = _mm256_max_epi32(mVec, t0);
            return *this;
        }
        // MMAXSA
        inline SIMDVec_i & maxa(SIMDVecMask<8> const & mask, int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_max_epi32(mVec, t0);
            mVec = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return *this;
        }
        // MINV
        inline SIMDVec_i min(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_min_epi32(mVec, b.mVec);
            return SIMDVec_i(t0);
        }
        // MMINV
        inline SIMDVec_i min(SIMDVecMask<8> const & mask, SIMDVec_i const & b) const {
            __m256i t0 = _mm256_min_epi32(mVec, b.mVec);
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // MINS
        inline SIMDVec_i min(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_min_epi32(mVec, t0);
            return SIMDVec_i(t1);
        }
        // MMINS
        inline SIMDVec_i min(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_min_epi32(mVec, t0);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // MINVA
        inline SIMDVec_i & mina(SIMDVec_i const & b) {
            mVec = _mm256_min_epi32(mVec, b.mVec);
            return *this;
        }
        // MMINVA
        inline SIMDVec_i & mina(SIMDVecMask<8> const & mask, SIMDVec_i const & b) {
            __m256i t0 = _mm256_min_epi32(mVec, b.mVec);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // MINSA
        inline SIMDVec_i & mina(int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            mVec = _mm256_min_epi32(mVec, t0);
            return *this;
        }
        // MMINSA
        inline SIMDVec_i & mina(SIMDVecMask<8> const & mask, int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_min_epi32(mVec, t0);
            mVec = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return *this;
        }
        // HMAX
        inline int32_t hmax() const {
            __m256i t0 = _mm256_set1_epi32(std::numeric_limits<int32_t>::min());
            __m256i t1 = _mm256_permute2f128_si256(mVec, t0, 1);
            __m256i t2 = _mm256_max_epi32(mVec, t1);
            __m256i t3 = _mm256_shuffle_epi32(t2, 0xB);
            __m256i t4 = _mm256_max_epi32(t2, t3);
            __m256i t5 = _mm256_shuffle_epi32(t4, 0x1);
            __m256i t6 = _mm256_max_epi32(t5, t4);
            int32_t retval = _mm256_extract_epi32(t6, 0);
            return retval;
        }
        // MHMAX
        inline int32_t hmax(SIMDVecMask<8> const & mask) const {
            __m256i t0 = _mm256_set1_epi32(std::numeric_limits<int32_t>::min());
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            __m256i t2 = _mm256_permute2f128_si256(t1, t0, 1);
            __m256i t3 = _mm256_max_epi32(t1, t2);
            __m256i t4 = _mm256_shuffle_epi32(t3, 0xB);
            __m256i t5 = _mm256_max_epi32(t3, t4);
            __m256i t6 = _mm256_shuffle_epi32(t5, 0x1);
            __m256i t7 = _mm256_max_epi32(t6, t5);
            int32_t retval = _mm256_extract_epi32(t7, 0);
            return retval;
        }
        // IMAX
        // MIMAX
        // HMIN
        inline int32_t hmin() const {
            __m256i t0 = _mm256_set1_epi32(std::numeric_limits<int32_t>::max());
            __m256i t1 = _mm256_permute2f128_si256(mVec, t0, 1);
            __m256i t2 = _mm256_min_epi32(mVec, t1);
            __m256i t3 = _mm256_shuffle_epi32(t2, 0xB);
            __m256i t4 = _mm256_min_epi32(t2, t3);
            __m256i t5 = _mm256_shuffle_epi32(t4, 0x1);
            __m256i t6 = _mm256_min_epi32(t5, t4);
            int32_t retval = _mm256_extract_epi32(t6, 0);
            return retval;
        }
        // MHMIN
        inline int32_t hmin(SIMDVecMask<8> const & mask) const {
            __m256i t0 = _mm256_set1_epi32(std::numeric_limits<int32_t>::max());
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            __m256i t2 = _mm256_permute2f128_si256(t1, t0, 1);
            __m256i t3 = _mm256_min_epi32(t1, t2);
            __m256i t4 = _mm256_shuffle_epi32(t3, 0xB);
            __m256i t5 = _mm256_min_epi32(t3, t4);
            __m256i t6 = _mm256_shuffle_epi32(t5, 0x1);
            __m256i t7 = _mm256_min_epi32(t6, t5);
            int32_t retval = _mm256_extract_epi32(t7, 0);
            return retval;
        }
        // IMIN
        // MIMIN

        // BANDV
        inline SIMDVec_i band(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_and_si256(mVec, b.mVec);
            return SIMDVec_i(t0);
        }
        inline SIMDVec_i operator& (SIMDVec_i const & b) const {
            return band(b);
        }
        // MBANDV
        inline SIMDVec_i band(SIMDVecMask<8> const & mask, SIMDVec_i const & b) const {
            __m256i t0 = _mm256_and_si256(mVec, b.mVec);
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // BANDS
        inline SIMDVec_i band(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_and_si256(mVec, t0);
            return SIMDVec_i(t1);
        }
        inline SIMDVec_i operator& (int32_t b) const {
            return band(b);
        }
        // MBANDS
        inline SIMDVec_i band(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_and_si256(mVec, t0);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // BANDVA
        inline SIMDVec_i & banda(SIMDVec_i const & b) {
            mVec = _mm256_and_si256(mVec, b.mVec);
            return *this;
        }
        inline SIMDVec_i & operator&= (SIMDVec_i const & b) {
            return banda(b);
        }
        // MBANDVA
        inline SIMDVec_i & banda(SIMDVecMask<8> const & mask, SIMDVec_i const & b) {
            __m256i t0 = _mm256_and_si256(mVec, b.mVec);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // BANDSA
        inline SIMDVec_i & banda(int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            mVec = _mm256_and_si256(mVec, t0);
            return *this;
        }
        inline SIMDVec_i & operator&= (bool b) {
            return banda(b);
        }
        // MBANDSA
        inline SIMDVec_i & banda(SIMDVecMask<8> const & mask, int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_and_si256(mVec, t0);
            mVec = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return *this;
        }
        // BORV
        inline SIMDVec_i bor(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_or_si256(mVec, b.mVec);
            return SIMDVec_i(t0);
        }
        inline SIMDVec_i operator| (SIMDVec_i const & b) const {
            return bor(b);
        }
        // MBORV
        inline SIMDVec_i bor(SIMDVecMask<8> const & mask, SIMDVec_i const & b) const {
            __m256i t0 = _mm256_or_si256(mVec, b.mVec);
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // BORS
        inline SIMDVec_i bor(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_or_si256(mVec, t0);
            return SIMDVec_i(t1);
        }
        inline SIMDVec_i operator| (int32_t b) const {
            return bor(b);
        }
        // MBORS
        inline SIMDVec_i bor(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_or_si256(mVec, t0);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // BORVA
        inline SIMDVec_i & bora(SIMDVec_i const & b) {
            mVec = _mm256_or_si256(mVec, b.mVec);
            return *this;
        }
        inline SIMDVec_i & operator|= (SIMDVec_i const & b) {
            return bora(b);
        }
        // MBORVA
        inline SIMDVec_i & bora(SIMDVecMask<8> const & mask, SIMDVec_i const & b) {
            __m256i t0 = _mm256_or_si256(mVec, b.mVec);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // BORSA
        inline SIMDVec_i & bora(int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            mVec = _mm256_or_si256(mVec, t0);
            return *this;
        }
        inline SIMDVec_i & operator|= (int32_t b) {
            return bora(b);
        }
        // MBORSA
        inline SIMDVec_i & bora(SIMDVecMask<8> const & mask, int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_or_si256(mVec, t0);
            mVec = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return *this;
        }
        // BXORV
        inline SIMDVec_i bxor(SIMDVec_i const & b) const {
            __m256i t0 = _mm256_xor_si256(mVec, b.mVec);
            return SIMDVec_i(t0);
        }
        inline SIMDVec_i operator^ (SIMDVec_i const & b) const {
            return bxor(b);
        }
        // MBXORV
        inline SIMDVec_i bxor(SIMDVecMask<8> const & mask, SIMDVec_i const & b) const {
            __m256i t0 = _mm256_xor_si256(mVec, b.mVec);
            __m256i t1 = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return SIMDVec_i(t1);
        }
        // BXORS
        inline SIMDVec_i bxor(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_xor_si256(mVec, t0);
            return SIMDVec_i(t1);
        }
        inline SIMDVec_i operator^ (int32_t b) const {
            return bxor(b);
        }
        // MBXORS
        inline SIMDVec_i bxor(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_xor_si256(mVec, t0);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // BXORVA
        inline SIMDVec_i & bxora(SIMDVec_i const & b) {
            mVec = _mm256_xor_si256(mVec, b.mVec);
            return *this;
        }
        inline SIMDVec_i & operator^= (SIMDVec_i const & b) {
            return bxora(b);
        }
        // MBXORVA
        inline SIMDVec_i & bxora(SIMDVecMask<8> const & mask, SIMDVec_i const & b) {
            __m256i t0 = _mm256_xor_si256(mVec, b.mVec);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // BXORSA
        inline SIMDVec_i & bxora(int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            mVec = _mm256_xor_si256(mVec, t0);
            return *this;
        }
        inline SIMDVec_i & operator^= (int32_t b) {
            return bxora(b);
        }
        // MBXORSA
        inline SIMDVec_i & bxora(SIMDVecMask<8> const & mask, int32_t b) {
            __m256i t0 = _mm256_set1_epi32(b);
            __m256i t1 = _mm256_xor_si256(mVec, t0);
            mVec = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return *this;
        }
        // BNOT
        inline SIMDVec_i bnot() const {
            __m256i t0 = _mm256_set1_epi32(0xFFFFFFFF);
            __m256i t1 = _mm256_xor_si256(mVec, t0);
            return SIMDVec_i(t1);
        }
        inline SIMDVec_i operator~ () const {
            return bnot();
        }
        // MBNOT
        inline SIMDVec_i bnot(SIMDVecMask<8> const & mask) const {
            __m256i t0 = _mm256_set1_epi32(0xFFFFFFFF);
            __m256i t1 = _mm256_xor_si256(mVec, t0);
            __m256i t2 = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return SIMDVec_i(t2);
        }
        // BNOTA
        inline SIMDVec_i & bnota() {
            __m256i t0 = _mm256_set1_epi32(0xFFFFFFFF);
            mVec = _mm256_xor_si256(mVec, t0);
            return *this;
        }
        // MBNOTA
        inline SIMDVec_i bnota(SIMDVecMask<8> const & mask) {
            __m256i t0 = _mm256_set1_epi32(0xFFFFFFFF);
            __m256i t1 = _mm256_xor_si256(mVec, t0);
            mVec = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return *this;
        }
        // HBAND
        inline int32_t hband() const {
            __m256i t0 = _mm256_set1_epi32(0xFFFFFFFF);
            __m256i t1 = _mm256_permute2f128_si256(mVec, t0, 1);
            __m256i t2 = _mm256_and_si256(mVec, t1);
            __m256i t3 = _mm256_shuffle_epi32(t2, 0xB);
            __m256i t4 = _mm256_and_si256(t2, t3);
            __m256i t5 = _mm256_shuffle_epi32(t4, 0x1);
            __m256i t6 = _mm256_and_si256(t5, t4);
            int32_t retval = _mm256_extract_epi32(t6, 0);
            return retval;
        }
        // MHBAND
        inline int32_t hband(SIMDVecMask<8> const & mask) const {
            __m256i t0 = _mm256_set1_epi32(0xFFFFFFFF);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            __m256i t2 = _mm256_permute2f128_si256(t1, t0, 1);
            __m256i t3 = _mm256_and_si256(t1, t2);
            __m256i t4 = _mm256_shuffle_epi32(t3, 0xB);
            __m256i t5 = _mm256_and_si256(t3, t4);
            __m256i t6 = _mm256_shuffle_epi32(t5, 0x1);
            __m256i t7 = _mm256_and_si256(t6, t5);
            int32_t retval = _mm256_extract_epi32(t7, 0);
            return retval;
        }
        // HBANDS
        inline int32_t hband(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(0xFFFFFFFF);
            __m256i t1 = _mm256_permute2f128_si256(mVec, t0, 1);
            __m256i t2 = _mm256_and_si256(mVec, t1);
            __m256i t3 = _mm256_shuffle_epi32(t2, 0xB);
            __m256i t4 = _mm256_and_si256(t2, t3);
            __m256i t5 = _mm256_shuffle_epi32(t4, 0x1);
            __m256i t6 = _mm256_and_si256(t5, t4);
            int32_t retval = _mm256_extract_epi32(t6, 0);
            return retval & b;
        }
        // MHBANDS
        inline int32_t hband(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(0xFFFFFFFF);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            __m256i t2 = _mm256_permute2f128_si256(t1, t0, 1);
            __m256i t3 = _mm256_and_si256(t1, t2);
            __m256i t4 = _mm256_shuffle_epi32(t3, 0xB);
            __m256i t5 = _mm256_and_si256(t3, t4);
            __m256i t6 = _mm256_shuffle_epi32(t5, 0x1);
            __m256i t7 = _mm256_and_si256(t6, t5);
            int32_t retval = _mm256_extract_epi32(t7, 0);
            return retval & b;
        }
        // HBOR
        inline int32_t hbor() const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_permute2f128_si256(mVec, t0, 1);
            __m256i t2 = _mm256_or_si256(mVec, t1);
            __m256i t3 = _mm256_shuffle_epi32(t2, 0xB);
            __m256i t4 = _mm256_or_si256(t2, t3);
            __m256i t5 = _mm256_shuffle_epi32(t4, 0x1);
            __m256i t6 = _mm256_or_si256(t5, t4);
            int32_t retval = _mm256_extract_epi32(t6, 0);
            return retval;
        }
        // MHBOR
        inline int32_t hbor(SIMDVecMask<8> const & mask) const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            __m256i t2 = _mm256_permute2f128_si256(t1, t0, 1);
            __m256i t3 = _mm256_or_si256(t1, t2);
            __m256i t4 = _mm256_shuffle_epi32(t3, 0xB);
            __m256i t5 = _mm256_or_si256(t3, t4);
            __m256i t6 = _mm256_shuffle_epi32(t5, 0x1);
            __m256i t7 = _mm256_or_si256(t6, t5);
            int32_t retval = _mm256_extract_epi32(t7, 0);
            return retval;
        }
        // HBORS
        inline int32_t hbor(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_permute2f128_si256(mVec, t0, 1);
            __m256i t2 = _mm256_or_si256(mVec, t1);
            __m256i t3 = _mm256_shuffle_epi32(t2, 0xB);
            __m256i t4 = _mm256_or_si256(t2, t3);
            __m256i t5 = _mm256_shuffle_epi32(t4, 0x1);
            __m256i t6 = _mm256_or_si256(t5, t4);
            int32_t retval = _mm256_extract_epi32(t6, 0);
            return retval | b;
        }
        // MHBORS
        inline int32_t hbor(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            __m256i t2 = _mm256_permute2f128_si256(t1, t0, 1);
            __m256i t3 = _mm256_or_si256(t1, t2);
            __m256i t4 = _mm256_shuffle_epi32(t3, 0xB);
            __m256i t5 = _mm256_or_si256(t3, t4);
            __m256i t6 = _mm256_shuffle_epi32(t5, 0x1);
            __m256i t7 = _mm256_or_si256(t6, t5);
            int32_t retval = _mm256_extract_epi32(t7, 0);
            return retval | b;
        }
        // HBXOR
        inline int32_t hbxor() const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_permute2f128_si256(mVec, t0, 1);
            __m256i t2 = _mm256_xor_si256(mVec, t1);
            __m256i t3 = _mm256_shuffle_epi32(t2, 0xB);
            __m256i t4 = _mm256_xor_si256(t2, t3);
            __m256i t5 = _mm256_shuffle_epi32(t4, 0x1);
            __m256i t6 = _mm256_xor_si256(t5, t4);
            int32_t retval = _mm256_extract_epi32(t6, 0);
            return retval;
        }
        // MHBXOR
        inline int32_t hbxor(SIMDVecMask<8> const & mask) const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            __m256i t2 = _mm256_permute2f128_si256(t1, t0, 1);
            __m256i t3 = _mm256_xor_si256(t1, t2);
            __m256i t4 = _mm256_shuffle_epi32(t3, 0xB);
            __m256i t5 = _mm256_xor_si256(t3, t4);
            __m256i t6 = _mm256_shuffle_epi32(t5, 0x1);
            __m256i t7 = _mm256_xor_si256(t6, t5);
            int32_t retval = _mm256_extract_epi32(t7, 0);
            return retval;
        }
        // HBXORS
        inline int32_t hbxor(int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_permute2f128_si256(mVec, t0, 1);
            __m256i t2 = _mm256_xor_si256(mVec, t1);
            __m256i t3 = _mm256_shuffle_epi32(t2, 0xB);
            __m256i t4 = _mm256_xor_si256(t2, t3);
            __m256i t5 = _mm256_shuffle_epi32(t4, 0x1);
            __m256i t6 = _mm256_xor_si256(t5, t4);
            int32_t retval = _mm256_extract_epi32(t6, 0);
            return retval ^ b;
        }
        // MHBXORS
        inline int32_t hbxor(SIMDVecMask<8> const & mask, int32_t b) const {
            __m256i t0 = _mm256_set1_epi32(0);
            __m256i t1 = _mm256_blendv_epi8(t0, mVec, mask.mMask);
            __m256i t2 = _mm256_permute2f128_si256(t1, t0, 1);
            __m256i t3 = _mm256_xor_si256(t1, t2);
            __m256i t4 = _mm256_shuffle_epi32(t3, 0xB);
            __m256i t5 = _mm256_xor_si256(t3, t4);
            __m256i t6 = _mm256_shuffle_epi32(t5, 0x1);
            __m256i t7 = _mm256_xor_si256(t6, t5);
            int32_t retval = _mm256_extract_epi32(t7, 0);
            return retval ^ b;
        }
        // GATHERS
        inline SIMDVec_i & gather(int32_t const * baseAddr, uint32_t const * indices) {
            __m256i t0 = _mm256_loadu_si256((__m256i*)indices);
            mVec = _mm256_i32gather_epi32((const int *)baseAddr, t0, 4);
            return *this;
        }
        // MGATHERS
        inline SIMDVec_i & gather(SIMDVecMask<8> const & mask, int32_t const * baseAddr, uint32_t const * indices) {
            __m256i t0 = _mm256_loadu_si256((__m256i*)indices);
            __m256i t1 = _mm256_i32gather_epi32((const int *)baseAddr, t0, 4);
            mVec = _mm256_blendv_epi8(mVec, t1, mask.mMask);
            return *this;
        }
        // GATHERV
        inline SIMDVec_i & gather(int32_t const * baseAddr, SIMDVec_u<uint32_t, 8> const & indices) {
            mVec = _mm256_i32gather_epi32((const int *)baseAddr, indices.mVec, 4);
            return *this;
        }
        // MGATHERV
        inline SIMDVec_i & gather(SIMDVecMask<8> const & mask, int32_t const * baseAddr, SIMDVec_u<uint32_t, 8> const & indices) {
            __m256i t0 = _mm256_i32gather_epi32((const int *)baseAddr, indices.mVec, 4);
            mVec = _mm256_blendv_epi8(mVec, t0, mask.mMask);
            return *this;
        }
        // SCATTERS
        inline int32_t* scatter(int32_t* baseAddr, uint32_t* indices) const {
            alignas(32) int32_t raw[8];
            _mm256_store_si256((__m256i*) raw, mVec);
            for (int i = 0; i < 8; i++) { baseAddr[indices[i]] = raw[i]; };
            return baseAddr;
        }
        // MSCATTERS
        inline int32_t* scatter(SIMDVecMask<8> const & mask, int32_t* baseAddr, uint32_t* indices) const {
            alignas(32) int32_t raw[8];
            alignas(32) uint32_t rawMask[8];
            _mm256_store_si256((__m256i*) raw, mVec);
            _mm256_store_si256((__m256i*) rawMask, mask.mMask);
            for (int i = 0; i < 8; i++) { if (rawMask[i] == SIMDVecMask<8>::TRUE()) baseAddr[indices[i]] = raw[i]; };
            return baseAddr;
        }
        // SCATTERV
        inline int32_t* scatter(int32_t* baseAddr, SIMDVec_u<uint32_t, 8> const & indices) const {
            alignas(32) int32_t raw[8];
            alignas(32) uint32_t rawIndices[8];
            _mm256_store_si256((__m256i*) raw, mVec);
            _mm256_store_si256((__m256i*) rawIndices, indices.mVec);
            for (int i = 0; i < 8; i++) { baseAddr[rawIndices[i]] = raw[i]; };
            return baseAddr;
        }
        // MSCATTERV
        inline int32_t* scatter(SIMDVecMask<8> const & mask, int32_t* baseAddr, SIMDVec_u<uint32_t, 8> const & indices) const {
            alignas(32) int32_t raw[8];
            alignas(32) uint32_t rawIndices[8];
            alignas(32) uint32_t rawMask[8];
            _mm256_store_si256((__m256i*) raw, mVec);
            _mm256_store_si256((__m256i*) rawIndices, indices.mVec);
            _mm256_store_si256((__m256i*) rawMask, mask.mMask);
            for (int i = 0; i < 8; i++) {
                if (rawMask[i] == SIMDVecMask<8>::TRUE())
                    baseAddr[rawIndices[i]] = raw[i];
            };
            return baseAddr;
        }
        // LSHV
        // MLSHV
        // LSHS
        // MLSHS
        // LSHVA
        // MLSHVA
        // LSHSA
        // MLSHSA
        // RSHV
        // MRSHV
        // RSHS
        // MRSHS
        // RSHVA
        // MRSHVA
        // RSHSA
        // MRSHSA
        // ROLV
        // MROLV
        // ROLS
        // MROLS
        // ROLVA
        // MROLVA
        // ROLSA
        // MROLSA
        // RORV
        // MRORV
        // RORS
        // MRORS
        // RORVA
        // MRORVA
        // RORSA
        // MRORSA

        // NEG
        inline SIMDVec_i operator- () const {
            return neg();
        }
        // MNEG
        // NEGA
        // MNEGA
        // ABS
        inline SIMDVec_i abs() const {
            __m128i a_low = _mm256_extractf128_si256(mVec, 0);
            __m128i a_high = _mm256_extractf128_si256(mVec, 1);
            __m256i ret = _mm256_setzero_si256();
            ret = _mm256_insertf128_si256(ret, _mm_abs_epi32(a_low), 0);
            ret = _mm256_insertf128_si256(ret, _mm_abs_epi32(a_high), 1);
            return SIMDVec_i(ret);
        }
        // MABS
        inline SIMDVec_i abs(SIMDVecMask<8> const & mask) const {
            __m128i a_low = _mm256_extractf128_si256(mVec, 0);
            __m128i a_high = _mm256_extractf128_si256(mVec, 1);
            __m128i m_low = _mm256_extractf128_si256(mask.mMask, 0);
            __m128i m_high = _mm256_extractf128_si256(mask.mMask, 1);

            __m128i r_low = _mm_blendv_epi8(a_low, _mm_abs_epi32(a_low), m_low);
            __m128i r_high = _mm_blendv_epi8(a_high, _mm_abs_epi32(a_high), m_high);
            __m256i ret = _mm256_setzero_si256();
            ret = _mm256_insertf128_si256(ret, r_low, 0);
            ret = _mm256_insertf128_si256(ret, r_high, 1);
            return SIMDVec_i(ret);
        }
        // ABSA
        // MABSA

        // PACK
        inline SIMDVec_i & pack(SIMDVec_i<int32_t, 4> const & a, SIMDVec_i<int32_t, 4> const & b) {
            mVec = _mm256_inserti128_si256(mVec, a.mVec, 0);
            mVec = _mm256_inserti128_si256(mVec, b.mVec, 1);
            return *this;
        }
        // PACKLO
        inline SIMDVec_i & packlo(SIMDVec_i<int32_t, 4> const & a) {
            mVec = _mm256_inserti128_si256(mVec, a.mVec, 0);
            return *this;
        }
        // PACKHI
        inline SIMDVec_i & packhi(SIMDVec_i<int32_t, 4> const & b) {
            mVec = _mm256_inserti128_si256(mVec, b.mVec, 1);
            return *this;
        }
        // UNPACK
        inline void unpack(SIMDVec_i<int32_t, 4> & a, SIMDVec_i<int32_t, 4> & b) const {
            a.mVec = _mm256_extracti128_si256(mVec, 0);
            b.mVec = _mm256_extracti128_si256(mVec, 1);
        }
        // UNPACKLO
        inline SIMDVec_i<int32_t, 4> unpacklo() const {
            __m128i t0 = _mm256_extracti128_si256(mVec, 0);
            return SIMDVec_i<int32_t, 4>(t0);
        }
        // UNPACKHI
        inline SIMDVec_i<int32_t, 4> unpackhi() const {
            __m128i t0 = _mm256_extracti128_si256(mVec, 1);
            return SIMDVec_i<int32_t, 4>(t0);
        }

        // PROMOTE
        inline operator SIMDVec_i<int64_t, 8>() const;
        // DEGRADE
        inline operator SIMDVec_i<int16_t, 8>() const;

        // ITOU
        inline operator SIMDVec_u<uint32_t, 8>() const;
        // ITOF
        inline operator SIMDVec_f<float, 8>() const;
    };

}
}

#endif
