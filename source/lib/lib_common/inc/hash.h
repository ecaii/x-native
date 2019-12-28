#pragma once

namespace shared
{
	// Just a simple util for hashing and comparison
	template <class T>
	class CompilerHash
	{
	public:
		typedef unsigned int Value;

#ifdef _DEBUG
		CompilerHash(const T* psData) : m_HashValue(const_hash(psData)), m_String(psData)
		{
		}
#else
		constexpr CompilerHash(const T* psData) : m_HashValue(const_hash(psData))
		{
		}
#endif

		CompilerHash() : m_HashValue(0) { }

		bool operator==(const T* psData) const
		{
			return const_hash(psData) == m_HashValue;
		}

		bool operator!=(const T* psData) const
		{
			return const_hash(psData) != m_HashValue;
		}

		bool operator==(Value value) const { return value == m_HashValue; }
		bool operator!=(Value value) const { return value == m_HashValue; }
		bool operator==(const CompilerHash& value) const { return value.m_HashValue == m_HashValue; }
		bool operator!=(const CompilerHash& value) const { return value.m_HashValue == m_HashValue; }

		constexpr Value GetValue() const { return m_HashValue; }
#ifdef _DEBUG
		const T* GetString() const { return m_String; }
#endif 
	protected:
		// https://stackoverflow.com/questions/2111667/compile-time-string-hashing
		static constexpr unsigned const_hash(const T* psData)
		{
			return *psData ?
				static_cast<unsigned int>(*psData) + 33 * const_hash(psData + 1) :
				5381;
		}

		Value m_HashValue;
#ifdef _DEBUG
		const T* m_String;
#endif
	};

	typedef CompilerHash<char>   CompilerHashU8;
};