#include "HiveMind.hh"
#include "ResourceStore.hh"

HiveMind HiveMind::Default{};

void HiveMind::Update(const TimeStep& time)
{
}

Citizen::Id HiveMind::CreateCitizen()
{
	Citizen citizen{};
	citizen.id = ++m_nextCitizenId;
	m_allCitizens[citizen] = citizen;
	return citizen;
}

void HiveMind::AssignToBuilding(std::chrono::milliseconds now, Citizen::Id citizenId, BuildingDef::Id building)
{
	auto& citizen(m_allCitizens[citizenId]); // todo: once private take in citizen ref

	Discipline newJobType = Discipline::Construction;

	// TODO
	//const auto& oldJob(JobDef::TryGet(citizen.production.production));
	//const auto& newJob(JobDef::Get(production));

	//citizen.production = Job
	//{
	//	.production = newJob,
	//	.originalStart = now,
	//	.start = now,
	//};

	//// todo: pull out into own method
	//switch (newJob.type)
	//{
	//case JobType::Resources:
	//	for (const auto& input : newJob.inputs)
	//	{
	//		auto& store(m_stores[input.resource]);
	//		store.quantity -= input.quantity;
	//	}
	//	break;

	//case JobType::Spawner:
	//	break;

	//default:
	//	break;
	//}

	//if (oldJob && oldJob->discipline == newJob.discipline) return;
	//citizen.lastDiscipline = newJob.discipline;
	citizen.proficiency = 0;
}
