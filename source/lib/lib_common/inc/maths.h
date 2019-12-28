#pragma once
#define  NOMINMAX
#include <algorithm>

namespace shared
{
	namespace Maths
	{
		//
		// Base Math Helpers
		//
		// Takes a numerical type and returns the value rounded to nPlaces precision
		template <typename T>
		T round(T dbVal, int nPlaces = 0)
		{
			const double dbShift = pow(10.0, nPlaces);
			return  floor(dbVal * dbShift + 0.5) / dbShift;
		}

		constexpr double pi()
		{
			return 3.141592653589793238462643383279502884;
		}
		constexpr double deg2rad(double deg)
		{
			return (deg * pi()) / 180.0f;
		}

		//
		// Bit Helpers (maybe move out of Math?)
		//
		template <typename _Type>
		__forceinline uint32_t countBits(_Type i)
		{
			i = i - ((i >> 1) & 0x55555555);
			i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
			return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
		}

		template <typename _Type>
		__forceinline uint32_t highestBit(_Type val)
		{
			uint32_t v = static_cast<uint32_t>(val);
			// msbDeBruijn32
			static const uint32_t MultiplyDeBruijnBitPosition[32] =
			{
				0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
				8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
			};

			v |= v >> 1; // first round down to one less than a power of 2
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;

			return static_cast<_Type>(MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27] + 1);
		}

		template <typename _Type>
		__forceinline uint32_t lowestBit(_Type v)
		{
			// msbDeBruijn32
			static const int MultiplyDeBruijnBitPosition[32] =
			{
				0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
				31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
			};

			return MultiplyDeBruijnBitPosition[((uint32_t)((v & -v) * 0x077CB531U)) >> 27];
		}

		// We don't have a 'highest bit' for floats, it is split across two sections
		template <>
		__forceinline uint32_t highestBit<float>(float)
		{
			return sizeof(float) * 8;
		}

		template <>
		__forceinline uint32_t highestBit<double>(double)
		{
			return sizeof(double) * 8;
		}

		// We know bools only have one state, return just the one
		template <>
		__forceinline uint32_t highestBit<bool>(bool)
		{
			return 1;
		}
	}
}