#pragma once
#include "getvfunc.h"
#include "dt_recv2.h"
#include <Windows.h>

typedef float matrix3x4[3][4];
typedef unsigned int Panel;

#define	FL_ONGROUND (1<<0)
#define FL_DUCKING (1<<1)
#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1
#define BLU_TEAM 3
#define RED_TEAM 2

class CBaseEntity;

class IEntList
{
public:
	CBaseEntity* GetClientEntity(int entnum)
	{
		typedef CBaseEntity* (__thiscall* OriginalFn)(void*, int);
		return getvfunc<OriginalFn>(this, 3)(this, entnum);
	}
	CBaseEntity* GetClientEntityFromHandle(int hEnt)
	{
		typedef CBaseEntity* (__thiscall* OriginalFn)(void*, int);
		return getvfunc<OriginalFn>(this, 4)(this, hEnt);
	}
	int GetHighestEntityIndex(void)
	{
		typedef int(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 6)(this);
	}
};

class ClientClass
{
private:
	unsigned char _chPadding[8];
public:
	char* chName;
	RecvTable* Table;
	ClientClass* pNextClass;
	int iClassID;
};


class IClient
{
public:
	ClientClass * GetAllClasses(void)
	{
		typedef ClientClass* (__thiscall* OriginalFn)(void*); //Anything inside a VTable is a __thiscall unless it completly disregards the thisptr. You can also call them as __stdcalls, but you won't have access to the __thisptr.
		return getvfunc<OriginalFn>(this, 8)(this); //Return the pointer to the head CClientClass.
	}
};

#include "studio.h"
struct model_t;
class IMaterial;
class IModelInfo
{
public:
	const char* GetModelName(DWORD* model)
	{
		typedef const char* (__thiscall* OriginalFn)(void*, DWORD*);
		return getvfunc<OriginalFn>(this, 3)(this, model);
	}
	studiohdr_t* GetStudiomodel(const DWORD *mod)
	{
		typedef studiohdr_t* (__thiscall* GetStudiomodelFn)(void*, const DWORD*);
		return getvfunc< GetStudiomodelFn >(this, 28)(this, mod);
	}
	int	GetModelIndex(const char* name)
	{
		typedef int(__thiscall* GetModelIndexFn)(void*, const char*);
		return getvfunc< GetModelIndexFn >(this, 2)(this, name);
	}
	void GetModelMaterials(const model_t *model, int count, IMaterial **ppMaterials)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, const model_t *, int, IMaterial **);
		return getvfunc<OriginalFn>(this, 16)(this, model, count, ppMaterials);
	}
};

class CUserCmd
{
public:
	virtual ~CUserCmd() {}; //Destructor 0
	int command_number; //4
	int tick_count; //8
	Vector viewangles; //C
	float forwardmove; //18
	float sidemove; //1C
	float upmove; //20
	int	buttons; //24
	byte impulse; //28
	int weaponselect; //2C
	int weaponsubtype; //30
	int random_seed; //34
	short mousedx; //38
	short mousedy; //3A
	bool hasbeenpredicted; //3C;
	int unk; // 40
};

class INetChannel;
typedef struct player_info_s player_info_t;



//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CUDPSocket;
class CUtlBuffer;
class CNetPacket;
class CNetChannel;
class INetChannel;

// 0 == regular, 1 == file stream
enum
{
	FRAG_NORMAL_STREAM = 0,
	FRAG_FILE_STREAM,

	MAX_STREAMS
};

#define NET_MAX_DATAGRAM_PAYLOAD 1400
#define NET_MAX_PAYLOAD_BITS	11		// 2^NET_MAX_PALYLOAD_BITS > NET_MAX_PAYLOAD
#define DEFAULT_RATE 10000
#define SIGNON_TIME_OUT	120.0f
#define CONNECTION_PROBLEM_TIME	15.0f

#define MAX_RATE 50000
#define MIN_RATE 100

#define FRAGMENT_BITS		8
#define FRAGMENT_SIZE		(1<<FRAGMENT_BITS)
#define MAX_FILE_SIZE_BITS	26
#define MAX_FILE_SIZE		((1<<MAX_FILE_SIZE_BITS)-1)	// maximum transferable size is	64MB

