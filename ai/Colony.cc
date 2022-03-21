#include "Colony.hh"
#include "ResourceStore.hh"

Colony Colony::Default{};

Building::Id Colony::Provision(BuildingDef::Id buildingDefId)
{
	const auto& buildingDef(BuildingDef::Get(buildingDefId));
	Building building
	{
		.building = buildingDef,
		.state = BuildingState::Constructing,
		.consDemoProgress = 0,
		.production
		{
			.production = buildingDef.production,
			.generation = 0,
			.state = ProductionState::NotStarted,
		},
		.citizensEmployed = 1, // TODO: TESTING
	};
	return Add(building);
}

Building::Id Colony::Add(Building building)
{
	building.id = ++m_nextBuildingId;
	m_allBuildings[building] = building;
	// building type (e.g. storage)
	return building;
}

void Colony::Remove(Building::Id buildingId)
{
	// building type reset
	// return value?
	m_allBuildings.erase(buildingId);
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
	for (auto& bpair : m_allBuildings)
	{
		const auto buildingDef(BuildingDef::TryGet(bpair.second.building));
		if (!buildingDef)
			continue;

		// todo: check building state

		auto& production(bpair.second.production);
		const auto& productionDef(ProductionDef::TryGet(production.production));
		switch (production.state)
		{
		case ProductionState::NotStarted:
			production.waitingInputs.assign(productionDef->inputs.begin(), productionDef->inputs.end());
			production.state = ProductionState::WaitingForResources;
			break;

		case ProductionState::WaitingForResources:
			if (bpair.second.citizensEmployed == 0)
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
				production.start = time.total;
				production.state = ProductionState::Producing;
			}
			break;

		case ProductionState::Producing:
			// already producing, stop if citizens removed?

			if (time.total >= production.start + productionDef->duration)
			{
				for (Resource resource : productionDef->outputs)
				{
					resource.quantity = CalculateScale(
						productionDef->scaleUpMethod, 
						bpair.second.citizensEmployed, resource.quantity);
					ResourceStore::Default.Deposit(resource);
				}
				++production.generation;
				production.state = ProductionState::NotStarted;
			}
			break;
		}
	}
}

#include <iostream>
void Colony::PrintBuildings() const
{
	for (const auto& bpair : m_allBuildings)
	{
		std::cout << "Building (ID " << bpair.first << "): ";
			
		const auto buildingDef(BuildingDef::TryGet(bpair.second.building));
		if (!buildingDef)
		{
			std::cout << "[UNKNOWN]\n";
			continue;
		}

		std::cout << buildingDef->name << "\n";

		auto& production(bpair.second.production);
		const auto& productionDef(ProductionDef::TryGet(production.production));
		if (productionDef)
		{
			std::cout << "\tProduction: " << productionDef->name << "\n";
			std::cout << "\t\tGeneration: " << production.generation << "\n";
			std::cout << "\t\tState: " << (int)production.state << "\n";
		}
	}
}