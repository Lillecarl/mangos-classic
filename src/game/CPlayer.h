#ifndef _NEWPLAYER_H
#define _NEWPLAYER_H

#include "Player.h"
#include "ObjectGuid.h"
#include "GossipDef.h"

enum MessageTypes
{
    CHAT_BOX,
    CHAT_WIDE,
    CHAT_BOTH
};

typedef std::map<ObjectGuid, float> DMGHEALMap;
typedef std::vector<std::string> RewardMap;

class CPlayer : public Player
{
public:
    explicit CPlayer(WorldSession* session);
    ~CPlayer() {};

    Player* ToPlayer() { return static_cast<Player*>(this); }

    /************************************************************************/
    /********************************PVPKILLREWARD***************************/
    /************************************************************************/
public:
    void HandlePvPKill();
    void AddDamage(ObjectGuid guid, uint32 amount);
    void AddHealing(ObjectGuid guid, uint32 amount);
    DMGHEALMap& GetDamagers() { return m_Damagers; }
    DMGHEALMap& GetHealers() { return m_Healers; }
    void AddReward(std::string name, float amount);
    std::string GetRewardNames(bool duplicates = true);
    static std::string GetGoldString(uint32 copper);
private:
    DMGHEALMap m_Damagers;
    DMGHEALMap m_Healers;
    RewardMap m_Rewards;
    float m_PendingReward;

    /************************************************************************/
    /*************************************CFBG*******************************/
    /************************************************************************/
public:
    typedef std::vector<ObjectGuid> FakedPlayers;

    bool NativeTeam() const;
    uint8 getFRace() const { return m_fRace; }
    uint8 getORace() const { return m_oRace; }
    uint32 getOFaction() const { return m_oFaction; }
    uint32 getFFaction() const { return m_fFaction; }
    uint32 getOPlayerBytes() const { return m_oPlayerBytes; }
    uint32 getFPlayerBytes() const { return m_fPlayerBytes; }
    uint32 getOPlayerBytes2() const { return m_oPlayerBytes2; }
    uint32 getFPlayerBytes2() const { return m_fPlayerBytes2; }

    void SetFakeValues();
    void RecachePlayersFromList();
    void RecachePlayersFromBG();
    WorldPacket BuildNameQuery();
    bool GetRecache() { return m_Recache; }
    void SetRecache() { m_Recache = true; }
    void SetFakedPlayers(FakedPlayers guidlist) { m_FakedPlayers = guidlist; }

    void JoinBattleGround(BattleGround* bg);
    void LeaveBattleGround(BattleGround* bg);

    void FakeDisplayID();

    void SetFakeOnNextTick(bool value = true) { m_FakeOnNextTick = value; }
    bool GetFakeOnNextTick() { return m_FakeOnNextTick; }

    bool SendBattleGroundChat(ChatMsg msgtype, std::string message);

    void Sometimes();

private:
    uint8 m_fRace;
    uint8 m_oRace;
    uint32 m_fFaction;
    uint32 m_oFaction;
    uint32 m_oPlayerBytes;
    uint32 m_oPlayerBytes2;
    uint32 m_fPlayerBytes;
    uint32 m_fPlayerBytes2;

    FakedPlayers m_FakedPlayers;

    bool m_Recache;
    bool m_FakeOnNextTick;

    /************************************************************************/
    /*************************************Misc*******************************/
    /************************************************************************/
public:
    typedef std::vector<uint32> DelayedSpellLearn;

    std::stringstream BoxChat;
    std::stringstream WideChat;
    std::stringstream BothChat;

    void SendSavedChat(MessageTypes type, std::stringstream &ss);

    void CUpdate(uint32 diff);

    std::string GetNameLink(bool applycolors = false);

    uint32 GetAVGILevel(bool levelasmin = false);

    void SetScriptID(uint32 id) { m_ScriptID = id; }
    uint32 GetScriptID() { return m_ScriptID; }

    void AddGossipMenuItem(Icon::Icon icon, std::string message, uint32 sender, uint32 action)
    {
        PlayerTalkClass->GetGossipMenu().AddMenuItem(icon, message.c_str(), sender, action, "", 0);
    }

    void LearnGreenSpells();
    void FillGreenSpellList();

private:
    uint32 m_ScriptID;


    DelayedSpellLearn m_DelayedSpellLearn;
};

#endif
