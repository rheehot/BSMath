#pragma once

#include <cstdalign>
#include <emmintrin.h>
#include "Random.h"
#include "Utility.h"

namespace BSMath
{
	struct IntPoint final
	{
	public: // Templates
		[[nodiscard]] constexpr static IntPoint Zero() noexcept
		{
			return IntPoint{ 0 };
		}

		[[nodiscard]] constexpr static IntPoint One() noexcept
		{
			return IntPoint{ 1 };
		}

		[[nodiscard]] constexpr static IntPoint Right() noexcept
		{
			return IntPoint{ 1, 0 };
		}

		[[nodiscard]] constexpr static IntPoint Left() noexcept
		{
			return IntPoint{ -1, 0 };
		}

		[[nodiscard]] constexpr static IntPoint Up() noexcept
		{
			return IntPoint{ 0, 1 };
		}

		[[nodiscard]] constexpr static IntPoint Down() noexcept
		{
			return IntPoint{ 0, -1 };
		}

	public:
		constexpr IntPoint() : x(0), y(0) {}

		explicit constexpr IntPoint(int n)
			: x(n), y(n) {}

		explicit constexpr IntPoint(const int* ptr)
			: x(ptr[0]), y(ptr[1]) {}

		explicit constexpr IntPoint(int inX, int inY)
			: x(inX), y(inY) {}

		explicit constexpr IntPoint(const struct IntVector& vec);

		explicit IntPoint(const __m128i& simd)
		{
			alignas(16) int ret[4];
			_mm_store_si128(reinterpret_cast<__m128i*>(ret), simd);
			x = ret[0]; y = ret[1];
		}

		constexpr void Set(int inX, int inY) noexcept
		{
			x = inX; y = inY;
		}

		[[nodiscard]] constexpr int GetMin() const noexcept { return Min(x, y); }
		[[nodiscard]] constexpr int GetMax() const noexcept { return Max(x, y); }

		[[nodiscard]] bool operator==(const IntPoint& other) const noexcept;
		[[nodiscard]] inline bool operator!=(const IntPoint& other) const noexcept { return !(*this == other); }

		IntPoint operator-() const noexcept;

		IntPoint& operator+=(const IntPoint& other) noexcept;
		IntPoint& operator-=(const IntPoint& other) noexcept;

		IntPoint& operator*=(const IntPoint& other) noexcept;
		IntPoint& operator*=(int scaler) noexcept;

		IntPoint& operator/=(const IntPoint& other) noexcept;
		IntPoint& operator/=(int divisor) noexcept;

		[[nodiscard]] constexpr int& operator[](size_t idx) noexcept { return (&x)[idx]; }
		[[nodiscard]] constexpr int operator[](size_t idx) const noexcept { return (&x)[idx]; }

		[[nodiscard]] static int Dot(const IntPoint& lhs, const IntPoint& rhs) noexcept;

	public:
		int x;
		int y;
	};

	inline bool IntPoint::operator==(const IntPoint& other) const noexcept
	{
		const __m128i lhs = _mm_set_epi32(0, 0, y, x);
		const __m128i rhs = _mm_set_epi32(0, 0, other.y, other.x);
		const auto val = _mm_movemask_epi8((_mm_cmpeq_epi32(lhs, rhs)));
		return val == 0xFFFF;
	}

	[[nodiscard]] inline IntPoint IntPoint::operator-() const noexcept
	{
		const __m128i zero = _mm_setzero_si128();
		const __m128i vec = _mm_set_epi32(0, 0, y, x);
		return IntPoint{ _mm_sub_epi32(zero, vec) };
	}

	inline IntPoint& IntPoint::operator+=(const IntPoint& other) noexcept
	{
		const __m128i lhs = _mm_set_epi32(0, 0, y, x);
		const __m128i rhs = _mm_set_epi32(0, 0, other.y, other.x);
		return *this = IntPoint{ _mm_add_epi32(lhs, rhs) };
	}

	inline IntPoint& IntPoint::operator-=(const IntPoint& other) noexcept
	{
		const __m128i lhs = _mm_set_epi32(0, 0, y, x);
		const __m128i rhs = _mm_set_epi32(0, 0, other.y, other.x);
		return *this = IntPoint{ _mm_sub_epi32(lhs, rhs) };
	}

	inline IntPoint& IntPoint::operator*=(const IntPoint& other) noexcept
	{
		const __m128i operand = _mm_set_epi32(0, other.y, 0, other.x);
		__m128i point = _mm_set_epi32(0, y, 0, x);
		point = _mm_mul_epu32(point, operand);
		return *this = IntPoint{ _mm_shuffle_epi32(point, _MM_SHUFFLE(3, 1, 2, 0)) };
	}

	inline IntPoint& IntPoint::operator*=(int scaler) noexcept
	{
		return *this *= IntPoint{ scaler };
	}

	// ToDo: Use integer specific instruction
	inline IntPoint& IntPoint::operator/=(const IntPoint& other) noexcept
	{
		const __m128 lhs = _mm_set_ps(0.0f, 0.0f, static_cast<float>(y), static_cast<float>(x));
		const __m128 rhs = _mm_set_ps(0.0f, 0.0f, static_cast<float>(other.y), static_cast<float>(other.x));
		return *this = IntPoint{ _mm_cvtps_epi32(_mm_div_ps(lhs, rhs)) };
	}

