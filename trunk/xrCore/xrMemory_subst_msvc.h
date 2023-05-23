#pragma once

#ifdef DEBUG_MEMORY_NAME
template <class T, class... Args>
IC T* xr_new(Args&&... args)
{
	T* ptr = (T*)Memory.mem_alloc(sizeof(T), typeid(T).name());
	return new (ptr) T(std::forward<Args>(args)...);
}
#else // DEBUG_MEMORY_NAME
template <class T, class... Args>
IC T* xr_new(Args&&... args)
{
	T* ptr = static_cast<T*>(Memory.mem_alloc(sizeof(T)));
	return new (ptr) T(std::forward<Args>(args)...);
}
#endif // DEBUG_MEMORY_NAME

template <class T>
IC void xr_delete(T*& ptr)
{
	if (ptr)
	{
		xr_special_free<std::is_polymorphic<T>::value, T>()(ptr);
		ptr = NULL;
	}
}
template <class T>
IC void xr_delete(T* const& ptr)
{
	if (ptr)
	{
		xr_special_free<std::is_polymorphic<T>::value, T>()(const_cast<T*&>(ptr));
		const_cast<T*&>(ptr) = NULL;
	}
}



#ifdef DEBUG_MEMORY_MANAGER
	void XRCORE_API mem_alloc_gather_stats				(const bool &value);
	void XRCORE_API mem_alloc_gather_stats_frequency	(const float &value);
	void XRCORE_API mem_alloc_show_stats				();
	void XRCORE_API mem_alloc_clear_stats				();
#endif // DEBUG_MEMORY_MANAGER