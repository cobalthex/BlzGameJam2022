#pragma once

#include "Types.hh"
#include <unordered_map>

namespace Z
{
	void Tick(); // call at the beginning of every frame

	Citizen::Id Add(Citizen citizen);
	Building::Id Add(Building building);

	Citizen* const TryGet(Citizen::Id citizenId);
	Building* const TryGet(Building::Id buildingId);

	Citizen& Get(Citizen::Id citizenId);
	Building& Get(Building::Id buildingId);

	Iterator<std::unordered_map<Citizen::Id, Citizen>> AllCitizens();
	Iterator<std::unordered_map<Building::Id, Building>> AllBuildings();

	std::ostream& State(std::ostream& ostream);
};