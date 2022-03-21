#include "ResourceStore.hh"

ResourceStore ResourceStore::Default{};

void ResourceStore::Deposit(const Resource& resource)
{
	if (resource.quantity < 0)
		return;

	m_resources[resource.resource].quantity += resource.quantity;
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
	return quantity;
}

#include <iostream>
void ResourceStore::PrintStores() const
{
	for (const auto& store : m_resources)
	{
		const auto& resource(ResourceDef::Get(store.first));
		std::cout << resource.name << ": " << store.second.quantity
			<< " -- " << resource.description << "\n";
	}
}