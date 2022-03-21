#pragma once

#include <vector>
#include <string_view>
#include <chrono>

using Duration = std::chrono::nanoseconds;

struct IIdent
{
	using Id = size_t;
	static const Id None{ 0 };

	Id id{ None };
	operator Id() const { return id; }
};

template <typename TDef>
struct IDef : public IIdent
{
	static TDef& Get(Id id)
	{
		return s_definitions[id];
	}
	static Id Add(TDef def)
	{
		const auto nextId(s_definitions.size());
		def.id = nextId;
		s_definitions.push_back(def);
		return def.id;
	}
	static const TDef* TryGet(Id id)
	{
		if (s_definitions.size() > id)
			return &s_definitions[id];
		return nullptr;
	}

private:
	static std::vector<TDef> s_definitions;
};

template <typename TDef>
std::vector<TDef> IDef<TDef>::s_definitions(1); // fill 0 with empty

struct ResourceDef : public IDef<ResourceDef>
{
	std::string_view name;
	std::string_view description;
};

struct Resource
{
	ResourceDef::Id resource;
	int quantity;
};

enum class ProductionType // needed?
{
	Resources, // default resource consume/produce
	Spawner, // outputs are citizens

	// ---
	Unknown,
};

// rough groupings for job types
enum class Discipline
{
	Unknown,
	Construction,
	Farming,
	Cooking,
	Mining,
	HeavyIndustry,
	Retail,

};

enum class ProductionScaleUpMethod
{
	Linear,
	Constant,
};

struct ProductionDef : public IDef<ProductionDef>
{
	ProductionType type;
	Discipline discipline;
	ProductionScaleUpMethod scaleUpMethod;

	Duration duration;

	std::vector<Resource> inputs;
	std::vector<Resource> outputs;
};

enum class ProductionState
{
	NotStarted,
	WaitingForResources,
	Producing,
};

struct Production
{
	ProductionDef::Id production;
	size_t generation;
	Duration start;
	ProductionState state;
	std::vector<Resource> waitingInputs;
};

enum class ZoningRestriction
{
	None,
	Transit, // roads/etc
	Industrial, // base production facilities
	Commercial, // citizens can utilize these directly
	Residential, // Where citizens can live
	Administrative, // government/defense
};

enum class BuildingState
{
	Constructing,
	Demolishing,
	InUse,
	Abandoned,
};

struct BuildingDef : public IDef<BuildingDef>
{
	std::string name;
	ZoningRestriction zone;

	ProductionDef production;
};

using Progress = int;

struct Building : public IIdent
{
	BuildingDef::Id building{};

	BuildingState state{};
	Progress consDemoProgress{}; // construction/demolition progress

	Production production;
	int citizensEmployed;
	// todo: store geo coords
};

struct Citizen : public IIdent
{
	int happiness;
	int hunger;

	int proficiency; // increases the longer a citizen performs their job
					  // changing job disciplines resets this

	Discipline lastDiscipline;

	// todo: types
	int goal;
	int action;
	int actionCompletionTime;

	int location;
};

struct TimeStep
{
	Duration delta;
	Duration total; // since game/sim start
};