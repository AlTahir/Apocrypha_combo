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
    Author:     Zhur, Bloody.Rabbit
*/

#include "EVEServerPCH.h"

/*
 * CharacterTypeData
 */
CharacterTypeData::CharacterTypeData(
    const char *_bloodlineName,
    EVERace _race,
    const char *_desc,
    const char *_maleDesc,
    const char *_femaleDesc,
    uint32 _shipTypeID,
    uint32 _corporationID,
    uint8 _perception,
    uint8 _willpower,
    uint8 _charisma,
    uint8 _memory,
    uint8 _intelligence,
    const char *_shortDesc,
    const char *_shortMaleDesc,
    const char *_shortFemaleDesc)
: bloodlineName(_bloodlineName),
  race(_race),
  description(_desc),
  maleDescription(_maleDesc),
  femaleDescription(_femaleDesc),
  shipTypeID(_shipTypeID),
  corporationID(_corporationID),
  perception(_perception),
  willpower(_willpower),
  charisma(_charisma),
  memory(_memory),
  intelligence(_intelligence),
  shortDescription(_shortDesc),
  shortMaleDescription(_shortMaleDesc),
  shortFemaleDescription(_shortFemaleDesc)
{
}

/*
 * CharacterType
 */
CharacterType::CharacterType(
    uint32 _id,
    uint8 _bloodlineID,
    // ItemType stuff:
    const ItemGroup &_group,
    const TypeData &_data,
    // CharacterType stuff:
    const ItemType &_shipType,
    const CharacterTypeData &_charData)
: ItemType(_id, _group, _data),
  m_bloodlineID(_bloodlineID),
  m_bloodlineName(_charData.bloodlineName),
  m_description(_charData.description),
  m_maleDescription(_charData.maleDescription),
  m_femaleDescription(_charData.femaleDescription),
  m_shipType(_shipType),
  m_corporationID(_charData.corporationID),
  m_perception(_charData.perception),
  m_willpower(_charData.willpower),
  m_charisma(_charData.charisma),
  m_memory(_charData.memory),
  m_intelligence(_charData.intelligence),
  m_shortDescription(_charData.shortDescription),
  m_shortMaleDescription(_charData.shortMaleDescription),
  m_shortFemaleDescription(_charData.shortFemaleDescription)
{
    // check for consistency
    assert(_data.race == _charData.race);
    assert(_charData.shipTypeID == _shipType.id());
}

CharacterType *CharacterType::Load(ItemFactory &factory, uint32 characterTypeID)
{
    return ItemType::Load<CharacterType>( factory, characterTypeID );
}

template<class _Ty>
_Ty *CharacterType::_LoadCharacterType(ItemFactory &factory, uint32 typeID, uint8 bloodlineID,
    // ItemType stuff:
    const ItemGroup &group, const TypeData &data,
    // CharacterType stuff:
    const ItemType &shipType, const CharacterTypeData &charData)
{
    // enough data for construction
    return new CharacterType( typeID, bloodlineID, group, data, shipType, charData );
}

/*
 * CharacterData
 */
CharacterData::CharacterData(
    uint32 _accountID,
    const char *_title,
    const char *_desc,
    bool _gender,
    double _bounty,
    double _balance,
    double _securityRating,
    uint32 _logonMinutes,
    uint32 _corporationID,
    uint32 _allianceID,
    uint32 _stationID,
    uint32 _solarSystemID,
    uint32 _constellationID,
    uint32 _regionID,
    uint32 _ancestryID,
    uint32 _careerID,
    uint32 _schoolID,
    uint32 _careerSpecialityID,
    uint64 _startDateTime,
    uint64 _createDateTime,
    uint64 _corporationDateTime)
