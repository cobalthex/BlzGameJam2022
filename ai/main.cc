#include "Types.hh"
#include "HiveMind.hh"
#include "Colony.hh"
#include "Z.hh"

#include <iostream>

// TODO: Precompiled header

void PrintMenu();

int main(int argc, char* argv[])
{
	TimeStep time
	{
		.delta = Duration(1),
		.now = Duration(0),
	};

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
		.resourceScalar {
			.method = ProductionScaleMethod::Linear,
			.coefficient = 1
		},
		.timeScalar {
			.method = ProductionScaleMethod::Constant,
			.coefficient = 1
		},
		.duration = Duration(1),
		.outputs {
			Resource {
				.definition = iron,
				.quantity = 1,
			},
		},
	}));
	const ProductionDef::Id mineCoal(ProductionDef::Add(
	{
		.name = "Mine coal",
		.discipline = Discipline::Mining,
		.resourceScalar {
			.method = ProductionScaleMethod::Linear,
			.coefficient = 1
		},
		.timeScalar {
			.method = ProductionScaleMethod::Constant,
			.coefficient = 1
		},
		.duration = Duration(1),
		.outputs {
			Resource {
				.definition = coal,
				.quantity = 1,
			},
		},
	}));

	const ProductionDef::Id makeSteel(ProductionDef::Add(
	{
		.name = "Make steel",
		.discipline = Discipline::HeavyIndustry,
		.resourceScalar {
			.method = ProductionScaleMethod::Linear,
			.coefficient = 1
		},
		.timeScalar {
			.method = ProductionScaleMethod::Constant,
			.coefficient = 1
		},
		.duration = Duration(2),
		.inputs {
			Resource {
				.definition = coal,
				.quantity = 2,
			},
			Resource{
				.definition = iron,
				.quantity = 1,
			},
		},
		.outputs {
			Resource {
				.definition = steel,
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
 	const auto ironMine0(Colony::Default.Provision(ironMine));

	BuildingDef::Id coalMine(BuildingDef::Add(
	{
		.name = "Coal Mine",
		.zone = ZoningRestriction::Industrial,
		.production = mineCoal,
	}));
	const auto coalMine0(Colony::Default.Provision(coalMine));

	BuildingDef::Id steelMill(BuildingDef::Add(
	{
		.name = "Steel Mill",
		.zone = ZoningRestriction::Industrial,
		.production = makeSteel,
	}));
	const auto steelMill0(Colony::Default.Provision(steelMill));

	auto citizen(Z::Add(Citizen{}));
	HiveMind::Default.Employ(time.now, citizen, ironMine0);

	citizen = Z::Add(Citizen{});
	HiveMind::Default.Employ(time.now, citizen, coalMine0);

	citizen = Z::Add(Citizen{});
	HiveMind::Default.Employ(time.now, citizen, steelMill0);

	while (true)
	{
		time.now += time.delta;

		PrintMenu();
		const auto key(std::cin.get());
		std::cin.get(); // remove enter
		switch (key)
		{
		case '1':
			Z::Tick();

			Colony::Default.Update(time);
			HiveMind::Default.Update(time);
			break;
		case '2':
			std::cout << Z::State << "\n";
			break;

		case '3':
			Z::Add(Citizen{});
			break;

		case 'q':
		case 'Q':
			return 0;

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
		"Simulation:"
		"\n--------"
		"\n1: Tick"
		"\n2: State"
		"\n3: Employ citizen"
		"\nQ: Quit"
		"\n";
}