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

template <uint32_t _MaxID,
	uint32_t _ReservedBits>
	class NetworkIdentifier
{
public:
	static_assert(_ReservedBits >= 1, "NetworkIdentifier requires at least 1 bit reserved for local conditional check");

	typedef uint32_t UniqueID;

	constexpr int GetUniqueIDBitCount() const {
		return NETWORK_BYTE_SIZE * sizeof(UniqueID);
	}

	NetworkIdentifier(uint32_t id)
	{
		m_ID = id;

		// Other bits can be reserved for gameplay, but the unique bits need to be within the identifier bounds
		assert_ctx(UniqueID uid = GetUniqueIdentifier());
		assert_crash(uid < _MaxID, "id >= _MaxID");
		todo("need to cull unique identifier bits if over MaxID to ensure no overflow");
	}

	operator uint32_t() const { return m_ID; }

	bool operator==(NetworkIdentifier& b) const { return m_ID == b.m_ID; }
	bool operator!=(NetworkIdentifier& b) const { return m_ID != b.m_ID; }

	uint32_t     GetIdentifier()   const { return m_ID; }
	bool         IsLocal() const { return (GetIdentifier() & NETWORK_IDENTIFIER_BIT_LOCAL) != 0; }
	void         SetIsLocal(bool value) { SetStateBit(NETWORK_IDENTIFIER_BIT_LOCAL, value); }
	void         Synchronise(NetworkSynchronisation& sync) { sync.Synchronise(m_ID, NETWORK_IDENTIFIER_SYNCED_BITS); }

	virtual UniqueID GetUniqueIdentifier() const
	{
		return (GetIdentifier() & ~(1 << (GetUniqueIDBitCount() - _ReservedBits)));
	}

protected:
	void SetStateBit(uint32_t bit, bool value)
	{
		if (value) {
			m_ID |= bit;
		}
		else {
			m_ID &= ~(bit);
		}
	}

private:
	uint32_t m_ID;
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
