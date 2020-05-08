#pragma once

#include "Vector.h"

// Huge sdk dump for this class
// as a result most of this file is left with the original variable, function and class names
// to make reference to the original sdk much easier

#define STEAM_KEYSIZE 2048

#define MAX_AREA_STATE_BYTES 32
#define MAX_AREA_PORTAL_STATE_BYTES 24
#define MAX_PLAYER_NAME_LENGTH 32
#define MAX_CUSTOM_FILES 4 // max 4 files
#define MAX_DEMOS 32
#define MAX_DEMONAME 32

// Resource counts;
#define MAX_MODEL_INDEX_BITS 10 // sent as a short
#define MAX_MODELS (1 << MAX_MODEL_INDEX_BITS)

#define MAX_GENERIC_INDEX_BITS 9
#define MAX_GENERIC (1 << MAX_GENERIC_INDEX_BITS)
#define MAX_DECAL_INDEX_BITS 9
#define MAX_BASE_DECALS (1 << MAX_DECAL_INDEX_BITS)

#define MAX_SOUND_INDEX_BITS 14

struct model_t;
class ClientClass;

// dummy pointers for classes we dont want or need
// TODO impl if nesercary
typedef int INetworkStringTable, PackedEntity, C_ServerClassInfo, CNetworkStringTableContainer, CSfxTable, WaitForResourcesHandle_t;

class CEntityInfo
{
public:
	CEntityInfo()
	{
		m_nOldEntity = -1;
		m_nNewEntity = -1;
		m_nHeaderBase = -1;
	}
	virtual ~CEntityInfo() {};

	bool          m_bAsDelta;
	void* *m_pFrom;
	void* *m_pTo;

	void* m_UpdateType;

	int m_nOldEntity; // current entity index in m_pFrom
	int m_nNewEntity; // current entity index in m_pTo

	int m_nHeaderBase;
	int m_nHeaderCount;
};

// PostDataUpdate calls are stored in a list until all ents have been updated.
class CPostDataUpdateCall
{
public:
	int              m_iEnt;
	int m_UpdateType;
};

class CEntityReadInfo : public CEntityInfo
{

public:
	CEntityReadInfo()
	{
		m_nPostDataUpdateCalls = 0;
		m_nLocalPlayerBits = 0;
		m_nOtherPlayerBits = 0;
		m_UpdateType = nullptr;
	}

	int *m_pBuf;
	int      m_UpdateFlags; // from the subheader
	bool     m_bIsEntity;

	int  m_nBaseline; // what baseline index do we use (0/1)
	bool m_bUpdateBaselines; // update baseline while parsing snaphsot

	int m_nLocalPlayerBits; // profiling data
	int m_nOtherPlayerBits; // profiling data

	CPostDataUpdateCall m_PostDataUpdateCalls[32];
	int                 m_nPostDataUpdateCalls;
};

class CClockDriftMgr
{
	friend class CBaseClientState;

public:
	CClockDriftMgr();

	// Is clock correction even enabled right now?
	static bool IsClockCorrectionEnabled();

	// Clear our state.
	void Clear();

	// This is called each time a server packet comes in. It is used to correlate
	// where the server is in time compared to us.
	void SetServerTick(int iServerTick);

	// Pass in the frametime you would use, and it will drift it towards the server clock.
	float AdjustFrameTime(float inputFrameTime);

	// Returns how many ticks ahead of the server the client is.
	float GetCurrentClockDifference() const;

private:
	void ShowDebugInfo(float flAdjustment);

	// This scales the offsets so the average produced is equal to the
	// current average + flAmount. This way, as we add corrections,
	// we lower the average accordingly so we don't keep responding
	// as much as we need to after we'd adjusted it a couple times.
	void AdjustAverageDifferenceBy(float flAmountInSeconds);

private:
	enum
	{
		// This controls how much it smoothes out the samples from the server.
		NUM_CLOCKDRIFT_SAMPLES = 16
	};

