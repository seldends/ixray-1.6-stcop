////////////////////////////////////////////////////////////////////////////
//	Module 		: level_spawn_constructor.cpp
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Level spawn constructor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_spawn_constructor.h"
#include "game_level_cross_table.h"
#include "level_graph.h"
#include "graph_engine_editor.h"
#include "..\xrServerEntities\xrMessages.h"
#include "..\xrServerEntities\xrServer_Objects_ALife_All.h"
#include "..\xrServerEntities\clsid_game.h"
#include "..\xrServerEntities\game_base_space.h"
#include "game_spawn_constructor.h"
#include "patrol_path_storage.h"
#include "space_restrictor_wrapper.h"
#include "..\xrServerEntities\restriction_space.h"

#define IGNORE_ZERO_SPAWN_POSITIONS

const float y_shift_correction = .15f;

CLevelSpawnConstructor::~CLevelSpawnConstructor()
{
	/*/GRAPH_POINT_STORAGE::iterator	I = m_graph_points.begin();	GRAPH_POINT_STORAGE::iterator	E = m_graph_points.end();
	for ( ; I != E; ++I)
		F_entity_Destroy			((CSE_Abstract*&)(*I));
		*/
	VERIFY(!m_level_graph);
	VERIFY(!m_cross_table);
}

IC const IGameGraph& CLevelSpawnConstructor::game_graph() const
{
	return (m_game_spawn_constructor->game_graph());
}

IC CLevelSpawnConstructor::LEVEL_CHANGER_STORAGE& CLevelSpawnConstructor::level_changers() const
{
	return (m_game_spawn_constructor->level_changers());
}

IC u32 CLevelSpawnConstructor::level_id(shared_str level_name) const
{
	return (m_game_spawn_constructor->level_id(*level_name));
}

IC const ILevelGraph& CLevelSpawnConstructor::level_graph() const
{
	return (*m_level_graph);
}

IC const IGameLevelCrossTable& CLevelSpawnConstructor::cross_table() const
{
	return (*m_cross_table);
}


void CLevelSpawnConstructor::init()
{
	//m_level_graph = new ILevelGraph();
	m_level_graph = Scene->GetLevelGraph();
	m_game_spawn_constructor->game_graph().set_current_level(game_graph().header().level(*m_level.name()).id());
	m_cross_table = &game_graph().cross_table();
	m_game_spawn_constructor->patrol_path_storage().load_editor(&level_graph(), &cross_table(), &game_graph());
	/*// loading level graph
	string_path				file_name;
	FS.update_path			(file_name,"$game_levels$",*m_level.name());
	xr_strcat				(file_name,"\\");

	
	// loading cross table
	

	// loading patrol paths
	FS.update_path			(file_name,"$game_levels$",*m_level.name());
	xr_strcat					(file_name,"\\level.game");
	if (FS.exist(file_name)) {
		IReader				*stream	= FS.r_open(file_name);
		VERIFY				(stream);
		
		FS.r_close			(stream);
	}*/
}

CSE_Abstract* CLevelSpawnConstructor::create_object(IReader* chunk)
{
	R_ASSERT(false);
	/*
	NET_Packet				net_packet;
	net_packet.B.count		= chunk->length();
	chunk->r				(net_packet.B.data,net_packet.B.count);
//	we do not need to close chunk since we iterate on them
//	chunk->close			();
	u16						ID;
	net_packet.r_begin		(ID);
	R_ASSERT2				(M_SPAWN==ID,"ID doesn't match to the spawn-point ID!");
	string64				section_name;
	net_packet.r_stringZ	(section_name);
	CSE_Abstract			*abstract = F_entity_Create(section_name);
	if (!abstract) {
		string256			temp;
		xr_sprintf				(temp,"Can't create entity '%s' !\n",section_name);
		R_ASSERT2			(abstract,temp);
	}
	abstract->Spawn_Read	(net_packet);
	return					(abstract);*/
	return 0;
}

void CLevelSpawnConstructor::add_graph_point(CSE_Abstract* abstract)
{
	CSE_ALifeGraphPoint* graph_point = dynamic_cast<CSE_ALifeGraphPoint*>(abstract);
	R_ASSERT(graph_point);
	m_graph_points.push_back(graph_point);
}

