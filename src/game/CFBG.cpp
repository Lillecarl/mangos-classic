/*
* See AUTHORS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "World.h"
#include "Custom.h"
#include "ObjectMgr.h"
#include "DBCStores.h"
#include "Player.h"
#include "BattleGround/BattleGround.h"
#include "BattleGround/BattleGroundMgr.h"
#include "CPlayer.h"

Team Player::GetTeam() const
{
    if (GetBattleGround())
        return m_bgData.bgTeam ? m_bgData.bgTeam : GetOTeam();

    return GetOTeam();
}

bool CPlayer::NativeTeam() const
{
    return GetTeam() == GetOTeam();
}

void CPlayer::SetFakeValues()
{
    m_oRace = GetByteValue(UNIT_FIELD_BYTES_0, 0);
    m_oFaction = GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE);

    m_fRace = sCustom.PickFakeRace(getORace(), getClass(), GetOTeam());
    m_fFaction = Player::getFactionForRace(m_fRace);

    m_oPlayerBytes = GetUInt32Value(PLAYER_BYTES);
    m_oPlayerBytes2 = GetUInt32Value(PLAYER_BYTES_2);
    m_fPlayerBytes = sCustom.GetFakePlayerBytes(m_fRace, getGender());
    m_fPlayerBytes2 = sCustom.GetFakePlayerBytes2(m_fRace, getGender());

    if (!m_fPlayerBytes)
        m_fPlayerBytes = m_oPlayerBytes;

    if (!m_fPlayerBytes2)
        m_fPlayerBytes2 = m_oPlayerBytes2;
}

void CPlayer::RecachePlayersFromBG()
{
    BattleGround* bg = GetBattleGround();

    if (!bg)
        return;

    for (auto& itr : bg->GetPlayers())
    {
        if (Player* player = sObjectMgr.GetPlayer(itr.first))
        {
            if (!player->ToCPlayer()->NativeTeam())
            {
                // Erase bg players from source player cache
                WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
                data << player->GetObjectGuid();
                GetSession()->SendPacket(&data);
                // Send bg player data to source player
                data = player->ToCPlayer()->BuildNameQuery();
                GetSession()->SendPacket(&data);
            }

            if (!NativeTeam())
            {
                // Erase source player from bg players cache
                WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
                data << GetObjectGuid();
                player->GetSession()->SendPacket(&data);
                // Send new source data to bg players
                data = BuildNameQuery();
                player->GetSession()->SendPacket(&data);
            }
        }
        else
        {
            // Couldn't find bgplayer, recache him for source player in case he logs in again.
            WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
            data << itr.first;
            GetSession()->SendPacket(&data);
        }
    }
}

void CPlayer::RecachePlayersFromList()
{
    for (auto& itr : m_FakedPlayers)
    {
        WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
        data << itr;
        GetSession()->SendPacket(&data);

        if (Player* player = sObjectMgr.GetPlayer(itr))
        {
            WorldPacket data = player->ToCPlayer()->BuildNameQuery();
            GetSession()->SendPacket(&data);
        }
    }

    m_FakedPlayers.clear();
}

WorldPacket CPlayer::BuildNameQuery()
{
    WorldPacket data(SMSG_NAME_QUERY_RESPONSE, (8 + 1 + 4 + 4 + 4 + 10));
    data << GetObjectGuid();      // player guid
    data << GetName();            // player name
    data << uint8(0);                       // realm name
    data << uint32(getRace());    // player race
    data << uint32(getGender());  // player gender
    data << uint32(getClass());   // player class
    data << uint8(0);                       // is not declined

    return data;
}

void CPlayer::FakeDisplayID()
{
    if (NativeTeam())
        return;

    PlayerInfo const* info = sObjectMgr.GetPlayerInfo(getRace(), getClass());
    if (!info)
    {
        for (auto i = 0; i < MAX_CLASSES; ++i)
        {
            info = sObjectMgr.GetPlayerInfo(getRace(), i);
            if (info)
                break;
        }
    }

    if (!info)
    {
        sLog.outError("Player %u has incorrect race/class pair. Can't init display ids.", GetGUIDLow());
        return;
    }

    // reset scale before reapply auras
    SetObjectScale(DEFAULT_OBJECT_SCALE);

    uint8 gender = getGender();
    switch (gender)
    {
    case GENDER_FEMALE:
        SetDisplayId(info->displayId_f);
        SetNativeDisplayId(info->displayId_f);
        break;
    case GENDER_MALE:
        SetDisplayId(info->displayId_m);
        SetNativeDisplayId(info->displayId_m);
        break;
    default:
        sLog.outError("Invalid gender %u for player", gender);
        return;
    }

    SetUInt32Value(PLAYER_BYTES, getFPlayerBytes());
    SetUInt32Value(PLAYER_BYTES_2, getFPlayerBytes2());
}

void CPlayer::JoinBattleGround(BattleGround* bg)
{
    if (!NativeTeam())
    {
        m_FakedPlayers.push_back(GetObjectGuid());
        SetByteValue(UNIT_FIELD_BYTES_0, 0, getFRace());
        setFaction(getFFaction());
    }

    SetRecache();
    FakeDisplayID();
}

void CPlayer::LeaveBattleGround(BattleGround* bg)
{
    SetByteValue(UNIT_FIELD_BYTES_0, 0, getORace());
    setFaction(getOFaction());
    InitDisplayIds();

    SetFakedPlayers(m_FakedPlayers);
    SetRecache();

    SetUInt32Value(PLAYER_BYTES, getOPlayerBytes());
    SetUInt32Value(PLAYER_BYTES_2, getOPlayerBytes2());
}

bool CPlayer::SendBattleGroundChat(ChatMsg msgtype, std::string message)
{
    // Select distance to broadcast to.
    float distance = sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_SAY);

    if (msgtype == CHAT_MSG_YELL)
        sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_YELL);
    else if (msgtype == CHAT_MSG_EMOTE)
        sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE);

    BattleGround* pBattleGround = GetBattleGround();

    if (!pBattleGround) // Only fake chat in BG's. CFBG should not interfere with arenas.
        return false;

    for (auto& itr : pBattleGround->GetPlayers())
    {
        if (Player* pPlayer = sObjectMgr.GetPlayer(itr.first))
        {
            if (GetDistance2d(pPlayer->GetPositionX(), pPlayer->GetPositionY()) <= distance)
            {
                WorldPacket data(SMSG_MESSAGECHAT, 200);

                if (GetTeam() == pPlayer->GetTeam())
                    ChatHandler::BuildChatPacket(data, msgtype, message.c_str(), LANG_UNIVERSAL, GetChatTag(), GetObjectGuid(), GetName());
                else if (msgtype != CHAT_MSG_EMOTE)
                    ChatHandler::BuildChatPacket(data, msgtype, message.c_str(), pPlayer->GetOTeam() == ALLIANCE ? LANG_ORCISH : LANG_COMMON, GetChatTag(), GetObjectGuid(), GetName());

                pPlayer->GetSession()->SendPacket(&data);
            }
        }
    }
    return true;
}

bool BattleGroundQueue::CheckMixedMatch(BattleGround* bg_template, BattleGroundBracketId bracket_id, uint32 minPlayers, uint32 maxPlayers)
{
    if (!sWorld.getConfig(CONFIG_BOOL_CFBG_ENABLED))
        return false;

    // Empty selection pool, we do not want old data.
    m_SelectionPools[BG_TEAM_ALLIANCE].Init();
    m_SelectionPools[BG_TEAM_HORDE].Init();

    uint32 addedally = 0;
    uint32 addedhorde = 0;
    uint32 ilevelally = 0;
    uint32 ilevelhorde = 0;

    std::multimap<uint32, GroupQueueInfo*> ItemLevelSorting;

    for (auto& itr : m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE])
    {
        uint32 GroupIlevel = 0;

        for (auto& jtr : itr->Players)
        if (Player* pPlayer = sObjectMgr.GetPlayer(jtr.first))
            GroupIlevel += pPlayer->ToCPlayer()->GetAVGILevel(true);

        ItemLevelSorting.insert(std::make_pair(GroupIlevel, itr));
    }

    for (auto itr = ItemLevelSorting.crbegin(); itr != ItemLevelSorting.crend(); ++itr)
    {
        GroupQueueInfo* ginfo = itr->second;

        if (!ginfo->IsInvitedToBGInstanceGUID)
        {
            ginfo->GroupTeam = TEAM_NONE;

            if (ilevelally == ilevelhorde && addedally == addedhorde)
                urand(0, 1) ? ginfo->GroupTeam = ALLIANCE : ginfo->GroupTeam = HORDE;
            else if (ilevelally == ilevelhorde && addedally != addedhorde)
                addedally < addedhorde ? ginfo->GroupTeam = ALLIANCE : ginfo->GroupTeam = HORDE;

            if (ginfo->GroupTeam == TEAM_NONE)
                ilevelally < ilevelhorde ? ginfo->GroupTeam = ALLIANCE : ginfo->GroupTeam = HORDE;

            uint32 GroupIlevel = 0;

            for (auto& jtr : ginfo->Players)
            if (Player* pPlayer = sObjectMgr.GetPlayer(jtr.first))
                GroupIlevel += pPlayer->ToCPlayer()->GetAVGILevel(true);

            if (m_SelectionPools[ginfo->GroupTeam == ALLIANCE ? BG_TEAM_ALLIANCE : BG_TEAM_HORDE].AddGroup(ginfo, maxPlayers))
            {
                ginfo->GroupTeam == ALLIANCE ? ilevelally += GroupIlevel : ilevelhorde += GroupIlevel;
                ginfo->GroupTeam == ALLIANCE ? addedally += GroupIlevel : addedhorde += GroupIlevel;
            }
            else
                break;

            if (m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount() >= minPlayers &&
                m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount() >= minPlayers)
                return true;
        }
    }

    if (sBattleGroundMgr.isTesting() &&
        (m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount() ||
        m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount()))
        return true;

    return false;
}

bool BattleGroundQueue::MixPlayersToBG(BattleGround* bg, BattleGroundBracketId bracket_id)
{
    if (!sWorld.getConfig(CONFIG_BOOL_CFBG_ENABLED))
        return false;

    int32 allyFree = bg->GetFreeSlotsForTeam(ALLIANCE);
    int32 hordeFree = bg->GetFreeSlotsForTeam(HORDE);

    uint32 addedally = bg->GetMaxPlayersPerTeam() - bg->GetFreeSlotsForTeam(ALLIANCE);
    uint32 addedhorde = bg->GetMaxPlayersPerTeam() - bg->GetFreeSlotsForTeam(HORDE);

    for (auto& itr : m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE])
    {
        GroupQueueInfo* ginfo = itr;
        if (!ginfo->IsInvitedToBGInstanceGUID)
        {
            bool makeally = addedally < addedhorde;

            if (addedally == addedhorde)
                makeally = urand(0, 1);

            makeally ? ++addedally : ++addedhorde;

            ginfo->GroupTeam = makeally ? ALLIANCE : HORDE;

            if (m_SelectionPools[makeally ? BG_TEAM_ALLIANCE : BG_TEAM_HORDE].AddGroup(ginfo, makeally ? allyFree : hordeFree))
                makeally ? addedally += ginfo->Players.size() : addedhorde += ginfo->Players.size();
            else
                break;
        }
    }

    return true;
}