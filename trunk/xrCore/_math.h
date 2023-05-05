#ifndef __XR_MATH_H__
#define __XR_MATH_H__

namespace FPU {
	XRCORE_API void	 m24	(void);
	XRCORE_API void	 m24r	(void);	
	XRCORE_API void	 m53	(void);	
	XRCORE_API void	 m53r	(void);	
	XRCORE_API void	 m64	(void);	
	XRCORE_API void	 m64r	(void);	
};

extern XRCORE_API	void	_initialize_cpu			();
extern XRCORE_API	void	_initialize_cpu_thread	();

// threading
typedef				void	thread_t				( void * );
extern XRCORE_API	void	thread_name				( const char* name);
extern XRCORE_API	void	thread_spawn			(
	thread_t*	entry,
	const char*	name,
	unsigned	stack,
	void*		arglist 
	);

#endif //__XR_MATH_H__