//void CLevelSpawnConstructor::add_spawn_group					(CSE_Abstract			*abstract)
//{
//	CSE_SpawnGroup			*spawn_group = smart_cast<CSE_SpawnGroup*>(abstract);
//	R_ASSERT				(spawn_group);
//	m_spawn_groups.insert	(std::make_pair(spawn_group->name_replace(),spawn_group));
//	if (xr_strlen(spawn_group->m_spawn_control))
//		add_group_object	(spawn_group,spawn_group->m_spawn_control);
//	add_free_object			(abstract);
//}

void CLevelSpawnConstructor::add_story_object(CSE_ALifeDynamicObject* dynamic_object)
{
	m_game_spawn_constructor->add_story_object(dynamic_object->m_story_id, dynamic_object, *m_level.name());
}

void CLevelSpawnConstructor::add_space_restrictor(CSE_ALifeDynamicObject* dynamic_object)
{
	if (!dynamic_object)
		return;
	CSE_ALifeSpaceRestrictor* space_restrictor = dynamic_cast<CSE_ALifeSpaceRestrictor*>(dynamic_object);
	if (!space_restrictor)
		return;

	if (space_restrictor->m_space_restrictor_type == RestrictionSpace::eRestrictorTypeNone)
		return;

	if (!space_restrictor->m_flags.test(CSE_ALifeObject::flCheckForSeparator))
		return;

	m_space_restrictors.push_back(new CSpaceRestrictorWrapper(space_restrictor));
}

void CLevelSpawnConstructor::add_level_changer(CSE_Abstract* abstract)
{
	CSE_ALifeLevelChanger* level_changer = dynamic_cast<CSE_ALifeLevelChanger*>(abstract);
	R_ASSERT(level_changer);
	m_game_spawn_constructor->add_level_changer(level_changer);
	m_level_changers.push_back(level_changer);
}

void CLevelSpawnConstructor::add_free_object(CSE_Abstract* abstract)
{
	m_game_spawn_constructor->add_object(abstract);
}

//void CLevelSpawnConstructor::add_group_object					(CSE_Abstract			*abstract, shared_str group_section, bool)
//{
//	SPAWN_GRPOUP_OBJECTS::iterator	I = m_spawn_objects.find(group_section);
//	if (I == m_spawn_objects.end()) {
//		xr_vector<CSE_Abstract*>	*temp = new GROUP_OBJECTS();
//		temp->clear					();
//		temp->push_back				(abstract);
//		m_spawn_objects.insert		(std::make_pair(group_section,temp));
//	}
//	else
//		(*I).second->push_back		(abstract);
//}

//void CLevelSpawnConstructor::add_group_object					(CSE_Abstract			*abstract, shared_str group_section)
//{
//	string256					temp;
//	for (u32 i=0, n=_GetItemCount(*group_section); i<n; ++i)
//		add_group_object		(abstract,_GetItem(*group_section,i,temp),true);
//}

bool CLevelSpawnConstructor::load_objects()
{
	// loading spawn points
	Msg("Loading spawns ...");
	u32 id;

	for (auto& Obj : Scene->ListObj(OBJCLASS_SPAWNPOINT))
	{
		CSpawnPoint* Spawn = dynamic_cast<CSpawnPoint*>(Obj);
		if (!Spawn->m_SpawnData.Valid())
			continue;

		CSE_Abstract* abstract = Spawn->GetEntity();
		if (!abstract)
		{
			continue;
		}
		if (abstract->m_tClassID == CLSID_AI_GRAPH)
		{
			add_graph_point(abstract);
			continue;
		}

		//		if (abstract->m_tClassID == CLSID_AI_SPAWN_GROUP) {
		//			add_spawn_group		(abstract);
		//			continue;
		//		}

		/*if (!abstract->m_gameType.MatchType(eGameIDSingle))
		{
			continue;
		}*/

		CSE_ALifeObject* alife_object = dynamic_cast<CSE_ALifeObject*>(abstract);
		if (!alife_object)
		{
			continue;
		}

		CSE_ALifeCreatureActor* actor = dynamic_cast<CSE_ALifeCreatureActor*>(alife_object);
		if (actor)
		{
			if (m_actor)
			{
				Msg("! Too many actors on the level");
				return false;
			}
			m_actor = actor;
		}

		m_spawns.push_back(alife_object);

		CSE_ALifeDynamicObject* dynamic_object = dynamic_cast<CSE_ALifeDynamicObject*>(alife_object);
		if (dynamic_object)
		{
			add_story_object(dynamic_object);
			add_space_restrictor(dynamic_object);
		}

		if (dynamic_cast<CSE_ALifeLevelChanger*>(alife_object))
			add_level_changer(abstract);

		//		if (xr_strlen(alife_object->m_spawn_control))
		//			add_group_object	(alife_object,alife_object->m_spawn_control);

		add_free_object(alife_object);
	}

	if (m_spawns.empty())
	{
		Msg("! There are no spawn-points!");
		return false;
	}
	return true;
}

