#pragma once

#include "../../../ai_space.h"
#include "../monster_cover_manager.h"
#include "../../../cover_point.h"
#include "../../../level.h"
#include "../../../level_debug.h"

void CStateControlHideLite::initialize()
{
	inherited::initialize			();

	select_target_point				();
	this->object->path().prepare_builder	();

}

void CStateControlHideLite::execute()
{
	this->object->path().set_target_point		(target.position, target.node);
	this->object->path().set_rebuild_time		(0);
	this->object->path().set_distance_to_end	(0.f);
	this->object->path().set_use_covers		(true);

	this->object->anim().accel_activate		(eAT_Aggressive);
	this->object->anim().accel_set_braking	(false);

	this->object->sound().play				(MonsterSound::eMonsterSoundAggressive, 0,0,this->object->db().m_dwAttackSndDelay);
	this->object->custom_dir().head_look_point(this->object->EnemyMan.get_enemy_position());

	this->object->custom_anim().set_body_state(CControllerAnimation::eTorsoRun,CControllerAnimation::eLegsTypeRun);
}

bool CStateControlHideLite::check_start_conditions()
{
	return true;
}

void CStateControlHideLite::reinit()
{
	inherited::reinit();
	m_time_finished = 0;
}


void CStateControlHideLite::finalize()
{
	inherited::finalize();
	m_time_finished	= Device.dwTimeGlobal;
}


bool CStateControlHideLite::check_completion()
{
	if ((this->object->ai_location().level_vertex_id() == target.node) && 
		!this->object->control().path_builder().is_moving_on_path()) return true;
	
	return (!this->object->EnemyMan.see_enemy_now());
}

void CStateControlHideLite::select_target_point()
{
#ifdef DEBUG
	DBG().level_info(this).clear();
#endif

	const CCoverPoint	*point = this->object->CoverMan->find_cover(this->object->EnemyMan.get_enemy_position(),10.f,30.f);
	//VERIFY(point);
	if (point) {
		target.node					= point->level_vertex_id	();
		target.position				= point->position			();
	} else {
		target.node					= 0;
		target.position				= ai().level_graph().vertex_position(target.node);			
	}
}
