#pragma once
#include <cinttypes>
#include <minmax.h>
#include <cstring>

#include "unittests.h"
#include "assert.h"
#include "maths.h"

#define NETWORK_BYTE_SIZE 8

#define NETWORK_VARIABLE_SIGN_BIT       0x80000000
#define NETWORK_IDENTIFIER_BIT_LOCAL    0x80000000
#define NETWORK_IDENTIFIER_SYNCED_BITS  8

class NetworkSynchronisation;
class NetworkSynchronisationRead;
class NetworkSynchronisationWrite;

template <
	uint32_t _MaxID,
	uint32_t _ReservedBits
>
class NetworkIdentifier : public shared::UnitTestable
{
public:
	static_assert(_ReservedBits >= 1, "NetworkIdentifier requires at least 1 bit reserved for local conditional check");

	typedef uint32_t UniqueID;

	static constexpr int GetUniqueIDBitCount()
	{
		return (NETWORK_BYTE_SIZE * sizeof(UniqueID)) - _ReservedBits;
	}

	NetworkIdentifier(uint32_t identifier)
	{
		m_Bits = (identifier << GetUniqueIDBitCount());
		m_ID = (identifier && ~(1 << GetUniqueIDBitCount()));
	}

	NetworkIdentifier(UniqueID id, uint32_t bits)
	{
		m_ID = id;
		m_Bits = bits;
	}

	operator uint32_t() const
	{
		return GetIdentifier();
	}

	bool operator==(NetworkIdentifier& b) const { return GetIdentifier() == b.GetIdentifier(); }
	bool operator!=(NetworkIdentifier& b) const { return GetIdentifier() != b.GetIdentifier(); }

	uint32_t GetIdentifier() const
	{
		return (m_Bits << GetUniqueIDBitCount()) & m_ID;
	}

	UniqueID GetUniqueIdentifier() const
	{
		return m_ID;
	}

	uint32_t GetBits() const
	{
		return m_Bits;
	}

	bool         IsLocal() const                           { return IsBitSet(NETWORK_IDENTIFIER_BIT_LOCAL); }
	void         SetIsLocal(bool value)                    { SetStateBit(NETWORK_IDENTIFIER_BIT_LOCAL, value); }
	void         Synchronise(NetworkSynchronisation& sync) { sync.Synchronise(m_ID, NETWORK_IDENTIFIER_SYNCED_BITS); }

protected:
	__forceinline void SetStateBit(uint32_t bit, bool value)
	{
		if (value) {
			m_Bits |= bit;
		}
		else {
			m_Bits &= ~(bit);
		}
	}

	__forceinline bool IsBitSet(uint32_t bit) const
	{
		return (m_Bits & bit) != 0;
	}

private:
	uint32_t m_Bits : ( _ReservedBits );
	uint32_t m_ID   : ( GetUniqueIDBitCount() );
};

class NetworkSynchronisation : public shared::UnitTestable
{
public:
	enum Mode
	{
		SYNC_MODE_NONE,
		SYNC_MODE_WRITE,
		SYNC_MODE_READ
	};

	// Consturctor that allocates and takes ownership of its own byte bufffer
	NetworkSynchronisation(uint32_t size);

	virtual ~NetworkSynchronisation();

	TESTABLE_OBJECT(NetworkSynchronisation);

	///
	///    Unspecified
	///
	template <typename _Type>
	__forceinline void Synchronise(_Type& value, uint32_t bits)
	{
		if (GetMode() == Mode::SYNC_MODE_READ) {
			return Read(value, bits);
		}
		else if (GetMode() == Mode::SYNC_MODE_WRITE) {
			return Write(value, bits);
		}

		assert(0, "m_Mode invalid");
	}

	///
	///   Cursor and Misc
	///
	void ResetCursor()
	{
		m_BitCursor = 0;
	}

	void Reset()
	{
		memset(m_Buffer, 0, m_Size);
		ResetCursor();
	}

#if _DEBUG
	void Print();
#endif 

	// Copies the specified buffer into the underlying buffer
	template <uint32_t _InBytes>
	void SetBuffer(const char(&buffer)[_InBytes])
	{
		SetBuffer(buffer, _InBytes);
	}

	void SetBuffer(const char* buffer, uint32_t size)
	{
		assert_crash(size <= m_Size, "Incoming buffer is too large!");
		memcpy(m_Buffer, buffer, min(m_Size, size));
	}

	const char* GetBuffer() { return m_Buffer; }
	uint32_t    GetSize() { return m_Size; }
	uint32_t    GetByteCursorLo() const { return m_BitCursor / NETWORK_BYTE_SIZE; }
	uint32_t    GetByteCursorHi() const { return m_BitCursor % NETWORK_BYTE_SIZE; }
	Mode        GetMode() const { return m_Mode; }

protected:
	void SetMode(Mode mode)
	{
		m_Mode = mode;
		ResetCursor();
	}

	///
	///   Read Interface
	///
	template <typename _Type>
	__forceinline void Read(_Type& value, uint32_t bits)
	{
		ReadBits(reinterpret_cast<char*>(&value), sizeof(_Type), bits);
	}

	template <int _OutBytes>
	__forceinline void ReadBits(char(&buffer)[_OutBytes], uint32_t bits)
	{
		ReadBits(buffer, _OutBytes, bits);
	}

	void ReadBits(char* buffer, uint32_t size, uint32_t bits);

	///
	///   Write Interface
	///
	template <typename _Type>
	__forceinline void Write(_Type value, uint32_t bits)
	{
		assert_crash(shared::Maths::highestBit(value) <= bits, "Tried writing too much data with insufficient bits!");
		WriteBits(reinterpret_cast<const char*>(&value), sizeof(_Type), bits);
	}

	template <int _InBytes>
	__forceinline void WriteBits(const char(&buffer)[_InBytes], uint32_t bits)
	{
		WriteBits(buffer, _InBytes, bits);
	}

	void WriteBits(const char* buffer, uint32_t size, uint32_t bits);

	Mode       m_Mode;
	char*      m_Buffer;
	uint32_t   m_Size;
	uint32_t   m_BitCursor;
};

///
///  Explicits
///

class NetworkSynchronisationWrite : public NetworkSynchronisation
{
public:
	NetworkSynchronisationWrite(uint32_t size) :
		NetworkSynchronisation(size)
	{
		SetMode(SYNC_MODE_WRITE);
	}

	NetworkSynchronisationWrite(NetworkSynchronisationRead& read);
};

class NetworkSynchronisationRead : public NetworkSynchronisation
{
public:
	NetworkSynchronisationRead(uint32_t size) :
		NetworkSynchronisation(size)
	{
		SetMode(SYNC_MODE_READ);
	}

	NetworkSynchronisationRead(NetworkSynchronisationWrite& read);
};
