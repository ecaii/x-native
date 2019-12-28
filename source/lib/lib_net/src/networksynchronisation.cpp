#include "networksynchronisation.h"

#ifdef _DEBUG
struct TestData
{
	TestData() :
		m_X(0),
		m_Y(0)
	{ }

	int m_X;
	int m_Y;

	void Synchronise(NetworkSynchronisation& sync)
	{
		sync.Synchronise(m_X, 4);
		sync.Synchronise(m_Y, 4);
	}
};

void TestMagicBufferSynchronise(char(&buffer)[32], NetworkSynchronisation& sync)
{
	sync.Synchronise(reinterpret_cast<uint32_t&>(buffer), NETWORK_BYTE_SIZE * 4);
	sync.Synchronise(reinterpret_cast<uint32_t&>(buffer), NETWORK_BYTE_SIZE * 4);
}

TESTABLE_DEFINE(NetworkSynchronisation, {

	// TEST:  Creates a magic buffer of 32 bytes, then reads two 4 byte chunks - then tests the write back is the same
	{
		// Run the write
		NetworkSynchronisationWrite write(32);

		char original[32];
		char aftersync[32];
		for (int i = 0; i < 32; ++i) {
			original[i] = (i % 32);
			aftersync[i] = 0;
		}

		TestMagicBufferSynchronise(original, write);

		// Now test the read
		NetworkSynchronisationRead read(write);
		TestMagicBufferSynchronise(aftersync, read);

		read.Print();

		for (int i = 0; i < 4; ++i) {
			if (aftersync[i] != original[i]) {
				DbgError("Index %u (%hhx, %hhx)", i, aftersync[i], original[i]);
				return false;
			}
		}
	}

	// TEST:  Writes two non aligned amount of bits, then tests the read back is working.
	//        Primarily to test if the hi bits are wrote and read correctly
	{
		NetworkSynchronisation sync(32);

		const unsigned bits_a = 3;
		const unsigned bits_b = 2;

		unsigned value_a = 1;
		unsigned value_b = 2;

		bool  bool_value = true;
		
		sync.Write(value_a, bits_a);
		sync.Write(value_b, bits_b);
		sync.Write(bool_value, 1);

		sync.Print();
		sync.ResetCursor();

		unsigned read_a = 0;
		unsigned read_b = 0;
		bool     read_bool = false;

		sync.Read(read_a, bits_a);
		sync.Read(read_b, bits_b);
		sync.Read(read_bool, 1);

		if (read_a != value_a) {
			DbgError("Mismatch (A), read  %u but expected %u", read_a, value_a);
			return false;
		}
		if (read_a != value_a) {
			DbgError("Mismatch (A), read  %u but expected %u", read_a, value_a);
			return false;
		}
		if (bool_value != read_bool) {
			DbgError("Mismatch (bool), read  %u but expected %u", read_bool, bool_value);
			return false;
		}
	}

	// TEST:  Writes a more complicated structure type, then compares it
	{
		TestData original;
		original.m_X = 3;
		original.m_Y = 2;

		NetworkSynchronisationWrite write(32);
		original.Synchronise(write);

		// Now read it
		NetworkSynchronisationRead read(write);
		read.Print();

		TestData mouse;
		mouse.Synchronise(read);

		if (mouse.m_X != original.m_X) {
			DbgError("Mismatch (m_X), read %u but expected %u", mouse.m_X, original.m_X);
			return false;
		}
		if (mouse.m_Y != original.m_Y) {
			DbgError("Mismatch (m_X), read %u but expected %u", mouse.m_Y, original.m_Y);
			return false;
		}
	}

	return true;
});

#endif

void NetworkSynchronisation::WriteBits(const char* buffer, uint32_t size, uint32_t bits)
{
	(void)size;
	todo("should size be read?");

	for (uint32_t bit = 0; bit < bits; ++bit)
	{
		const uint32_t cursorLo = GetByteCursorLo();
		const uint32_t cursorHi = GetByteCursorHi();

		const uint32_t lo = bit / NETWORK_BYTE_SIZE;
		const uint32_t hi = bit % NETWORK_BYTE_SIZE;

		if (cursorHi == 0)
		{
			// We need to reset the current byte, as we are going into it
			m_Buffer[cursorLo] = 0;
		}

		if ((buffer[lo] & (1 << hi)) != 0)
		{
			m_Buffer[cursorLo] |= (1 << cursorHi);
		}

		++m_BitCursor;
	}
}

void NetworkSynchronisation::ReadBits(char* buffer, uint32_t size, uint32_t bits)
{
	(void)size;
	todo("should size be read?");

	for (uint32_t bit = 0; bit < bits; ++bit)
	{
		const uint32_t cursorLo = GetByteCursorLo();
		const uint32_t cursorHi = GetByteCursorHi();

		const uint32_t lo = bit / NETWORK_BYTE_SIZE;
		const uint32_t hi = bit % NETWORK_BYTE_SIZE;

		if (lo >= m_Size) {
			assert(0, "lo >= m_Size");
			break;
		}

		if (cursorHi == 0)
		{
			// We need to reset the current byte, as we are going into it
			buffer[cursorLo] = 0;
		}

		if ((m_Buffer[lo] & (1 << cursorHi)) != 0)
		{
			buffer[lo] |= (1 << hi);
		}

		++m_BitCursor;
	}
}

#ifdef _DEBUG
void NetworkSynchronisation::Print()
{
	// Print the lo
	for (uint32_t byte = 0; byte < m_Size; ++byte)
	{
		DbgLogRaw("%hhx ", m_Buffer[ byte ]);
	}

	// Print the hi
	const uint32_t remainingBits = m_Size % NETWORK_BYTE_SIZE;
	for (uint32_t bit = 0; bit < remainingBits; ++bit)
	{
		DbgLogRaw("%hhx ", m_Buffer[ m_Size ]);
	}

	DbgLogRaw("\n");
}
#endif

NetworkSynchronisation::NetworkSynchronisation(uint32_t size)
{
	m_Buffer = new char[size];
	m_Size = size;
	m_BitCursor = 0;
	m_Mode = Mode::SYNC_MODE_NONE;

	memset(m_Buffer, 0, size);
}

NetworkSynchronisation::~NetworkSynchronisation()
{
	delete[] m_Buffer;
}

///
///  Explicits
///
NetworkSynchronisationWrite::NetworkSynchronisationWrite(NetworkSynchronisationRead& read) :
	NetworkSynchronisation(read.GetSize())
{
	SetBuffer(read.GetBuffer(), read.GetSize());
	SetMode(SYNC_MODE_WRITE);
}

NetworkSynchronisationRead::NetworkSynchronisationRead(NetworkSynchronisationWrite& read) :
	NetworkSynchronisation(read.GetSize())
{
	SetBuffer(read.GetBuffer(), read.GetSize());
	SetMode(SYNC_MODE_READ);
}
