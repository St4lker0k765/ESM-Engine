////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_ex_void.h
//	Created 	: 06.02.2004
//  Modified 	: 11.01.2005
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : Script callbacks with return value but specialized with void
////////////////////////////////////////////////////////////////////////////

#pragma once

template <>
class CScriptCallbackEx<void> : public CScriptCallbackEx_<void> {
	template <typename T>
	static void do_return(T&& value, std::true_type)
	{
		(void)value;
	}

	template <typename T>
	static return_type do_return(T&& value, std::false_type /*is_void*/) {
		return return_type(std::forward<T>(value));
	}

	template <typename T>
	static decltype(auto) do_return(T&& value) {
		return do_return(std::forward<T>(value), std::is_void<return_type>());
	}
public:
	template<typename... Args>
	return_type operator()(Args&&... args) const
	{
		try
		{
			try
			{
				if (this->m_functor)
				{
					VERIFY(this->m_functor.is_valid());
					if (this->m_object.is_valid())
					{
						VERIFY(this->m_object.is_valid());
						return do_return(this->m_functor(this->m_object, std::forward<Args>(args)...));
					}
					else
						return do_return(this->m_functor(std::forward<Args>(args)...));
				}
			}
			catch (std::exception&)
			{
			}
		}
		catch (...) {
			const_cast<CScriptCallbackEx<return_type>*>(this)->clear();
		}
		return do_return(0);
	}

	template <typename... Args>
	return_type operator()(Args&&... args)
	{
		try
		{
			try
			{
				if (this->m_functor)
				{
					VERIFY(this->m_functor.is_valid());
					if (this->m_object.is_valid()) {
						VERIFY(this->m_object.is_valid());
						return do_return(this->m_functor(this->m_object, std::forward<Args>(args)...));
					}
					else
						return do_return(this->m_functor(std::forward<Args>(args)...));
				}
			}
			catch (std::exception&)
			{
			}
		}
		catch (...) {
			const_cast<CScriptCallbackEx<return_type>*>(this)->clear();
		}
		return do_return(0);
	}
public:
//#	define	macros_return_operator
#	undef	SCRIPT_CALLBACK_EX_GENERATORS
#	include "script_callback_ex_generators.h"
//#	undef	macros_return_operator
};