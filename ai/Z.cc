#include "Z.hh"

#include <unordered_map>
#include <iostream>

namespace Z
{
	std::unordered_map<ResourceDef::Id, Resource> allResources;
	std::unordered_map<ResourceDef::Id, ptrdiff_t> resourceDeltas;

	std::unordered_map<Citizen::Id, Citizen> allCitizens;
	std::unordered_map<Building::Id, Building> allBuildings;

	Building::Id m_nextBuildingId;
	Citizen::Id m_nextCitizenId;

	Event<Citizen&> OnAddCitizen;
	Event<Building&> OnAddBuilding;

	Event<Citizen&> OnRemoveCitizen;
	Event<Building&> OnRemoveBuilding;

	void Tick()
	{
		resourceDeltas.clear();
	}

	void Deposit(Resource resource)
	{
		if (resource.quantity < 0)
			return;

		allResources[resource.definition].quantity += resource.quantity;
		resourceDeltas[resource.definition] += resource.quantity;
	}

	int TryWithdraw(const Resource& resource, bool allowPartial)
	{
		if (resource.quantity < 0)
			return 0;

		const auto& found(allResources.find(resource.definition));
		if (found == allResources.end() || (!allowPartial && found->second.quantity < resource.quantity))
			return 0;

		// todo: add to deltas even if none left

		const auto quantity(std::min(resource.quantity, found->second.quantity));
		found->second.quantity -= quantity;

		resourceDeltas[resource.definition] -= quantity;

		return quantity;
	}

	Citizen::Id Add(Citizen citizen)
	{
		citizen.id = static_cast<Citizen::Id>(++(size_t&)(m_nextCitizenId));
		allCitizens[citizen.id] = citizen;
		OnAddCitizen.Invoke(citizen);
		return citizen.id;
	}
	Building::Id Add(Building definition)
	{
		definition.id = static_cast<Building::Id>(++(size_t&)(m_nextBuildingId));
		allBuildings[definition.id] = definition;
		return definition.id;
	}

	bool Remove(Citizen::Id citizenId)
	{
		const auto found(allCitizens.find(citizenId));
		if (found == allCitizens.end())
			return false;

		OnRemoveCitizen.Invoke(found->second);
		allCitizens.erase(found);
		return true;
	}

	bool Remove(Building::Id buildingId)
	{
		const auto found(allBuildings.find(buildingId));
		if (found == allBuildings.end())
			return false;

		OnRemoveBuilding.Invoke(found->second);
		allBuildings.erase(found);
		return true;
	}

	Citizen* const TryGet(Citizen::Id citizenId)
	{
		const auto found(allCitizens.find(citizenId));
		return found == allCitizens.end() ? nullptr : &found->second;
	}
	Building* const TryGet(Building::Id buildingId)
	{
		const auto found(allBuildings.find(buildingId));
		return found == allBuildings.end() ? nullptr : &found->second;
	}

	Citizen& Get(Citizen::Id citizenId)
	{
		return allCitizens[citizenId];
	}
	Building& Get(Building::Id buildingId)
	{
		return allBuildings[buildingId];
	}

	Iterator<std::unordered_map<Citizen::Id, Citizen>> AllCitizens()
	{
		return Iterator(allCitizens);
	}
	Iterator<std::unordered_map<Building::Id, Building>> AllBuildings()
	{
		return Iterator(allBuildings);
	}

	Iterator<std::unordered_map<ResourceDef::Id, Resource>> AllResources()
	{
		return Iterator(allResources);
	}

	std::ostream& State(std::ostream& ostream)
	{
		ostream << "Resources:\n";
		for (const auto& store : allResources)
		{
			const auto& resource(ResourceDef::Get(store.first));
			ostream << "\t" << resource.name << ": " << store.second.quantity
				<< " -- " << resource.description << "\n";
		}

		ostream << "\nBuildings:\n";
		for (const auto& bpair : allBuildings)
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
		for (const auto& cpair : allCitizens)
		{
			ostream << "\tID " << static_cast<size_t>(cpair.first) << "\n";
			ostream << "\t\tHunger: " << cpair.second.hunger << "\n";
			ostream << "\t\tHappiness: " << cpair.second.happiness << "\n";
		}

		ostream << std::endl;

		return ostream;
	}
}