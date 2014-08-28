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

#ifndef _CPLUSMGR_H
#define _CPLUSMGR_H

#include "Policies/Singleton.h"
#include "Util.h"
#include "Log.h"
#include "Database/DatabaseEnv.h"

#include <typeinfo>

class Creature;
class CreatureAI;
class Player;
class GameObject;
class Quest;
class Item;
class SpellCastTargets;
class ObjectGuid;

template<class T>
class ScriptStorage
{
public:
    ScriptStorage() {};
    ~ScriptStorage() {};

    void Add(T* script)
    {
        T* result = nullptr;

        for (auto& itr : m_RegisteredScripts)
        {
            if (itr->GetName() == script->GetName())
            {
                result = itr;
                break;
            }
        }

        if (result)
            sLog.outString("ScriptStorage: Trying to double register %s!", script->GetName().c_str());
        else
        {
            sLog.outString("ScriptStorage: registering script %s type %s!", script->GetName().c_str(), typeid(T).name());
            m_RegisteredScripts.push_back(script);
        }
    }

    T* GetScript(uint32 id)
    {
        if (m_ScriptMap.find(id) != m_ScriptMap.end())
            return m_ScriptMap[id];
        else
            sLog.outString("Trying to reach script with id %u type %s", id, typeid(T).name());

        return nullptr;
    }

    void LoadScriptNames(std::string colname, std::string tablename)
    {
        char buffer[255];
        sprintf(buffer, "SELECT %s, ScriptName FROM %s WHERE ScriptName <> ''", colname.c_str(), tablename.c_str());

        sLog.outString(buffer);

        QueryResult* result = WorldDatabase.Query(buffer);
        if (!result)
            return;

        do
        {
            Field* fields = result->Fetch();
            uint32 entry = fields[0].GetUInt32();
            std::string name = fields[1].GetCppString();

            sLog.outString("Trying to register %s %u to scriptname %s", tablename.c_str(), entry, name.c_str());

            for (auto& itr : m_RegisteredScripts)
            {
                if (itr->GetName() == name)
                {
                    m_ScriptMap.insert(std::make_pair(entry, itr));
                    sLog.outString("Success!");
                    break;
                }
            }
        } while (result->NextRow());
    }

private:
    UNORDERED_MAP<uint32, T*> m_ScriptMap;
    std::vector<T*> m_RegisteredScripts;
};

class ScriptObject
{
public:
    ScriptObject(std::string name) : m_name(name) { }
    virtual ~ScriptObject() {}

    std::string GetName() { return m_name; }

private:
    std::string m_name;
};

void AddScripts();

class CreatureScript;
class GameObjectScript;
class ItemScript;
class PlayerScript;

class CPlusMgr
{
public:
    CPlusMgr() {};
    ~CPlusMgr() {};

    void LoadScripts()
    {
        AddScripts();

        m_CreatureScripts.LoadScriptNames("entry", "creature_template");
        m_GameObjectScripts.LoadScriptNames("entry", "gameobject_template");
        m_ItemScripts.LoadScriptNames("entry", "item_template");
        m_PlayerScripts.LoadScriptNames("entry", "playerscript_template");
    }

    /* CreatureScript */
    CreatureAI* GetCreatureAI(Creature* pCreature);
    bool OnGossipHello(Player* pPlayer, Creature* pCreature);
    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action, std::string code);
    bool OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const* pQuest);
    bool OnQuestRewarded(Player* pPlayer, Creature* pCreature, Quest const* pQuest);
    uint32 GetDialogStatus(Player* pPlayer, Creature* pCreature);

    /* GameObjectScript */

    bool OnGameObjectUse(Player* pPlayer, GameObject* pGameObject);
    bool OnGossipHello(Player* pPlayer, GameObject* pGameObject);
    bool OnGossipSelect(Player* pPlayer, GameObject* pGameObject, uint32 sender, uint32 action, std::string code);
    bool OnQuestAccept(Player* pPlayer, GameObject* pGameObject, Quest const* pQuest);
    bool OnQuestRewarded(Player* pPlayer, GameObject* pGameObject, Quest const* pQuest);
    uint32 GetDialogStatus(Player* pPlayer, GameObject* pGameObject);

    /* ItemScript */

    bool OnGossipSelect(Player* pPlayer, Item* pItem, uint32 sender, uint32 action, std::string code);
    bool OnQuestAccept(Player* pPlayer, Item* pItem, Quest const* pQuest);
    bool OnItemUse(Player* pPlayer, Item* pItem, SpellCastTargets const& targets);

    /* PlayerScript */

    bool OnGossipHello(Player* pPlayer);
    bool OnGossipHello(Player* pPlayer, uint32 scriptid);
    bool OnGossipSelect(Player* pPlayer, uint32 sender, uint32 action, std::string code);