//IC	void CLevelSpawnConstructor::normalize_probability			(CSE_ALifeAnomalousZone *zone)
//{
//	float						accumulator = 0.f;
//	for (int ii=0; ii<zone->m_wItemCount; ii++)
//		accumulator				+= zone->m_faWeights[ii];
//
//	accumulator					*= zone->m_fBirthProbability;
//
//	for (int ii=0; ii<zone->m_wItemCount; ii++)
//		zone->m_faWeights[ii]	/= accumulator;
//}

//IC	void CLevelSpawnConstructor::free_group_objects					()
//{
//	SPAWN_GRPOUP_OBJECTS::iterator	I = m_spawn_objects.begin();
//	SPAWN_GRPOUP_OBJECTS::iterator	E = m_spawn_objects.end();
//	for ( ; I != E; I++)
//		xr_delete					((*I).second);
//}

//void CLevelSpawnConstructor::fill_spawn_groups					()
//{
//	SPAWN_GRPOUP_OBJECTS::iterator				I = m_spawn_objects.begin();
//	SPAWN_GRPOUP_OBJECTS::iterator				E = m_spawn_objects.end();
//	
//	for ( ; I != E; I++) {
//		R_ASSERT								(xr_strlen(*(*I).first));
//		R_ASSERT								((*I).second);
//		SPAWN_GROUPS::iterator					J = m_spawn_groups.find((*I).first);
//		if (J == m_spawn_groups.end())
//			clMsg								("! ERROR (spawn group not found!) : %s",*(*I).first);
//		R_ASSERT3								(J != m_spawn_groups.end(),"Specified group control not found!",(*(*I).second)[0]->name_replace());
//		
//		GROUP_OBJECTS::iterator					i = (*I).second->begin();
//		GROUP_OBJECTS::iterator					e = (*I).second->end();
//		for ( ; i != e; i++) {
//			m_game_spawn_constructor->add_edge	((*J).second->m_tSpawnID,(*i)->m_tSpawnID,(*i)->m_spawn_probability);
//			CSE_ALifeAnomalousZone				*zone = smart_cast<CSE_ALifeAnomalousZone*>(*i);
//			if (zone)
//				normalize_probability			(zone);
//		}
//	}
//	
//	free_group_objects							();
//}