: accountID(_accountID),
  title(_title),
  description(_desc),
  gender(_gender),
  bounty(_bounty),
  balance(_balance),
  securityRating(_securityRating),
  logonMinutes(_logonMinutes),
  corporationID(_corporationID),
  allianceID(_allianceID),
  stationID(_stationID),
  solarSystemID(_solarSystemID),
  constellationID(_constellationID),
  regionID(_regionID),
  ancestryID(_ancestryID),
  careerID(_careerID),
  schoolID(_schoolID),
  careerSpecialityID(_careerSpecialityID),
  startDateTime(_startDateTime),
  createDateTime(_createDateTime),
  corporationDateTime(_corporationDateTime)
{
}

/*
 * CharacterAppearance
 */
CharacterAppearance::CharacterAppearance() {
    //NULL all dynamic fields
#define NULL_FIELD(v) \
    v = NULL;

#define INT_DYN(v) NULL_FIELD(v)
#define REAL_DYN(v) NULL_FIELD(v)
#include "character/CharacterAppearance_fields.h"

#undef NULL_FIELD
}

CharacterAppearance::CharacterAppearance( const CharacterAppearance& from )
{
    //just do deep copy
    *this = from;
}

CharacterAppearance::~CharacterAppearance()
{
    //delete all dynamic fields
#define CLEAR_FIELD(v) \
    Clear_##v();

#define INT_DYN(v) CLEAR_FIELD(v)
#define REAL_DYN(v) CLEAR_FIELD(v)
#include "character/CharacterAppearance_fields.h"

#undef CLEAR_FIELD
}

