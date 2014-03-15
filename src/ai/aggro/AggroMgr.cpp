#include <AI.h>
#include <aggro/AggroMgr.h>
#include <algorithm>

namespace ai {

AggroMgr::AggroMgr() :
		_lastUpdateTime(0L), _dirty(false) {
}

AggroMgr::~AggroMgr() {
}

void AggroMgr::cleanupList() {
	std::size_t remove = 0u;
	for (Entries::iterator i = _entries.begin(); i != _entries.end(); ++i) {
		const float aggroValue = (*i)->getAggro();
		if (aggroValue > 0.0f)
			break;

		++remove;
	}

	if (remove == 0u)
		return;

	const std::size_t size = _entries.size();
	if (size == remove)
		_entries.clear();
	else
		_entries.erase(_entries.begin(), _entries.begin() + remove);
}

void AggroMgr::update(long currentMillis) {
	const long delta = currentMillis - _lastUpdateTime;
	const std::size_t size = _entries.size();
	for (std::size_t i = 0; i < size; ++i)
		_dirty |= _entries[i]->reduceByTime(delta);

	if (_dirty) {
		sort();
		cleanupList();
	}
	_lastUpdateTime = currentMillis;
}

class CharacterIdPredicate {
private:
	const CharacterId& _id;
public:
	CharacterIdPredicate(const CharacterId& id) :
			_id(id) {
	}

	bool operator()(const EntryPtr &n1) {
		return n1->getCharacterId() == _id;
	}
};

bool EntrySorter(const EntryPtr& a, const EntryPtr& b) {
	return a->getAggro() < b->getAggro();
}

inline void AggroMgr::sort() {
	if (!_dirty)
		return;
	std::sort(_entries.begin(), _entries.end(), EntrySorter);
	_dirty = false;
}

Entry* AggroMgr::addAggro(AI& entity, float amount) {
	const CharacterId id = entity.getCharacter().getId();
	const CharacterIdPredicate p(id);
	Entries::const_iterator i = std::find_if(_entries.begin(), _entries.end(), p);
	if (i == _entries.end()) {
		Entry* e = new Entry(id, amount);
		const EntryPtr newEntry(e);
		_entries.push_back(newEntry);
		_dirty = true;
		return e;
	}

	(*i)->addAggro(amount);
	_dirty = true;
	return i->get();
}

/**
 * @brief Get the entry with the highest aggro value.
 */
EntryPtr AggroMgr::getHighestEntry() {
	if (_entries.empty())
		return EntryPtr();

	sort();

	return _entries.back();
}

}
