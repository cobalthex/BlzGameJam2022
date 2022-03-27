#include "ResourceStore.hh"
#include <iostream>

ResourceStore ResourceStore::Default{};

void ResourceStore::Deposit(const Resource& resource)
{
	if (resource.quantity < 0)
		return;

	m_resources[resource.resource].quantity += resource.quantity;
	m_deltas[resource.resource] += resource.quantity;
}

int ResourceStore::TryWithdraw(const Resource& resource, bool allowPartial)
{
	if (resource.quantity < 0)
		return 0;

	const auto& found(m_resources.find(resource.resource));
	if (found == m_resources.end() || (!allowPartial && found->second.quantity < resource.quantity))
		return 0;

	const auto quantity(std::min(resource.quantity, found->second.quantity));
	found->second.quantity -= quantity;

	m_deltas[resource.resource] -= quantity;

	return quantity;
}

void ResourceStore::Update(const TimeStep& time)
{
	m_deltas.clear();
}

std::ostream& operator<<(std::ostream& ostream, const ResourceStore& rs)
{
	ostream << "Resources:\n";
	for (const auto& store : rs.m_resources)
	{
		const auto& resource(ResourceDef::Get(store.first));
		ostream << "\t" << resource.name << ": " << store.second.quantity
			<< " -- " << resource.description << "\n";
	}

	return ostream;
}
