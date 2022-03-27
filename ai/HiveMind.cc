#include "HiveMind.hh"
#include "ResourceStore.hh"
#include "Colony.hh"
#include "Z.hh"

HiveMind HiveMind::Default{};

Citizen::Id HiveMind::CreateCitizen()
{
	const auto citizenId(Z::Add(Citizen()));
	m_joblessCitizens.insert(std::pair(Discipline::Unknown, citizenId));
	return citizenId;
}

void HiveMind::RemoveCitizen(Citizen::Id citizenId)
{
	const auto citizen(Z::TryGet(citizenId));
	if (!citizen) return;

	// TODO: erase from jobless, remove from job
}

void HiveMind::AssignToBuilding(Duration now, Citizen::Id citizenId, Building::Id buildingId)
{
	auto& citizen(Z::Get(citizenId)); // todo: once private take in citizen ref?
	auto& building(Z::Get(buildingId)); // do this elsewhere?
	const auto& buildingDef(BuildingDef::Get(building.building));

	if (citizen.lastDiscipline != buildingDef.production.discipline)
	{
		citizen.lastDiscipline = buildingDef.production.discipline;
		citizen.proficiency = 0;
	}

	building.citizensEmployed.push_back(citizen);
	citizen.happiness += 1; // some value here?
}

void HiveMind::Update(const TimeStep& time)
{
	auto citizenIter(Z::AllCitizens());
	for (auto citizen(citizenIter.begin); citizen != citizenIter.end; ++citizen)
	{
		++citizen->second.hunger;

		// TODO: check hunger
		// consume food if high enough
		// if no food available then get mad
		// if hunger high enough, die (?)

		// if happiness < 0 then depart?
	}


}
