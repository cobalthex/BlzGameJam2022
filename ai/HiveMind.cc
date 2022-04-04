#include "HiveMind.hh"
#include "Colony.hh"
#include "Z.hh"
#include "Utils.hh"

bool HiveMind::EmployeePriorityComparer::operator()(Citizen::Id a, Citizen::Id b)
{
	const auto hydratedA(z.TryGet(a));
	if (!hydratedA)
		return true;
	const auto hydratedB(z.TryGet(b));
	if (!hydratedB)
		return false;

	return hydratedA->proficiency > hydratedB->proficiency;
}

HiveMind::HiveMind(Z& z)
	: m_z(z), m_comparer{z}
{
	m_z.OnAddCitizen.Add(this, &HiveMind::OnAddCitizen);
	m_z.OnAddBuilding.Add(this, &HiveMind::OnAddBuilding);

	m_z.OnRemoveCitizen.Add(this, &HiveMind::OnRemoveCitizen);
	m_z.OnRemoveBuilding.Add(this, &HiveMind::OnRemoveBuilding);
}

HiveMind::~HiveMind()
{
	m_z.OnAddCitizen.Remove(this, &HiveMind::OnAddCitizen);
	m_z.OnAddBuilding.Remove(this, &HiveMind::OnAddBuilding);

	m_z.OnRemoveCitizen.Remove(this, &HiveMind::OnRemoveCitizen);
	m_z.OnRemoveBuilding.Remove(this, &HiveMind::OnRemoveBuilding);
}

void HiveMind::OnAddCitizen(Citizen& citizen)
{
	// TODO: choose based on priority

	// find any building with matching priority
	Building::Id buildingId{};
	ptrdiff_t minSlotsAvailable = PTRDIFF_MAX;
	const auto iter(m_z.AllBuildings());
	for (auto it(iter.begin); it != iter.end; ++it)
	{
		const ptrdiff_t slotsAvailable(it->second.size - it->second.citizensEmployed.size());
		if (slotsAvailable <= 0)
			continue;

		const auto& productionDef(ProductionDef::TryGet(it->second.production.definition));

		if (
			// citizen doesn't have any specialty
			(citizen.lastDiscipline == Discipline::Id::None && 
				slotsAvailable < minSlotsAvailable) ||
			// prefer things the citizen can already do
			(citizen.lastDiscipline == 
				ProductionDef::TryGet(it->second.production.definition)->discipline)
			)
		{
			buildingId = it->first;
			minSlotsAvailable = slotsAvailable;
		}
	}

	if (buildingId != Building::Id::None)
	{
		Employ(citizen.id, buildingId);
		return;
	}

	// otherwise, stick them in the unemployment queue
	auto found(m_joblessCitizens.find(citizen.lastDiscipline));
	if (found == m_joblessCitizens.end())
		found = m_joblessCitizens.try_emplace(citizen.lastDiscipline, m_comparer).first;

	found->second.push(citizen);
}
void HiveMind::OnAddBuilding(Building& building)
{
	auto slotsAvailable = building.size - building.citizensEmployed.size();
	if (slotsAvailable <= 0)
		return;

	const auto def(ProductionDef::TryGet(building.production.definition));
	if (def)
	{
		const auto matchedDiscipline(m_joblessCitizens.find(def->discipline));
		if (matchedDiscipline != m_joblessCitizens.end())
		{
			while (slotsAvailable > 0 && !matchedDiscipline->second.empty())
			{
				const auto citizenId(matchedDiscipline->second.top());
				matchedDiscipline->second.pop();
				Employ(citizenId, building.id);
				--slotsAvailable;
			}
		}

	}

	if (slotsAvailable <= 0)
		return;

	for (auto& discipline : m_joblessCitizens)
	{
		// TODO: sort by discipline priority

		while (slotsAvailable > 0 && !discipline.second.empty())
		{
			const auto citizenId(discipline.second.top());
			discipline.second.pop();
			Employ(citizenId, building.id);
			--slotsAvailable;
		}

		if (slotsAvailable <= 0)
			return;
	}
}

void HiveMind::OnRemoveCitizen(Citizen& citizen)
{
	m_joblessCitizens.erase(citizen.lastDiscipline);

	const auto found(m_citizensToBuildings.find(citizen));
	if (found != m_citizensToBuildings.end())
	{
		const auto definition(m_z.TryGet(found->second));
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
		auto citizen(m_z.TryGet(citizenId));
		if (!citizen)
			continue;

		m_citizensToBuildings.erase(citizen->id);
	}
}

void HiveMind::Employ(Citizen::Id citizenId, Building::Id buildingId)
{
	auto& citizen(m_z.Get(citizenId)); // todo: once private take in citizen ref?
	auto& building(m_z.Get(buildingId)); // do this elsewhere?
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

	const std::string log
		= "Employed citizen "
		+ std::to_string(static_cast<size_t>(citizenId))
		+ " to '"
		+ buildingDef.name
		+ "' (ID "
		+ std::to_string(static_cast<size_t>(building.id))
		+ ")";
	DebugLog(log.data());
}

void HiveMind::Update(const TimeStep& time)
{
	auto citizenIter(m_z.AllCitizens());
	for (auto citizen(citizenIter.begin); citizen != citizenIter.end; ++citizen)
	{
		++citizen->second.hunger;

		// TODO: check hunger
		// consume food if high enough
		// if no food available then get mad
		// if hunger high enough, die (?)

		// if happiness < 0 then depart?
	}

	// jobless citizens should have their proficiency go down over time
}