#define NET_MAX_PAYLOAD			4000
#define NET_MAX_MESSAGE			4096
#define MIN_ROUTEABLE_PACKET	16
#define MAX_ROUTEABLE_PACKET	1400	// Ethernet 1518 - ( CRC + IP + UDP header)
#define UDP_HEADER_SIZE			28

// each channel packet has 1 byte of FLAG bits
#define PACKET_FLAG_RELIABLE			(1<<0)	// packet contains subchannel stream data
#define PACKET_FLAG_CHOKED				(1<<1)  // packet was choked by sender

// FIXME: Make an INetChannel
class CNetChannel
{
public:
	explicit CNetChannel();
	~CNetChannel();

	// Methods of INetChannel
	virtual int GetConnectionState();
	virtual const int &GetRemoteAddress(void) const;

	void		Setup(bool serverSide, const int *remote_address, CUDPSocket *sendSocket, char const *name, void *handler);
	void		Reset();
	void		Clear();
	void		Shutdown(const char *reason);

	CUDPSocket *GetSocket();

	void		SetDataRate(float rate);
	void		SetTimeout(float seconds);

	bool		StartProcessingPacket(CNetPacket *packet);
	bool		ProcessPacket(CNetPacket *packet);
	void		EndProcessingPacket(CNetPacket *packet);

	bool		CanSendPacket(void) const;
	void		SetChoked(void); // choke a packet
	bool		HasPendingReliableData(void);

	// Queues data for sending:

	// send a net message
	bool		AddNetMsg(int *msg, bool bForceReliable = false);

	// send a chunk of data
	bool		AddData(int &msg, bool bReliable = true);

	// Puts data onto the wire:

	int			SendDatagram(int *data); // Adds data to unreliable payload and then calls transmits the data
	bool		Transmit(bool onlyReliable = false); // send data from buffers (calls SendDataGram( NULL ) )

	bool		IsOverflowed(void) const;
	bool		IsTimedOut(void) const;
	bool		IsTimingOut() const;

	// Info:

	const char  *GetName(void) const;
	const char  *GetAddress(void) const;
	float		GetTimeConnected(void) const;
	float		GetTimeSinceLastReceived(void) const;
	int			GetDataRate(void) const;

	float		GetLatency(int flow) const;
	float		GetAvgLatency(int flow) const;
	float		GetAvgLoss(int flow) const;
	float		GetAvgData(int flow) const;
	float		GetAvgChoke(int flow) const;
	float		GetAvgPackets(int flow) const;
	int			GetTotalData(int flow) const;

	void SetConnectionState(int state);

	int			ProcessPacketHeader(int &buf);
	bool		ProcessControlMessage(int cmd, int &buf);
	bool		ProcessMessages(int &buf);

	int m_ConnectionState;

	// last send outgoing sequence number
	int			m_nOutSequenceNr;
	// last received incoming sequnec number
	int			m_nInSequenceNr;
	// last received acknowledge outgoing sequnce number
	int			m_nOutSequenceNrAck;

	// state of outgoing reliable data (0/1) flip flop used for loss detection
	int			m_nOutReliableState;
	// state of incoming reliable data
	int			m_nInReliableState;

	int			m_nChokedPackets;	//number of choked packets
	int			m_PacketDrop;

	// Reliable data buffer, send wich each packet (or put in waiting list)
	int	m_StreamReliable;
	byte		m_ReliableDataBuffer[8 * 1024];	// In SP, we don't need much reliable buffer, so save the memory (this is mostly for xbox).
	void* m_ReliableDataBufferMP;

	// unreliable message buffer, cleared wich each packet
	int	m_StreamUnreliable;
	byte		m_UnreliableDataBuffer[NET_MAX_DATAGRAM_PAYLOAD];

	// don't use any vars below this (only in net_ws.cpp)

	CUDPSocket	*m_pSocket;   // NS_SERVER or NS_CLIENT index, depending on channel.
	int			m_StreamSocket;	// TCP socket handle

	unsigned int m_MaxReliablePayloadSize;	// max size of reliable payload in a single packet	

	// Address this channel is talking to.
	int	remote_address;

	// For timeouts.  Time last message was received.
	float		last_received;
	// Time when channel was connected.
	float      connect_time;

	// Bandwidth choke
	// Bytes per second
	int			m_Rate;
	// If realtime > cleartime, free to send next packet
	float		m_fClearTime;

	float		m_Timeout;		// in seconds 

	char			m_Name[32];		// channel name

// packet history
	// netflow_t		m_DataFlow[ MAX_FLOWS ];  

