#include "CPlayer.h"
#include "Custom.h"

CPlayer::CPlayer(WorldSession* session) : Player(session)
{
    m_PendingReward = 0;
}

void CPlayer::CUpdate(uint32 diff)
{
    SendSavedChat(CHAT_BOX, BoxChat);
    SendSavedChat(CHAT_WIDE, WideChat);
    SendSavedChat(CHAT_BOTH, BothChat);
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
