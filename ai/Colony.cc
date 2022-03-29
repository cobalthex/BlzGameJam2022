#include "Colony.hh"
#include "ResourceStore.hh"
#include "Z.hh"

Colony Colony::Default{};

Colony::Colony()
{
	Z::OnAddCitizen.Add(this, &OnAddCitizen);
	Z::OnAddBuilding.Add(this, &OnAddBuilding);

	Z::OnRemoveCitizen.Add(this, &OnRemoveCitizen);
	Z::OnRemoveBuilding.Add(this, &OnRemoveBuilding);
}

Colony::~Colony()
{
	Z::OnAddCitizen.Remove(this, &OnAddCitizen);
	Z::OnAddBuilding.Remove(this, &OnAddBuilding);
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
		.building = buildingDef,
		.state = BuildingState::Constructing,
		.production
		{
			.production = buildingDef.production,
			.generation = 0,
			.state = ProductionState::NotStarted,
		},
		.citizensEmployed = {},
	};
	return Z::Add(building);
}

int CalculateScale(ProductionScaleUpMethod method, int scale, int value)
{
	switch (method)
	{
	case ProductionScaleUpMethod::Linear:
		return scale * value;

	case ProductionScaleUpMethod::Constant:
	default:
		return value;
	}
}

void Colony::Update(const TimeStep& time)
{
	const auto bldgIter(Z::AllBuildings());
	for (auto bpair(bldgIter.begin); bpair != bldgIter.end; ++bpair)
	{
		const auto buildingDef(BuildingDef::TryGet(bpair->second.building));
		if (!buildingDef)
			continue;

		// todo: check building state

		auto& production(bpair->second.production);
		const auto& productionDef(ProductionDef::TryGet(production.production));
		switch (production.state)
		{
		case ProductionState::NotStarted:
			production.waitingInputs.assign(productionDef->inputs.begin(), productionDef->inputs.end());
			production.state = ProductionState::WaitingForResources;
			break;

		case ProductionState::WaitingForResources:
			if (bpair->second.citizensEmployed.empty())
				break;

			for (size_t i = 0; i < production.waitingInputs.size(); ++i)
			{
				auto& input(production.waitingInputs[i]);
				input.quantity -= ResourceStore::Default.TryWithdraw(input, true);
				if (input.quantity <= 0)
				{
					production.waitingInputs[i] = production.waitingInputs.back();
					production.waitingInputs.erase(production.waitingInputs.end() - 1);
					--i;
				}
			}
			if (production.waitingInputs.size() == 0)
			{
				production.start = time.now;
				production.state = ProductionState::Producing;
			}
			break;

		case ProductionState::Producing:
			// already producing, stop if citizens removed?

			if (time.now >= production.start + productionDef->duration)
			{
				for (Resource resource : productionDef->outputs)
				{
					resource.quantity = CalculateScale(
						productionDef->scaleUpMethod, 
						(int)bpair->second.citizensEmployed.size(), resource.quantity);
					ResourceStore::Default.Deposit(resource);
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