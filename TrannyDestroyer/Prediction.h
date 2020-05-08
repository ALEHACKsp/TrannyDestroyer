#include "Engine.h"
#include "Entities.h"
#include "Trace.h"

class CMoveData
{
public:
	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;

	int m_nPlayerHandle; // edict index on server, client entity handle on client

	int    m_nImpulseCommand; // Impulse command issued.
	Vector m_vecViewAngles; // Command view angles (local space)
	Vector m_vecAbsViewAngles; // Command view angles (world space)
	int    m_nButtons; // Attack buttons.
	int    m_nOldButtons; // From host_client->oldbuttons;
	float  m_flForwardMove;
	float  m_flSideMove;
	float  m_flUpMove;

	float m_flMaxSpeed;
	float m_flClientMaxSpeed;

	// Variables from the player edict (sv_player) or entvars on the client.
	// These are copied in here before calling and copied out after calling.
	Vector m_vecVelocity; // edict::velocity		// Current movement direction.
	Vector m_vecAngles; // edict::angles
	Vector m_vecOldAngles;

	// Output only
	float  m_outStepHeight; // how much you climbed this move
	Vector m_outWishVel; // This is where you tried
	Vector m_outJumpVel; // This is your jump velocity

	// Movement constraints	(radius 0 means no constraint)
	Vector m_vecConstraintCenter;
	float  m_flConstraintRadius;
	float  m_flConstraintWidth;
	float  m_flConstraintSpeedFactor;

	void          SetAbsOrigin(const Vector &vec);
	const Vector &GetAbsOrigin() const;

	// private:
	Vector m_vecAbsOrigin; // edict::origin
};

class IGameMovement
{
public:
	virtual ~IGameMovement(void)
	{
	}

	// Process the current movement command
	virtual void ProcessMovement(CBaseEntity *pPlayer, CMoveData *pMove) = 0;
	virtual void StartTrackPredictionErrors(CBaseEntity *pPlayer) = 0;
	virtual void FinishTrackPredictionErrors(CBaseEntity *pPlayer) = 0;
	virtual void DiffPrint(char const *fmt, ...) = 0;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual Vector GetPlayerMins(bool ducked) const = 0;
	virtual Vector GetPlayerMaxs(bool ducked) const = 0;
	virtual Vector GetPlayerViewOffset(bool ducked) const = 0;
};

class CGameMovement : public IGameMovement
{
public:
	// DECLARE_CLASS_NOBASE( CGameMovement );

	CGameMovement(void) = delete;
	virtual ~CGameMovement(void);

	void ProcessMovement(CBaseEntity *pPlayer, CMoveData *pMove)
	{
		return getvfunc<void(__thiscall *)(void *, CBaseEntity *, CMoveData *)>(this, 1)(this, player, pMove);
	}

	virtual void   StartTrackPredictionErrors(CBaseEntity *pPlayer);
	virtual void   FinishTrackPredictionErrors(CBaseEntity *pPlayer);
	virtual void   DiffPrint(char const *fmt, ...);
	virtual Vector GetPlayerMins(bool ducked) const;
	virtual Vector GetPlayerMaxs(bool ducked) const;
	virtual Vector GetPlayerViewOffset(bool ducked) const;

	// For sanity checking getting stuck on CMoveData::SetAbsOrigin
	virtual void TracePlayerBBox(const Vector &start, const Vector &end, unsigned int fMask, int collisionGroup, trace_t &pm);

	// allows derived classes to exclude entities from trace
	virtual void TryTouchGround(const Vector &start, const Vector &end, const Vector &mins, const Vector &maxs, unsigned int fMask, int collisionGroup, trace_t &pm);

#define BRUSH_ONLY true
	virtual unsigned int PlayerSolidMask(bool brushOnly = false); ///< returns the solid mask for the given player, so bots can have a more-restrictive set
	CBaseEntity *        player;
	CMoveData *          GetMoveData()
	{
		return mv;
	}

	// Input/Output for this movement
	CMoveData *mv;

	int   m_nOldWaterLevel;
	float m_flWaterEntryTime;
	int   m_nOnLadder;

	Vector m_vecForward;
	Vector m_vecRight;
	Vector m_vecUp;

	// Does most of the player movement logic.
	// Returns with origin, angles, and velocity modified in place.
	// were contacted during the move.
	virtual void PlayerMove(void);

	// Set ground data, etc.
	void FinishMove(void);

	virtual float CalcRoll(const Vector &angles, const Vector &velocity, float rollangle, float rollspeed);

	virtual void DecayPunchAngle(void);

	virtual void CheckWaterJump(void);

	virtual void WaterMove(void);

