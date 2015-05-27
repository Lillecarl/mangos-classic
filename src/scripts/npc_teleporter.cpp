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

void TeleportPlayerToTeleName(CPlayer* pCPlayer, std::string telename)
{
    auto tele = sObjectMgr.GetGameTele(telename);
    if (!tele)
    {
        pCPlayer->BoxChat << "This teleport location is broken, please report to devs! (Name: " << telename << ")" << std::endl;
        return;
    }

    pCPlayer->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
}

class npc_teleporter : public CreatureScript
{
public:
    npc_teleporter() : CreatureScript("npc_teleporter") {}

    bool GossipHello(Player* pPlayer, Creature* pCreature)
    {
        CPlayer* pCPlayer = pPlayer->ToCPlayer();

        if (pCPlayer->GetAreaId() != 2079)
            pCPlayer->AddGossipMenuItem(Icon::TAXI, "Teleport to Mall", 0, 1);
        if (pCPlayer->GetAreaId() != 1741 && pCPlayer->GetAreaId() != 2177)
            pCPlayer->AddGossipMenuItem(Icon::TAXI, "Teleport to Gurubashi Arena", 0, 2);
		if (pCPlayer->GetAreaId() != 190)
			pCPlayer->AddGossipMenuItem(Icon::TAXI, "Teleport to Gurubashi Arena", 0, 5);


        pCPlayer->AddGossipMenuItem(Icon::GEAR, "Reset talents", 0, 3);

        if (pCPlayer->getClass() == CLASS_HUNTER)
        pCPlayer->AddGossipMenuItem(Icon::GEAR, "Reset pet skills", 0, 4);

        if (!pPlayer->PlayerTalkClass->GetGossipMenu().Empty())
            pPlayer->PlayerTalkClass->SendGossipMenu("Your wish, my law!", pCreature->GetObjectGuid());
        return true;
    }

    bool GossipSelect(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action, std::string code)
    {
        CPlayer* pCPlayer = pPlayer->ToCPlayer();
        pCPlayer->PlayerTalkClass->CloseGossip();

        if (sender == 0)
        {
            if (action == 0)
                GossipHello(pPlayer, pCreature);
            else if (action == 1)
                TeleportPlayerToTeleName(pCPlayer, "tele_mall");
            else if (action == 2)
                TeleportPlayerToTeleName(pCPlayer, "tele_guru");
            else if (action == 3)
                pCPlayer->SendTalentWipeConfirm(pCreature->GetObjectGuid());
            else if (action == 4)
                pCPlayer->SendPetSkillWipeConfirm();
			else if (action == 5)
				TeleportPlayerToTeleName(pCPlayer, "tele_hearth");
        }


        if (!pPlayer->PlayerTalkClass->GetGossipMenu().Empty())
            pPlayer->PlayerTalkClass->SendGossipMenu("Your wish, my law!", pCreature->GetObjectGuid());
        return true;
    }
};

void AddSC_npc_teleporter()
{
    new npc_teleporter;
}