	// This holds how many ticks the client is ahead each time we get a server tick.
	// We average these together to get our estimate of how far ahead we are.
	float m_ClockOffsets[NUM_CLOCKDRIFT_SAMPLES];
	int   m_iCurClockOffset;

	int m_nServerTick; // Last-received tick from the server.
	int m_nClientTick; // The client's own tick counter (specifically, for interpolation during rendering).
		// The server may be on a slightly different tick and the client will drift towards it.
};

class ClientClass;
class CPureServerWhitelist;

//-----------------------------------------------------------------------------
// Purpose: CClientState should hold all pieces of the client state
//   The client_state_t structure is wiped completely at every server signon
//-----------------------------------------------------------------------------
class CClientState
{
	typedef struct CustomFile_s
	{
		int     crc; // file CRC
		unsigned int reqID; // download request ID
	} CustomFile_t;

public:
	CClientState() = delete;
	~CClientState();

public: // IConnectionlessPacketHandler interface:
	bool ProcessConnectionlessPacket(struct netpacket_s *packet);

public: // CBaseClientState overrides:
	void        Disconnect(bool bShowMainMenu);
	void        FullConnect(int &adr);
	bool        SetSignonState(int state, int count);
	void        PacketStart(int incoming_sequence, int outgoing_acknowledged);
	void        PacketEnd(void);
	void        FileReceived(const char *fileName, unsigned int transferID);
	void        FileRequested(const char *fileName, unsigned int transferID);
	void        FileDenied(const char *fileName, unsigned int transferID);
	void        ConnectionCrashed(const char *reason);
	void        ConnectionClosing(const char *reason);
	const char *GetCDKeyHash(void);
	void        SetFriendsID(unsigned int friendsID, const char *friendsName);
	void        SendClientInfo(void);
	void        InstallStringTableCallback(char const *tableName);
	bool        HookClientStringTable(char const *tableName);
	bool        InstallEngineStringTableCallback(char const *tableName);

	void StartUpdatingSteamResources();
	void CheckUpdatingSteamResources();
	void CheckFileCRCsWithServer();
	void FinishSignonState_New();
	void ConsistencyCheck(bool bForce);
	void RunFrame();

	void ReadEnterPVS(CEntityReadInfo &u);
	void ReadLeavePVS(CEntityReadInfo &u);
	void ReadDeltaEnt(CEntityReadInfo &u);
	void ReadPreserveEnt(CEntityReadInfo &u);
	void ReadDeletions(CEntityReadInfo &u);

	// In case the client DLL is using the old interface to set area bits,
	// copy what they've passed to us into the m_chAreaBits array (and 0xFF-out the m_chAreaPortalBits array).
	void UpdateAreaBits_BackwardsCompatible();

	// Used to be pAreaBits.
	unsigned char **GetAreaBits_BackwardCompatibility();

public: // IServerMessageHandlers
	void* unkn1;
	void* unkn2;
	void* unkn3;
	void* unkn4;
	void* unkn5;
	void* unkn6;
	void* unkn7;
	void* unkn8;
	void* unkn9;
	void* unkn10;
	void* unkn11;
	void* unkn12;
	void* unkn13;
	void* unkn14;
	void* unkn15;
	void* unkn16;
	void* unkn17;

public:
	float m_flLastServerTickTime; // the timestamp of last message
	bool  insimulation;

	int   oldtickcount; // previous tick
	float m_tickRemainder; // client copy of tick remainder
	float m_frameTime; // dt of the current frame

	int lastoutgoingcommand; // Sequence number of last outgoing command
	int chokedcommands; // number of choked commands
	int last_command_ack; // last command sequence number acknowledged by server
	int command_ack; // current command sequence acknowledged by server
	int m_nSoundSequence; // current processed reliable sound sequence number

	//
	// information that is static for the entire time connected to a server
	//
	bool ishltv; // true if HLTV server/demo

