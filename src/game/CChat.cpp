#include "Chat.h"
#include "Config/Config.h"
#include "WorldSession.h"
#include "Database/DatabaseEnv.h"
#include "Player.h"

bool ChatHandler::HandleNpcRenameCommand(char* args)
{
    auto guid = m_session->GetPlayer()->GetTargetGuid();
    if (guid.GetHigh() != HIGHGUID_UNIT)
        PSendSysMessage("You must select a creature to rename!");

    char* name = ExtractQuotedOrLiteralArg(&args);
    if (!name)
        return false;

    WorldDatabase.PExecute("UPDATE creature_template SET Name = '%s' WHERE Entry = %u", name, guid.GetEntry());
    PSendSysMessage("Creature name update to %s %u", name, guid.GetEntry());

    return true;
}

bool ChatHandler::HandleNpcResubnameCommand(char* args)
{
    auto guid = m_session->GetPlayer()->GetTargetGuid();
    if (guid.GetHigh() != HIGHGUID_UNIT)
        PSendSysMessage("You must select a creature to rename!");

    char* name = ExtractQuotedOrLiteralArg(&args);
    if (!name)
        return false;

    WorldDatabase.PExecute("UPDATE creature_template SET Subname = '%s' WHERE Entry = %u", name, guid.GetEntry());
    PSendSysMessage("Creature subname update to %s %u", name, guid.GetEntry());

    return true;
}

bool ChatHandler::HandleNpcSetcolCommand(char* args)
{
    auto guid = m_session->GetPlayer()->GetTargetGuid();
    if (guid.GetHigh() != HIGHGUID_UNIT)
        PSendSysMessage("You must select a creature to rename!");

    char* colname = ExtractQuotedOrLiteralArg(&args);
    if (!colname)
        return false;

    char* value = ExtractQuotedOrLiteralArg(&args);
    if (!value)
        return false;

    std::string connectionstring = sConfig.GetStringDefault("WorldDatabaseInfo", "");
    std::string dbname = "";
    bool databasechar = false;

    for (auto i : connectionstring)
    {
        if (i == '/')
            databasechar = true;

        if (databasechar)
            dbname.insert(dbname.end(), i);
    }

    auto result = WorldDatabase.PQuery("SELECT 1 INFORMATION_SCHEMA.COLUMNS WHERE COLUMN_NAME = '%s' AND TABLE_SCHEMA = '%s'", colname, dbname.c_str());
    if (!result)
    {
        PSendSysMessage("Column not found!");
        return true;
    }

    if (result)
        WorldDatabase.PExecute("UPDATE creature_template SET %s = '%s' WHERE Entry = %u", colname, value, guid.GetEntry());

    delete result;
    return true;
}