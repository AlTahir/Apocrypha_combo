/*
	------------------------------------------------------------------------------------
	LICENSE:
	------------------------------------------------------------------------------------
	This file is part of EVEmu: EVE Online Server Emulator
	Copyright 2006 - 2008 The EVEmu Team
	For the latest information visit http://evemu.mmoforge.org
	------------------------------------------------------------------------------------
	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU Lesser General Public License as published by the Free Software
	Foundation; either version 2 of the License, or (at your option) any later
	version.

	This program is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place - Suite 330, Boston, MA 02111-1307, USA, or go to
	http://www.gnu.org/copyleft/lesser.txt.
	------------------------------------------------------------------------------------
	Author:		Zhur
*/

/*
Dynamic Bodies:
	- shape
	- `mass`
	- `radius`
	- `volume`???
	- `Inertia`???
	- inertia information
	- position
	- velocity
	- angular velocity
	- collide with things
		- approximate eve "run into and stop/turn around" collisions

Ship: extends Dynamic
	- `maxVelocity`
	- thrust (propulsion + speed ratio)
	- angular thrust
	- "stopping" algorithm
	- "orbit" algorithm
	- "turning" algorithm 

Static Bodies:
	- shape
	- position

detect clients moving into agro radius
*/

#ifndef EVE_CLIENT_H
#define EVE_CLIENT_H

#include "ClientSession.h"

#include "inventory/InventoryItem.h"
#include "character/Character.h"
#include "ship/Ship.h"

#include "system/SystemEntity.h"
#include "ship/ModuleManager.h"

class CryptoChallengePacket;
class EVENotificationStream;
class PySubStream;
class InventoryItem;
class SystemManager;
class PyServiceMgr;
class PyCallStream;
class PyTuple;
class LSCChannel;
class PyAddress;
class PyList;
class PyDict;
class PyPacket;
class Client;
class PyRep;

