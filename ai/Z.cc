#include "Z.hh"

#include <unordered_map>
#include "Utils.hh"
#include <iostream>

Z::ResourceDelta& Z::ResourceDelta::operator+=(int quantity)
{
	// exponential weighted average
	average -= average / windowSize;
	average += (float)quantity / windowSize;

	return *this;
}

void Z::Deposit(Resource resource)
{
	if (resource.quantity < 0)
		return;

	m_allResources[resource.definition].quantity += resource.quantity;
	m_resourceDeltas[resource.definition] += resource.quantity;
}

int Z::TryWithdraw(const Resource& resource, bool allowPartial)
{
	if (resource.quantity < 0)
		return 0;

	const auto& found(m_allResources.find(resource.definition));
	if (found == m_allResources.end() || (!allowPartial && found->second.quantity < resource.quantity))
		return 0;

	// todo: add to deltas even if none left

	const auto quantity(std::min(resource.quantity, found->second.quantity));
	found->second.quantity -= quantity;

	m_resourceDeltas[resource.definition] -= quantity;

	return quantity;
}

Citizen::Id Z::Add(Citizen citizen)
{
	citizen.id = static_cast<Citizen::Id>(++(size_t&)(m_nextCitizenId));
	auto& ref(m_allCitizens[citizen.id] = citizen);
	OnAddCitizen.Invoke(ref);
	return citizen.id;
}
Building::Id Z::Add(Building building)
{
	building.id = static_cast<Building::Id>(++(size_t&)(m_nextBuildingId));
	auto& ref(m_allBuildings[building.id] = building);
	OnAddBuilding.Invoke(ref);
	return building.id;
}

bool Z::Remove(Citizen::Id citizenId)
{
	const auto found(m_allCitizens.find(citizenId));
	if (found == m_allCitizens.end())
		return false;

	OnRemoveCitizen.Invoke(found->second);
	m_allCitizens.erase(found);
	return true;
}

bool Z::Remove(Building::Id buildingId)
{
	const auto found(m_allBuildings.find(buildingId));
	if (found == m_allBuildings.end())
		return false;

	OnRemoveBuilding.Invoke(found->second);
	m_allBuildings.erase(found);
	return true;
}

Citizen* const Z::TryGet(Citizen::Id citizenId)
{
	const auto found(m_allCitizens.find(citizenId));
	return found == m_allCitizens.end() ? nullptr : &found->second;
}
Building* const Z::TryGet(Building::Id buildingId)
{
	const auto found(m_allBuildings.find(buildingId));
	return found == m_allBuildings.end() ? nullptr : &found->second;
}

Citizen& Z::Get(Citizen::Id citizenId)
{
	return m_allCitizens[citizenId];
}
Building& Z::Get(Building::Id buildingId)
{
	return m_allBuildings[buildingId];
}

Iterator<std::unordered_map<Citizen::Id, Citizen>> Z::AllCitizens()
{
	return Iterator(m_allCitizens);
}
Iterator<std::unordered_map<Building::Id, Building>> Z::AllBuildings()
{
	return Iterator(m_allBuildings);
}

Iterator<std::unordered_map<ResourceDef::Id, Resource>> Z::AllResources()
{
	return Iterator(m_allResources);
}

std::ostream& operator <<(std::ostream& ostream, const Z& z)
{
	ostream << "Resources:\n";
	for (const auto& store : z.m_allResources)
	{
		const auto& resource(ResourceDef::Get(store.first));
		ostream << "\t" << resource.name << ": " << store.second.quantity
			<< " -- " << resource.description << "\n";
	}

	ostream << "\nDeltas:\n";
	for (const auto& delta : z.m_resourceDeltas)
	{
		const auto& resource(ResourceDef::Get(delta.first));
		ostream << "\t" << resource.name << ": " << delta.second.average << "\n";
	}

	ostream << "\nBuildings:\n";
	for (const auto& bpair : z.m_allBuildings)
	{
		ostream << "\tID " << static_cast<size_t>(bpair.first) << ": ";

		const auto buildingDef(BuildingDef::TryGet(bpair.second.definition));
		if (!buildingDef)
		{
			ostream << "[UNKNOWN]\n";
			continue;
		}

		ostream << buildingDef->name << "\n";

		ostream << "\t\tCitizens employed: " << bpair.second.citizensEmployed.size() << "\n";

		const auto& production(bpair.second.production);
		const auto& productionDef(ProductionDef::TryGet(production.definition));
		if (productionDef)
		{
			ostream << "\t\tProduction: " << productionDef->name << "\n";
			ostream << "\t\t\tGeneration: " << production.generation << "\n";
			ostream << "\t\t\tState: " << (int)production.state << "\n";
			ostream << "\t\t\tTime remaining: " << production.timeRemaining << "\n";
		}
	}

	ostream << "\nCitizens:\n";
	for (const auto& cpair : z.m_allCitizens)
	{
		ostream << "\tID " << static_cast<size_t>(cpair.first) << "\n";
		ostream << "\t\tHunger: " << cpair.second.hunger << "\n";
		ostream << "\t\tHappiness: " << cpair.second.happiness << "\n";
	}

	ostream << std::endl;

	return ostream;
}