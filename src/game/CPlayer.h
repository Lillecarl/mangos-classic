#ifndef _NEWPLAYER_H
#define _NEWPLAYER_H

#include "Player.h"
#include "ObjectGuid.h"

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
    /*************************************Misc*******************************/
    /************************************************************************/
public:
    std::stringstream BoxChat;
    std::stringstream WideChat;
    std::stringstream BothChat;

    void SendSavedChat(MessageTypes type, std::stringstream &ss);

    void CUpdate(uint32 diff);

    std::string GetNameLink(bool applycolors = false);
private:
};

#endif