bool CLevelSpawnConstructor::correct_objects()
{
	Msg("Correct objects ...");
	u32 m_level_graph_vertex_id = u32(-1);
	u32 dwStart = game_graph().header().vertex_count(), dwFinish = game_graph().header().vertex_count(), dwCount = 0;
	for (u32 i = 0; i < game_graph().header().vertex_count(); ++i)
		if (game_graph().vertex(i)->level_id() == m_level.id())
		{
			if (m_level_graph_vertex_id == u32(-1))
				m_level_graph_vertex_id = i;
			dwCount++;
		}

	for (int i = 0; i < (int)game_graph().header().vertex_count(); i++)
		if (game_graph().vertex(i)->level_id() == m_level.id())
		{
			if (dwStart > (u32)i)
				dwStart = (u32)i;
		}
		else
		{
			if ((dwStart <= (u32)i) && (dwFinish > (u32)i))
			{
				dwFinish = i;
				break;
			}
		}
	if (dwStart >= dwFinish)
	{
		string4096 S;
		xr_sprintf(S, "! There are no graph vertices in the game graph for the level '%s' !\n", *m_level.name());
		Msg(S);
		return false;
	}

	for (int i = 0; i < (int)m_spawns.size(); i++)
	{
		if (!m_spawns[i]->used_ai_locations())
		{
			m_spawns[i]->m_tGraphID = (GameGraph::_GRAPH_ID)m_level_graph_vertex_id;
			m_spawns[i]->m_fDistance = 0.f;
			m_spawns[i]->m_tNodeID = game_graph().vertex(m_level_graph_vertex_id)->level_vertex_id();
			continue;
		}
		Fvector position = m_spawns[i]->o_Position;
		position.y += y_shift_correction;
		m_spawns[i]->m_tNodeID = level_graph().vertex(u32(-1), position);
		VERIFY(level_graph().valid_vertex_id(m_spawns[i]->m_tNodeID));
		if (m_spawns[i]->used_ai_locations() && !level_graph().inside(level_graph().vertex(m_spawns[i]->m_tNodeID),
		                                                              position))
		{
			Fvector new_position = level_graph().vertex_position(m_spawns[i]->m_tNodeID);
			clMsg("[%s][%s][%s] : position changed from [%f][%f][%f] -> [%f][%f][%f]", *m_level.name(),
			      *m_spawns[i]->s_name, m_spawns[i]->name_replace(),VPUSH(position),VPUSH(new_position));
			m_spawns[i]->o_Position = new_position;
		}
		u32 dwBest = cross_table().vertex(m_spawns[i]->m_tNodeID).game_vertex_id();
		if (game_graph().vertex(dwBest)->level_id() != m_level.id())
		{
			string4096 S1;
			char* S = S1;
			S += xr_sprintf(S, sizeof(S1) - (S1 - &S[0]),
			                "Corresponding graph vertex for the spawn point is located on the ANOTHER level\n",
			                m_spawns[i]->name_replace());
			S += xr_sprintf(S, sizeof(S1) - (S1 - &S[0]), "Current level  : [%d][%s]\n", m_level.id(),
			                *game_graph().header().level(m_level.id()).name());
			S += xr_sprintf(S, sizeof(S1) - (S1 - &S[0]), "Conflict level : [%d][%s]\n",
			                game_graph().vertex(dwBest)->level_id(),
			                *game_graph().header().level(game_graph().vertex(dwBest)->level_id()).name());
			S += xr_sprintf(S, sizeof(S1) - (S1 - &S[0]),
			                "Probably, you filled offsets in \"game_levels.ltx\" incorrect");
			Msg(S1);
			return false;
		}

		float fCurrentBestDistance = cross_table().vertex(m_spawns[i]->m_tNodeID).distance();
		if (dwBest == u32(-1))
		{
			string4096 S1;
			char* S = S1;
			S += xr_sprintf(S, sizeof(S1) - (S1 - &S[0]),
			                "Can't find a corresponding GRAPH VERTEX for the spawn-point %s\n",
			                m_spawns[i]->name_replace());
			S += xr_sprintf(S, sizeof(S1) - (S1 - &S[0]), "Level ID    : %d\n", m_level.id());
			S += xr_sprintf(S, sizeof(S1) - (S1 - &S[0]), "Spawn index : %d\n", i);
			S += xr_sprintf(S, sizeof(S1) - (S1 - &S[0]), "Spawn node  : %d\n", m_spawns[i]->m_tNodeID);
			S += xr_sprintf(S, sizeof(S1) - (S1 - &S[0]), "Spawn point : [%7.2f][%7.2f][%7.2f]\n",
			                m_spawns[i]->o_Position.x, m_spawns[i]->o_Position.y, m_spawns[i]->o_Position.z);
			Msg(S1);
			return false;
		}
		m_spawns[i]->m_tGraphID = (GameGraph::_GRAPH_ID)dwBest;
		m_spawns[i]->m_fDistance = fCurrentBestDistance;
	}
	return true;
}

void CLevelSpawnConstructor::correct_level_changers()
{
	LEVEL_CHANGER_STORAGE::const_iterator I = m_level_changers.begin();
	LEVEL_CHANGER_STORAGE::const_iterator E = m_level_changers.end();
	for (; I != E; ++I)
	{
		Fvector position = (*I)->o_Position;
		position.y += y_shift_correction;
		(*I)->m_tNodeID = level_graph().vertex(u32(-1), position);
		VERIFY(level_graph().valid_vertex_id((*I)->m_tNodeID));

		u32 dwBest = cross_table().vertex((*I)->m_tNodeID).game_vertex_id();
		VERIFY(game_graph().vertex(dwBest)->level_id() == m_level.id());
		(*I)->m_tGraphID = (GameGraph::_GRAPH_ID)dwBest;

		(*I)->m_fDistance = cross_table().vertex((*I)->m_tNodeID).distance();
	}
}

struct remove_too_far_predicate
{
	float m_radius_sqr;
	const ILevelGraph* m_graph;
	Fvector m_position;

