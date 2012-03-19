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

#ifndef __MARKETDB_H_INCL__
#define __MARKETDB_H_INCL__

#include "ServiceDB.h"

class PyRep;

static const uint32 HISTORY_AGGREGATION_DAYS = 5;	//how many days in the past is the cutoff between "new" and "old" price history.

typedef enum
{
	TransactionTypeSell = 0,
	TransactionTypeBuy = 1
} MktTransType;

class MarketDB
: public ServiceDB
{
public:
	PyRep *GetStationAsks(uint32 stationID);
	PyRep *GetSystemAsks(uint32 solarSystemID);
	PyRep *GetRegionBest(uint32 regionID);

	PyRep *GetOrders(uint32 regionID, uint32 typeID);
	PyRep *GetCharOrders(uint32 characterID);
	PyRep *GetOrderRow(uint32 orderID);

	PyRep *GetOldPriceHistory(uint32 regionID, uint32 typeID);
	PyRep *GetNewPriceHistory(uint32 regionID, uint32 typeID);
	PyRep *GetTransactions(uint32 characterID, uint32 typeID, uint32 quantity, double minPrice, double maxPrice, uint64 fromDate, int buySell);

	PyObject *GetMarketGroups();
	PyObject *GetRefTypes();
	PyObject *GetCorporationBills(uint32 corpID, bool payable);
	
	uint32 FindBuyOrder(uint32 stationID, uint32 typeID, double price, uint32 quantity, uint32 orderRange);
	uint32 FindSellOrder(uint32 stationID, uint32 typeID, double price, uint32 quantity, uint32 orderRange);

	bool GetOrderInfo(uint32 orderID, uint32 *orderOwnerID, uint32 *typeID, uint32 *stationID, uint32 *quantity, double *price, bool *isBuy, bool *isCorp);
	bool AlterOrderQuantity(uint32 orderID, uint32 new_qty);
	bool AlterOrderPrice(uint32 orderID, double new_price);
	bool DeleteOrder(uint32 orderID);

	bool AddCharacterBalance(uint32 char_id, double delta);

	uint32 StoreBuyOrder(uint32 clientID, uint32 accountID, uint32 stationID, uint32 typeID, double price, uint32 quantity, uint8 orderRange, uint32 minVolume, uint8 duration, bool isCorp);
	uint32 StoreSellOrder(uint32 clientID, uint32 accountID, uint32 stationID, uint32 typeID, double price, uint32 quantity, uint8 orderRange, uint32 minVolume, uint8 duration, bool isCorp);
	bool RecordTransaction(uint32 typeID, uint32 quantity, double price, MktTransType ttype, uint32 charID, uint32 regionID, uint32 stationID);
	
	bool BuildOldPriceHistory();
	
	uint32 GetPlayerNumberOutstandingContracts(uint32 characterID);
	PyRep *GetPlayerItemsInStation( uint32 characterID, uint32 stationID );
	PyRep *GetContractListForOwner( uint32 characterID, bool isAccepted, uint32 status, uint32 contractType, uint32 issuerCorpID );
	PyRep *GetContractList(  );
	PyRep *CollectMyPageInfo( uint32 clientID );
	PyRep *CreateContract( uint32 characterID, uint32 characterCorpID, uint32 type, uint32 avail, uint32 assigneeID, uint32 expiretime, uint32 duration, uint32 startStationID, uint32 endStationID, uint32 startSolarSystemID, uint32 endSolarSystemID, uint32 startRegionID, uint32 endRegionID, uint32 price, uint32 reward, uint32 collateral, std::string title, std::string description/*, std::vector<int32> itemsID, std::vector<int32> quantity, uint32 flag, std::vector<int32> requestItemID, std::vector<int32> requestItemQuantity*/, bool forCorp );
	PyRep *GetContract( uint32 contractID );
	PyRep *GetContractItems( uint32 contractID );
	PyRep *GetContractItemsForOwner( uint32 contractID );
	PyRep *GetContractBids( uint32 contractID );
	PyRep *DeleteContract( uint32 contractID, uint32 characterID );
	PyRep *AcceptContract( uint32 contractID, uint32 characterID, bool unknown );
	PyRep *CompleteContract( uint32 contractID, uint32 characterID, uint32 unknown );
protected:
	uint32 _StoreOrder(uint32 clientID, uint32 accountID, uint32 stationID, uint32 typeID, double price, uint32 quantity, uint8 orderRange, uint32 minVolume, uint8 duration, bool isCorp, bool isBuy);
};





#endif


