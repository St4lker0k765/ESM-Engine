#pragma once

// Note:
// ZNear - always 0.0f
// ZFar  - always 1.0f

#include "pure.h"
#include "ftimer.h"
#include "stats.h"

#define VIEWPORT_NEAR  0.2f

#define DEVICE_RESET_PRECACHE_FRAME_COUNT 10

#include "../Include/xrRender/RenderDeviceRender.h"
#include "Threading/xrThreadingCriticalSection.h"
#include "Threading/EventThreading.hpp"
#include "Threading/EventThreadingID.h"

// refs
class ENGINE_API CRenderDevice 
{
private:
    // Main objects used for creating and rendering the 3D scene
    u32										m_dwWindowStyle;
    RECT									m_rcWindowBounds;
    RECT									m_rcWindowClient;

	u32										Timer_MM_Delta;
	CTimer_paused							Timer;
	CTimer_paused							TimerGlobal;
	CTimer									TimerMM;

	void									_Create		(LPCSTR shName);
	void									_Destroy	(BOOL	bKeepTextures);
	void									_SetupStates();
public:
    HWND									m_hWnd;
	LRESULT									MsgProc		(HWND,UINT,WPARAM,LPARAM);

	u32										dwFrame;
	u32										dwPrecacheFrame;
	u32										dwPrecacheTotal;

	u32										dwWidth, dwHeight;
	float									fWidth_2, fHeight_2;
	BOOL									b_is_Ready;
	BOOL									b_is_Active;
	void									OnWM_Activate(WPARAM wParam, LPARAM lParam);
public:
	
    BOOL									m_bNearer; 
    
	IRenderDeviceRender* m_pRender;

	void									SetNearer	(BOOL enabled)
	{
		if (enabled&&!m_bNearer){
			m_bNearer						= TRUE;
			mProject._43					-= EPS_L;
		}else if (!enabled&&m_bNearer){
			m_bNearer						= FALSE;
			mProject._43					+= EPS_L;
		}
		m_pRender->SetCacheXform(mView, mProject);
	}
public:
	// Registrators
	CRegistrator	<pureRender			>			seqRender;
	CRegistrator	<pureAppActivate	>			seqAppActivate;
	CRegistrator	<pureAppDeactivate	>			seqAppDeactivate;
	CRegistrator	<pureAppStart		>			seqAppStart;
	CRegistrator	<pureAppEnd			>			seqAppEnd;
	CRegistrator	<pureFrame			>			seqFrame;
	CRegistrator	<pureFrame			>			seqFrameMT;
	CRegistrator	<pureDeviceReset	>			seqDeviceReset;
	xr_vector		<fastdelegate::FastDelegate0<> >	seqParallel;

	// Dependent classes
 
	CStats*									Statistic;
	

	// Engine flow-control
	float									fTimeDelta;
	float									fTimeGlobal;
	u32										dwTimeDelta;
	u32										dwTimeGlobal;
	u32										dwTimeContinual;

	// Cameras & projection
	Fvector									vCameraPosition;
	Fvector									vCameraDirection;
	Fvector									vCameraTop;
	Fvector									vCameraRight;
	Fmatrix									mView;
	Fmatrix									mProject;
	Fmatrix									mFullTransform;

	// Copies of corresponding members. Used for synchronization.
	Fvector									vCameraPosition_saved;

	Fmatrix									mView_saved;
	Fmatrix									mProject_saved;
	Fmatrix									mFullTransform_saved;

	Fmatrix mInvFullTransform;
	float									fFOV;
	float									fASPECT;
	
	CRenderDevice();

	void	Pause							(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason);
	BOOL	Paused							();

	// Scene control
	void PreCache							(u32 frames);
	BOOL Begin								();
	void Clear								();
	void End								();
	void FrameMove							();
	
	void overdrawBegin						();
	void overdrawEnd						();

	// Mode control
	void DumpFlags							();
	IC	 CTimer_paused* GetTimerGlobal		()	{ return &TimerGlobal;								}
	u32	 TimerAsync							()	{ return TimerGlobal.GetElapsed_ms();				}
	u32	 TimerAsync_MMT						()	{ return TimerMM.GetElapsed_ms() +	Timer_MM_Delta; }