public:
    void Add(CreatureScript* script) { m_CreatureScripts.Add(script); }
    void Add(GameObjectScript* script) { m_GameObjectScripts.Add(script); }
    void Add(ItemScript* script) { m_ItemScripts.Add(script); }
    void Add(PlayerScript* script) { m_PlayerScripts.Add(script); }

private:
    ScriptStorage<CreatureScript> m_CreatureScripts;
    ScriptStorage<GameObjectScript> m_GameObjectScripts;
    ScriptStorage<ItemScript> m_ItemScripts;
    ScriptStorage<PlayerScript> m_PlayerScripts;
};

#define sCPlusMgr MaNGOS::Singleton<CPlusMgr>::Instance()

class CreatureScript : public ScriptObject
{
protected:
    CreatureScript(std::string name) : ScriptObject(name) { sCPlusMgr.Add(this); }

public:
    virtual CreatureAI* GetCreatureAI(Creature* /*pCreature*/) { return nullptr; }
    virtual bool GossipHello(Player* /*pPlayer*/, Creature* /*pCreature*/) { return false; }
    virtual bool GossipSelect(Player* /*pPlayer*/, Creature* /*pCreature*/, uint32 /*sender*/, uint32 /*action*/, std::string /*code*/) { return false; }
    virtual bool QuestAccept(Player* /*pPlayer*/, Creature* /*pCreature*/, Quest const* /*pQuest*/) { return false; }
    virtual bool QuestRewarded(Player* /*pPlayer*/, Creature* /*pCreature*/, Quest const* /*pQuest*/) { return false; }
    virtual uint32 GetDialogStatus(Player* /*pPlayer*/, Creature* /*pCreature*/) { return 0; }
};

class GameObjectScript : public ScriptObject
{
protected:
    GameObjectScript(std::string name) : ScriptObject(name) { sCPlusMgr.Add(this); }

public:
    virtual bool GameObjectUse(Player* /*pPlayer*/, GameObject* /*pGameObject*/) { return false; }
    virtual bool GossipHello(Player* /*pPlayer*/, GameObject* /*pGameObject*/) { return false; }
    virtual bool GossipSelect(Player* /*pPlayer*/, GameObject* /*pGameObject*/, uint32 /*sender*/, uint32 /*action*/, std::string /*code*/) { return false; }
    virtual bool QuestAccept(Player* /*pPlayer*/, GameObject* /*pGameObject*/, Quest const* /*pQuest*/) { return false; }
    virtual bool QuestRewarded(Player* /*pPlayer*/, GameObject* /*pGameObject*/, Quest const* /*pQuest*/) { return false; }
    virtual uint32 GetDialogStatus(Player* /*pPlayer*/, GameObject* /*pGameObject*/) { return 0; }
};

class ItemScript : public ScriptObject
{
protected:
    ItemScript(std::string name) : ScriptObject(name) { sCPlusMgr.Add(this); }

public:
    virtual bool GossipSelect(Player* /*pPlayer*/, Item* /*pItem*/, uint32 /*sender*/, uint32 /*action*/, std::string /*code*/) { return false; }
    virtual bool QuestAccept(Player* /*pPlayer*/, Item* /*pItem*/, Quest const* /*pQuest*/) { return false; }
    virtual bool ItemUse(Player* /*pPlayer*/, Item* /*pItem*/, SpellCastTargets const& /*targets*/) { return false; }
};

class PlayerScript : public ScriptObject
{
protected:
    PlayerScript(std::string name) : ScriptObject(name) { sCPlusMgr.Add(this); }

public:
    ObjectGuid GetHearthStoneOrPlayerGuid(Player* pPlayer);

    virtual bool GossipHello(Player* /*pPlayer*/) { return false; }
    virtual bool GossipSelect(Player* /*pPlayer*/, uint32 /*sender*/, uint32 /*action*/, std::string /*code*/) { return false; }
};

#endif