void CharacterAppearance::Build( const std::map<std::string, PyRep*>& from )
{
    //builds our contents from strdict
    std::map<std::string, PyRep*>::const_iterator itr;

    _log(CLIENT__MESSAGE, "  Appearance Data:");

#define INT(v) \
    itr = from.find( #v ); \
    if( itr != from.end() ) \
    { \
        if( itr->second->IsInt() ) \
        { \
            v = itr->second->AsInt()->value(); \
            _log( CLIENT__MESSAGE, "     %s: %u", itr->first.c_str(), v ); \
        } \
        else \
            _log( CLIENT__ERROR, "Invalid type for " #v ": expected integer, got %s.", itr->second->TypeString() ); \
    }
#define INT_DYN(v) \
    itr = from.find( #v ); \
    if( itr != from.end() ) \
    { \
        if( itr->second->IsInt() ) \
        { \
            Set_##v( itr->second->AsInt()->value() ); \
            _log( CLIENT__MESSAGE, "     %s: %u", itr->first.c_str(), Get_##v() ); \
        } \
        else \
            _log( CLIENT__ERROR, "Invalid type for " #v ": expected integer, got %s.", itr->second->TypeString() ); \
    }
#define REAL(v) \
    itr = from.find( #v ); \
    if( itr != from.end() ) \
    { \
        if( itr->second->IsFloat() ) \
        { \
            v = itr->second->AsFloat()->value(); \
            _log( CLIENT__MESSAGE, "     %s: %f", itr->first.c_str(), v ); \
        } \
        else \
            _log( CLIENT__ERROR, "Invalid type for " #v ": expected real, got %s.", itr->second->TypeString() ); \
    }
#define REAL_DYN(v) \
    itr = from.find( #v ); \
    if( itr != from.end() ) \
    { \
        if( itr->second->IsFloat() ) \
        { \
            Set_##v( itr->second->AsFloat()->value() ); \
            _log( CLIENT__MESSAGE, "     %s: %f", itr->first.c_str(), Get_##v() ); \
        } \
        else \
            _log( CLIENT__ERROR, "Invalid type for " #v ": expected real, got %s.", itr->second->TypeString() ); \
    }
#include "character/CharacterAppearance_fields.h"
}

void CharacterAppearance::operator=( const CharacterAppearance& from )
{
#define COPY(v) \
    v = from.v;
#define COPY_DYN(v) \
    if(!from.IsNull_##v()) \
        Set_##v(from.Get_##v());

#define INT(v) COPY(v)
#define INT_DYN(v) COPY_DYN(v)
#define REAL(v) COPY(v)
#define REAL_DYN(v) COPY_DYN(v)
#include "character/CharacterAppearance_fields.h"

#undef COPY
#undef COPY_DYN
}

/*
 * CorpMemberInfo
 */
CorpMemberInfo::CorpMemberInfo(
    uint32 _corpHQ,
    uint64 _corpRole,
    uint64 _rolesAtAll,
    uint64 _rolesAtBase,
    uint64 _rolesAtHQ,
    uint64 _rolesAtOther)
: corpHQ(_corpHQ),
  corpRole(_corpRole),
  rolesAtAll(_rolesAtAll),
  rolesAtBase(_rolesAtBase),
  rolesAtHQ(_rolesAtHQ),
  rolesAtOther(_rolesAtOther)
{
}

/*
 * Character
 */
Character::Character(
    ItemFactory &_factory,
    uint32 _characterID,
    // InventoryItem stuff:
    const CharacterType &_charType,
    const ItemData &_data,
    // Character stuff:
    const CharacterData &_charData,
    const CorpMemberInfo &_corpData)
: Owner(_factory, _characterID, _charType, _data),
  m_accountID(_charData.accountID),
  m_title(_charData.title),
  m_description(_charData.description),
  m_gender(_charData.gender),
  m_bounty(_charData.bounty),
  m_balance(_charData.balance),
  m_securityRating(_charData.securityRating),
  m_logonMinutes(_charData.logonMinutes),
  m_corporationID(_charData.corporationID),
  m_corpHQ(_corpData.corpHQ),
  m_allianceID(_charData.allianceID),
  m_corpRole(_corpData.corpRole),
  m_rolesAtAll(_corpData.rolesAtAll),
  m_rolesAtBase(_corpData.rolesAtBase),
  m_rolesAtHQ(_corpData.rolesAtHQ),
  m_rolesAtOther(_corpData.rolesAtOther),
  m_stationID(_charData.stationID),
  m_solarSystemID(_charData.solarSystemID),
  m_constellationID(_charData.constellationID),
  m_regionID(_charData.regionID),
  m_ancestryID(_charData.ancestryID),
  m_careerID(_charData.careerID),
  m_schoolID(_charData.schoolID),
  m_careerSpecialityID(_charData.careerSpecialityID),
  m_startDateTime(_charData.startDateTime),
  m_createDateTime(_charData.createDateTime),
  m_corporationDateTime(_charData.corporationDateTime)
{
    // allow characters to be only singletons
    assert(singleton() && quantity() == 1);
}

CharacterRef Character::Load(ItemFactory &factory, uint32 characterID)
{
    return InventoryItem::Load<Character>( factory, characterID );
}

template<class _Ty>
RefPtr<_Ty> Character::_LoadCharacter(ItemFactory &factory, uint32 characterID,
    // InventoryItem stuff:
    const CharacterType &charType, const ItemData &data,
    // Character stuff:
    const CharacterData &charData, const CorpMemberInfo &corpData)
{
    // construct the item
    return CharacterRef( new Character( factory, characterID, charType, data, charData, corpData ) );
}

CharacterRef Character::Spawn(ItemFactory &factory,
    // InventoryItem stuff:
    ItemData &data,
    // Character stuff:
    CharacterData &charData, CharacterAppearance &appData, CorpMemberInfo &corpData
) {
    uint32 characterID = Character::_Spawn( factory, data, charData, appData, corpData );
    if( characterID == 0 )
        return CharacterRef();
    return Character::Load( factory, characterID );
}

uint32 Character::_Spawn(ItemFactory &factory,
    // InventoryItem stuff:
    ItemData &data,
    // Character stuff:
    CharacterData &charData, CharacterAppearance &appData, CorpMemberInfo &corpData
) {
    // make sure it's a character
    const CharacterType *ct = factory.GetCharacterType(data.typeID);
    if(ct == NULL)
        return 0;

    // make sure it's a singleton with qty 1
    if(!data.singleton || data.quantity != 1) {
        _log(ITEM__ERROR, "Tried to create non-singleton character %s.", data.name.c_str());
        return 0;
    }

    // first the item
    uint32 characterID = Owner::_Spawn(factory, data);
    if(characterID == 0)
        return 0;

    // then character
    if(!factory.db().NewCharacter(characterID, charData, appData, corpData)) {
        // delete the item
        factory.db().DeleteItem(characterID);

        return 0;
    }

    return characterID;
}

bool Character::_Load()
{
    if( !LoadContents( m_factory ) )
        return false;

    if( !m_factory.db().LoadSkillQueue( itemID(), m_skillQueue ) )
        return false;

    return Owner::_Load();
}

void Character::Delete() {
    // delete contents
    DeleteContents( m_factory );

    // delete character record
    m_factory.db().DeleteCharacter(itemID());

    // let the parent care about the rest
    Owner::Delete();
}

bool Character::AlterBalance(double balanceChange) {
    if(balanceChange == 0)
        return true;

    double result = balance() + balanceChange;

    //remember, this can take a negative amount...
    if(result < 0)
        return false;

    m_balance = result;

    //TODO: save some info to journal.
    SaveCharacter();

    return true;
}

void Character::SetLocation(uint32 stationID, uint32 solarSystemID, uint32 constellationID, uint32 regionID) {
    m_stationID = stationID;
    m_solarSystemID = solarSystemID;
    m_constellationID = constellationID;
    m_regionID = regionID;

    SaveCharacter();
}

void Character::JoinCorporation(uint32 corporationID) {
    m_corporationID = corporationID;

    //TODO: load new roles
    m_corpRole = 0;
    m_rolesAtAll = 0;
    m_rolesAtBase = 0;
    m_rolesAtHQ = 0;
    m_rolesAtOther = 0;

    //TODO: recursively change corp on all our items.

    SaveCharacter();
}

void Character::SetDescription(const char *newDescription) {
    m_description = newDescription;

    SaveCharacter();
}

bool Character::HasSkill(uint32 skillTypeID) const
{
    return(GetSkill(skillTypeID) != NULL);
}

SkillRef Character::GetSkill(uint32 skillTypeID) const
{
    InventoryItemRef skill = GetByTypeFlag( skillTypeID, flagSkill );
    if( !skill )
        skill = GetByTypeFlag( skillTypeID, flagSkillInTraining );

    return SkillRef::StaticCast( skill );
}

SkillRef Character::GetSkillInTraining() const
{
    return SkillRef::StaticCast( FindFirstByFlag( flagSkillInTraining ) );
}

double Character::GetSPPerMin(SkillRef skill) const
{
    double primaryVal = attributes.GetReal( (EVEAttributeMgr::Attr)skill->primaryAttribute() );
    double secondaryVal = attributes.GetReal( (EVEAttributeMgr::Attr)skill->secondaryAttribute() );

    uint8 skillLearningLevel = 0;

    //3374 - Skill Learning
    SkillRef skillLearning = GetSkill( 3374 );
    if( skillLearning )
        skillLearningLevel = skillLearning->skillLevel();

    return (primaryVal + secondaryVal / 2.0)
         * (1.0 + 0.02 * skillLearningLevel)
         *  2.0; /* this is hacky and should be applied only if total SP < 1.6M */
}

uint64 Character::GetEndOfTraining() const
{
    SkillRef skill = GetSkillInTraining();
    if( !skill )
        return 0;

    return skill->expiryTime();
}

bool Character::InjectSkillIntoBrain(SkillRef skill)
{
    Client *c = m_factory.entity_list.FindCharacter( itemID() );

    SkillRef oldSkill = GetSkill( skill->typeID() );
    if( oldSkill )
    {
        //TODO: build and send proper UserError for CharacterAlreadyKnowsSkill.
        if( c != NULL )
            c->SendNotifyMsg( "You already know this skill." );
        return false;
    }

    // TODO: based on config options later, check to see if another character, owned by this characters account,
    // is training a skill.  If so, return. (flagID=61).
    if( !skill->SkillPrereqsComplete( *this ) )
    {
        // TODO: need to send back a response to the client.  need packet specs.
        _log( ITEM__TRACE, "%s (%u): Requested to train skill %u item %u but prereq not complete.", itemName().c_str(), itemID(), skill->typeID(), skill->itemID() );

        if( c != NULL )
            c->SendNotifyMsg( "Injection failed!  Skill prerequisites incomplete." );
        return false;
    }

    // are we injecting from a stack of skills?
    if( skill->quantity() > 1 )
    {
        // split the stack to obtain single item
        InventoryItemRef single_skill = skill->Split( 1 );
        if( !single_skill )
        {
            _log( ITEM__ERROR, "%s (%u): Unable to split stack of %s (%u).", itemName().c_str(), itemID(), skill->itemName().c_str(), skill->itemID() );
            return false;
        }

        // use single_skill ...
        single_skill->MoveInto( *this, flagSkill );
    }
    else
        // use original skill
        skill->MoveInto( *this, flagSkill );

    if( c != NULL )
        c->SendNotifyMsg( "Injection of skill complete." );
    return true;
}

bool Character::InjectSkillIntoBrain(SkillRef skill, uint8 level)
{
    Client *c = m_factory.entity_list.FindCharacter( itemID() );
	

    SkillRef oldSkill = GetSkill( skill->typeID() );
    if( oldSkill )
    {

		oldSkill->attributes.SetNotify(true);
		oldSkill->Set_skillLevel( level );
		oldSkill->Set_skillPoints( pow(2, ( 2.5 * level ) - 2.5 ) * SKILL_BASE_POINTS * ( oldSkill->attributes.GetInt( oldSkill->attributes.Attr_skillTimeConstant ) ) );

		return true;
    }

    // are we injecting from a stack of skills?
    if( skill->quantity() > 1 )
    {
        // split the stack to obtain single item
        InventoryItemRef single_skill = skill->Split( 1 );
        if( !single_skill )
        {
            _log( ITEM__ERROR, "%s (%u): Unable to split stack of %s (%u).", itemName().c_str(), itemID(), skill->itemName().c_str(), skill->itemID() );
            return false;
        }

        // use single_skill ...
        single_skill->MoveInto( *this, flagSkill );
    }
    else
		skill->MoveInto( *this, flagSkill );

	skill->Set_skillLevel( level );
	//TODO: get right number of skill points
	//skill->Set_skillPoints( SKILL_BASE_POINTS * skillTimeConstant() * pow(32, (level - 1) / 2.0) );
	skill->Set_skillPoints( pow(2,( 2.5 * level) - 2.5 ) * SKILL_BASE_POINTS * ( skill->attributes.GetInt( skill->attributes.Attr_skillTimeConstant ) ) );
	
	
    return true;
}



void Character::AddToSkillQueue(uint32 typeID, uint8 level)
{
    QueuedSkill qs;
    qs.typeID = typeID;
    qs.level = level;

    m_skillQueue.push_back( qs );
}

void Character::ClearSkillQueue()
{
    m_skillQueue.clear();
}

void Character::UpdateSkillQueue()
{
    Client *c = m_factory.entity_list.FindCharacter( itemID() );

    SkillRef currentTraining = GetSkillInTraining();
    if( currentTraining )
    {
        if( m_skillQueue.empty()
            || currentTraining->typeID() != m_skillQueue.front().typeID )
        {
            // either queue is empty or skill with different typeID is in training ...
            // stop training:
            _log( ITEM__ERROR, "%s (%u): Stopping training of skill %s (%u).", itemName().c_str(), itemID(), currentTraining->itemName().c_str(), currentTraining->itemID() );

            uint64 timeEndTrain = currentTraining->expiryTime();
            if(timeEndTrain != 0)
            {
                double nextLevelSP = currentTraining->GetSPForLevel( currentTraining->skillLevel() + 1 );
                double SPPerMinute = GetSPPerMin( currentTraining );
                double minRemaining = (double)(timeEndTrain - Win32TimeNow()) / (double)Win32Time_Minute;

                currentTraining->Set_skillPoints( nextLevelSP - (minRemaining * SPPerMinute) );
            }

            currentTraining->Clear_expiryTime();

            currentTraining->MoveInto( *this, flagSkill, true );

            if( c != NULL )
            {
                OnSkillTrainingStopped osst;
                osst.itemID = currentTraining->itemID();
                osst.endOfTraining = 0;

                PyTuple* tmp = osst.Encode();
                c->QueueDestinyEvent( &tmp );
                PySafeDecRef( tmp );

                c->UpdateSkillTraining();
            }

            // nothing currently in training
            currentTraining = SkillRef();
        }
    }

    uint64 nextStartTime = Win32TimeNow();
    while( !m_skillQueue.empty() )
    {
        if( !currentTraining )
        {
            // something should be trained, get desired skill
            uint32 skillTypeID = m_skillQueue.front().typeID;

            currentTraining = GetSkill( skillTypeID );
            if( !currentTraining )
            {
                _log( ITEM__ERROR, "%s (%u): Skill %u to train was not found.", itemName().c_str(), itemID(), skillTypeID );
                break;
            }

            _log( ITEM__TRACE, "%s (%u): Starting training of skill %s (%u).", m_itemName.c_str(), m_itemID, currentTraining->itemName().c_str(), currentTraining->itemID() );

            double SPPerMinute = GetSPPerMin( currentTraining );
            double SPToNextLevel = currentTraining->GetSPForLevel( currentTraining->skillLevel() + 1 ) - currentTraining->skillPoints();

            uint64 timeTraining = nextStartTime + Win32Time_Minute * SPToNextLevel / SPPerMinute;

            currentTraining->MoveInto( *this, flagSkillInTraining );
            currentTraining->Set_expiryTime( timeTraining );

            if( c != NULL )
            {
                OnSkillStartTraining osst;
                osst.itemID = currentTraining->itemID();
                osst.endOfTraining = timeTraining;

                PyTuple* tmp = osst.Encode();
                c->QueueDestinyEvent( &tmp );
                PySafeDecRef( tmp );

                c->UpdateSkillTraining();
            }
        }

        if( currentTraining->expiryTime() <= Win32TimeNow() )
        {
            // training has been finished:
            _log( ITEM__ERROR, "%s (%u): Finishing training of skill %s (%u).", itemName().c_str(), itemID(), currentTraining->itemName().c_str(), currentTraining->itemID() );

            currentTraining->Set_skillLevel( currentTraining->skillLevel() + 1 );
            currentTraining->Set_skillPoints( currentTraining->GetSPForLevel( currentTraining->skillLevel() ) );

            nextStartTime = currentTraining->expiryTime();
            currentTraining->Clear_expiryTime();

            currentTraining->MoveInto( *this, flagSkill, true );

            if( c != NULL )
            {
                OnSkillTrained ost;
                ost.itemID = currentTraining->itemID();

                PyTuple* tmp = ost.Encode();
                c->QueueDestinyEvent( &tmp );
                PySafeDecRef( tmp );

                c->UpdateSkillTraining();
            }

            // erase first element in skill queue
            m_skillQueue.erase( m_skillQueue.begin() );

            // nothing currently in training
            currentTraining = SkillRef();
        }
        // else the skill is in training ...
        else
            break;
    }

    SaveSkillQueue();
}

PyObject *Character::CharGetInfo() {
    //TODO: verify that we are a char?

    if( !LoadContents( m_factory ) ) {
        codelog(ITEM__ERROR, "%s (%u): Failed to load contents for CharGetInfo", m_itemName.c_str(), m_itemID);
        return NULL;
    }

    Rsp_CommonGetInfo result;
    Rsp_CommonGetInfo_Entry entry;

    //first encode self.
    if(!Populate(entry))
        return NULL;    //print already done.

    result.items[m_itemID] = entry.Encode();

    //now encode skills...
    std::vector<InventoryItemRef> skills;
    //find all the skills contained within ourself.
    FindByFlag( flagSkill, skills );
    FindByFlag( flagSkillInTraining, skills );

    //encode an entry for each one.
    std::vector<InventoryItemRef>::iterator cur, end;
    cur = skills.begin();
    end = skills.end();
    for(; cur != end; cur++) {
        if(!(*cur)->Populate(entry)) {
            codelog(ITEM__ERROR, "%s (%u): Failed to load skill item %u for CharGetInfo", m_itemName.c_str(), itemID(), (*cur)->itemID());
        } else {
            result.items[(*cur)->itemID()] = entry.Encode();
        }
    }

    return(result.Encode());
}

PyObject *Character::GetDescription() const
{
    util_Row row;

    row.header.push_back("description");

    row.line = new PyList;
    row.line->AddItemString( description().c_str() );

    return row.Encode();
}

PyList *Character::GetSkillQueue() {
    // return skills from skill queue
    PyList *list = new PyList;

    SkillQueue::iterator cur, end;
    cur = m_skillQueue.begin();
    end = m_skillQueue.end();
    for(; cur != end; cur++)
    {
        SkillQueue_Element el;

        el.typeID = cur->typeID;
        el.level = cur->level;

        list->AddItem( el.Encode() );
    }

    return list;
}

void Character::AddItem(InventoryItemRef item)
{
    Inventory::AddItem( item );

    if( item->flag() == flagSkill
        || item->flag() == flagSkillInTraining )
    {
        // Skill has been added ...
        if( item->categoryID() != EVEDB::invCategories::Skill ) {
            _log( ITEM__WARNING, "%s (%u): %s has been added with flag %d.", itemName().c_str(), itemID(), item->category().name().c_str(), (int)item->flag() );
        } else
        {
            SkillRef skill = SkillRef::StaticCast( item );

            if( !skill->singleton() )
            {
                _log( ITEM__TRACE, "%s (%u): Injecting %s.", itemName().c_str(), itemID(), item->itemName().c_str() );

                // Make it singleton and set initial skill values.
                skill->ChangeSingleton( true );

                skill->Set_skillLevel( 0 );
                skill->Set_skillPoints( 0 );

                if( skill->flag() != flagSkillInTraining )
                    skill->Clear_expiryTime();
            }
        }
    }
}

void Character::SaveCharacter() const
{
    _log( ITEM__TRACE, "Saving character %u.", itemID() );

    // character data
    m_factory.db().SaveCharacter(
        itemID(),
        CharacterData(
            accountID(),
            title().c_str(),
            description().c_str(),
            gender(),
            bounty(),
            balance(),
            securityRating(),
            logonMinutes(),
            corporationID(),
            allianceID(),
            stationID(),
            solarSystemID(),
            constellationID(),
            regionID(),
            ancestryID(),
            careerID(),
            schoolID(),
            careerSpecialityID(),
            startDateTime(),
            createDateTime(),
            corporationDateTime()
        )
    );

    // corporation data
    m_factory.db().SaveCorpMemberInfo(
        itemID(),
        CorpMemberInfo(
            corporationHQ(),
            corpRole(),
            rolesAtAll(),
            rolesAtBase(),
            rolesAtHQ(),
            rolesAtOther()
        )
    );
}

void Character::SaveSkillQueue() const {
    _log( ITEM__TRACE, "Saving skill queue of character %u.", itemID() );

    // skill queue
    m_factory.db().SaveSkillQueue(
        itemID(),
        m_skillQueue
    );
}



