#pragma once

#include <vector>
#include <string_view>
#include <chrono>

using Duration = std::chrono::nanoseconds;

template <typename TBase>
struct IIdent
{
	enum class Id : size_t
	{
		None = 0,
	};

	Id id{ Id::None };
	operator Id() const { return id; }
};

template <typename TDef>
struct IDef : public IIdent<TDef>
{
	// Use with care
	static TDef& Get(IIdent<TDef>::Id id)
	{
		if (id == IIdent<TDef>::Id::None)
			throw "ID out of range";
		return s_definitions[static_cast<size_t>(id)];
	}

	static const TDef* TryGet(IIdent<TDef>::Id id)
	{
		if (id != IIdent<TDef>::Id::None && s_definitions.size() > (size_t)id)
			return &s_definitions[static_cast<size_t>(id)];
		return nullptr;
	}

	static IIdent<TDef>::Id Add(TDef def)
	{
		const auto nextId(static_cast<IIdent<TDef>::Id>(s_definitions.size()));
		def.id = nextId;
		s_definitions.push_back(def);
		return def.id;
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
	Government,
};

struct Citizen : public IIdent<Citizen>
{
	int happiness;
	int hunger;

	int proficiency; // increases the longer a citizen performs their job
					  // changing job disciplines resets this

	Discipline lastDiscipline;

	// TODO: goal planning state machine
};

enum class ProductionScaleUpMethod
{
	Linear,
	Constant,
};

struct ProductionDef : public IDef<ProductionDef>
{
	std::string_view name;
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
	std::string_view name;
	ZoningRestriction zone;

	ProductionDef production;
};

struct Building : public IIdent<Building>
{
	BuildingDef::Id building{};

	BuildingState state{};

	Production production;
	std::vector<Citizen::Id> citizensEmployed;
};


struct TimeStep
{
	Duration delta;
	Duration now; // since game/sim start
};

template <typename T>
struct Iterator
{
	Iterator(T& ty) : begin(ty.begin()), end(ty.end()) { }

	T::iterator begin;
	T::iterator end;
};

template <typename T>
struct CIterator
{
	CIterator(const T& ty) : begin(ty.cbegin()), end(ty.cend()) { }

	T::const_iterator begin;
	T::const_iterator end;
};