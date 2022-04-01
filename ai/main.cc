#include "Utils.hh"
#include "Types.hh"
#include "HiveMind.hh"
#include "Colony.hh"
#include "Z.hh"

#include <iostream>
#include <fstream>
#include <random>

// TODO: Precompiled header

void PrintMenu();

int main(int argc, char* argv[])
{
	TimeStep time
	{
		.delta = Duration(1),
		.now = Duration(0),
	};

	try
	{
		std::ifstream fin("data/definitions.json");
		nlohmann::json json(nlohmann::json::parse(fin, nullptr, true /* allow exceptions */, true /* ignore comments */));

		for (const auto& rd : json["resources"])
		{
			ResourceDef resDef;
			rd.get_to(resDef);
			ResourceDef::Add(resDef);
		}
		for (const auto& pd : json["productions"])
		{
			ProductionDef prodDef;
			pd.get_to(prodDef);
			ProductionDef::Add(prodDef);
		}
		for (const auto& bd : json["buildings"])
		{
			BuildingDef bldgDef;
			bd.get_to(bldgDef);
			BuildingDef::Add(bldgDef);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "!!! Failed to parse definiations.json\n" << e.what() << std::endl;
		return 1;
	}

	// TODO

	std::random_device random;

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
			std::cout << "Resources:\n" << ResourceDef::Definitions << "\n";
			std::cout << "Productions:\n" << ProductionDef::Definitions << "\n";
			std::cout << "Buildings:\n" << BuildingDef::Definitions << "\n";
			break;

		case '4':
			Z::Add(Citizen{});
			break;

		case '5':
			if (BuildingDef::Count() < 1)
			{
				std::cout << "No building definitions available";
				break;
			}
			Colony::Default.Provision(BuildingDef::Get(BuildingDef::Id(random() % BuildingDef::Count() + 1)));
			break;

		case 'q':
		case 'Q':
			return 0;

		default:
			std::cout << "Unknown key: '" << (char)key << "' (" << key << ")";
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
		"\n3: Definitions"
		"\n4: Provision citizen"
		"\n5: Provision random building"
		"\nQ: Quit"
		"\n";
}