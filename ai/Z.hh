#pragma once

#include "Types.hh"
#include "Event.hh"
#include <unordered_map>

// Stores all of the citizen and building data.
namespace Z
{
	void Tick(); // call at the beginning of every frame

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

	Iterator<std::unordered_map<Citizen::Id, Citizen>> AllCitizens();
	Iterator<std::unordered_map<Building::Id, Building>> AllBuildings();
	Iterator<std::unordered_map<ResourceDef::Id, Resource>> AllResources();

	// called just after addition
	extern Event<Citizen&> OnAddCitizen;
	extern Event<Building&> OnAddBuilding;

	// called just before removal
	extern Event<Citizen&> OnRemoveCitizen;
	extern Event<Building&> OnRemoveBuilding;

	std::ostream& State(std::ostream& ostream);
};

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