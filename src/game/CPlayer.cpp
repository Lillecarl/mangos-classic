#include "CPlayer.h"
#include "ObjectMgr.h"
#include "Spell.h"

CPlayer::CPlayer(WorldSession* session) : Player(session)
{
}

CPlayer::~CPlayer()
{

}

void CPlayer::OnUpdate(uint32 update_diff, uint32 time)
{
}

void CPlayer::OnLogin()
{
	learnAvailableSpells();
}

void CPlayer::learnAvailableSpells()
{
	uint32 trainerid = 0;

	switch (getClass())
	{
	case CLASS_WARRIOR: trainerid = 4595;  break;
	case CLASS_PALADIN: trainerid = 5491;  break;
	case CLASS_HUNTER:  trainerid = 10930; break;
	case CLASS_ROGUE:   trainerid = 3401;  break;
	case CLASS_PRIEST:  trainerid = 376;   break;
	case CLASS_SHAMAN:  trainerid = 986;   break;
	case CLASS_MAGE:    trainerid = 7312;  break;
	case CLASS_WARLOCK: trainerid = 5612;  break;
	case CLASS_DRUID:   trainerid = 8142;  break;
	default:
		break;
	}

	if (!trainerid)
		return;

	auto trainerspells = sObjectMgr.GetNpcTrainerSpells(trainerid);

	bool LearnSpells = true;

	while (LearnSpells && IsInWorld())
	{
		LearnSpells = false;

		for (auto& i : trainerspells->spellList)
		{
			TrainerSpell const* trainer_spell = &i.second;

			uint32 reqLevel = 0;
			if (!IsSpellFitByClassAndRace(trainer_spell->spell, &reqLevel))
				continue;

			reqLevel = trainer_spell->isProvidedReqLevel ? trainer_spell->reqLevel : std::max(reqLevel, trainer_spell->reqLevel);
			if (GetTrainerSpellState(trainer_spell, reqLevel) != TRAINER_SPELL_GREEN)
				continue;

			SpellEntry const* proto = sSpellStore.LookupEntry(trainer_spell->spell);
			SpellEntry const* spellInfo = sSpellStore.LookupEntry(proto->EffectTriggerSpell[0]);

			Spell* spell;
			if (proto->SpellVisual == 222)
				spell = new Spell(this, proto, false);
			else
				spell = new Spell(this, proto, false);

			SpellCastTargets targets;
			targets.setUnitTarget(this);

			spell->SpellStart(&targets);
		}

		for (auto& i : trainerspells->spellList)
		{
			TrainerSpell const* trainer_spell = &i.second;

			uint32 reqLevel = 0;
			if (!IsSpellFitByClassAndRace(trainer_spell->spell, &reqLevel))
				continue;

			reqLevel = trainer_spell->isProvidedReqLevel ? trainer_spell->reqLevel : std::max(reqLevel, trainer_spell->reqLevel);
			if (GetTrainerSpellState(trainer_spell, reqLevel) != TRAINER_SPELL_GREEN)
				continue;

			LearnSpells = true;
		}
	}
}