	void			*m_MessageHandler;	// who registers and processes messages
};

class IEngine
{
public:
	void GetScreenSize(int& width, int& height)
	{
		typedef void(__thiscall* OriginalFn)(void*, int&, int&);
		return getvfunc<OriginalFn>(this, 5)(this, width, height);
	}
	bool GetPlayerInfo(int ent_num, player_info_t *pinfo)
	{
		typedef bool(__thiscall* OriginalFn)(void*, int, player_info_t *);
		return getvfunc<OriginalFn>(this, 8)(this, ent_num, pinfo);
	}
	bool Con_IsVisible(void)
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 11)(this);
	}
	int GetLocalPlayer(void)
	{
		typedef int(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 12)(this);
	}
	float Time(void)
	{
		typedef float(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 14)(this);
	}
	void GetViewAngles(Vector& va)
	{
		typedef void(__thiscall* OriginalFn)(void*, Vector& va);
		return getvfunc<OriginalFn>(this, 19)(this, va);
	}
	Vector GetViewAngles()
	{
		Vector out;
		GetViewAngles(out);
		return out;
	}
	void SetViewAngles(Vector& va)
	{
		typedef void(__thiscall* OriginalFn)(void*, Vector& va);
		return getvfunc<OriginalFn>(this, 20)(this, va);
	}
	int GetMaxClients(void)
	{
		typedef int(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 21)(this);
	}
	bool IsInGame(void)
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 26)(this);
	}
	bool IsConnected(void)
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 27)(this);
	}
	bool IsDrawingLoadingImage(void)
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 28)(this);
	}
	const matrix3x4& WorldToScreenMatrix(void)
	{
		typedef const matrix3x4& (__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 36)(this);
	}
	bool IsTakingScreenshot(void)
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 85)(this);
	}
	CNetChannel* GetNetChannelInfo(void)
	{
		typedef CNetChannel* (__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 72)(this);
	}
	const char* GetProductVersionString()
	{
		typedef const char* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 99)(this);
	}
	void ClientCmd_Unrestricted(const char* chCommandString)
	{
		typedef void(__thiscall* OriginalFn)(void*, const char *);
		return getvfunc<OriginalFn>(this, 106)(this, chCommandString);
	}
};

class ClientModeShared
{
public:
	bool IsChatPanelOutOfFocus(void)
	{
		typedef void*(__thiscall* OriginalFn)(void*);
		void* CHudChat = getvfunc<OriginalFn>(this, 19)(this);
		if (CHudChat)
		{
			return *(PFLOAT)((DWORD)CHudChat + 0xFC) == 0;
		}
		return false;
	}
};

class IGlobals
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float curtime;
	float frametime;
	int maxclients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
};

enum playercontrols
{
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_ATTACK2 = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_ALT1 = (1 << 14),
	IN_ALT2 = (1 << 15),
	IN_SCORE = (1 << 16),	// Used by client.dll for when scoreboard is held down
	IN_SPEED = (1 << 17),	// Player is holding the speed key
	IN_WALK = (1 << 18),	// Player holding walk key
	IN_ZOOM = (1 << 19),	// Zoom key for HUD zoom
	IN_WEAPON1 = (1 << 20),	// weapon defines these bits
	IN_WEAPON2 = (1 << 21),	// weapon defines these bits
	IN_BULLRUSH = (1 << 22),
};