	inline IntPoint& IntPoint::operator/=(int divisor) noexcept
	{
		return *this /= IntPoint{ divisor };
	}

	[[nodiscard]] inline IntPoint operator+(const IntPoint& lhs, const IntPoint& rhs) noexcept
	{
		return IntPoint{ lhs } += rhs;
	}

	[[nodiscard]] inline IntPoint operator-(const IntPoint& lhs, const IntPoint& rhs) noexcept
	{
		return IntPoint{ lhs } -= rhs;
	}

	[[nodiscard]] inline IntPoint operator*(const IntPoint& lhs, const IntPoint& rhs) noexcept
	{
		return IntPoint{ lhs } *= rhs;
	}

	[[nodiscard]] inline IntPoint operator*(const IntPoint& vec, int scaler) noexcept
	{
		return IntPoint{ vec } *= scaler;
	}

	[[nodiscard]] inline IntPoint operator*(int scaler, const IntPoint& vec) noexcept
	{
		return IntPoint{ vec } *= scaler;
	}

	[[nodiscard]] inline IntPoint operator/(const IntPoint& lhs, const IntPoint& rhs) noexcept
	{
		return IntPoint{ lhs } /= rhs;
	}

	[[nodiscard]] inline IntPoint operator/(const IntPoint& vec, int divisor) noexcept
	{
		return IntPoint{ vec } /= divisor;
	}

	[[nodiscard]] inline int operator|(const IntPoint& lhs, const IntPoint& rhs) noexcept
	{
		auto mul = lhs * rhs;
		return mul.x + mul.y;
	}

	inline int IntPoint::Dot(const IntPoint& lhs, const IntPoint& rhs) noexcept
	{
		return lhs | rhs;
	}

	template <>
	[[nodiscard]] inline auto Min<IntPoint>(const IntPoint& lhs, const IntPoint& rhs) noexcept
	{
		const __m128i lhsSimd = _mm_set_epi32(0, 0, lhs.y, lhs.x);
		const __m128i rhsSimd = _mm_set_epi32(0, 0, rhs.y, rhs.x);
		const __m128i mask = _mm_cmplt_epi32(lhsSimd, rhsSimd);
		return IntPoint{ _mm_xor_si128(rhsSimd, _mm_and_si128(mask, _mm_xor_si128(lhsSimd, rhsSimd))) };
	}

	template <>
	[[nodiscard]] inline auto Max<IntPoint>(const IntPoint& lhs, const IntPoint& rhs) noexcept
	{
		const __m128i lhsSimd = _mm_set_epi32(0, 0, lhs.y, lhs.x);
		const __m128i rhsSimd = _mm_set_epi32(0, 0, rhs.y, rhs.x);
		const __m128i mask = _mm_cmpgt_epi32(lhsSimd, rhsSimd);
		return IntPoint{ _mm_xor_si128(rhsSimd, _mm_and_si128(mask, _mm_xor_si128(lhsSimd, rhsSimd))) };
	}

	template <>
	[[nodiscard]] inline auto Clamp<IntPoint>(const IntPoint& n, const IntPoint& min, const IntPoint& max) noexcept
	{
		IntPoint realMin = Min(min, max);
		IntPoint realMax = Max(min, max);
		return Max(realMin, Min(realMax, n));
	}

	template <>
	[[nodiscard]] inline IntPoint Abs<IntPoint>(const IntPoint& n) noexcept
	{
		__m128i point = _mm_set_epi32(0, 0, n.y, n.x);
		__m128i mask = _mm_cmplt_epi32(point, _mm_setzero_si128());
		point = _mm_xor_si128(point, mask);
		mask = _mm_and_si128(mask, _mm_set1_epi32(1));
		return IntPoint{ _mm_add_epi32(point, mask) };
	}

	template <>
	[[nodiscard]] inline IntPoint Sign<IntPoint>(const IntPoint& n) noexcept
	{
		const __m128i zero = _mm_setzero_si128();
		const __m128i simd = _mm_set_epi32(0, 0, n.y, n.x);
		const __m128i positive = _mm_and_si128(_mm_cmpgt_epi32(simd, zero), _mm_set1_epi32(1));
		const __m128i negative = _mm_and_si128(_mm_cmplt_epi32(simd, zero), _mm_set1_epi32(-1));
		return IntPoint{ _mm_or_si128(positive, negative) };
	}

	// Random
	class UniformIntPointDistribution : public std::uniform_int_distribution<int>
	{
		using Super = std::uniform_int_distribution<int>;

	public:
		using result_type = IntPoint;

		template <class Engine>
		[[nodiscard]] result_type operator()(Engine& engine)
		{
			const int x = Super::operator()(engine);
			const int y = Super::operator()(engine);
			return IntPoint{ x, y };
		}

		template <class Engine>
		[[nodiscard]] result_type operator()(Engine& engine, const param_type& param)
		{
			const int x = Super::operator()(engine, param);
			const int y = Super::operator()(engine, param);
			return IntPoint{ x, y };
		}
	};

	using UniformIntPointRandom = Random<IntPoint, std::mt19937, UniformIntPointDistribution>;
}