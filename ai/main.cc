#include "Utils.hh"
#include "Types.hh"
#include "HiveMind.hh"
#include "Colony.hh"
#include "Z.hh"

#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <mutex>

// TODO: Precompiled header

void PrintMenu();

int main(int argc, char* argv[])
{
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

	Z z;
	Colony colony(z);
	HiveMind hiveMind(z);

	std::random_device random;

	std::chrono::high_resolution_clock::time_point then;

	std::mutex cliMutex;

	std::jthread sim([&](std::stop_token st)
	{
		TimeStep time
		{
			.now = Duration(0),
		};

		while (!st.stop_requested())
		{
			const std::lock_guard lock(cliMutex);

			const auto now(std::chrono::high_resolution_clock::now());
			time.Advance(now - then);

			colony.Update(time);
			hiveMind.Update(time);

			then = now;
		}
	});

	while (true)
	{
		PrintMenu();
		const auto key(std::cin.get());
		std::cin.get(); // remove enter
		const std::lock_guard lock(cliMutex);
		switch (key)
		{
		case '1':
			std::cout << z << "\n";
			break;

		case '2':
			std::cout << "Resources:\n" << ResourceDef::Definitions << "\n";
			std::cout << "Disciplines:\n" << Discipline::Definitions << "\n";
			std::cout << "Productions:\n" << ProductionDef::Definitions << "\n";
			std::cout << "Zoning restrictions:\n" << ZoningRestriction::Definitions << "\n";
			std::cout << "Buildings:\n" << BuildingDef::Definitions << "\n";
			break;

		case '3':
			z.Add(Citizen{});
			break;

		case '4':
			if (BuildingDef::Count() < 1)
			{
				std::cout << "No building definitions available";
				break;
			}
			colony.Provision(BuildingDef::Get(BuildingDef::Id(random() % BuildingDef::Count() + 1)));
			break;

		case 'q':
		case 'Q':
			sim.request_stop();
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
		"\n1: State"
		"\n2: Definitions"
		"\n3: Provision citizen"
		"\n4: Provision random building"
		"\nQ: Quit"
		"\n";
}