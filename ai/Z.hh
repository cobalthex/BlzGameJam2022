#pragma once

#include "Types.hh"
#include "Event.hh"
#include <unordered_map>

// Stores all of the citizen and building data.
namespace Z
{
	void Tick(); // call at the beginning of every frame

	Citizen::Id Add(Citizen citizen);
	Building::Id Add(Building building);

	bool Remove(Citizen::Id citizen);
	bool Remove(Building::Id building);

	Citizen* const TryGet(Citizen::Id citizenId);
	Building* const TryGet(Building::Id buildingId);

	Citizen& Get(Citizen::Id citizenId);
	Building& Get(Building::Id buildingId);

	Iterator<std::unordered_map<Citizen::Id, Citizen>> AllCitizens();
	Iterator<std::unordered_map<Building::Id, Building>> AllBuildings();

	// called just after addition
	extern Event<Citizen&> OnAddCitizen;
	extern Event<Building&> OnAddBuilding;

	// called just before removal
	extern Event<Citizen&> OnRemoveCitizen;
	extern Event<Building&> OnRemoveBuilding;

	std::ostream& State(std::ostream& ostream);
};