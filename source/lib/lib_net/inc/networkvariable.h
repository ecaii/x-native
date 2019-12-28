#pragma once
#include "hash.h"
#include "networksynchronisation.h"
#include <type_traits>

class NetworkVariableBase
{
public:
	NetworkVariableBase(const shared::CompilerHashU8& name) :
		m_Name(name)
	{
	}

	virtual void              Synchronise(NetworkSynchronisation& sync) = 0;
	const shared::CompilerHashU8&     GetName() const { return m_Name; }

protected:
	shared::CompilerHashU8 m_Name;
};

template <class _Type, uint32_t _Bits>
class NetworkVariableUInt : public NetworkVariableBase
{
public:
	// Unsigned Variant, for Signed use NetworkVariableInt for correct synchronisation
	static_assert(std::is_unsigned<_Type>::value, "bad T");

	NetworkVariableUInt(const shared::CompilerHashU8& name) :
		NetworkVariableBase(name),
		m_Value(0)
	{
	}

	virtual void Synchronise(NetworkSynchronisation& sync)
	{
		sync.Synchronise(m_Value, _Bits);
	}

	operator _Type() const { return m_Value; }

protected:
	_Type m_Value;
};

template <class _Type, uint32_t _Bits>
class NetworkVariableInt : public NetworkVariableBase
{
public:
	// Signed Variant, for Unsigned use NetworkVariableUInt for correct synchronisation
	static_assert(!std::is_unsigned<_Type>::value, "bad T");

	NetworkVariableInt(const shared::CompilerHashU8& name) :
		NetworkVariableBase(name),
		m_Value(0)
	{
	}

	virtual void Synchronise(NetworkSynchronisation& sync)
	{
		bool signBit = ((m_Value & NETWORK_VARIABLE_SIGN_BIT) != 0);
		sync.Synchronise(signBit, 1);

		// Take off the sign bit for synchronisation, as we've already synchronised it
		m_Value &= ~(NETWORK_VARIABLE_SIGN_BIT);
		sync.Synchronise(m_Value, _Bits);

		// Then set it if we have to
		if (signBit) {
			m_Value |= NETWORK_VARIABLE_SIGN_BIT;
		}
	}

	operator _Type() const { return m_Value; }

protected:
	_Type m_Value;
};