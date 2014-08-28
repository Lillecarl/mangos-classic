#include "CPlayer.h"
#include "Custom.h"

CPlayer::CPlayer(WorldSession* session) : Player(session)
{
    m_PendingReward = 0;

    m_fRace = 0;
    m_oRace = 0;
    m_fFaction = 0;
    m_oFaction = 0;
    m_oPlayerBytes = 0;
    m_oPlayerBytes2 = 0;
    m_fPlayerBytes = 0;
    m_fPlayerBytes2 = 0;
    m_Recache = 0;
    m_FakeOnNextTick = 0;
    m_ScriptID = 0;
}

void CPlayer::CUpdate(uint32 diff)
{
    SendSavedChat(CHAT_BOX, BoxChat);
    SendSavedChat(CHAT_WIDE, WideChat);
    SendSavedChat(CHAT_BOTH, BothChat);

    for (auto& i : sCustom.GetRefreshItems())
    {
        if (!(i.first == getClass() || i.first == 0))
            continue;

        if (GetItemCount(i.second) < 2)
            StoreNewItemInBestSlots(i.second, 1);
    }
}

void CPlayer::SendSavedChat(MessageTypes type, std::stringstream &ss)
{
    if (!ss.str().empty())
    {
        std::string item;

        while (std::getline(ss, item))
        {
            const char* msg = item.c_str();

            if (type == CHAT_BOX || type == CHAT_BOTH)
                ChatHandler(GetSession()).SendSysMessage(msg);
            if (type == CHAT_WIDE || type == CHAT_BOTH)
            {
                WorldPacket data(SMSG_NOTIFICATION, (strlen(msg) + 1));
                data << msg;
                GetSession()->SendPacket(&data);
            }
        }

        ss.str(""); // Clear content
        ss.clear(); // Clear state flags.
    }
}

std::string CPlayer::GetNameLink(bool applycolors)
{
    std::string name = GetName();
    std::string teamcolor = GetTeam() == ALLIANCE ? MSG_COLOR_DARKBLUE : MSG_COLOR_RED;
    std::ostringstream ss;

    if (isGameMaster())
        teamcolor = MSG_COLOR_PURPLE;

    ss << "|Hplayer:" << name << "|h";

    if (applycolors)
        ss << teamcolor << "[" << sCustom.GetClassColor(getClass()) << name << teamcolor << "]|h";
    else
        ss << "[" << name << "]|h";

    return ss.str();
}

uint32 CPlayer::GetAVGILevel(bool levelasmin)
{
    uint32 TotLevel = 0;
    uint8 ItemCount = 0;

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        // Ignore these, because either they do not matter to the calculation
        // or they're allowed to be swapped during the game.
        if (i == EQUIPMENT_SLOT_MAINHAND || i == EQUIPMENT_SLOT_OFFHAND ||
            i == EQUIPMENT_SLOT_RANGED || i == EQUIPMENT_SLOT_TABARD ||
            i == EQUIPMENT_SLOT_BODY)
            continue;

        ++ItemCount;

        if (Item* pItem = GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            TotLevel += pItem->GetProto()->ItemLevel;
    }

    uint32 avg = TotLevel / ItemCount;

    if (!levelasmin)
        return avg;

    return (avg > getLevel() ? avg : getLevel());
}

void CPlayer::Sometimes()
{
    if (GetRecache())
    {
        RecachePlayersFromList();
        RecachePlayersFromBG();
    }

    if (GetFakeOnNextTick())
    {
        SetFakeOnNextTick(false);

        SetByteValue(UNIT_FIELD_BYTES_0, 0, getFRace());
        setFaction(getFFaction());
        FakeDisplayID();

        SetUInt32Value(PLAYER_BYTES, getFPlayerBytes());
        SetUInt32Value(PLAYER_BYTES_2, getFPlayerBytes2());
    }
}