	// Creation & Destroying
	void ConnectToRender();
	void Create								(void);
	void Run								(void);
	void Destroy							(void);
	void Reset								(bool precache = true);

	void Initialize							(void);
	void ShutDown							(void);

public:
	void time_factor						(const float &time_factor)
	{
		Timer.time_factor		(time_factor);
		TimerGlobal.time_factor	(time_factor);
	}
	
	IC	const float &time_factor			() const
	{
		VERIFY					(Timer.time_factor() == TimerGlobal.time_factor());
		return					(Timer.time_factor());
	}

#define SEQUANTIAL_AUX_THREADS_COUNT 2
	// Multi-threading
	xrCriticalSection	mt_csEnter;
	xrCriticalSection	mt_csLeave;
	volatile BOOL		mt_bMustExit;

	// Dependable from MainThread Aux thread 1 workload pool
	xr_vector		<fastdelegate::FastDelegate0<>>	auxThreadPool_1_;
	// Dependable from MainThread Aux thread 5 workload pool
	xr_vector		<fastdelegate::FastDelegate0<>>	auxThreadPool_5_;

	// Processing copies of threads workload pools

	xr_vector		<fastdelegate::FastDelegate0<>>	AuxThreadPool_1_TempCopy_;

	// Locks
	AccessLock		AuxPool_1_Protection_;
	AccessLock		AuxPool_5_Protection_;
private:
    //depended from main thread auxilary thread 1
	static void AuxThread_1(void* context);
public:

	Event auxThread_1_Allowed_;
	Event auxThread_1_Ready_;

	Event aux1ExitSync_; // Aux thread 1 exit event sync

	void				SetAuxThreadsMustExit(bool val);
	bool				IsAuxThreadsMustExit();

	volatile bool		auxTreadsMustExit_;

	ICF		void			remove_from_seq_parallel	(const fastdelegate::FastDelegate0<> &delegate)
	{
		xr_vector<fastdelegate::FastDelegate0<> >::iterator I = std::find(
			seqParallel.begin(),
			seqParallel.end(),
			delegate
		);
		if (I != seqParallel.end())
			seqParallel.erase	(I);
	}

	ICF	void			AddToAuxThread_Pool(u8 aux_thread_no, const fastdelegate::FastDelegate0<>& delegate)
	{
		R_ASSERT(aux_thread_no > 0 && aux_thread_no <= SEQUANTIAL_AUX_THREADS_COUNT);

		if (aux_thread_no == 1)
		{
			AuxPool_1_Protection_.Enter();
			auxThreadPool_1_.push_back(delegate);
			AuxPool_1_Protection_.Leave();
		}
		else if (aux_thread_no == 2)
		{
			AuxPool_5_Protection_.Enter();
			auxThreadPool_5_.push_back(delegate);
			AuxPool_5_Protection_.Leave();
		}
		else
			R_ASSERT(false);
	}

	//// Individual thread object removal

	ICF	void			RemoveFromAuxthread1Pool(const fastdelegate::FastDelegate0<>& delegate)
	{
		AuxPool_1_Protection_.Enter();

		xr_vector<fastdelegate::FastDelegate0<> >::iterator I = std::find(
			auxThreadPool_1_.begin(),
			auxThreadPool_1_.end(),
			delegate
		);

		if (I != auxThreadPool_1_.end())
			auxThreadPool_1_.erase(I);

		AuxPool_1_Protection_.Leave();
	}

	ICF	void			RemoveFromAuxthread5Pool(const fastdelegate::FastDelegate0<>& delegate)
	{
		AuxPool_5_Protection_.Enter();

		xr_vector<fastdelegate::FastDelegate0<> >::iterator I = std::find(
			auxThreadPool_5_.begin(),
			auxThreadPool_5_.end(),
			delegate
		);

		if (I != auxThreadPool_5_.end())
			auxThreadPool_5_.erase(I);

		AuxPool_5_Protection_.Leave();
	}
public:
	void xr_stdcall		on_idle();
private:
	void					message_loop();
};

extern		ENGINE_API		CRenderDevice		Device;

#define RDEVICE Device

extern		ENGINE_API		bool				g_bBenchmark;

typedef fastdelegate::FastDelegate0<bool>		LOADING_EVENT;
extern	ENGINE_API xr_list<LOADING_EVENT>		g_loading_events;