	IC remove_too_far_predicate(const ILevelGraph* graph, const Fvector& position, float radius)
	{
		VERIFY(graph);
		m_graph = graph;
		m_position = position;
		m_radius_sqr = _sqr(radius);
	}

	IC bool operator()(const u32& vertex_id) const
	{
		return (m_graph->vertex_position(vertex_id).distance_to_sqr(m_position) > m_radius_sqr);
	}
};

#ifndef IGNORE_ZERO_SPAWN_POSITIONS
class remove_invalid_zones_predicate {
public:
	typedef CLevelSpawnConstructor::SPAWN_STORAGE	SPAWN_STORAGE;

private:
	const SPAWN_STORAGE					*m_zones;
	const CLevelSpawnConstructor		*m_level_spawn_constructor;

public:
	IC			remove_invalid_zones_predicate	(const CLevelSpawnConstructor *level_spawn_constructor, const SPAWN_STORAGE *zones)
	{
		VERIFY							(level_spawn_constructor);
		m_level_spawn_constructor		= level_spawn_constructor;

		VERIFY							(zones);
		m_zones							= zones;
	}

	IC	bool	operator()						(CSE_ALifeObject *object) const
	{
		SPAWN_STORAGE::const_iterator	I = std::find(m_zones->begin(),m_zones->end(),object);
		if (I == m_zones->end())
			return						(false);

		VERIFY							(!object->m_spawn_control);
		VERIFY							(object->m_story_id == INVALID_STORY_ID);
		m_level_spawn_constructor->game_spawn_constructor().remove_object(object);
		CSE_Abstract					*abstract = object;
		F_entity_Destroy				(abstract);
		return							(true);
	}
};
#endif