	void WaterJump(void);

	// Handles both ground friction and water friction
	void Friction(void);

	virtual void AirAccelerate(Vector &wishdir, float wishspeed, float accel);

	virtual void  AirMove(void);
	virtual float GetAirSpeedCap(void)
	{
		return 30.f;
	}

	virtual bool CanAccelerate();
	virtual void Accelerate(Vector &wishdir, float wishspeed, float accel);

	// Only used by players.  Moves along the ground when player is a MOVETYPE_WALK.
	virtual void WalkMove(void);

	// Try to keep a walking player on the ground when running down slopes etc
	void StayOnGround(void);

	// Handle MOVETYPE_WALK.
	virtual void FullWalkMove();

	// allow overridden versions to respond to jumping
	virtual void OnJump(float fImpulse)
	{
	}
	virtual void OnLand(float fVelocity)
	{
	}

	// Implement this if you want to know when the player collides during OnPlayerMove
	virtual void OnTryPlayerMoveCollision(trace_t &tr)
	{
	}

	virtual Vector GetPlayerMins(void) const; // uses local player
	virtual Vector GetPlayerMaxs(void) const; // uses local player

	typedef enum {
		GROUND = 0,
		STUCK,
		LADDER
	} IntervalType_t;

	virtual int GetCheckInterval(IntervalType_t type);

	// Useful for things that happen periodically. This lets things happen on the specified interval, but
	// spaces the events onto different frames for different players so they don't all hit their spikes
	// simultaneously.
	bool CheckInterval(IntervalType_t type);

	// Decompoosed gravity
	void StartGravity(void);
	void FinishGravity(void);

	// Apply normal ( undecomposed ) gravity
	void AddGravity(void);

	// Handle movement in noclip mode.
	void FullNoClipMove(float factor, float maxacceleration);

	// Returns true if he started a jump (ie: should he play the jump animation)?
	virtual bool CheckJumpButton(void); // Overridden by each game.

	// Dead player flying through air., e.g.
	virtual void FullTossMove(void);

	// Player is a Observer chasing another player
	void FullObserverMove(void);

	// Handle movement when in MOVETYPE_LADDER mode.
	virtual void FullLadderMove();

	// The basic solid body movement clip that slides along multiple planes
	virtual int TryPlayerMove(Vector *pFirstDest = NULL, trace_t *pFirstTrace = NULL);

	virtual bool  LadderMove(void);
	virtual bool  OnLadder(trace_t &trace);
	virtual float LadderDistance(void) const
	{
		return 2.0f;
	} ///< Returns the distance a player can be from a ladder and still attach to it
	virtual unsigned int LadderMask(void) const
	{
		return MASK_PLAYERSOLID;
	}
	virtual float ClimbSpeed(void) const
	{
		return 200.f;
	}
	virtual float LadderLateralMultiplier(void) const
	{
		return 1.0f;
	}

	// See if the player has a bogus velocity value.
	void CheckVelocity(void);

	// Does not change the entities velocity at all
	void PushEntity(Vector &push, trace_t *pTrace);

	// Slide off of the impacting object
	// returns the blocked flags:
	// 0x01 == floor
	// 0x02 == step / wall
	int ClipVelocity(Vector &in, Vector &normal, Vector &out, float overbounce);

	// If pmove.origin is in a solid position,
	// try nudging slightly on all axis to
	// allow for the cut precision of the net coordinates
	virtual int CheckStuck(void);

	// Check if the point is in water.
	// Sets refWaterLevel and refWaterType appropriately.
	// If in water, applies current to baseVelocity, and returns true.
	virtual bool CheckWater(void);

	// Determine if player is in water, on ground, etc.
	virtual void CategorizePosition(void);

	virtual void CheckParameters(void);

	virtual void ReduceTimers(void);

	virtual void CheckFalling(void);

	virtual void PlayerRoughLandingEffects(float fvol);

	void PlayerWaterSounds(void);

	void ResetGetPointContentsCache();
	int  GetPointContentsCached(const Vector &point, int slot);

	// Ducking
	virtual void Duck(void);
	virtual void HandleDuckingSpeedCrop();
	virtual void FinishUnDuck(void);
	virtual void FinishDuck(void);
	virtual bool CanUnduck();
	void         UpdateDuckJumpEyeOffset(void);
	bool         CanUnDuckJump(trace_t &trace);
	void         StartUnDuckJump(void);
	void         FinishUnDuckJump(trace_t &trace);
	void         SetDuckedEyeOffset(float duckFraction);
	void         FixPlayerCrouchStuck(bool moveup);

	float SplineFraction(float value, float scale);

