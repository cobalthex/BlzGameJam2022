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

		for (const auto& jso : json["resources"])
		{
			ResourceDef def;
			jso.get_to(def);
			ResourceDef::Add(def);
		}
		for (const auto& jso : json["disciplines"])
		{
			Discipline def;
			jso.get_to(def);
			Discipline::Add(def);
		}
		for (const auto& jso : json["productions"])
		{
			ProductionDef def;
			jso.get_to(def);
			ProductionDef::Add(def);
		}
		for (const auto& jso : json["zoningRestrictions"])
		{
			ZoningRestriction def;
			jso.get_to(def);
			ZoningRestriction::Add(def);
		}
		for (const auto& jso : json["buildings"])
		{
			BuildingDef def;
			jso.get_to(def);
			BuildingDef::Add(def);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "!!! Failed to parse definiations.json\n" << e.what() << std::endl;
		return 1;
	}

	Colony colony;
	HiveMind hiveMind;

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

			colony.Update(time);
			hiveMind.Update(time);
			break;
		case '2':
			std::cout << Z::State << "\n";
			break;

		case '3':
			std::cout << "Resources:\n" << ResourceDef::Definitions << "\n";
			std::cout << "Disciplines:\n" << Discipline::Definitions << "\n";
			std::cout << "Productions:\n" << ProductionDef::Definitions << "\n";
			std::cout << "Zoning restrictions:\n" << ZoningRestriction::Definitions << "\n";
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
			colony.Provision(BuildingDef::Get(BuildingDef::Id(random() % BuildingDef::Count() + 1)));
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