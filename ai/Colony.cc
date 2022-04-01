#include "Colony.hh"
#include "Z.hh"

Colony Colony::Default{};

Colony::Colony()
{
	//Z::OnAddCitizen.Add(this, &Colony::OnAddCitizen);
	//Z::OnAddBuilding.Add(this, &Colony::OnAddBuilding);
	//
	//Z::OnRemoveCitizen.Add(this, &Colony::OnRemoveCitizen);
	//Z::OnRemoveBuilding.Add(this, &Colony::OnRemoveBuilding);
}

Colony::~Colony()
{
	Z::OnAddCitizen.Remove(this, &Colony::OnAddCitizen);
	Z::OnAddBuilding.Remove(this, &Colony::OnAddBuilding);

	Z::OnRemoveCitizen.Remove(this, &Colony::OnRemoveCitizen);
	Z::OnRemoveBuilding.Remove(this, &Colony::OnRemoveBuilding);
}

void Colony::OnAddCitizen(Citizen& citizen)
{

}
void Colony::OnAddBuilding(Building& building)
{
}

void Colony::OnRemoveCitizen(Citizen& citizen)
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
		.production
		{
			.definition = buildingDef.production,
			.generation = 0,
			.state = ProductionState::NotStarted,
		},
		.citizensEmployed = {},
	};
	return Z::Add(building);
}

int CalculateScale(const ProductionScale& scalar, size_t citizensEmployed, long long value)
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

void Colony::Update(const TimeStep& time)
{
	const auto bldgIter(Z::AllBuildings());
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
				input.quantity -= Z::TryWithdraw(input, true);
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
					Z::Deposit(output);
				}

				for (auto citizenId : bpair->second.citizensEmployed)
				{
					auto citizen(Z::TryGet(citizenId));
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