enum tf_cond
{
	TFCond_Slowed = (1 << 0), //Toggled when a player is slowed down. 
	TFCond_Zoomed = (1 << 1), //Toggled when a player is zoomed. 
	TFCond_Disguising = (1 << 2), //Toggled when a Spy is disguising.  
	TFCond_Disguised = (1 << 3), //Toggled when a Spy is disguised. 
	TFCond_Cloaked = (1 << 4), //Toggled when a Spy is invisible. 
	TFCond_Ubercharged = (1 << 5), //Toggled when a player is ÜberCharged. 
	TFCond_TeleportedGlow = (1 << 6), //Toggled when someone leaves a teleporter and has glow beneath their feet. 
	TFCond_Taunting = (1 << 7), //Toggled when a player is taunting. 
	TFCond_UberchargeFading = (1 << 8), //Toggled when the ÜberCharge is fading. 
	TFCond_CloakFlicker = (1 << 9), //Toggled when a Spy is visible during cloak. 
	TFCond_Teleporting = (1 << 10), //Only activates for a brief second when the player is being teleported; not very useful. 
	TFCond_Kritzkrieged = (1 << 11), //Toggled when a player is being crit buffed by the KritzKrieg. 
	TFCond_TmpDamageBonus = (1 << 12), //Unknown what this is for. Name taken from the AlliedModders SDK. 
	TFCond_DeadRingered = (1 << 13), //Toggled when a player is taking reduced damage from the Deadringer. 
	TFCond_Bonked = (1 << 14), //Toggled when a player is under the effects of The Bonk! Atomic Punch. 
	TFCond_Stunned = (1 << 15), //Toggled when a player's speed is reduced from airblast or a Sandman ball. 
	TFCond_Buffed = (1 << 16), //Toggled when a player is within range of an activated Buff Banner. 
	TFCond_Charging = (1 << 17), //Toggled when a Demoman charges with the shield. 
	TFCond_DemoBuff = (1 << 18), //Toggled when a Demoman has heads from the Eyelander. 
	TFCond_CritCola = (1 << 19), //Toggled when the player is under the effect of The Crit-a-Cola. 
	TFCond_InHealRadius = (1 << 20), //Unused condition, name taken from AlliedModders SDK. 
	TFCond_Healing = (1 << 21), //Toggled when someone is being healed by a medic or a dispenser. 
	TFCond_OnFire = (1 << 22), //Toggled when a player is on fire. 
	TFCond_Overhealed = (1 << 23), //Toggled when a player has >100% health. 
	TFCond_Jarated = (1 << 24), //Toggled when a player is hit with a Sniper's Jarate. 
	TFCond_Bleeding = (1 << 25), //Toggled when a player is taking bleeding damage. 
	TFCond_DefenseBuffed = (1 << 26), //Toggled when a player is within range of an activated Battalion's Backup. 
	TFCond_Milked = (1 << 27), //Player was hit with a jar of Mad Milk. 
	TFCond_MegaHeal = (1 << 28), //Player is under the effect of Quick-Fix charge. 
	TFCond_RegenBuffed = (1 << 29), //Toggled when a player is within a Concheror's range. 
	TFCond_MarkedForDeath = (1 << 30), //Player is marked for death by a Fan O'War hit. Effects are similar to TFCond_Jarated. 
	TFCond_NoHealingDamageBuff = (1 << 31), //Unknown what this is used for.

	TFCondEx_SpeedBuffAlly = (1 << 0), //Toggled when a player gets hit with the disciplinary action. 
	TFCondEx_HalloweenCritCandy = (1 << 1), //Only for Scream Fortress event maps that drop crit candy. 
	TFCondEx_CritCanteen = (1 << 2), //Player is getting a crit boost from a MVM canteen.
	TFCondEx_CritDemoCharge = (1 << 3), //From demo's shield
	TFCondEx_CritHype = (1 << 4), //Soda Popper crits. 
	TFCondEx_CritOnFirstBlood = (1 << 5), //Arena first blood crit buff. 
	TFCondEx_CritOnWin = (1 << 6), //End of round crits. 
	TFCondEx_CritOnFlagCapture = (1 << 7), //CTF intelligence capture crits. 
	TFCondEx_CritOnKill = (1 << 8), //Unknown what this is for. 
	TFCondEx_RestrictToMelee = (1 << 9), //Unknown what this is for. 
	TFCondEx_DefenseBuffNoCritBlock = (1 << 10), //MvM Buff.
	TFCondEx_Reprogrammed = (1 << 11), //MvM Bot has been reprogrammed.
	TFCondEx_PyroCrits = (1 << 12), //Player is getting crits from the Mmmph charge. 
	TFCondEx_PyroHeal = (1 << 13), //Player is being healed from the Mmmph charge. 
	TFCondEx_FocusBuff = (1 << 14), //Player is getting a focus buff.
	TFCondEx_DisguisedRemoved = (1 << 15), //Disguised remove from a bot.
	TFCondEx_MarkedForDeathSilent = (1 << 16), //Player is under the effects of the Escape Plan/Equalizer or GRU.
	TFCondEx_DisguisedAsDispenser = (1 << 17), //Bot is disguised as dispenser.
	TFCondEx_Sapped = (1 << 18), //MvM bot is being sapped.
	TFCondEx_UberchargedHidden = (1 << 19), //MvM Related
	TFCondEx_UberchargedCanteen = (1 << 20), //Player is receiving ÜberCharge from a canteen.
	TFCondEx_HalloweenBombHead = (1 << 21), //Player has a bomb on their head from Merasmus.
	TFCondEx_HalloweenThriller = (1 << 22), //Players are forced to dance from Merasmus.
	TFCondEx_BulletCharge = (1 << 26), //Player is receiving 75% reduced damage from bullets.
	TFCondEx_ExplosiveCharge = (1 << 27), //Player is receiving 75% reduced damage from explosives.
	TFCondEx_FireCharge = (1 << 28), //Player is receiving 75% reduced damage from fire.
	TFCondEx_BulletResistance = (1 << 29), //Player is receiving 10% reduced damage from bullets.
	TFCondEx_ExplosiveResistance = (1 << 30), //Player is receiving 10% reduced damage from explosives.
	TFCondEx_FireResistance = (1 << 31), //Player is receiving 10% reduced damage from fire.

