#ifndef	RenderVisual_included
#define	RenderVisual_included
#pragma once

class IKinematics;
class IKinematicsAnimated;
class IParticleCustom;
struct vis_data;

class IRenderVisual
{
public:
	virtual ~IRenderVisual() {;}

	virtual vis_data&	_BCL	getVisData() = 0;
	virtual u32					getType() = 0;

#ifdef DEBUG
	virtual shared_str	_BCL	getDebugName() = 0;
#endif

	virtual	IKinematics*	_BCL	dcast_PKinematics			()				{ return nullptr;	}
	virtual	IKinematicsAnimated*	dcast_PKinematicsAnimated	()				{ return nullptr;	}
	virtual IParticleCustom*		dcast_ParticleCustom		()				{ return nullptr;	}
};

#endif	//	RenderVisual_included