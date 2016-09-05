#include "Player.h"

#ifndef  _CPLAYER_H
#define _CPLAYER_H

class CPlayer : public Player
{
public:
	explicit CPlayer(WorldSession* session);
	~CPlayer();

	void OnUpdate(uint32 update_diff, uint32 time);
	void OnLogin();
	void learnAvailableSpells();
protected:
private:
	
};

#endif
