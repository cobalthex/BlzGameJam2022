#include "HiveMind.hh"
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

	Z::OnRemoveCitizen.Remove(this, &HiveMind::OnRemoveCitizen);
	Z::OnRemoveBuilding.Remove(this, &HiveMind::OnRemoveBuilding);
}

void HiveMind::OnAddCitizen(Citizen& citizen)
{
	m_joblessCitizens[citizen.lastDiscipline].push(citizen);
}
void HiveMind::OnAddBuilding(Building& building)
{
	const auto def(ProductionDef::TryGet(building.production.definition));
	if (def)
	{
		const auto matchedDiscipline(m_joblessCitizens.find(def->discipline));
		// TODO: employ matched discipline citizens first

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

	const auto found(m_citizensToBuildings.find(citizen));
	if (found != m_citizensToBuildings.end())
	{
		const auto definition(Z::TryGet(found->second));
		if (definition)
		{
			const auto foundEmployee(std::find(definition->citizensEmployed.begin(), definition->citizensEmployed.end(), citizen.id));
			if (foundEmployee != definition->citizensEmployed.end())
				definition->citizensEmployed.erase(foundEmployee);
		}

		m_citizensToBuildings.erase(found);
	}
}
void HiveMind::OnRemoveBuilding(Building& building)
{
	for (const auto citizenId : building.citizensEmployed)
	{
		auto citizen(Z::TryGet(citizenId));
		if (!citizen)
			continue;

		m_citizensToBuildings.erase(citizen->id);
	}
}

void HiveMind::Employ(Duration now, Citizen::Id citizenId, Building::Id buildingId)
{
	auto& citizen(Z::Get(citizenId)); // todo: once private take in citizen ref?
	auto& building(Z::Get(buildingId)); // do this elsewhere?
	const auto& buildingDef(BuildingDef::Get(building.definition));
	const auto& productionDef(ProductionDef::Get(buildingDef.production));

	if (citizen.lastDiscipline != productionDef.discipline)
	{
		citizen.lastDiscipline = productionDef.discipline;
		citizen.proficiency = 0;
	}

	building.citizensEmployed.push_back(citizen);
	m_citizensToBuildings[citizen] = building;
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
