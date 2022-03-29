#include "HiveMind.hh"
#include "ResourceStore.hh"
#include "Colony.hh"
#include "Z.hh"

bool EmployeePriorityComparer::operator()(Citizen::Id a, Citizen::Id b)
{
	const auto hydratedA(Z::TryGet(a));
	if (!hydratedA)
		return true;
	const auto hydratedB(Z::TryGet(b));
	if (!hydratedB)
		return false;

	return hydratedA->proficiency > hydratedB->proficiency;
}

HiveMind HiveMind::Default{};

HiveMind::HiveMind()
{
	Z::OnAddCitizen.Add(this, &HiveMind::OnAddCitizen);
	Z::OnAddBuilding.Add(this, &HiveMind::OnAddBuilding);

	Z::OnRemoveCitizen.Add(this, &HiveMind::OnRemoveCitizen);
	Z::OnRemoveBuilding.Add(this, &HiveMind::OnRemoveBuilding);
}

HiveMind::~HiveMind()
{
	Z::OnAddCitizen.Remove(this, &HiveMind::OnAddCitizen);
	Z::OnAddBuilding.Remove(this, &HiveMind::OnAddBuilding);
}

void HiveMind::OnAddCitizen(Citizen& citizen)
{
	m_joblessCitizens[citizen.lastDiscipline].push(citizen);
}
void HiveMind::OnAddBuilding(Building& building)
{
	const auto def(ProductionDef::TryGet(building.production.production));
	if (def)
	{
		const auto matchedDiscipline(m_joblessCitizens.find(def->discipline));
		// TODO
	}

	// employ jobless citizens
	for (const auto& citizens : m_joblessCitizens)
	{
		// employ up to (delta)
		
	}
}

void HiveMind::OnRemoveCitizen(Citizen& citizen)
{
	m_joblessCitizens.erase(citizen.lastDiscipline);

	// remove the citizen from their job
	// TODO
	/*auto building(Z::TryGet(citizen.occupation));
	if (building)
	{
		const auto foundEmployee(std::find(building->citizensEmployed.begin(), building->citizensEmployed.end(), citizen.id));
		if (foundEmployee != building->citizensEmployed.end())
			building->citizensEmployed.erase(foundEmployee);
	}*/
}
void HiveMind::OnRemoveBuilding(Building& building)
{
	for (const auto citizenId : building.citizensEmployed)
	{
		auto citizen(Z::TryGet(citizenId));
		if (!citizen)
			continue;

		//m_joblessCitizens.emplace(citizen->lastDiscipline, citizen);
		// todo: remove citizen's occupation
	}
}

bool HiveMind::GetEmployeePriority(Citizen::Id a, Citizen::Id b)
{
	return false;
}

void HiveMind::Employ(Duration now, Citizen::Id citizenId, Building::Id buildingId)
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
	//citizen.occupation = building; // TODO
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