	void CategorizeGroundSurface(trace_t &pm);

	bool InWater(void);

	// Commander view movement
	void IsometricMove(void);

	// Traces the player bbox as it is swept from start to end
	virtual void unkn();//virtual CBaseHandle TestPlayerPosition(const Vector &pos, int collisionGroup, trace_t &pm);

	// Checks to see if we should actually jump
	void PlaySwimSound();

	bool IsDead(void) const;

	// Figures out how the constraint should slow us down
	float ComputeConstraintSpeedFactor(void);

	virtual void SetGroundEntity(trace_t *pm);

	virtual void StepMove(Vector &vecDestination, trace_t &trace);

	// when we step on ground that's too steep, search to see if there's any ground nearby that isn't too steep
	void TryTouchGroundInQuadrants(const Vector &start, const Vector &end, unsigned int fMask, int collisionGroup, trace_t &pm);

protected:
	// Performs the collision resolution for fliers.
	void PerformFlyCollisionResolution(trace_t &pm, Vector &move);

	virtual bool GameHasLadders() const;

	enum
	{
		// eyes, waist, feet points (since they are all deterministic
		MAX_PC_CACHE_SLOTS = 3,
	};

	// Cache used to remove redundant calls to GetPointContents().
	int    m_CachedGetPointContents[33][3];
	Vector m_CachedGetPointContentsPoint[33][3];

	Vector m_vecProximityMins; // Used to be globals in sv_user.cpp.
	Vector m_vecProximityMaxs;

	float m_fFrameTime;

	// private:
	int m_iSpeedCropped;

	float m_flStuckCheckTime[33 + 1][2]; // Last time we did a full test

// special function for teleport-with-duck for episodic
#ifdef HL2_EPISODIC
public:
	void ForceDuck(void);

#endif
};

class CPrediction
{
public:
	void SetupMove(CBaseEntity *player, CUserCmd *ucmd, IMoveHelper *helper, CMoveData *move)
	{
		return getvfunc<void(__thiscall *)(void *, CBaseEntity *, CUserCmd *, IMoveHelper *, CMoveData *)>(this, 18)(this, player, ucmd, helper, move);
	}
	void FinishMove(CBaseEntity *player, CUserCmd *ucmd, CMoveData *move)
	{
		return getvfunc<void(__thiscall *)(void *, CBaseEntity *, CUserCmd *, CMoveData *)>(this, 19)(this, player, ucmd, move);
	}
	void RunCommand(CBaseEntity *pBaseEntity, CUserCmd *pCommand, IMoveHelper *moveHelper)
	{
		return getvfunc<void(__thiscall *)(void *, CBaseEntity *, CUserCmd *, IMoveHelper*)>(this, 17)(this, pBaseEntity, pCommand, moveHelper);
	}
};

class IMoveHelper
{
public:
	// Call this to set the singleton
	static IMoveHelper *GetSingleton()
	{
		return sm_pSingleton;
	}

	// Methods associated with a particular entity
	virtual void unkn();

	// Adds the trace result to touch list, if contact is not already in list.
	virtual void ResetTouchList(void) = 0;
	virtual bool AddToTouched(const CGameTrace &tr, const Vector &impactvelocity) = 0;
	virtual void ProcessImpacts(void) = 0;

	// Numbered line printf
	virtual void Con_NPrintf(int idx, char const *fmt, ...) = 0;

	// These have separate server vs client impementations
	virtual void StartSound(const Vector &origin, int channel, char const *sample, float volume, int soundlevel, int fFlags, int pitch) = 0;
	virtual void StartSound(const Vector &origin, const char *soundname) = 0;
	virtual void PlaybackEventFull(int flags, int clientindex, unsigned short eventindex, float delay, Vector &origin, Vector &angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) = 0;

	// Apply falling damage to m_pHostPlayer based on m_pHostPlayer->m_flFallVelocity.
	virtual bool PlayerFallingDamage(void) = 0;

	// Apply falling damage to m_pHostPlayer based on m_pHostPlayer->m_flFallVelocity.
	virtual void PlayerSetAnimation(int playerAnim) = 0;

	virtual /*IPhysicsSurfaceProps **/ void *GetSurfaceProps(void) = 0;

	virtual bool IsWorldEntity(const void*  &handle) = 0;

protected:
	// Inherited classes can call this to set the singleton
	static void SetSingleton(IMoveHelper *pMoveHelper)
	{
		sm_pSingleton = pMoveHelper;
	}

	// Clients shouldn't call delete directly
	virtual ~IMoveHelper()
	{
	}

	// The global instance
	static IMoveHelper *sm_pSingleton;
};