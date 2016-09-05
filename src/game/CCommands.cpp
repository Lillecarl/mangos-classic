#include "Chat.h"
#include "CPlayer.h"

bool ChatHandler::HandleGobAppear(char* args)
{
    GameObject* target = m_session->GetPlayer()->ToCPlayer()->GetTargetGob();

    if (target)
    {
        m_session->GetPlayer()->TeleportTo(target->GetMapId(), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());
        PSendSysMessage("Appeared to gob %u %s", target->GetGUIDLow(), target->GetGOInfo()->name);
    }
    else
        PSendSysMessage("You have no valid object target!");

    return true;
}