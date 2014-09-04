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

#include "Precompiled.h"

class npc_hordevendor : public CreatureScript
{
public:
    npc_hordevendor() : CreatureScript("npc_hordevendor") {}

    bool GossipHello(Player* pPlayer, Creature* pCreature)
    {
        return (pPlayer->GetOTeam() == ALLIANCE);
    }
};

class npc_alliancevendor : public CreatureScript
{
public:
    npc_alliancevendor() : CreatureScript("npc_alliancevendor") {}

    bool GossipHello(Player* pPlayer, Creature* pCreature)
    {
        return (pPlayer->GetOTeam() == HORDE);
    }
};

void AddSC_factionvendors()
{
    new npc_hordevendor;
    new npc_alliancevendor;
}