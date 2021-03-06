//no macroguard on purpose

COMMAND( create, ROLE_ADMIN,
         "(itemID) [count] - Create count or 1 of the specified item." )
COMMAND( search, ROLE_ADMIN,
         "(text) - Search for items matching the specified query" )
COMMAND( translocate, ROLE_ADMIN,
         "(entityID) - Translocate to the specified entity" )
COMMAND( tr, ROLE_ADMIN,
         "(who) (entityID) - Translocate the specified person to the specified entity" )
COMMAND( giveisk, ROLE_ADMIN,
         "(entityID) (amount) - Give the specified amount of cash to the specified character. 0=self." )
COMMAND( pop, ROLE_ADMIN,
         "(type) (key) (value) - Send an OnRemoteMessage" )
COMMAND( goto, ROLE_ADMIN,
         "(x) (y) (z) - Jump to the specified position in space. Stopped." )
COMMAND( spawn, ROLE_ADMIN,
         "(typeID) - Spawn an NPC with the specified type." )
COMMAND( location, ROLE_PLAYER,
         "- Gives you back your current location in space." )
COMMAND( syncloc, ROLE_PLAYER,
         "- Synchonizes your location in client with location on server." )
COMMAND( setbpattr, ROLE_ADMIN,
         "(itemID) (copy) (materialLevel) (productivityLevel) (licensedProductionRunsRemaining) - Change blueprint's attributes." )
COMMAND( state, ROLE_ADMIN,
         "- Sends DoDestinyUpdate SetState." )
COMMAND( getattr, ROLE_ADMIN,
         "(itemID) (attributeID) - Retrieves attribute value." )
COMMAND( setattr, ROLE_ADMIN,
         "(itemID) (attributeID) (value) - Sets attributeID of itemID to value." )
COMMAND( fit, ROLE_ADMIN,
		"(itemID) - Fits selected item to active ship." )
COMMAND( giveskill, ROLE_ADMIN,
		"(itemID) - gives skills to fit item." )
COMMAND( kick, ROLE_ADMIN,
		"(charName) - kicks [charName] from the server")
COMMAND( ban, ROLE_ADMIN,
		"(charName) - bans player's account from the server")
COMMAND( unban, ROLE_ADMIN,
		"(charName) - removes ban on player's account")



