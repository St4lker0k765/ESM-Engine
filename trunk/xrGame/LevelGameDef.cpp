#include "stdafx.h"
#include "LevelGameDef.h"

xr_token rpoint_type[]={
	{ "Actor Spawn",	rptActorSpawn	},
	{ "Artefact Spawn",	rptArtefactSpawn	},
	{ "TeamBase Particle",	rptTeamBaseParticle	},
	{ nullptr,				0	}
};

xr_token rpoint_game_type[]={
	{ "Any game",		rpgtGameAny				},
	{ "Deathmatch",		rpgtGameDeathmatch		},
	{ "TeamDeathmatch",	rpgtGameTeamDeathmatch	},
	{ "ArtefactHunt",	rpgtGameArtefactHunt	},
	{ nullptr,				0	}
};
 