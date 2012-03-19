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

#include "EVEServerPCH.h"
//#include "ContractMgrService.h"



PyCallable_Make_InnerDispatcher(ContractMgrService)

ContractMgrService::ContractMgrService(PyServiceMgr *mgr)
: PyService(mgr, "contractMgr"),
  m_dispatch(new Dispatcher(this))
{
	_SetCallDispatcher(m_dispatch);

	PyCallable_REG_CALL(ContractMgrService, NumRequiringAttention);
	PyCallable_REG_CALL(ContractMgrService, CollectMyPageInfo);
	PyCallable_REG_CALL(ContractMgrService, NumOutstandingContracts);
	PyCallable_REG_CALL(ContractMgrService, GetItemsInStation);
	PyCallable_REG_CALL(ContractMgrService, GetContractListForOwner);
	PyCallable_REG_CALL(ContractMgrService, GetContractList);
	PyCallable_REG_CALL(ContractMgrService, CreateContract);
	PyCallable_REG_CALL(ContractMgrService, GetContract);
	PyCallable_REG_CALL(ContractMgrService, DeleteContract);
	PyCallable_REG_CALL(ContractMgrService, AcceptContract);
	PyCallable_REG_CALL(ContractMgrService, CompleteContract);
}

ContractMgrService::~ContractMgrService()
{
	delete m_dispatch;
}

PyResult ContractMgrService::Handle_NumRequiringAttention( PyCallArgs& call )
{
    sLog.Debug( "ContractMgrService", "Called NumRequiringAttention stub." );

	PyDict* args = new PyDict;
	args->SetItemString( "n", new PyInt( 0 ) );
	args->SetItemString( "ncorp", new PyInt( 0 ) );

	return new PyObject(
        new PyString( "util.KeyVal" ), args
    );
}

PyResult ContractMgrService::Handle_CollectMyPageInfo( PyCallArgs& call )
{
	sLog.Debug( "ContractMgrService", "Called CollectMyPageInfo stub." );
	return new PyObject(
		new PyString("util.Rowset"), m_db.CollectMyPageInfo( call.client->GetCharacterID() )
		);
}
 
PyResult ContractMgrService::Handle_NumOutstandingContracts( PyCallArgs& call)
{
	sLog.Debug( "ContractMgrService", "Called NumOutstandingContracts stub." );
	// We should only get the number of contracts a user has and return it...
	return new PyInt( m_db.GetPlayerNumberOutstandingContracts(call.client->GetCharacterID()) );
}

PyResult ContractMgrService::Handle_GetItemsInStation(PyCallArgs &call)
{
	sLog.Debug( "ContractMgrService", "Called GetItemsInStation stub." );
	return m_db.GetPlayerItemsInStation( call.client->GetCharacterID(), call.client->GetLocationID() );
}

PyResult ContractMgrService::Handle_GetContractListForOwner( PyCallArgs& call )
{
	sLog.Debug( "ContractMgrService", "Called GetContractListForOwner stub." );
	Call_GetContractListForOwner arg;
	PyDict* _contract = new PyDict;

	if( !arg.Decode(&call.tuple) )
	{
		codelog(SERVICE__ERROR, "%s: Bad arguments to GetContractListForOwner", call.client->GetName());
		return NULL;
	}

	_contract->SetItemString( "contracts", m_db.GetContractListForOwner( arg.characterID, arg.isAccepted, arg.status, arg.contractType, call.client->GetCorporationID() ));
	_contract->SetItemString( "items", m_db.GetContractItemsForOwner( arg.characterID ) );

	return new PyObject(
		new PyString( "util.KeyVal" ), _contract
		);
}

PyResult ContractMgrService::Handle_GetContractList( PyCallArgs& call )
{
	sLog.Debug( "ContractMgrService", "Called GetContractList stub." );
	//Call_GetContractList args;
	PyDict* _contract = new PyDict;
	
	/*if( !args.Decode(&call.tuple) )
	{
		codelog(SERVICE__ERROR, "%s: Bad arguments to GetContractListForOwner", call.client->GetName());
		return NULL;
	}*/

	_contract->SetItemString( "contracts", m_db.GetContractList( ));
	_contract->SetItemString( "items", m_db.GetContractItems( NULL ) );
	_contract->SetItemString( "bids", m_db.GetContractBids( NULL ) );

	return new PyObject(
		new PyString( "util.KeyVal" ), _contract
		);
}

PyResult ContractMgrService::Handle_CreateContract( PyCallArgs& call )
{
	sLog.Debug( "ContractMgrService", "Called CreateContract stub.");
	Call_CreateContract args;
	if( !args.Decode( &call.tuple ) )
	{
		codelog(SERVICE__ERROR, "%s: Bad arguments to CreateContract", call.client->GetName() );
		return NULL;
	}
	return m_db.CreateContract( call.client->GetCharacterID(), call.client->GetCorporationID(), args.type, args.avail, args.assigneeID, args.expiretime, args.duration, args.startStationID, args.endStationID, call.client->GetSystemID(), 0, call.client->GetRegionID(), 0, args.price, args.reward, args.reward, args.title, args.description/*, args.itemID, args.quantity, args.flag, args.requestItemID, args.requestQuantity*/, false);
}

PyResult ContractMgrService::Handle_GetContract( PyCallArgs& call )
{
	sLog.Debug( "ContractMgrService", "Called GetContract stub.");
	Call_SingleIntegerArg arg;
	PyDict* _contract = new PyDict;

	if( !arg.Decode( &call.tuple ) )
	{
		codelog(SERVICE__ERROR, "%s: Bad arguments to GetContract", call.client->GetName() );
		return NULL;
	}

	_contract->SetItemString( "contract", m_db.GetContract( arg.arg ) );
	_contract->SetItemString( "items", m_db.GetContractItems( arg.arg ) );
	_contract->SetItemString( "bids", m_db.GetContractBids( arg.arg ) );

	return new PyObject(
		new PyString("util.KeyVal"), _contract
		);
}

PyResult ContractMgrService::Handle_DeleteContract( PyCallArgs& call )
{
	sLog.Debug( "ContractMgrService", "Called DeleteContract stub." );
	Call_DeleteContract arg;

	if( !arg.Decode( &call.tuple) )
	{
		codelog(SERVICE__ERROR, "%s: Bad arguments to DeleteContract", call.client->GetName() );
		return NULL;
	}

	return m_db.DeleteContract( arg.contractID, call.client->GetCharacterID() );
}

PyResult ContractMgrService::Handle_AcceptContract( PyCallArgs& call )
{
	sLog.Debug( "ContractMgrService", "Called AcceptContract stub." );
	Call_AcceptContract arg;

	if( !arg.Decode( &call.tuple ) )
	{
		codelog(SERVICE__ERROR, "%s: Bad arguments to AcceptContract", call.client->GetName() );
		return NULL;
	}

	return m_db.AcceptContract(arg.contractID, call.client->GetCharacterID(), arg.unknown);
}

PyResult ContractMgrService::Handle_CompleteContract( PyCallArgs& call )
{
	sLog.Debug( "ContractMgrService", "Called CompleteContract stub." );
	Call_CompleteContract arg;

	if( !arg.Decode( &call.tuple ) )
	{
		codelog(SERVICE__ERROR, "%s: Bad arguments to AcceptContract", call.client->GetName() );
		return NULL;
	}

	return m_db.CompleteContract(arg.contractID, call.client->GetCharacterID(), arg.status);
}
