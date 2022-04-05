#pragma once

#include "Types.hh"
#include "Event.hh"
#include <unordered_map>

// Stores all of the citizen and building data.
// rename 'Simulation' ?
class Z
{
public:
	struct ResourceDelta
	{
		static const int windowSize = 1000;

		float average;

		ResourceDelta& operator +=(int quantity);
		inline ResourceDelta& operator -=(int quantity)
		{
			return operator +=(-quantity);
		}
	};

	void Deposit(Resource resource);
	int TryWithdraw(const Resource& resource, bool allowPartial);

	Citizen::Id Add(Citizen citizen);
	Building::Id Add(Building definition);

	bool Remove(Citizen::Id citizen);
	bool Remove(Building::Id definition);

	Citizen* const TryGet(Citizen::Id citizenId);
	Building* const TryGet(Building::Id buildingId);

	Citizen& Get(Citizen::Id citizenId);
	Building& Get(Building::Id buildingId);

	Iterator<std::unordered_map<Citizen::Id, Citizen>> AllCitizens() { return Iterator(m_allCitizens); }
	Iterator<std::unordered_map<Building::Id, Building>> AllBuildings() { return Iterator(m_allBuildings); }
	Iterator<std::unordered_map<ResourceDef::Id, Resource>> AllResources() { return Iterator(m_allResources); }
	CIterator<std::unordered_map<ResourceDef::Id, ResourceDelta>> ResourceDeltas() const { return CIterator(m_resourceDeltas); }

	// called just after addition
	Event<Citizen&> OnAddCitizen;
	Event<Building&> OnAddBuilding;

	// called just before removal
	Event<Citizen&> OnRemoveCitizen;
	Event<Building&> OnRemoveBuilding;

	void ResetDelta(ResourceDef::Id resource)
	{
		m_resourceDeltas.erase(resource);
	}

private:
	friend std::ostream& operator <<(std::ostream&, const Z& z);

private:
	std::unordered_map<ResourceDef::Id, Resource> m_allResources;
	std::unordered_map<ResourceDef::Id, ResourceDelta> m_resourceDeltas;

	std::unordered_map<Citizen::Id, Citizen> m_allCitizens;
	std::unordered_map<Building::Id, Building> m_allBuildings;

	Building::Id m_nextBuildingId;
	Citizen::Id m_nextCitizenId;
};

extern std::ostream& operator <<(std::ostream&, const Z& z);

/* some design stuff

citizens are assigned to the building most in demand

priority determined by:
	- any resource that has zero inventory
	- any resource that is required but overconsumed

--

citizens will be removed from their job if
	- high positive production rate


buildings are allocated by
	- research area
	- resource type unfulfilled
	- resource in high demand

*/