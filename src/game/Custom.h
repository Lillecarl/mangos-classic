#ifndef _CUSTOM_H
#define _CUSTOM_H

#define MSG_COLOR_LIGHTRED     "|cffff6060"
#define MSG_COLOR_LIGHTBLUE    "|cff00ccff"
#define MSG_COLOR_ANN_GREEN    "|c1f40af20"
#define MSG_COLOR_RED          "|cffff0000"
#define MSG_COLOR_GOLD         "|cffffcc00"
#define MSG_COLOR_SUBWHITE     "|cffbbbbbb"
#define MSG_COLOR_MAGENTA      "|cffff00ff"
#define MSG_COLOR_YELLOW       "|cffffff00"
#define MSG_COLOR_CYAN         "|cff00ffff"
#define MSG_COLOR_DARKBLUE     "|cff0000ff"

#define MSG_COLOR_GREY         "|cff9d9d9d"
#define MSG_COLOR_WHITE        "|cffffffff"
#define MSG_COLOR_GREEN        "|cff1eff00"
#define MSG_COLOR_BLUE         "|cff0080ff"
#define MSG_COLOR_PURPLE       "|cffb048f8"
#define MSG_COLOR_ORANGE       "|cffff8000"

#define MSG_COLOR_DRUID        "|cffff7d0a"
#define MSG_COLOR_HUNTER       "|cffabd473"
#define MSG_COLOR_MAGE         "|cff69ccf0"
#define MSG_COLOR_PALADIN      "|cfff58cba"
#define MSG_COLOR_PRIEST       "|cffffffff"
#define MSG_COLOR_ROGUE        "|cfffff569"
#define MSG_COLOR_SHAMAN       "|cff0070de"
#define MSG_COLOR_WARLOCK      "|cff9482c9"
#define MSG_COLOR_WARRIOR      "|cffc79c6e"

#include <unordered_map>
#include "Creature.h"

struct FakePlayerBytes
{
    uint32 PlayerBytes[2];
    uint32 PlayerBytes2[2];
};

class CPlayer;
class ObjectGuid;

class Custom
{
public:
    Custom() { };
    ~Custom() {};

    typedef std::map<uint8, FakePlayerBytes> FakePlayerBytesContainer;
    typedef std::unordered_multimap<uint8, uint32> RefreshItems;
    typedef std::vector<TrainerSpell> SpellContainer;
    typedef std::map<uint32, SpellContainer*> CachedSpellContainer;

    static CPlayer* GetCPlayer(const char* name);                                 ///< Wrapper for ObjectAccessor::FindPlayerByName
    static CPlayer* GetCPlayer(ObjectGuid guid, bool inWorld = true);             ///< Wrapper for ObjectAccessor::FindPlayer

    std::string GetClassColor(uint8 classid) { return m_ClassColor[classid]; }

    SpellContainer GetSpellContainerByCreatureEntry(uint32 entry);
    SpellContainer* GetCachedSpellContainer(uint32 crval);

    void CacheSpellContainer(uint32 crval, SpellContainer* container)
    {
        m_CachedSpellContainer.insert(std::make_pair(crval, container));
    }

    void ClearCachedSpellContainer()
    {
        m_CachedSpellContainer.clear();
    }

    void LoadFakePlayerBytes();

    uint8 PickFakeRace(uint8 fallbackrace, uint8 pclass, Team team);

    uint32 GetFakePlayerBytes(uint8 race, uint8 gender)
    {
        if (m_FakePlayerBytesContainer.find(race) != m_FakePlayerBytesContainer.end())
            return m_FakePlayerBytesContainer[race].PlayerBytes[gender];

        return 0;
    }

    uint32 GetFakePlayerBytes2(uint8 race, uint8 gender)
    {
        if (m_FakePlayerBytesContainer.find(race) != m_FakePlayerBytesContainer.end())
            return m_FakePlayerBytesContainer[race].PlayerBytes2[gender];

        return 0;
    }

    void LoadRefreshItems();
    RefreshItems const& GetRefreshItems() { return m_RefreshItems; }

private:
    static const std::string m_ClassColor[];
    FakePlayerBytesContainer m_FakePlayerBytesContainer;
    RefreshItems m_RefreshItems;
    CachedSpellContainer m_CachedSpellContainer;
};

#define sCustom MaNGOS::Singleton<Custom>::Instance()

#endif
