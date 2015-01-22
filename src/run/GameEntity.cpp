#include "GameEntity.h"
#include "GameMap.h"

namespace ai {
namespace example {

// returns a random start position within the boundaries
inline ai::Vector3f GameEntity::getStartPosition() const {
	const int size = _map->getSize();
	const int x = (rand() % (2 * size)) - size;
	const float y = 0.0f;
	const int z = (rand() % (2 * size)) - size;
	return ai::Vector3f(static_cast<float>(x), y, static_cast<float>(z));
}

GameEntity::GameEntity(const ai::CharacterId& id,
		const ai::example::GameMap* map,
		const ai::TreeNodePtr& root) :
		ai::ICharacter(id, root), _map(map), _hitpoints(100), _damage(5), _attackDelay(500) {
	// pick some random start position
	setPosition(getStartPosition());
	setAttribute(ai::attributes::NAME, "Example " + std::to_string(id));
	setSpeed(50.0f);
	_groupId = (id % 3) + 1;

	setAttribute(ai::attributes::GROUP, std::to_string(_groupId));
	setAttribute(ai::attributes::ID, std::to_string(getId()));
	setAttribute("Damage", std::to_string(_damage));
	setAttribute("Reloadtime", std::to_string(_attackDelay));

	_ai.getZone().getGroupMgr().add(_groupId, this);
}

void GameEntity::update(long deltaTime, bool debuggingActive) {
	ICharacter::update(deltaTime, debuggingActive);
	if (_ai.isPause()) {
		return;
	}

	// cap position to the map
	const float sizeF = static_cast<float>(_map->getSize());
	const Vector3f& currentPos = _position;
	Vector3f newPos(currentPos);
	if (currentPos.x < -sizeF) {
		newPos.x = sizeF;
	} else if (currentPos.x > sizeF) {
		newPos.x = -sizeF;
	}
	if (currentPos.z < -sizeF) {
		newPos.z = sizeF;
	} else if (currentPos.z > sizeF) {
		newPos.z = -sizeF;
	}
	setPosition(newPos);
	// TODO: switch direction when the respawn on another side of the map

	// update attributes for debugging
	if (debuggingActive) {
		setAttribute(ai::attributes::POSITION, std::to_string(getPosition()));
		setAttribute("Hitpoints", std::to_string(getHitpoints()));
		setAttribute("Reloadtime", std::to_string(getAttackDelay()));
		setAttribute("Damage", std::to_string(getDamage()));
		setAttribute(ai::attributes::SPEED, std::to_string(getSpeed()));
		setAttribute(ai::attributes::ORIENTATION, std::to_string(toDegrees(getOrientation())));
	}
}

}
}
