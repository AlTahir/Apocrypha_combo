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
    Author:     Zhur
*/

#include "EVEServerPCH.h"

PyCallable_Make_InnerDispatcher(CertificateMgrService)

CertificateMgrService::CertificateMgrService(PyServiceMgr *mgr)
: PyService(mgr, "certificateMgr"),
  m_dispatch(new Dispatcher(this))
{
    _SetCallDispatcher(m_dispatch);

    PyCallable_REG_CALL(CertificateMgrService, GetMyCertificates)
    PyCallable_REG_CALL(CertificateMgrService, GetCertificateCategories)
    PyCallable_REG_CALL(CertificateMgrService, GetAllShipCertificateRecommendations)
    PyCallable_REG_CALL(CertificateMgrService, GetCertificateClasses)
    PyCallable_REG_CALL(CertificateMgrService, GrantCertificate)
    PyCallable_REG_CALL(CertificateMgrService, BatchCertificateGrant)
    PyCallable_REG_CALL(CertificateMgrService, BatchCertificateUpdate)
}

CertificateMgrService::~CertificateMgrService()
{
	delete m_dispatch;
}

PyResult CertificateMgrService::Handle_GetMyCertificates(PyCallArgs &call) {
    return(m_db.GetMyCertificates(call.client->GetCharacterID()));
}

PyResult CertificateMgrService::Handle_GetCertificateCategories(PyCallArgs &call) {
    ObjectCachedMethodID method_id(GetName(), "GetCertificateCategories");

    if(!m_manager->cache_service->IsCacheLoaded(method_id)) {
        PyRep *res = m_db.GetCertificateCategories();
        if(res == NULL) {
            codelog(SERVICE__ERROR, "Failed to load cache, generating empty contents.");
            res = new PyNone();
        }
        m_manager->cache_service->GiveCache(method_id, &res);
    }

    return(m_manager->cache_service->MakeObjectCachedMethodCallResult(method_id));
}

PyResult CertificateMgrService::Handle_GetAllShipCertificateRecommendations(PyCallArgs &call) {
    ObjectCachedMethodID method_id(GetName(), "GetAllShipCertificateRecommendations");

    if(!m_manager->cache_service->IsCacheLoaded(method_id)) {
        PyRep *res = m_db.GetAllShipCertificateRecommendations();
        if(res == NULL) {
            codelog(SERVICE__ERROR, "Failed to load cache, generating empty contents.");
            res = new PyNone();
        }
        m_manager->cache_service->GiveCache(method_id, &res);
    }

    return(m_manager->cache_service->MakeObjectCachedMethodCallResult(method_id));
}

PyResult CertificateMgrService::Handle_GetCertificateClasses(PyCallArgs &call) {
    ObjectCachedMethodID method_id(GetName(), "GetCertificateClasses");

    if(!m_manager->cache_service->IsCacheLoaded(method_id)) {
        PyRep *res = m_db.GetCertificateClasses();
        if(res == NULL) {
            codelog(SERVICE__ERROR, "Failed to load cache, generating empty contents.");
            res = new PyNone();
        }
        m_manager->cache_service->GiveCache(method_id, &res);
    }

    return(m_manager->cache_service->MakeObjectCachedMethodCallResult(method_id));
}

PyResult CertificateMgrService::Handle_GrantCertificate(PyCallArgs &call) {
    Call_SingleIntegerArg arg;
    if(!arg.Decode(&call.tuple)) {
        _log(CLIENT__ERROR, "Failed to decode args.");
        return(NULL);
    }

    return(_GrantCertificate(call.client->GetCharacterID(), arg.arg) ? new PyInt(arg.arg) : NULL);
}

PyResult CertificateMgrService::Handle_BatchCertificateGrant(PyCallArgs &call) {
    Call_SingleIntList arg;
    if(!arg.Decode(&call.tuple)) {
        _log(CLIENT__ERROR, "Failed to decode args.");
        return(NULL);
    }

    PyList *res = new PyList;

    std::vector<int32>::iterator cur, end;
    cur = arg.ints.begin();
    end = arg.ints.end();
    for(; cur != end; cur++) {
        if(_GrantCertificate(call.client->GetCharacterID(), *cur))
            res->AddItemInt(*cur);
    }

    return res;
}

PyResult CertificateMgrService::Handle_BatchCertificateUpdate(PyCallArgs &call) {
    Call_BatchCertificateUpdate args;
    if(!args.Decode(&call.tuple)) {
        _log(CLIENT__ERROR, "Failed to decode args.");
        return(NULL);
    }

    std::map<uint32, uint32>::iterator cur, end;
    cur = args.update.begin();
    end = args.update.end();
    for(; cur != end; cur++)
        _UpdateCertificate(call.client->GetCharacterID(), cur->first, cur->second);

    return(NULL);
}

bool CertificateMgrService::_GrantCertificate(uint32 characterID, uint32 certificateID) {
    _log(SERVICE__MESSAGE, "%u asked to grant certificate %u.", characterID, certificateID);
    _log(SERVICE__ERROR, "Granting certificates not supported yet.");

    return(false);
}

bool CertificateMgrService::_UpdateCertificate(uint32 characterID, uint32 certificateID, bool pub) {
    _log(SERVICE__MESSAGE, "%u asked to make his certificate %u %s.", characterID, certificateID, (pub ? "public" : "private"));
    _log(SERVICE__ERROR, "Updating certificates not supported yet.");

    return(false);
}