// not needed now:
/*
class Functor {
public:
	Functor() {}
	virtual ~Functor() {}
	virtual void operator()() = 0;
};

class ClientFunctor : public Functor {
public:
	ClientFunctor(Client *c) : m_client(c) {}
	virtual ~ClientFunctor() {}
	//leave () pure
protected:
	Client *const m_client;
};

class FunctorTimerQueue {
public:
	typedef uint32 TimerID;
	
	FunctorTimerQueue();
	~FunctorTimerQueue();

	//schedule takes ownership of the functor.
	TimerID Schedule(Functor **what, uint32 in_how_many_ms);
	bool Cancel(TimerID id);
	
	void Process();
	
protected:
	//this could be done a TON better, but want to get something
	//working first, it is call encapsulated in here, so it should
	//be simple to gut this object and put in a real scheduler.
	class Entry {
	public:
		Entry(TimerID id, Functor *func, uint32 time_ms);
		~Entry();
		const TimerID id;
		Functor *const func;
		Timer when;
	};
	TimerID m_nextID;
	std::vector<Entry *> m_queue;	//not ordered or anything useful.
};
*/
//DO NOT INHERIT THIS OBJECT!
class Client
: public DynamicSystemEntity,
  protected EVEClientSession,
  protected EVEPacketDispatcher
{
public:
	Client(PyServiceMgr &services, EVETCPConnection** con);
	virtual ~Client();
	
	ModuleManager modules;

	bool            ProcessNet();
	virtual void    Process();

	PyServiceMgr& services() const { return m_services; }

    /********************************************************************/
    /* Session values                                                   */
    /********************************************************************/
    std::string GetAddress() const                  { return mSession.GetCurrentString( "address" ); }
    std::string GetLanguageID() const               { return mSession.GetCurrentString( "languageID" ); }

    uint32 GetAccountType() const                   { return mSession.GetCurrentInt( "userType" ); }
    uint32 GetAccountID() const                     { return mSession.GetCurrentInt( "userid" ); }
	uint32 GetAccountRole() const                   { return mSession.GetCurrentInt( "role" ); }

	uint32 GetCharacterID() const                   { return mSession.GetCurrentInt( "charid" ); }
	uint32 GetCorporationID() const                 { return mSession.GetCurrentInt( "corpid" ); }
	uint32 GetLocationID() const                    { return mSession.GetCurrentInt( "locationid" ); }
	uint32 GetStationID() const                     { return mSession.GetCurrentInt( "stationid" ); }
	uint32 GetSystemID() const                      { return mSession.GetCurrentInt( "solarsystemid2" ); }
	uint32 GetConstellationID() const               { return mSession.GetCurrentInt( "constellationid" ); }
	uint32 GetRegionID() const                      { return mSession.GetCurrentInt( "regionid" ); }

	uint32 GetCorpHQ() const                        { return mSession.GetCurrentInt( "hqID" ); }
    uint64 GetCorpRole() const                      { return mSession.GetCurrentLong( "corprole" ); }
	uint64 GetRolesAtAll() const                    { return mSession.GetCurrentLong( "rolesAtAll" ); }
	uint64 GetRolesAtBase() const                   { return mSession.GetCurrentLong( "rolesAtBase" ); }
	uint64 GetRolesAtHQ() const                     { return mSession.GetCurrentLong( "rolesAtHQ" ); }
	uint64 GetRolesAtOther() const                  { return mSession.GetCurrentLong( "rolesAtOther" ); }

	uint32 GetShipID() const                        { return mSession.GetCurrentInt( "shipid" ); }
    uint32 GetGangRole() const                      { return mSession.GetCurrentInt( "gangrole" ); }


	// character data
	CharacterRef GetChar() const                    { return m_char; }
	ShipRef GetShip() const                         { return ShipRef::StaticCast( Item() ); }

	bool IsInSpace() const                          { return ( GetStationID() == 0 ); }
	double x() const                                { return GetPosition().x; }	//this is terribly inefficient.
	double y() const                                { return GetPosition().y; }	//this is terribly inefficient.
	double z() const                                { return GetPosition().z; }	//this is terribly inefficient.

	uint32 GetAllianceID() const                    { return GetChar() ? GetChar()->allianceID() : 0; }
	double GetBounty() const                        { return GetChar() ? GetChar()->bounty() : 0.0; }
	double GetSecurityRating() const                { return GetChar() ? GetChar()->securityRating() : 0.0; }
	double GetBalance() const                       { return GetChar() ? GetChar()->balance() : 0.0; }

	bool AddBalance(double amount);

	void BoardShip(ShipRef new_ship);
	void MoveToLocation(uint32 location, const GPoint &pt);
	void MoveToPosition(const GPoint &pt);
	void MoveItem(uint32 itemID, uint32 location, EVEItemFlags flag);
	bool EnterSystem();
	bool SelectCharacter( uint32 char_id );
	void JoinCorporationUpdate(uint32 corp_id);
	void SavePosition();
	void UpdateSkillTraining();
	
	double GetPropulsionStrength() const;
	
	bool LaunchDrone(InventoryItemRef drone);
	
	void SendNotification(const PyAddress &dest, EVENotificationStream &noti, bool seq=true);
	void SendNotification(const char *notifyType, const char *idType, PyTuple **payload, bool seq=true);

	//destiny stuff...
	void WarpTo(const GPoint &p, double distance);
	void StargateJump(uint32 fromGate, uint32 toGate);
	
	void SendErrorMsg(const char *fmt, ...);
	void SendNotifyMsg(const char *fmt, ...);
	void SendInfoModalMsg(const char *fmt, ...);
	void SelfChatMessage(const char *fmt, ...);
	void SelfEveMail(const char *subject, const char *fmt, ...);
	void ChannelJoined(LSCChannel *chan);
	void ChannelLeft(LSCChannel *chan);
	void UpdateSession( const char *sessionType, int value );

	//FunctorTimerQueue::TimerID Delay( uint32 time_in_ms, void (Client::* clientCall)() );
	//FunctorTimerQueue::TimerID Delay( uint32 time_in_ms, ClientFunctor **functor );

	/** character notification messages
	  */
	void OnCharNoLongerInStation();
	void OnCharNowInStation();

    /********************************************************************/
    /* DynamicSystemEntity interface                                    */
    /********************************************************************/
	virtual EntityClass GetClass() const { return(ecClient); }
	virtual bool IsClient() const { return true; }
	virtual Client *CastToClient() { return(this); }
	virtual const Client *CastToClient() const { return(this); }

	virtual const char *GetName() const { return GetChar() ? GetChar()->itemName().c_str() : "(null)"; }
	virtual PyDict *MakeSlimItem() const;
	virtual void QueueDestinyUpdate(PyTuple** du);
	virtual void QueueDestinyEvent(PyTuple** multiEvent);

	virtual void TargetAdded(SystemEntity *who);
	virtual void TargetLost(SystemEntity *who);
	virtual void TargetedAdd(SystemEntity *who);
	virtual void TargetedLost(SystemEntity *who);
	virtual void TargetsCleared();

	virtual void ApplyDamageModifiers(Damage &d, SystemEntity *target);
	virtual bool ApplyDamage(Damage &d);
	virtual void Killed(Damage &fatal_blow);
	virtual SystemManager *System() const { return(m_system); }
	
protected:
	void _ReduceDamage(Damage &d);
    void _UpdateSession( const CharacterConstRef& character );

    // Packet stuff
	void _SendCallReturn( const PyAddress& source, uint64 callID, PyRep** return_value, const char* channel = NULL );
	void _SendException( const PyAddress& source, uint64 callID, MACHONETMSG_TYPE in_response_to, MACHONETERR_TYPE exception_type, PyRep** payload );
	void _SendSessionChange();
	void _SendPingRequest();
    void _SendPingResponse( const PyAddress& source, uint64 callID );

	PyServiceMgr& m_services;
	Timer m_pingTimer;
	ClientSession mSession;

	SystemManager *m_system;	//we do not own this
	CharacterRef m_char;

	std::set<LSCChannel *> m_channels;	//we do not own these.
	
	//this whole move system is a piece of crap:
	typedef enum {
		msIdle,
		msJump
	} _MoveState;
	void _postMove(_MoveState type, uint32 wait_ms=500);
	_MoveState m_moveState;
	Timer m_moveTimer;
	uint32 m_moveSystemID;
	GPoint m_movePoint;
	void _ExecuteJump();

	uint64 m_timeEndTrain;

    /********************************************************************/
    /* EVEClientSession interface                                       */
    /********************************************************************/
    void _GetVersion( VersionExchange& version );
    uint32 _GetUserCount();
    uint32 _GetQueuePosition() { /* hack */ return 1; }

    bool _VerifyVersion( VersionExchange& version );
    bool _VerifyCrypto( CryptoRequestPacket& cr );
    bool _VerifyLogin( CryptoChallengePacket& ccp );
    bool _VerifyVIPKey( const std::string& vipKey ) { /* do nothing */ return true; }
    bool _VerifyFuncResult( CryptoHandshakeResult& result );

    /********************************************************************/
    /* EVEPacketDispatcher interface                                    */
    /********************************************************************/
    bool Handle_CallReq( PyPacket* packet, PyCallStream& req );
    bool Handle_Notify( PyPacket* packet );
    bool Handle_PingReq( PyPacket* packet ) { _SendPingResponse( packet->dest, packet->source.callID ); return true; }
    bool Handle_PingRsp( PyPacket* packet ) { /* do nothing */ return true; }

private:
	//queues for destiny updates:
	PyList* m_destinyEventQueue;	//we own these. These are events as used in OnMultiEvent
	PyList* m_destinyUpdateQueue;	//we own these. They are the `update` which go into DoDestinyAction
	void _SendQueuedUpdates();

	//FunctorTimerQueue m_delayQueue;
	
	uint32 m_nextNotifySequence;
};


//simple functor for void calls.
//not needed right now
/*class SimpleClientFunctor : public ClientFunctor {
public:
	typedef void (Client::* clientCall)();
	SimpleClientFunctor(Client *c, clientCall call) : ClientFunctor(c), m_call(call) {}
	virtual ~SimpleClientFunctor() {}
	virtual void operator()() {
		(m_client->*m_call)();
	}
public:
	const clientCall m_call;
};*/

#endif