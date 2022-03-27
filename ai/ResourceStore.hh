#pragma once

#include "Systems.hh"
#include <unordered_map>

// Manages resources for the colony
class ResourceStore : public ISystem
{
public:
	static ResourceStore Default; // todo: not this

	ResourceStore() = default;
	virtual ~ResourceStore() = default;

	void Deposit(const Resource& resource);
	int TryWithdraw(const Resource& resource, bool allowPartial);

	void Update(const TimeStep& time);
private:
	// todo: move these to Z
	std::unordered_map<ResourceDef::Id, Resource> m_resources;
	std::unordered_map<ResourceDef::Id, ptrdiff_t> m_deltas;

	friend std::ostream& operator <<(std::ostream&, const ResourceStore&);
};

extern std::ostream& operator <<(std::ostream&, const ResourceStore&);