	TFCondEx2_Stealthed = (1 << 0),
	TFCondEx2_MedigunDebuff = (1 << 1),
	TFCondEx2_StealthedUserBuffFade = (1 << 2),
	TFCondEx2_BulletImmune = (1 << 3),
	TFCondEx2_BlastImmune = (1 << 4),
	TFCondEx2_FireImmune = (1 << 5),
	TFCondEx2_PreventDeath = (1 << 6),
	TFCondEx2_MVMBotRadiowave = (1 << 7),
	TFCondEx2_HalloweenSpeedBoost = (1 << 8), //Wheel has granted player speed boost.
	TFCondEx2_HalloweenQuickHeal = (1 << 9), //Wheel has granted player quick heal.
	TFCondEx2_HalloweenGiant = (1 << 10), //Wheel has granted player giant mode.
	TFCondEx2_HalloweenTiny = (1 << 11), //Wheel has granted player tiny mode.
	TFCondEx2_HalloweenInHell = (1 << 12), //Wheel has granted player in hell mode.
	TFCondEx2_HalloweenGhostMode = (1 << 13), //Wheel has granted player ghost mode.
	TFCondEx2_Parachute = (1 << 16), //Player has deployed the BASE Jumper.
	TFCondEx2_BlastJumping = (1 << 17), //Player has sticky or rocket jumped.

	TFCond_MiniCrits = (TFCond_Buffed | TFCond_CritCola),
	TFCond_IgnoreStates = (TFCond_Ubercharged | TFCond_Bonked),
	TFCondEx_IgnoreStates = (TFCondEx_PyroHeal)
};

enum tf_classes
{
	TF2_Scout = 1,
	TF2_Soldier = 3,
	TF2_Pyro = 7,
	TF2_Demoman = 4,
	TF2_Heavy = 6,
	TF2_Engineer = 9,
	TF2_Medic = 5,
	TF2_Sniper = 2,
	TF2_Spy = 8,
};

enum source_lifestates : byte
{
	LIFE_ALIVE,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};