void CLevelSpawnConstructor::generate_artefact_spawn_positions()
{
	// create graph engine
	CTimer Timer;
	Timer.Start();
	Msg("Generate artefact spawn positions ...");
	size_t CountThread = std::min((size_t)CPU::ID.n_threads, m_spawns.size());
	m_generate_artefact_spawn_positions_worker_counter = m_spawns.size();
	xr_vector<HANDLE> Threads;
	Msg("* create workers %d", CountThread);
	for (size_t i = 0; i < CountThread; i++)
	{
		string_path Name;
		xr_sprintf(Name, "GenerateArtefactSpawnPositionWorker_%d", i);
		Threads.push_back(thread_spawn(&CLevelSpawnConstructor::ThreadGenerateArtefactSpawnPositionStartup, Name, 0,
		                               this));
	}

	for (HANDLE& i : Threads)
	{
		WaitForSingleObject(i, INFINITE);
	}


	Msg("* Completed. Time %3.2f", Timer.GetElapsed_sec());
	//
	//	xr_vector<u32>						l_tpaStack;
	//	SPAWN_STORAGE						zones;
	//	l_tpaStack.reserve					(1024);
	//	SPAWN_STORAGE::iterator				I = m_spawns.begin();
	//	SPAWN_STORAGE::iterator				E = m_spawns.end();
	//	for ( ; I != E; I++) {
	//		CSE_ALifeAnomalousZone* zone = (*I)->CastALifeAnomalousZone();
	//		if (!zone)
	//			continue;
	//		CSE_Abstract* Abstract = (*I);
	////		if (!level_graph().valid_vertex_position(zone->o_Position)) {
	////			zone->m_artefact_spawn_count	= 0;
	////			zone->m_artefact_position_offset		= m_level_points.size();
	////			continue;
	////		}
	//
	//		(*I)->m_tNodeID						= level_graph().vertex((*I)->m_tNodeID, Abstract->o_Position);
	//		if (!level_graph().valid_vertex_position(Abstract->o_Position) || !level_graph().inside((*I)->m_tNodeID, Abstract->o_Position))
	//			(*I)->m_tNodeID					= level_graph().vertex(u32(-1), Abstract->o_Position);
	//		const IGameLevelCrossTable::CCell	&cell = cross_table().vertex((*I)->m_tNodeID);
	//		(*I)->m_tGraphID					= cell.game_vertex_id();
	//		(*I)->m_fDistance					= cell.distance();
	//
	//		graph_engine().search			(
	//			level_graph(),
	//			(*I)->m_tNodeID,
	//			(*I)->m_tNodeID,
	//			&l_tpaStack,
	//			SFlooder<
	//				float,
	//				u32,
	//				u32
	//			>(
	//				zone->m_offline_interactive_radius,
	//				u32(-1),
	//				u32(-1)
	//			)
	//		);
	//		
	//		l_tpaStack.erase				(
	//			std::remove_if(
	//				l_tpaStack.begin(),
	//				l_tpaStack.end(),
	//				remove_too_far_predicate(
	//					&level_graph(),
	//					Abstract->o_Position,
	//					zone->m_offline_interactive_radius
	//				)
	//			),
	//			l_tpaStack.end()
	//		);
	///*
	//		if (zone->m_artefact_spawn_count >= l_tpaStack.size()) 
	//		{
	//			zone->m_artefact_spawn_count	= (u16)l_tpaStack.size();
	//#ifndef IGNORE_ZERO_SPAWN_POSITIONS
	//			if (!zone->m_artefact_spawn_count) {
	//				Msg						("! CANNOT GENERATE ARTEFACT SPAWN POSITIONS FOR ZONE [%s] ON LEVEL [%s]",zone->name_replace(),*level().name());
	//				Msg						("! ZONE [%s] ON LEVEL [%s] IS REMOVED BY AI COMPILER",zone->name_replace(),*level().name());
	//				R_ASSERT3				(zone->m_story_id == INVALID_STORY_ID,"Cannot remove story object",zone->name_replace());
	//				R_ASSERT3				(!zone->m_spawn_control,"Cannot remove spawn control object",zone->name_replace());
	//				zones.push_back			(zone);
	//				l_tpaStack.clear		();
	//				continue;
	//			}
	//#endif
	//		}
	//		else		*/
	//			std::random_shuffle			(l_tpaStack.begin(),l_tpaStack.end());
	//
	//		zone->m_artefact_position_offset= m_level_points.size();
	//		m_level_points.resize			(zone->m_artefact_position_offset + zone->m_artefact_spawn_count);
	//
	////		Msg								("%s  %f [%f][%f][%f] : artefact spawn positions",zone->name_replace(),zone->m_fRadius,VPUSH(zone->o_Position));
	//
	//		LEVEL_POINT_STORAGE::iterator	I = m_level_points.begin() + zone->m_artefact_position_offset;
	//		LEVEL_POINT_STORAGE::iterator	E = m_level_points.end();
	//		xr_vector<u32>::iterator		i = l_tpaStack.begin();
	//		for ( ; I != E; ++I, ++i) {
	//			(*I).tNodeID				= *i;
	//			(*I).tPoint					= level_graph().vertex_position(*i);
	//			(*I).fDistance				= cross_table().vertex(*i).distance();
	////			Msg							("    [%f][%f][%f] : %f",VPUSH((*I).tPoint),zone->o_Position.distance_to((*I).tPoint));
	//		}
	//		
	//		l_tpaStack.clear				();
	//	}

#ifndef IGNORE_ZERO_SPAWN_POSITIONS
	I									= std::remove_if(m_spawns.begin(),m_spawns.end(),remove_invalid_zones_predicate(this,&zones));
	m_spawns.erase						(I,m_spawns.end());
#endif
}

void CLevelSpawnConstructor::ThreadGenerateArtefactSpawnPositionStartup(void* args)
{
	reinterpret_cast<CLevelSpawnConstructor*>(args)->generate_artefact_spawn_positions_worker();
}

