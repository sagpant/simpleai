/**
 * @file
 * @ingroup LUA
 */
#pragma once

#include "common/LUA.h"

namespace ai {

static inline const char *luaAI_metaai() {
	return "__meta_ai";
}

static inline const char *luaAI_metazone() {
	return "__meta_zone";
}

static inline const char* luaAI_metaaggromgr() {
	return "__meta_aggromgr";
}

static inline const char* luaAI_metaregistry() {
	return "__meta_registry";
}

static inline const char* luaAI_metagroupmgr() {
	return "__meta_groupmgr";
}

static inline const char* luaAI_metacharacter() {
	return "__meta_character";
}

static inline const char* luaAI_metavec() {
	return "__meta_vec";
}

static void luaAI_registerfuncs(lua_State* s, const luaL_Reg* funcs, const char *name) {
	luaL_newmetatable(s, name);
	// assign the metatable to __index
	lua_pushvalue(s, -1);
	lua_setfield(s, -2, "__index");
	luaL_setfuncs(s, funcs, 0);
}

static void luaAI_setupmetatable(lua_State* s, const std::string& type, const luaL_Reg *funcs, const std::string& name) {
	const std::string& metaFull = "__meta_" + name + "_" + type;
	// make global
	lua_setfield(s, LUA_REGISTRYINDEX, metaFull.c_str());
	// put back onto stack
	lua_getfield(s, LUA_REGISTRYINDEX, metaFull.c_str());

	// setup meta table - create a new one manually, otherwise we aren't
	// able to override the particular function on a per instance base. Also
	// this 'metatable' must not be in the global registry.
	lua_createtable(s, 0, 2);
	lua_pushvalue(s, -1);
	lua_setfield(s, -2, "__index");
	lua_pushstring(s, name.c_str());
	lua_setfield(s, -2, "__name");
	lua_pushstring(s, type.c_str());
	lua_setfield(s, -2, "type");
	luaL_setfuncs(s, funcs, 0);
	lua_setmetatable(s, -2);
}

static int luaAI_newindex(lua_State* s) {
	// -3 is userdata
	lua_getmetatable(s, -3);
	// -3 is now the field string
	const char *field = luaL_checkstring(s, -3);
	// push -2 to -1 (the value)
	lua_pushvalue(s, -2);
	// set the value into the field
	lua_setfield(s, -2, field);
	lua_pop(s, 1);
	return 0;
}

template<class T>
static inline T luaAI_getudata(lua_State* s, int n, const char *name) {
	void* dataVoid = luaL_checkudata(s, n, name);
	if (dataVoid == nullptr) {
		std::string error(name);
		error.append(" userdata must not be null");
		luaL_argcheck(s, dataVoid != nullptr, n, error.c_str());
	}
	return (T) dataVoid;
}

template<class T>
static inline void luaAI_newuserdata(lua_State* s, const T& data) {
	T* udata = (T*) lua_newuserdata(s, sizeof(T));
	*udata = data;
}

static void luaAI_globalpointer(lua_State* s, void* pointer, const char *name) {
	lua_pushlightuserdata(s, pointer);
	lua_setglobal(s, name);
}

template<class T>
static inline int luaAI_pushudata(lua_State* s, const T& data, const char *name) {
	luaAI_newuserdata<T>(s, data);
	luaL_getmetatable(s, name);
#if AI_LUA_SANTITY
	if (!lua_istable(s, -1)) {
		ai_log_error("LUA: metatable for %s doesn't exist", name);
		return 0;
	}
#endif
	lua_setmetatable(s, -2);
	return 1;
}

template<class T>
static T* luaAI_getlightuserdata(lua_State *s, const char *name) {
	lua_getglobal(s, name);
	if (lua_isnil(s, -1)) {
		return nullptr;
	}
	T* data = (T*) lua_touserdata(s, -1);
	lua_pop(s, 1);
	return data;
}

static AI* luaAI_toai(lua_State *s, int n) {
	return *(AI**)luaAI_getudata<AI*>(s, n, luaAI_metaai());
}

static Zone* luaAI_tozone(lua_State *s, int n) {
	return *(Zone**)luaAI_getudata<Zone*>(s, n, luaAI_metazone());
}

static AggroMgr* luaAI_toaggromgr(lua_State *s, int n) {
	return *(AggroMgr**)luaAI_getudata<AggroMgr*>(s, n, luaAI_metaaggromgr());
}

static GroupMgr* luaAI_togroupmgr(lua_State *s, int n) {
	return *(GroupMgr**)luaAI_getudata<GroupMgr*>(s, n, luaAI_metagroupmgr());
}

static ICharacter* luaAI_tocharacter(lua_State *s, int n) {
	return *(ICharacter**)luaAI_getudata<ICharacter*>(s, n, luaAI_metacharacter());
}

static glm::vec3* luaAI_tovec(lua_State *s, int n) {
	return luaAI_getudata<glm::vec3*>(s, n, luaAI_metavec());
}

static int luaAI_pushzone(lua_State* s, Zone* zone) {
	return luaAI_pushudata<Zone*>(s, zone, luaAI_metazone());
}

static int luaAI_pushaggromgr(lua_State* s, AggroMgr* aggroMgr) {
	return luaAI_pushudata<AggroMgr*>(s, aggroMgr, luaAI_metaaggromgr());
}

static int luaAI_pushgroupmgr(lua_State* s, GroupMgr* groupMgr) {
	return luaAI_pushudata<GroupMgr*>(s, groupMgr, luaAI_metagroupmgr());
}

static int luaAI_pushcharacter(lua_State* s, ICharacter* character) {
	return luaAI_pushudata<ICharacter*>(s, character, luaAI_metacharacter());
}

static int luaAI_pushai(lua_State* s, AI* ai) {
	return luaAI_pushudata<AI*>(s, ai, luaAI_metaai());
}

static int luaAI_pushvec(lua_State* s, const glm::vec3& v) {
	return luaAI_pushudata<glm::vec3>(s, v, luaAI_metavec());
}

static int luaAI_groupmgrposition(lua_State* s) {
	const GroupMgr* groupMgr = luaAI_togroupmgr(s, 1);
	const GroupId groupId = (GroupId)luaL_checkinteger(s, 2);
	return luaAI_pushvec(s, groupMgr->getPosition(groupId));
}

static int luaAI_groupmgradd(lua_State* s) {
	GroupMgr* groupMgr = luaAI_togroupmgr(s, 1);
	const GroupId groupId = (GroupId)luaL_checkinteger(s, 2);
	AI* ai = luaAI_toai(s, 3);
	const bool state = groupMgr->add(groupId, ai->ptr());
	lua_pushboolean(s, state);
	return 1;
}

static int luaAI_groupmgrremove(lua_State* s) {
	GroupMgr* groupMgr = luaAI_togroupmgr(s, 1);
	const GroupId groupId = (GroupId)luaL_checkinteger(s, 2);
	AI* ai = luaAI_toai(s, 3);
	const bool state = groupMgr->remove(groupId, ai->ptr());
	lua_pushboolean(s, state);
	return 1;
}

static int luaAI_groupmgrisleader(lua_State* s) {
	const GroupMgr* groupMgr = luaAI_togroupmgr(s, 1);
	const GroupId groupId = (GroupId)luaL_checkinteger(s, 2);
	AI* ai = luaAI_toai(s, 3);
	const bool state = groupMgr->isGroupLeader(groupId, ai->ptr());
	lua_pushboolean(s, state);
	return 1;
}

static int luaAI_groupmgrisingroup(lua_State* s) {
	const GroupMgr* groupMgr = luaAI_togroupmgr(s, 1);
	const GroupId groupId = (GroupId)luaL_checkinteger(s, 2);
	AI* ai = luaAI_toai(s, 3);
	const bool state = groupMgr->isInGroup(groupId, ai->ptr());
	lua_pushboolean(s, state);
	return 1;
}

static int luaAI_groupmgrisinanygroup(lua_State* s) {
	const GroupMgr* groupMgr = luaAI_togroupmgr(s, 1);
	AI* ai = luaAI_toai(s, 2);
	const bool state = groupMgr->isInAnyGroup(ai->ptr());
	lua_pushboolean(s, state);
	return 1;
}

static int luaAI_groupmgrsize(lua_State* s) {
	const GroupMgr* groupMgr = luaAI_togroupmgr(s, 1);
	const GroupId groupId = (GroupId)luaL_checkinteger(s, 2);
	lua_pushinteger(s, groupMgr->getGroupSize(groupId));
	return 1;
}

static int luaAI_groupmgrleader(lua_State* s) {
	const GroupMgr* groupMgr = luaAI_togroupmgr(s, 1);
	const GroupId groupId = (GroupId)luaL_checkinteger(s, 2);
	const AIPtr& ai = groupMgr->getLeader(groupId);
	if (!ai) {
		lua_pushnil(s);
	} else {
		luaAI_pushai(s, ai.get());
	}
	return 1;
}

static int luaAI_groupmgrtostring(lua_State* s) {
	const GroupMgr* groupMgr = luaAI_togroupmgr(s, 1);
	lua_pushfstring(s, "groupmgr: %p", groupMgr);
	return 1;
}

static int luaAI_zoneexecute(lua_State* s) {
	Zone* zone = luaAI_tozone(s, 1);
	luaL_checktype(s, 2, LUA_TFUNCTION);
	const int topIndex = lua_gettop(s);
	zone->execute([=] (const AIPtr& ai) {
		if (luaAI_pushai(s, ai.get()) <= 0) {
			return;
		}
		lua_pcall(s, 1, 0, 0);
		const int stackDelta = lua_gettop(s) - topIndex;
		if (stackDelta > 0) {
			lua_pop(s, stackDelta);
		}
	});
	return 0;
}

static int luaAI_zonegroupmgr(lua_State* s) {
	Zone* zone = luaAI_tozone(s, 1);
	return luaAI_pushgroupmgr(s, &zone->getGroupMgr());
}

static int luaAI_zonetostring(lua_State* s) {
	const Zone* zone = luaAI_tozone(s, 1);
	lua_pushfstring(s, "zone: %s", zone->getName().c_str());
	return 1;
}

static int luaAI_zonename(lua_State* s) {
	const Zone* zone = luaAI_tozone(s, 1);
	lua_pushstring(s, zone->getName().c_str());
	return 1;
}

static int luaAI_zoneai(lua_State* s) {
	Zone* zone = luaAI_tozone(s, 1);
	const CharacterId id = (CharacterId)luaL_checkinteger(s, 2);
	const AIPtr& ai = zone->getAI(id);
	if (!ai) {
		lua_pushnil(s);
	} else {
		luaAI_pushai(s, ai.get());
	}
	return 1;
}

static int luaAI_zonesize(lua_State* s) {
	const Zone* zone = luaAI_tozone(s, 1);
	lua_pushinteger(s, zone->size());
	return 1;
}

static int luaAI_aggromgrhighestentry(lua_State* s) {
	AggroMgr* aggroMgr = luaAI_toaggromgr(s, 1);
	EntryPtr entry = aggroMgr->getHighestEntry();
	if (entry == nullptr) {
		lua_pushnil(s);
		lua_pushnil(s);
	} else {
		lua_pushinteger(s, entry->getCharacterId());
		lua_pushnumber(s, entry->getAggro());
	}
	return 2;
}

static int luaAI_aggromgrentries(lua_State* s) {
	AggroMgr* aggroMgr = luaAI_toaggromgr(s, 1);
	const AggroMgr::Entries& entries = aggroMgr->getEntries();
	lua_newtable(s);
	const int top = lua_gettop(s);
	for (auto it = entries.begin(); it != entries.end(); ++it) {
		lua_pushinteger(s, it->getCharacterId());
		lua_pushnumber(s, it->getAggro());
		lua_settable(s, top);
	}
	return 1;
}

static int luaAI_aggromgraddaggro(lua_State* s) {
	AggroMgr* aggroMgr = luaAI_toaggromgr(s, 1);
	const CharacterId chrId = (CharacterId)luaL_checkinteger(s, 2);
	const float amount = luaL_checknumber(s, 3);
	const EntryPtr& entry = aggroMgr->addAggro(chrId, amount);
	lua_pushnumber(s, entry->getAggro());
	return 1;
}

static int luaAI_aggromgrtostring(lua_State* s) {
	lua_pushliteral(s, "aggroMgr");
	return 1;
}

static int luaAI_characterid(lua_State* s) {
	const ICharacter* chr = luaAI_tocharacter(s, 1);
	lua_pushinteger(s, chr->getId());
	return 1;
}

static int luaAI_characterposition(lua_State* s) {
	const ICharacter* chr = luaAI_tocharacter(s, 1);
	return luaAI_pushvec(s, chr->getPosition());
}

static int luaAI_charactersetposition(lua_State* s) {
	ICharacter* chr = luaAI_tocharacter(s, 1);
	const glm::vec3* v = luaAI_tovec(s, 2);
	chr->setPosition(*v);
	return 0;
}

static int luaAI_characterspeed(lua_State* s) {
	const ICharacter* chr = luaAI_tocharacter(s, 1);
	lua_pushnumber(s, chr->getSpeed());
	return 1;
}

static int luaAI_characterorientation(lua_State* s) {
	const ICharacter* chr = luaAI_tocharacter(s, 1);
	lua_pushnumber(s, chr->getOrientation());
	return 1;
}

static int luaAI_charactersetspeed(lua_State* s) {
	ICharacter* chr = luaAI_tocharacter(s, 1);
	const float value = luaL_checknumber(s, 2);
	chr->setSpeed(value);
	return 0;
}

static int luaAI_charactersetorientation(lua_State* s) {
	ICharacter* chr = luaAI_tocharacter(s, 1);
	const float value = luaL_checknumber(s, 2);
	chr->setOrientation(value);
	return 0;
}

static int luaAI_charactereq(lua_State* s) {
	const ICharacter* a = luaAI_tocharacter(s, 1);
	const ICharacter* b = luaAI_tocharacter(s, 2);
	const bool e = *a == *b;
	lua_pushboolean(s, e);
	return 1;
}

static int luaAI_characterattributes(lua_State* s) {
	const ICharacter* chr = luaAI_tocharacter(s, 1);
	const CharacterAttributes& attributes = chr->getAttributes();
	lua_newtable(s);
	const int top = lua_gettop(s);
	for (auto it = attributes.begin(); it != attributes.end(); ++it) {
		const std::string& key = it->first;
		const std::string& value = it->second;
		lua_pushlstring(s, key.c_str(), key.size());
		lua_pushlstring(s, value.c_str(), value.size());
		lua_settable(s, top);
	}
	return 1;
}

static int luaAI_charactersetattribute(lua_State* s) {
	ICharacter* chr = luaAI_tocharacter(s, 1);
	const char* key = luaL_checkstring(s, 2);
	const char* value = luaL_checkstring(s, 3);
	chr->setAttribute(key, value);
	return 0;
}

static int luaAI_charactertostring(lua_State* s) {
	ICharacter* character = luaAI_tocharacter(s, 1);
	lua_pushfstring(s, "Character: %d", (lua_Integer)character->getId());
	return 1;
}

static int luaAI_aiid(lua_State* s) {
	const AI* ai = luaAI_toai(s, 1);
	lua_pushinteger(s, ai->getId());
	return 1;
}

static int luaAI_aitime(lua_State* s) {
	const AI* ai = luaAI_toai(s, 1);
	lua_pushinteger(s, ai->getTime());
	return 1;
}

static int luaAI_aifilteredentities(lua_State* s) {
	const AI* ai = luaAI_toai(s, 1);
	const FilteredEntities& filteredEntities = ai->getFilteredEntities();
	lua_newtable(s);
	const int top = lua_gettop(s);
	int i = 0;
	for (auto it = filteredEntities.begin(); it != filteredEntities.end(); ++it) {
		lua_pushinteger(s, ++i);
		lua_pushinteger(s, *it);
		lua_settable(s, top);
	}
	return 1;
}

static int luaAI_aigetzone(lua_State* s) {
	const AI* ai = luaAI_toai(s, 1);
	return luaAI_pushzone(s, ai->getZone());
}

static int luaAI_aigetaggromgr(lua_State* s) {
	AI* ai = luaAI_toai(s, 1);
	return luaAI_pushaggromgr(s, &ai->getAggroMgr());
}

static int luaAI_aigetcharacter(lua_State* s) {
	const AI* ai = luaAI_toai(s, 1);
	return luaAI_pushcharacter(s, ai->getCharacter().get());
}

static int luaAI_aihaszone(lua_State* s) {
	const AI* ai = luaAI_toai(s, 1);
	lua_pushboolean(s, ai->hasZone() ? 1 : 0);
	return 1;
}

static int luaAI_aieq(lua_State* s) {
	const AI* a = luaAI_toai(s, 1);
	const AI* b = luaAI_toai(s, 2);
	const bool e = a->getId() == b->getId();
	lua_pushboolean(s, e);
	return 1;
}

static int luaAI_aitostring(lua_State* s) {
	const AI* ai = luaAI_toai(s, 1);
	TreeNodePtr treeNode = ai->getBehaviour();
	if (treeNode) {
		lua_pushfstring(s, "ai: %s", treeNode->getName().c_str());
	} else {
		lua_pushstring(s, "ai: no behaviour tree set");
	}
	return 1;
}

static int luaAI_vecadd(lua_State* s) {
	const glm::vec3* a = luaAI_tovec(s, 1);
	const glm::vec3* b = luaAI_tovec(s, 2);
	const glm::vec3& c = *a + *b;
	return luaAI_pushvec(s, c);
}

static int luaAI_vecdot(lua_State* s) {
	const glm::vec3* a = luaAI_tovec(s, 1);
	const glm::vec3* b = luaAI_tovec(s, 2);
	const float c = glm::dot(*a, *b);
	lua_pushnumber(s, c);
	return 1;
}

static int luaAI_vecdiv(lua_State* s) {
	const glm::vec3* a = luaAI_tovec(s, 1);
	const glm::vec3* b = luaAI_tovec(s, 2);
	const glm::vec3& c = *a / *b;
	luaAI_pushvec(s, c);
	return 1;
}

static int luaAI_veclen(lua_State* s) {
	const glm::vec3* a = luaAI_tovec(s, 1);
	const float c = glm::length(*a);
	lua_pushnumber(s, c);
	return 1;
}

static int luaAI_veceq(lua_State* s) {
	const glm::vec3* a = luaAI_tovec(s, 1);
	const glm::vec3* b = luaAI_tovec(s, 2);
	const bool e = glm::all(glm::epsilonEqual(*a, *b, 0.0001f));
	lua_pushboolean(s, e);
	return 1;
}

static int luaAI_vecsub(lua_State* s) {
	const glm::vec3* a = luaAI_tovec(s, 1);
	const glm::vec3* b = luaAI_tovec(s, 2);
	const glm::vec3& c = *a - *b;
	luaAI_pushvec(s, c);
	return 1;
}

static int luaAI_vecnegate(lua_State* s) {
	const glm::vec3* a = luaAI_tovec(s, 1);
	luaAI_pushvec(s, -(*a));
	return 1;
}

static int luaAI_vectostring(lua_State* s) {
	const glm::vec3* a = luaAI_tovec(s, 1);
	lua_pushfstring(s, "vec: %f:%f:%f", a->x, a->y, a->z);
	return 1;
}

static int luaAI_vecindex(lua_State *s) {
	const glm::vec3* v = luaAI_tovec(s, 1);
	const char* i = luaL_checkstring(s, 2);

	switch (*i) {
	case '0':
	case 'x':
	case 'r':
		lua_pushnumber(s, v->x);
		break;
	case '1':
	case 'y':
	case 'g':
		lua_pushnumber(s, v->y);
		break;
	case '2':
	case 'z':
	case 'b':
		lua_pushnumber(s, v->z);
		break;
	default:
		lua_pushnil(s);
		break;
	}

	return 1;
}

static int luaAI_vecnewindex(lua_State *s) {
	glm::vec3* v = luaAI_tovec(s, 1);
	const char *i = luaL_checkstring(s, 2);
	const float t = luaL_checknumber(s, 3);

	switch (*i) {
	case '0':
	case 'x':
	case 'r':
		v->x = t;
		break;
	case '1':
	case 'y':
	case 'g':
		v->y = t;
		break;
	case '2':
	case 'z':
	case 'b':
		v->z = t;
		break;
	default:
		break;
	}

	return 1;
}

}