	int serverCRC; // To determine if client is playing hacked .map. (entities lump is skipped)
	int serverClientSideDllCRC; // To determine if client is playing on a hacked client dll.

	unsigned char m_chAreaBits[MAX_AREA_STATE_BYTES];
	unsigned char m_chAreaPortalBits[MAX_AREA_PORTAL_STATE_BYTES];
	bool          m_bAreaBitsValid; // Have the area bits been set for this level yet?

	// refresh related state
	Vector               viewangles;
	void* addangle;
	float                addangletotal;
	float                prevaddangletotal;
	int                  cdtrack; // cd audio

	CustomFile_t m_nCustomFiles[MAX_CUSTOM_FILES]; // own custom files CRCs

	unsigned int m_nFriendsID;
	char         m_FriendsName[MAX_PLAYER_NAME_LENGTH];

	void* events; // list of received events

	// demo loop control
	int  demonum; // -1 = don't play demos
	char demos[MAX_DEMOS][MAX_DEMONAME]; // when not playing

public:
	// If 'insimulation', returns the time (in seconds) at the client's current tick.
	// Otherwise, returns the exact client clock.
	float GetTime() const;

	bool IsPaused() const;

	float GetFrameTime(void) const;
	void  SetFrameTime(float dt)
	{
		m_frameTime = dt;
	}

	float GetClientInterpAmount(); // Formerly cl_interp, now based on cl_interp_ratio and cl_updaterate.

	void Clear(void);

	void DumpPrecacheStats(const char *name);

	// Public API to models
	model_t *GetModel(int index);
	void     SetModel(int tableIndex);
	int      LookupModelIndex(char const *name);

	// Public API to generic
	char const *GetGeneric(int index);
	void        SetGeneric(int tableIndex);
	int         LookupGenericIndex(char const *name);

	// Public API to sounds
	CSfxTable * GetSound(int index);
	char const *GetSoundName(int index);
	void        SetSound(int tableIndex);
	int         LookupSoundIndex(char const *name);
	void        ClearSounds();

	// Public API to decals
	char const *GetDecalName(int index);
	void        SetDecal(int tableIndex);

	// customization files code
	void CheckOwnCustomFiles(); // load own custom file
	void CheckOthersCustomFile(int crc); // check if we have to download custom files from server
	void AddCustomFile(int slot, const char *resourceFile);

public:
	INetworkStringTable *m_pModelPrecacheTable;
	INetworkStringTable *m_pGenericPrecacheTable;
	INetworkStringTable *m_pSoundPrecacheTable;
	INetworkStringTable *m_pDecalPrecacheTable;
	INetworkStringTable *m_pInstanceBaselineTable;
	INetworkStringTable *m_pLightStyleTable;
	INetworkStringTable *m_pUserInfoTable;
	INetworkStringTable *m_pServerStartupTable;
	INetworkStringTable *m_pDownloadableFileTable;

    void* model_precache[MAX_MODELS];
	void* generic_precache[MAX_GENERIC];
	void* sound_precache[32];
	void* decal_precache[MAX_BASE_DECALS];

	WaitForResourcesHandle_t m_hWaitForResourcesHandle;
	bool                     m_bUpdateSteamResources;
	bool                     m_bShownSteamResourceUpdateProgress;
	bool                     m_bDownloadResources;
	bool                     m_bCheckCRCsWithServer;
	float                    m_flLastCRCBatchTime;

	// This is only kept around to print out the whitelist info if sv_pure is used.
	CPureServerWhitelist *m_pPureServerWhitelist;

private:
	// Note: This is only here for backwards compatibility. If it is set to something other than NULL,
	// then we'll copy its contents into m_chAreaBits in UpdateAreaBits_BackwardsCompatible.
	void *m_pAreaBits;

	// Set to false when we first connect to a server and true later on before we
	// respond to a new whitelist.
	bool m_bMarkedCRCsUnverified;
}; // CClientState