void CLevelSpawnConstructor::generate_artefact_spawn_positions_worker()
{
	xr_vector<u32> l_tpaStack;
	SPAWN_STORAGE zones;
	l_tpaStack.reserve(1024);

	while (InterlockedCompareExchange(&m_generate_artefact_spawn_positions_worker_counter, 0, 0))
	{
		size_t Index = InterlockedDecrement(&m_generate_artefact_spawn_positions_worker_counter);
		CSE_ALifeObject* Object = m_spawns[Index];
		CSE_ALifeAnomalousZone* zone = dynamic_cast<CSE_ALifeAnomalousZone*>(Object);
		if (!zone)
			continue;
		CSE_Abstract* Abstract = Object;
		//		if (!level_graph().valid_vertex_position(zone->o_Position)) {
		//			zone->m_artefact_spawn_count	= 0;
		//			zone->m_artefact_position_offset		= m_level_points.size();
		//			continue;
		//		}
		Object->m_tNodeID = level_graph().vertex(Object->m_tNodeID, Abstract->o_Position);
		if (!level_graph().valid_vertex_position(Abstract->o_Position) || !level_graph().inside(
			Object->m_tNodeID, Abstract->o_Position))
			Object->m_tNodeID = level_graph().vertex(u32(-1), Abstract->o_Position);
		const IGameLevelCrossTable::CCell& cell = cross_table().vertex(Object->m_tNodeID);
		Object->m_tGraphID = cell.game_vertex_id();
		Object->m_fDistance = cell.distance();

		xr_vector<u32> CheckNodes;
		l_tpaStack.push_back(Object->m_tNodeID);
		CheckNodes.push_back(Object->m_tNodeID);

		xr_set<u32> NodeVisited;

		float m_distance_xz = level_graph().header().cell_size();
		ILevelGraph::CVertex* MainNode = level_graph().vertex(Object->m_tNodeID);
		while(CheckNodes.size() > 0)
		{
			u32 CurrentNodeID = CheckNodes.back();
			CheckNodes.pop_back();
			ILevelGraph::CVertex* Node = level_graph().vertex(CurrentNodeID);
			NodeVisited.emplace(CurrentNodeID);

			auto DistanceNode = [this,m_distance_xz](ILevelGraph::CVertex* Node1,ILevelGraph::CVertex* Node2)
			{
				return level_graph().distance(Node1,Node2);
			};

			for (s32 NeighborIndex = 0; NeighborIndex < 4; NeighborIndex++)
			{
				const u32 NeighborID = Node->link(NeighborIndex);
				if (!level_graph().valid_vertex_id(NeighborID)) continue;
				ILevelGraph::CVertex* NeighborNode = level_graph().vertex(Object->m_tNodeID);
				if(DistanceNode(MainNode,NeighborNode) < zone->m_offline_interactive_radius)
				{
					if (!NodeVisited.contains(NeighborID))
					{
						l_tpaStack.push_back(NeighborID);
						CheckNodes.push_back(NeighborID);
					}
				}
			}
		}

		l_tpaStack.erase(
			std::remove_if(
				l_tpaStack.begin(),
				l_tpaStack.end(),
				remove_too_far_predicate(
					&level_graph(),
					Abstract->o_Position,
					zone->m_offline_interactive_radius
				)
			),
			l_tpaStack.end()
		);

		random_shuffle(l_tpaStack.begin(), l_tpaStack.end());

		m_generate_artefact_spawn_positions_worker_mutex.Enter();
		zone->m_artefact_position_offset = m_level_points.size();
		m_level_points.resize(zone->m_artefact_position_offset + zone->m_artefact_spawn_count);

		//		Msg								("%s  %f [%f][%f][%f] : artefact spawn positions",zone->name_replace(),zone->m_fRadius,VPUSH(zone->o_Position));


		LEVEL_POINT_STORAGE::iterator I = m_level_points.begin() + zone->m_artefact_position_offset;
		LEVEL_POINT_STORAGE::iterator E = m_level_points.end();
		xr_vector<u32>::iterator i = l_tpaStack.begin();
		for (; I != E; ++I, ++i)
		{
			(*I).tNodeID = *i;
			(*I).tPoint = level_graph().vertex_position(*i);
			(*I).fDistance = cross_table().vertex(*i).distance();
			//			Msg							("    [%f][%f][%f] : %f",VPUSH((*I).tPoint),zone->o_Position.distance_to((*I).tPoint));
		}
		m_generate_artefact_spawn_positions_worker_mutex.Leave();
		l_tpaStack.clear();
	}
}