enum scoutweapons
{
	//Primary
	WPN_Scattergun = 13,
	WPN_NewScattergun = 200,
	WPN_FAN = 45,
	WPN_Shortstop = 220,
	WPN_SodaPopper = 448,
	WPN_BabyFaceBlaster = 772,
	WPN_BotScattergunS = 799,
	WPN_BotScattergunG = 808,
	WPN_BotScattergunR = 888,
	WPN_BotScattergunB = 897,
	WPN_BotScattergunC = 906,
	WPN_BotScattergunD = 915,
	WPN_BotScattergunES = 964,
	WPN_BotScattergunEG = 973,
	WPN_FestiveFaN = 1078,
	WPN_BackScatter = 1103,
	//Secondary
	WPN_ScoutPistol = 23,
	WPN_NewPistol = 209,
	WPN_Bonk = 46,
	WPN_CritCola = 163,
	WPN_Lugermorph = 160,
	WPN_Milk = 222,
	WPN_Lugermorph2 = 294,
	WPN_Winger = 449,
	WPN_PocketPistol = 773,
	WPN_MutatedMilk = 1121,
	WPN_CAPPER = 30666,
	//Melee
	WPN_Bat = 0,
	WPN_NewBat = 190,
	WPN_Sandman = 44,
	WPN_Fish = 221,
	WPN_Cane = 317,
	WPN_BostonBasher = 325,
	WPN_SunStick = 349,
	WPN_FanOWar = 355,
	WPN_RuneBlade = 452,
	WPN_Saxxy = 423,
	WPN_Atomizer = 450,
	WPN_ConscientiousObjector = 474,
	WPN_UnarmedCombat = 572,
	WPN_WrapAssassin = 648,
	WPN_FestiveBat = 660,
	WPN_FestiveScattergun = 669,
	WPN_FlyingGuillotine1 = 812,
	WPN_FlyingGuillotine2 = 833,
	WPN_FreedomStaff = 880,
	WPN_BatOuttaHell = 939,
	WPN_MemoryMaker = 954,
	WPN_FestiveFish = 999,
	WPN_TheHamShank = 1013,
	WPN_CrossingGuard = 1127,
	WPN_NecroSmasher = 1123,
	WPN_Batsaber = 30667,
};
enum soldierweapons
{
	//Primary
	WPN_RocketLauncher = 18,
	WPN_NewRocketLauncher = 205,
	WPN_DirectHit = 127,
	WPN_BlackBox = 228,
	WPN_RocketJumper = 237,
	WPN_LibertyLauncher = 414,
	WPN_CowMangler = 441,
	WPN_Original = 513,
	WPN_FestiveRocketLauncher = 658,
	WPN_BeggersBazooka = 730,
	WPN_BotRocketlauncherS = 800,
	WPN_BotRocketlauncherG = 809,
	WPN_BotRocketlauncherR = 889,
	WPN_BotRocketlauncherB = 898,
	WPN_BotRocketlauncherC = 907,
	WPN_BotRocketlauncherD = 916,
	WPN_BotRocketlauncherES = 965,
	WPN_BotRocketlauncherEG = 974,
	WPN_FestiveBlackbox = 1085,
	WPN_Airstrike = 1104,
	//Secondary
	WPN_SoldierShotgun = 10,
	WPN_NewShotgun = 199,
	WPN_BuffBanner = 129,
	WPN_BattalionBackup = 226,
	WPN_Concheror = 354,
	WPN_ReserveShooter = 415,
	WPN_RighteousBison = 442,
	WPN_FestiveBuffBanner = 1001,
	WPN_PanicAttack = 1153,
	//Melee
	WPN_Shovel = 6,
	WPN_NewShovel = 196,
	WPN_Equalizer = 128,
	WPN_PainTrain = 154,
	WPN_Katana = 357,
	WPN_MarketGardener = 416,
	WPN_DisciplinaryAction = 447,
	WPN_EscapePlan = 775,
};
enum pyroweapons
{
	//Primary
	WPN_Flamethrower = 21,
	WPN_NewFlamethrower = 208,
	WPN_Backburner = 40,
	WPN_Degreaser = 215,
	WPN_FestiveFlamethrower = 659,
	WPN_Phlogistinator = 594,
	WPN_Rainblower = 741,
	WPN_BotFlamethrowerS = 798,
	WPN_BotFlamethrowerG = 807,
	WPN_BotFlamethrowerR = 887,
	WPN_BotFlamethrowerB = 896,
	WPN_BotFlamethrowerC = 905,
	WPN_BotFlamethrowerD = 914,
	WPN_BotFlamethrowerES = 963,
	WPN_BotFlamethrowerEG = 972,
	WPN_FestiveBackburner = 1146,
	//Secondary
	WPN_PyroShotgun = 12,
	WPN_Flaregun = 39,
	WPN_Detonator = 351,
	WPN_ManMelter = 595,
	WPN_ScorchShot = 740,
	WPN_FestiveFlaregun = 1081,
	//Melee
	WPN_Fireaxe = 2,
	WPN_NewAxe = 192,
	WPN_Axtingusher = 38,
	WPN_HomeWrecker = 153,
	WPN_PowerJack = 214,
	WPN_Backscratcher = 326,
	WPN_VolcanoFragment = 348,
	WPN_Maul = 466,
	WPN_Mailbox = 457,
	WPN_ThirdDegree = 593,
	WPN_Lollychop = 739,
	WPN_NeonAnnihilator1 = 813,
	WPN_NeonAnnihilator2 = 834,
	WPN_FestiveAxtingisher = 1000,
};
enum demomanweapons
{
	//Primary
	WPN_GrenadeLauncher = 19,
	WPN_NewGrenadeLauncher = 206,
	WPN_LochNLoad = 308,
	WPN_LoooseCannon = 996,
	WPN_FestiveGrenadeLauncher = 1007,
	WPN_IronBomber = 1151,
	//Secondary
	WPN_StickyLauncher = 20,
	WPN_NewStickyLauncher = 207,
	WPN_ScottishResistance = 130,
	WPN_StickyJumper = 265,
	WPN_FestiveStickyLauncher = 661,
	WPN_BotStickyS = 797,
	WPN_BotStickyG = 806,
	WPN_BotStickyR = 886,
	WPN_BotStickyB = 895,
	WPN_BotStickyC = 904,
	WPN_BotStickyD = 913,
	WPN_BotStickyES = 962,
	WPN_BotStickyEG = 971,
	WPN_QuickieBombLauncher = 1150,
	//Melee
	WPN_Bottle = 1,
	WPN_NewBottle = 191,
	WPN_Sword = 132,
	WPN_ScottsSkullctter = 172,
	WPN_Fryingpan = 264,
	WPN_Headless = 266,
	WPN_Ullapool = 307,
	WPN_Claidheamhmor = 327,
	WPN_PersainPersuader = 404,
	WPN_Golfclub = 482,
	WPN_ScottishHandshake = 609,
	WPN_GoldenFryingPan = 1071,
	WPN_FestiveEyelander = 1082,
};
enum heavyweapons
{
	//Primary
	WPN_Minigun = 15,
	WPN_NewMinigun = 202,
	WPN_Natascha = 41,
	WPN_IronCurtain = 298,
	WPN_BrassBeast = 312,
	WPN_Tomislav = 424,
	WPN_FestiveMinigun = 654,
	WPN_HuoLongHeatmaker1 = 811,
	WPN_HuoLongHeatmaker2 = 832,
	WPN_BotMinigunS = 793,
	WPN_BotMinigunG = 802,
	WPN_BotMinigunR = 882,
	WPN_BotMinigunB = 891,
	WPN_BotMinigunC = 900,
	WPN_BotMinigunD = 909,
	WPN_Deflector = 850,
	WPN_BotMinigunES = 958,
	WPN_BotMinigunEG = 967,
	//Secondary
	WPN_HeavyShotgun = 11,
	WPN_Sandvich = 42,
	WPN_CandyBar = 159,
	WPN_Steak = 311,
	WPN_Fishcake = 433,
	WPN_FamilyBuisness = 425, //GIVE 'EM THE BUISSNESS, TONY!
	WPN_RobotSandvich = 863,
	WPN_FestiveSandvich = 1002,
	//Melee
	WPN_Fists = 5,
	WPN_NewFists = 195,
	WPN_KGB = 43,
	WPN_GRU = 239,
	WPN_WarriorSpirit = 310,
	WPN_FistsOfSteel = 331,
	WPN_EvictionNotice = 426,
	WPN_ApocoFists = 587,
	WPN_HolidayPunch = 656,
	WPN_FestiveGRU = 1084,
	WPN_BreadBite = 1100,
};
enum engineerweapons
{
	//Primary
	WPN_EngineerShotgun = 9,
	WPN_FrontierJustice = 141,
	WPN_Widowmaker = 527,
	WPN_Pomson = 588,
	WPN_RescueRanger = 997,
	WPN_FestiveFrontierJustice = 1004,
	//Secondary
	WPN_EngineerPistol = 22,
	WPN_Wrangler = 140,
	WPN_ShortCircut = 528,
	WPN_FestiveWrangler = 1086,
	WPN_GeigerCounter = 30668,
	//Melee
	WPN_Wrench = 7,
	WPN_NewWrench = 197,
	WPN_Goldenwrench = 169,
	WPN_SouthernHospitality = 155,
	WPN_Gunslinger = 142,
	WPN_Jag = 329,
	WPN_FestiveWrench = 662,
	WPN_EurekaEffect = 589,
	WPN_BotWrenchS = 795,
	WPN_BotWrenchG = 804,
	WPN_BotWrenchR = 884,
	WPN_BotWrenchB = 893,
	WPN_BotWrenchC = 902,
	WPN_BotWrenchD = 911,
	WPN_BotWrenchES = 960,
	WPN_BotWrenchEG = 969,
	//Misc
	WPN_Builder = 25,
	WPN_Builder2 = 737,
	WPN_Destructor = 26,
	WPN_Toolbox = 28,
};
enum medicweapons
{
	//Primary
	WPN_SyringeGun = 17,
	WPN_NewSyringeGun = 204,
	WPN_Blutsauger = 36,
	WPN_Crossbow = 305,
	WPN_Overdose = 412,
	WPN_FestiveCrossbow = 1079,
	//Secondary
	WPN_Medigun = 29,
	WPN_NewMedigun = 211,
	WPN_Kritzkrieg = 35,
	WPN_QuickFix = 411,
	WPN_FestiveMedigun = 663,
	WPN_MedigunS = 796,
	WPN_MedigunG = 805,
	WPN_MedigunR = 885,
	WPN_MedigunB = 894,
	WPN_MedigunC = 903,
	WPN_MedigunD = 912,
	WPN_Vaccinator = 998,
	WPN_MedigunES = 961,
	WPN_MedigunEG = 970,
	//Melee
	WPN_Bonesaw = 8,
	WPN_NewBonesaw = 198,
	WPN_Ubersaw = 37,
	WPN_Vitasaw = 173,
	WPN_Amputator = 304,
	WPN_Solemnvow = 413,
	WPN_FestiveUbersaw = 1003,
};
enum sniperweapons
{
	//Primary
	WPN_SniperRifle = 14,
	WPN_NewSniperRifle = 201,
	WPN_Huntsman = 56,
	WPN_SydneySleeper = 230,
	WPN_Bazaarbargain = 402,
	WPN_Machina = 526,
	WPN_FestiveSniperRifle = 664,
	WPN_HitmanHeatmaker = 752,
	WPN_BotSniperRifleS = 792,
	WPN_BotSniperRifleG = 801,
	WPN_BotSniperRifleR = 881,
	WPN_BotSniperRifleB = 890,
	WPN_BotSniperRifleC = 899,
	WPN_BotSniperRifleD = 908,
	WPN_AWP = 851,
	WPN_BotSniperRifleES = 957,
	WPN_BotSniperRifleEG = 966,
	WPN_FestiveHuntsman = 1005,
	WPN_CompoundBow = 1092,
	WPN_ClassicSniperRifle = 1098,
	WPN_ShootingStar = 30665,
	//Secondary
	WPN_SMG = 16,
	WPN_NewSMG = 203,
	WPN_Jarate = 58,
	WPN_DarwinDangerShield = 231,
	WPN_CleanersCarbine = 751,
	WPN_FestiveJarate = 1083,
	WPN_SelfAwareBeautyMark = 1105,
	//Melee
	WPN_Kukri = 3,
	WPN_NewKukri = 193,
	WPN_TribalmansShiv = 171,
	WPN_Bushwacka = 232,
	WPN_Shahanshah = 401,
};
enum spyweapons
{
	//Primary
	WPN_Revolver = 24,
	WPN_NewRevolver = 210,
	WPN_Ambassador = 61,
	WPN_BigKill = 161,
	WPN_Letranger = 224,
	WPN_Enforcer = 460,
	WPN_Diamondback = 525,
	WPN_FestiveAmbassador = 1006,
	//Melee
	WPN_Knife = 4,
	WPN_NewKnife = 194,
	WPN_EternalReward = 225,
	WPN_DisguiseKit = 27,
	WPN_Kunai = 356,
	WPN_BigEarner = 461,
	WPN_WangaPrick = 574,
	WPN_SharpDresser = 638,
	WPN_Spycicle = 649,
	WPN_FestiveKnife = 665,
	WPN_BlackRose = 727,
	WPN_BotKnifeS = 794,
	WPN_BotKnifeG = 803,
	WPN_BotKnifeR = 883,
	WPN_BotKnifeB = 892,
	WPN_BotKnifeC = 901,
	WPN_BotKnifeD = 910,
	WPN_BotKnifeES = 959,
	WPN_BotKnifeEG = 968,
	//Watches
	WPN_InivisWatch = 30,
	WPN_DeadRinger = 59,
	WPN_CloakAndDagger = 60,
	WPN_TimePiece = 297,
	//Sapper
	WPN_Sapper = 735,
	WPN_Sapper2 = 736,
	WPN_RedTape1 = 810,
	WPN_RedTape2 = 831,
	WPN_ApSap = 933,
	WPN_FestiveSapper = 1080,
	WPN_SnackAttack = 1102,
};