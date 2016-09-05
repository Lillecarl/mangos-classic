#ifndef  _CPLAYER_H
#define _CPLAYER_H

#include "Player.h"
#include "Map.h"

class ObjectGuid;
class GameObject;

class CPlayer : public Player
{
public:
	explicit CPlayer(WorldSession* session);
	~CPlayer();

	void OnUpdate(uint32 update_diff, uint32 time);
	void OnLogin();
	void learnAvailableSpells();

    void SetTargetGob(ObjectGuid guid) { m_SelectedGobGuid = guid; }
    GameObject* GetTargetGob() { return GetMap()->GetGameObject(m_SelectedGobGuid); }
protected:
private:
    ObjectGuid m_SelectedGobGuid;
};

#endif
