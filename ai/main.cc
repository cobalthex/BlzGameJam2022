#include "Types.hh"
#include "ResourceStore.hh"
#include "HiveMind.hh"
#include "Colony.hh"

#include <iostream>

// TODO: Precompiled header

void PrintMenu();

int main(int argc, char* argv[])
{
	const ResourceDef::Id coal(ResourceDef::Add(
	{
		.name = "Coal",
		.description = "Magic black rocks",
	}));
	const ResourceDef::Id iron(ResourceDef::Add(
	{
		.name = "Iron",
		.description = "Magic brown rocks",
	}));
	const ResourceDef::Id steel(ResourceDef::Add(
	{
		.name = "Steel",
		.description = "Stronk",
	}));

	const ProductionDef::Id mineIron(ProductionDef::Add(
	{
		.name = "Mine iron",
		.discipline = Discipline::Mining,
		.duration = std::chrono::milliseconds(1),
		.outputs {
			Resource {
				.resource = iron,
				.quantity = 1,
			},
		},
	}));
	const ProductionDef::Id mineCoal(ProductionDef::Add(
	{
		.name = "Mine coal",
		.discipline = Discipline::Mining,
		.duration = std::chrono::milliseconds(1),
		.outputs {
			Resource {
				.resource = coal,
				.quantity = 1,
			},
		},
	}));

	const ProductionDef::Id makeSteel(ProductionDef::Add(
	{
		.name = "Make steel",
		.discipline = Discipline::HeavyIndustry,
		.duration = std::chrono::milliseconds(2),
		.inputs {
			Resource {
				.resource = coal,
				.quantity = 2,
			},
			Resource{
				.resource = iron,
				.quantity = 1,
			},
		},
		.outputs {
			Resource {
				.resource = steel,
				.quantity = 1,
			},
		},
	}));

	BuildingDef::Id ironMine(BuildingDef::Add(
	{
		.name = "Iron Mine",
		.zone = ZoningRestriction::Industrial,
		.production = mineIron,
	}));
	Colony::Default.Provision(ironMine);

	BuildingDef::Id coalMine(BuildingDef::Add(
	{
		.name = "Coal Mine",
		.zone = ZoningRestriction::Industrial,
		.production = mineCoal,
	}));
	Colony::Default.Provision(coalMine);

	BuildingDef::Id steelMill(BuildingDef::Add(
	{
		.name = "Steel Mill",
		.zone = ZoningRestriction::Industrial,
		.production = makeSteel,
	}));
	Colony::Default.Provision(steelMill);

	HiveMind::Default.CreateCitizen();
	HiveMind::Default.CreateCitizen();
	auto citizen3(HiveMind::Default.CreateCitizen());

	TimeStep time
	{
		.delta = std::chrono::milliseconds(1),
		.total = std::chrono::milliseconds(0),
	};

	//HiveMind.AssignJob(time.total, citizen3, makeSteel);

	while (true)
	{
		time.total += time.delta;

		PrintMenu();
		const auto key(std::cin.get());
		std::cin.get(); // remove enter
		switch (key)
		{
		case '1':
			Colony::Default.Update(time);
			HiveMind::Default.Update(time);
			break;
		case '2':
			ResourceStore::Default.PrintStores();
			break;
		case '3':
			Colony::Default.PrintBuildings();
			break;

		case 'q':
		case 'Q':
			break;

		default:
			std::cout << key;
		}
		std::cout << std::endl;
	}

	return 0;
}

void PrintMenu()
{
	std::cout <<
		"Aigh Eye"
		"\n--------"
		"\n1: Tick"
		"\n2: Stores"
		"\n3: Buildings"
		"\nQ: Quit"
		"\n";
}