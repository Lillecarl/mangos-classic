#include "CPlayer.h"
#include "Player.h"
#include "Custom.h"
#include "ObjectAccessor.h"

const std::string Custom::m_ClassColor[] =
{
    "",
    MSG_COLOR_WARRIOR,
    MSG_COLOR_PALADIN,
    MSG_COLOR_HUNTER,
    MSG_COLOR_ROGUE,
    MSG_COLOR_PRIEST,
    "",
    MSG_COLOR_SHAMAN,
    MSG_COLOR_MAGE,
    MSG_COLOR_WARLOCK,
    "",
    MSG_COLOR_DRUID
};

CPlayer* Custom::GetCPlayer(const char* name)
{
    Player* plr = ObjectAccessor::FindPlayerByName(name);
    if (!plr)
        return NULL;

    return static_cast<CPlayer*>(plr);
}
CPlayer* Custom::GetCPlayer(ObjectGuid guid, bool inWorld /*=true*/)
{
    Player* plr = ObjectAccessor::FindPlayer(guid, inWorld);
    if (!plr)
        return NULL;

    return static_cast<CPlayer*>(plr);
}