void CLevelSpawnConstructor::fill_level_changers()
{
	for (u32 i = 0, n = (u32)level_changers().size(); i < n; ++i)
	{
		if (level_id(level_changers()[i]->m_caLevelToChange) != m_level.id())
			continue;

		bool found = false;
		GRAPH_POINT_STORAGE::const_iterator I = m_graph_points.begin();
		GRAPH_POINT_STORAGE::const_iterator E = m_graph_points.end();
		for (; I != E; ++I)
			if (!xr_strcmp(*level_changers()[i]->m_caLevelPointToChange, (*I)->name_replace()))
			{
				bool ok = false;
				for (u32 ii = 0, nn = game_graph().header().vertex_count(); ii < nn; ++ii)
				{
					if ((game_graph().vertex(ii)->level_id() != m_level.id()) || !game_graph().vertex(ii)->level_point()
						.similar((*I)->o_Position, .001f))
						continue;
					level_changers()[i]->m_tNextGraphID = (GameGraph::_GRAPH_ID)ii;
					level_changers()[i]->m_tNextPosition = (*I)->o_Position;
					level_changers()[i]->m_tAngles = (*I)->o_Angle;
					level_changers()[i]->m_dwNextNodeID = game_graph().vertex(ii)->level_vertex_id();
					ok = true;
					break;
				}

				R_ASSERT3(
					ok,
					"Cannot find a correspndance between graph and graph points from level editor! Rebuild graph for the level ",
					*level_changers()[i]->m_caLevelToChange);

				level_changers().erase(level_changers().begin() + i);
				--i;
				--n;
				found = true;
				break;
			}

		if (!found)
		{
			clMsg("Graph point %s not found (level changer %s)", *level_changers()[i]->m_caLevelPointToChange,
			      level_changers()[i]->name_replace());
			VERIFY(false);
		}
	}
}

void CLevelSpawnConstructor::update_artefact_spawn_positions()
{
	u32 level_point_count = m_game_spawn_constructor->level_point_count();
	SPAWN_STORAGE::iterator I = m_spawns.begin();
	SPAWN_STORAGE::iterator E = m_spawns.end();
	for (; I != E; ++I)
	{
		CSE_Abstract* abstract = (*I);
		CSE_ALifeObject* alife_object = dynamic_cast<CSE_ALifeObject*>(abstract);
		//		R_ASSERT3						(level_graph().valid_vertex_id(alife_object->m_tNodeID),"Invalid node for object ",alife_object->name_replace());
		R_ASSERT2(alife_object, "Non-ALife object!");
		VERIFY(game_graph().vertex(alife_object->m_tGraphID)->level_id() == m_level.id());
		//		alife_object->m_spawn_control	= "";
		;
		if (CSE_ALifeAnomalousZone* zone = dynamic_cast<CSE_ALifeAnomalousZone*>(alife_object))
		{
			zone->m_artefact_position_offset = level_point_count;
			level_point_count += zone->m_artefact_spawn_count;
			//			Msg							("%s  %f [%f][%f][%f] : artefact spawn positions",zone->name_replace(),zone->m_fRadius,VPUSH(zone->o_Position));
			//			for (u32 i=zone->m_artefact_position_offset; i<level_point_count; ++i)
			//				Msg						("    [%f][%f][%f] : %f",VPUSH(m_level_points[i].tPoint),zone->o_Position.distance_to(m_level_points[i].tPoint));
		}
	}

	m_game_spawn_constructor->add_level_points(m_level_points);
}

bool CLevelSpawnConstructor::Execute()
{
	CTimer Timer;
	Timer.Start();
	if (!load_objects())
	{
		m_cross_table = 0;
		m_level_graph = 0;
		return false;
	}
	//	fill_spawn_groups					();

	init();

	if (!correct_objects())
	{
		m_cross_table = 0;
		m_level_graph = 0;
		return false;
	}
	generate_artefact_spawn_positions();
	correct_level_changers();
	if (MainForm->GetTopBarForm()->VerifySpaceRestrictors())
		if (!verify_space_restrictors())
			return false;

	//xr_delete							(m_level_graph);
	m_cross_table = 0;
	m_level_graph = 0;
	Msg("Spawn build completed time %3.2f", Timer.GetElapsed_sec());
	return true;
}

bool CLevelSpawnConstructor::update()
{
	//fill_level_changers					();
	update_artefact_spawn_positions();
	return true;
}

bool CLevelSpawnConstructor::verify_space_restrictors()
{
	Msg("Level [%s] : searching for AI map separators space restrictors", *m_level.name());
	SPACE_RESTRICTORS::iterator I = m_space_restrictors.begin();
	SPACE_RESTRICTORS::iterator E = m_space_restrictors.end();
	bool bResult = true;
	for (; I != E; ++I)
	{
		VERIFY(*I);

		if ((*I)->object().m_space_restrictor_type == RestrictionSpace::eRestrictorTypeNone)
			continue;

		if (!(*I)->verify(*m_level_graph, m_no_separator_check))
			bResult = false;
	}

	delete_data(m_space_restrictors);

	if (m_no_separator_check)
		Msg("Level [%s] : no separators found", *m_level.name());
	return bResult;
}
