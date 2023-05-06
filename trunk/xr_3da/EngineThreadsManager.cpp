#include "stdafx.h"

#include "CPU.h"

ENGINE_API bool check_thFreezing_ = true;

ENGINE_API Mutex GameSaveMutex;

AccessLock mustExitProtect_;
AccessLock freezeMustExitProtect_;

AccessLock	syncThreads_Freeze_and_Main_;

CTimer mainThreadTimer_, thread1Timer_, thread2Timer_, thread3Timer_, thread4Timer_, thread5Timer_, engineFreezeTimer_;

float auxThread1TimeDeffered_ = 0.f, auxThread2TimeDeffered_ = 0.f, auxThread3TimeDeffered_ = 0.f, auxThread4TimeDeffered_ = 0.f, auxThread5TimeDeffered_ = 0.f;
u32 auxThread1ObjectsDeffered_ = 0, auxThread2ObjectsDeffered_ = 0, auxThread3ObjectsDeffered_ = 0, auxThread4ObjectsDeffered_ = 0, auxThread5ObjectsDeffered_ = 0;

// to measure waiting count
bool thread1Ready_ = false;
bool thread2Ready_ = false;
bool thread3Ready_ = false;
bool thread4Ready_ = false;
bool thread5Ready_ = false;

AccessLock	bThreadReadyProtect_;

float ps_frames_per_sec = 144.f;

extern BOOL mtUseCustomAffinity_;

void CRenderDevice::SetAuxThreadsMustExit(bool val)
{
	mustExitProtect_.Enter();
	auxTreadsMustExit_ = val;
	mustExitProtect_.Leave();
}

bool CRenderDevice::IsAuxThreadsMustExit()
{
	mustExitProtect_.Enter();
	bool ress = auxTreadsMustExit_;
	mustExitProtect_.Leave();

	return ress;
}

//---------- Auxilary thread that executes delayed stuff while Main thread is buisy with Render (1)
void CRenderDevice::AuxThread_1(void* context)
{
	CRenderDevice& device = *static_cast<CRenderDevice*>(context);

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

	if (mtUseCustomAffinity_)
		SetThreadAffinityMask(GetCurrentThread(), CPU::GetSecondaryThreadBestAffinity());

	while (true)
	{
		device.auxThread_1_Allowed_.Wait(); // Wait until main thread allows aux thread 1 to execute

		if (device.IsAuxThreadsMustExit())
		{
			device.aux1ExitSync_.Set();

			return;
		}

		// we are ok to execute
		thread1Timer_.Start();

		thread1Ready_ = false;

		device.AuxPool_1_Protection_.Enter(); //Protect pool, while copying it to temporary one

		auxThread1ObjectsDeffered_ = device.auxThreadPool_1_.size() + device.seqFrameMT.R.size();

		// make a copy of corrent pool, so that the access to pool is not raced by threads
		device.AuxThreadPool_1_TempCopy_ = device.auxThreadPool_1_;
		device.auxThreadPool_1_.clear_not_free();

		device.AuxPool_1_Protection_.Leave();

		GameSaveMutex.lock();
		for (u32 pit = 0; pit < device.AuxThreadPool_1_TempCopy_.size(); pit++)
		{
			device.AuxThreadPool_1_TempCopy_[pit]();
		}

		device.seqFrameMT.Process(rp_Frame);

		GameSaveMutex.unlock();

		device.AuxThreadPool_1_TempCopy_.clear_not_free();

		bThreadReadyProtect_.Enter();
		auxThread1TimeDeffered_ = thread1Timer_.GetElapsed_sec() * 1000.f;
		thread1Ready_ = true;
		bThreadReadyProtect_.Leave();

		device.auxThread_1_Ready_.Set(); // tell main thread that aux thread 1 has finished its job
	}
}