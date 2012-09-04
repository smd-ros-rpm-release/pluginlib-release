//
// AtomicCounter.h
//
// $Id: //poco/1.3/Foundation/include/Poco/AtomicCounter.h#6 $
//
// Library: Foundation
// Package: Core
// Module:  AtomicCounter
//
// Definition of the AtomicCounter class.
//
// Copyright (c) 2009, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Foundation_AtomicCounter_INCLUDED
#define Foundation_AtomicCounter_INCLUDED


#include "Poco/Foundation.h"
#if POCO_OS == POCO_OS_WINDOWS_NT
#include "Poco/UnWindows.h"
#elif POCO_OS == POCO_OS_MAC_OS_X
#include <libkern/OSAtomic.h>
#else
#include "Poco/Mutex.h"
#endif // POCO_OS


namespace Poco {


class Foundation_API AtomicCounter
	/// This class implements a simple counter, which
	/// provides atomic operations that are safe to
	/// use in a multithreaded environment.
	///
	/// Typical usage of AtomicCounter is for implementing
	/// reference counting and similar things.
	///
	/// On some platforms, the implementation of AtomicCounter
	/// is based on atomic primitives specific to the platform
	/// (such as InterlockedIncrement, etc. on Windows), and
	/// thus very efficient. On platforms that do not support
	/// atomic primitives, operations are guarded by a FastMutex.
	///
	/// The following platforms currently have atomic
	/// primitives:
	///   - Windows
	///   - Mac OS X
{
public:
	typedef int ValueType; /// The underlying integer type.
	
	AtomicCounter();
		/// Creates a new AtomicCounter and initializes it to zero.
		
	explicit AtomicCounter(ValueType initialValue);
		/// Creates a new AtomicCounter and initializes it with
		/// the given value.
	
	AtomicCounter(const AtomicCounter& counter);
		/// Creates the counter by copying another one.
	
	~AtomicCounter();
		/// Destroys the AtomicCounter.

	AtomicCounter& operator = (const AtomicCounter& counter);
		/// Assigns the value of another AtomicCounter.
		
	AtomicCounter& operator = (ValueType value);
		/// Assigns a value to the counter.

	operator ValueType () const;
		/// Returns the value of the counter.
		
	ValueType value() const;
		/// Returns the value of the counter.
		
	ValueType operator ++ (); // prefix
		/// Increments the counter and returns the result.
		
	ValueType operator ++ (int); // postfix
		/// Increments the counter and returns the previous value.
		
	ValueType operator -- (); // prefix
		/// Decrements the counter and returns the result.
		
	ValueType operator -- (int); // postfix
		/// Decrements the counter and returns the previous value.
		
	bool operator ! () const;
		/// Returns true if the counter is zero, false otherwise.

private:
#if POCO_OS == POCO_OS_WINDOWS_NT
	typedef volatile LONG ImplType;
#elif POCO_OS == POCO_OS_MAC_OS_X
	typedef int32_t ImplType;
#else // generic implementation based on FastMutex
	struct ImplType
	{
		mutable FastMutex mutex;
		volatile int      value;
	};
#endif // POCO_OS

	ImplType _counter;
};


//
// inlines
//


#if POCO_OS == POCO_OS_WINDOWS_NT
//
// Windows
//
inline AtomicCounter::operator AtomicCounter::ValueType () const
{
	return _counter;
}

	
inline AtomicCounter::ValueType AtomicCounter::value() const
{
	return _counter;
}


inline AtomicCounter::ValueType AtomicCounter::operator ++ () // prefix
{
	return InterlockedIncrement(&_counter);
}

	
inline AtomicCounter::ValueType AtomicCounter::operator ++ (int) // postfix
{
	ValueType result(_counter);
	InterlockedIncrement(&_counter);
	return result;
}


inline AtomicCounter::ValueType AtomicCounter::operator -- () // prefix
{
	return InterlockedDecrement(&_counter);
}

	
inline AtomicCounter::ValueType AtomicCounter::operator -- (int) // postfix
{
	ValueType result(_counter);
	InterlockedDecrement(&_counter);
	return result;
}

	
inline bool AtomicCounter::operator ! () const
{
	return _counter == 0;
}


#elif POCO_OS == POCO_OS_MAC_OS_X
//
// Mac OS X
//
inline AtomicCounter::operator AtomicCounter::ValueType () const
{
	return _counter;
}

	
inline AtomicCounter::ValueType AtomicCounter::value() const
{
	return _counter;
}


inline AtomicCounter::ValueType AtomicCounter::operator ++ () // prefix
{
	return OSAtomicIncrement32(&_counter);
}

	
inline AtomicCounter::ValueType AtomicCounter::operator ++ (int) // postfix
{
	ValueType result(_counter);
	OSAtomicIncrement32(&_counter);
	return result;
}


inline AtomicCounter::ValueType AtomicCounter::operator -- () // prefix
{
	return OSAtomicDecrement32(&_counter);
}

	
inline AtomicCounter::ValueType AtomicCounter::operator -- (int) // postfix
{
	ValueType result(_counter);
	OSAtomicDecrement32(&_counter);
	return result;
}

	
inline bool AtomicCounter::operator ! () const
{
	return _counter == 0;
}


#else
//
// Generic implementation based on FastMutex
//
inline AtomicCounter::operator AtomicCounter::ValueType () const
{
	ValueType result;
	{
		FastMutex::ScopedLock lock(_counter.mutex);
		result = _counter.value;
	}
	return result;
}

	
inline AtomicCounter::ValueType AtomicCounter::value() const
{
	ValueType result;
	{
		FastMutex::ScopedLock lock(_counter.mutex);
		result = _counter.value;
	}
	return result;
}


inline AtomicCounter::ValueType AtomicCounter::operator ++ () // prefix
{
	ValueType result;
	{
		FastMutex::ScopedLock lock(_counter.mutex);
		result = ++_counter.value;
	}
	return result;
}

	
inline AtomicCounter::ValueType AtomicCounter::operator ++ (int) // postfix
{
	ValueType result;
	{
		FastMutex::ScopedLock lock(_counter.mutex);
		result = _counter.value++;
	}
	return result;
}


inline AtomicCounter::ValueType AtomicCounter::operator -- () // prefix
{
	ValueType result;
	{
		FastMutex::ScopedLock lock(_counter.mutex);
		result = --_counter.value;
	}
	return result;
}

	
inline AtomicCounter::ValueType AtomicCounter::operator -- (int) // postfix
{
	ValueType result;
	{
		FastMutex::ScopedLock lock(_counter.mutex);
		result = _counter.value--;
	}
	return result;
}

	
inline bool AtomicCounter::operator ! () const
{
	bool result;
	{
		FastMutex::ScopedLock lock(_counter.mutex);
		result = _counter.value == 0;
	}
	return result;
}


#endif // POCO_OS


} // namespace Poco


#endif // Foundation_AtomicCounter_INCLUDED
