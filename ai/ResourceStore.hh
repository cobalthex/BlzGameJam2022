#pragma once

#include "Systems.hh"
#include <unordered_map>

// Manages resources for the colony
class ResourceStore
{
public:
	static ResourceStore Default; // todo: not this

	ResourceStore() = default;
	virtual ~ResourceStore() = default;

	void Deposit(const Resource& resource);
	int TryWithdraw(const Resource& resource, bool allowPartial);

	void PrintStores() const;

private:
	std::unordered_map<ResourceDef::Id, Resource> m_resources; // table of max values?
};