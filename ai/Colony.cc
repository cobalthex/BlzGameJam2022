#include "Colony.hh"
#include "Z.hh"

Colony::Colony(Z& z)
	: m_z(z)
{
	m_z.OnAddBuilding.Add(this, &Colony::OnAddBuilding);
	m_z.OnRemoveBuilding.Add(this, &Colony::OnRemoveBuilding);
}

Colony::~Colony()
{
	m_z.OnAddBuilding.Remove(this, &Colony::OnAddBuilding);
	m_z.OnRemoveBuilding.Remove(this, &Colony::OnRemoveBuilding);
}

void Colony::OnAddBuilding(Building& building)
{

}
void Colony::OnRemoveBuilding(Building& building)
{
}

Building::Id Colony::Provision(BuildingDef::Id buildingDefId)
{
	const auto& buildingDef(BuildingDef::Get(buildingDefId));
	Building building
	{
		.definition = buildingDef,
		.state = BuildingState::Constructing,
		.size = 1,
		.production
		{
			.definition = buildingDef.production,
			.generation = 0,
			.state = ProductionState::NotStarted,
		},
		.citizensEmployed = {},
	};
	return m_z.Add(building);
}

int Colony::CalculateScale(const ProductionScale& scalar, size_t citizensEmployed, long long value)
{
	float scale = citizensEmployed * scalar.coefficient;
	switch (scalar.method)
	{
	case ProductionScaleMethod::Linear:
		return (int)(value * scale);

	case ProductionScaleMethod::Constant:
	default:
		return (int)value;
	}
}

void Colony::UpdateProductions(const TimeStep& time)
{
	const auto bldgIter(m_z.AllBuildings());
	for (auto bpair(bldgIter.begin); bpair != bldgIter.end; ++bpair)
	{
		const auto buildingDef(BuildingDef::TryGet(bpair->second.definition));
		if (!buildingDef)
			continue;

		// todo: check building state

		auto& production(bpair->second.production);
		const auto& productionDef(ProductionDef::TryGet(production.definition));
		switch (production.state)
		{
		case ProductionState::NotStarted:
			production.waitingInputs.clear();
			for (auto input : productionDef->inputs)
			{
				input.quantity = CalculateScale(
					productionDef->resourceScalar,
					(int)bpair->second.citizensEmployed.size(), input.quantity);
				production.waitingInputs.push_back(input);
			}
			production.state = ProductionState::WaitingForResources;
			break;

		case ProductionState::WaitingForResources:
			if (bpair->second.citizensEmployed.empty())
				break;

			for (size_t i = 0; i < production.waitingInputs.size(); ++i)
			{
				auto& input(production.waitingInputs[i]);
				input.quantity -= m_z.TryWithdraw(input, true);
				if (input.quantity <= 0)
				{
					production.waitingInputs[i] = production.waitingInputs.back();
					production.waitingInputs.erase(production.waitingInputs.end() - 1);
					--i;
				}
			}
			if (production.waitingInputs.size() == 0)
			{
				production.timeRemaining = productionDef->duration;
				production.state = ProductionState::Producing;
			}
			break;

		case ProductionState::WaitingForEmployees:
			if (bpair->second.citizensEmployed.size() > 0)
				production.state = ProductionState::Producing;
			break;

		case ProductionState::Producing:
			const auto citizensEmployed(bpair->second.citizensEmployed.size());
			if (citizensEmployed < 1)
			{
				production.state = ProductionState::WaitingForEmployees;
				break;
			}

			production.timeRemaining -= Duration(CalculateScale(
				productionDef->timeScalar, citizensEmployed, time.delta.count()));

			if (production.timeRemaining <= Duration::zero())
			{
				for (Resource output : productionDef->outputs)
				{
					output.quantity = CalculateScale(
						productionDef->resourceScalar,
						(int)bpair->second.citizensEmployed.size(), output.quantity);
					m_z.Deposit(output);
				}

				for (auto citizenId : bpair->second.citizensEmployed)
				{
					auto citizen(m_z.TryGet(citizenId));
					if (!citizen)
						continue;

					++citizen->happiness;
				}

				++production.generation;
				production.state = ProductionState::NotStarted;

				// todo: increase citizens' proficiency
			}
			break;
		}
	}
}

void Colony::Update(const TimeStep& time)
{
	UpdateProductions(time);


}