#pragma once

#include "thirdparty/Json.hpp"
#include <vector>
#include <string_view>
#include <chrono>

using Duration = std::chrono::high_resolution_clock::duration;
namespace std::chrono
{
	void from_json(const nlohmann::json&, Duration&);
}

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

struct TimeStep
{
	Duration delta;
	Duration now; // since game/sim start

	void Advance(Duration deltaSinceLastStep)
	{
		delta = deltaSinceLastStep;
		now = now;
	}
};

template <typename TBase>
struct IIdent
{
	enum class Id : size_t
	{
		None = 0,
	};

	Id id{ Id::None };
	operator Id() const { return id; } // TODO: remove
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

	static IIdent<TDef>::Id TryGetId(const std::string& name)
	{
		const auto& found(s_definitionsByName.find(name));
		if (found != s_definitionsByName.end())
			return found->second;
		return IIdent<TDef>::Id::None;
	}

	static IIdent<TDef>::Id Add(TDef def)
	{
		const auto nextId(static_cast<IIdent<TDef>::Id>(s_definitions.size()));
		def.id = nextId;
		s_definitions.push_back(def);
		s_definitionsByName[def.name] = def.id;
		return def.id;
	}

	static std::ostream& Definitions(std::ostream& os)
	{
		for (size_t i = 1; i < s_definitions.size(); ++i)
		{
			os << s_definitions[i] << "\n";
		}
		return os;
	}

	static size_t Count()
	{
		return s_definitions.size() - 1;
	}

private:
	// store as pointers so the storage doesn't move?
	// then return pointers directly? (there's no way to remove these so)
	static std::vector<TDef> s_definitions;
	static std::unordered_map<std::string, typename IIdent<TDef>::Id> s_definitionsByName;
};

template <typename TDef>
inline std::vector<TDef> IDef<TDef>::s_definitions(1); // fill 0 with empty

template <typename TDef>
inline std::unordered_map<std::string, typename IIdent<TDef>::Id> IDef<TDef>::s_definitionsByName;


struct ResourceDef : public IDef<ResourceDef>
{
	std::string name;
	std::string description;
};
extern std::ostream& operator <<(std::ostream&, const ResourceDef&);
extern void to_json(nlohmann::json&, const ResourceDef&);
extern void from_json(const nlohmann::json&, ResourceDef&);

struct Resource
{
	ResourceDef::Id definition;
	int quantity;
};
extern std::ostream& operator <<(std::ostream&, const Resource&);
extern void to_json(nlohmann::json&, const Resource&);
extern void from_json(const nlohmann::json&, Resource&);

struct Discipline : public IDef<Discipline>
{
	std::string name;
};
extern std::ostream& operator <<(std::ostream&, const Discipline&);
extern void to_json(nlohmann::json&, const Discipline&);
extern void from_json(const nlohmann::json&, Discipline&);

struct Citizen : public IIdent<Citizen>
{
	int happiness;
	int hunger;

	int proficiency; // increases the longer a citizen performs their job
					  // changing job disciplines resets this

	Discipline::Id lastDiscipline; // should these be pointers?

	// TODO: goal planning state machine
};

enum class ProductionScaleMethod
{
	Linear,
	Constant,
	ScaleWithSize, // scale with building size, not employees
};

struct ProductionScale
{
	ProductionScaleMethod method;
	float coefficient = 1;
};
extern std::ostream& operator <<(std::ostream&, const ProductionScale&);
extern void to_json(nlohmann::json&, const ProductionScale&);
extern void from_json(const nlohmann::json&, ProductionScale&);

struct ProductionDef : public IDef<ProductionDef>
{
	std::string name;
	Discipline discipline;
	//todo: rethink these
	ProductionScale resourceScalar;
	ProductionScale timeScalar;

	Duration duration;
	int progressPerEmployeePerTick = 1; // todo: rethink this

	std::vector<Resource> inputs;
	std::vector<Resource> outputs;
};
extern std::ostream& operator <<(std::ostream&, const ProductionDef&);
extern void to_json(nlohmann::json&, const ProductionDef&);
extern void from_json(const nlohmann::json&, ProductionDef&);

enum class ProductionState
{
	NotStarted,
	WaitingForResources,
	WaitingForEmployees,
	Producing,
};

struct Production
{
	ProductionDef::Id definition;
	size_t generation;
	ProductionState state;
	Duration timeRemaining;
	std::vector<Resource> waitingInputs;
};

struct ZoningRestriction : public IDef<ZoningRestriction>
{
	std::string name;
};
extern std::ostream& operator <<(std::ostream&, const ZoningRestriction&);
extern void to_json(nlohmann::json&, const ZoningRestriction&);
extern void from_json(const nlohmann::json&, ZoningRestriction&);

// merge with production?
struct BuildingDef : public IDef<BuildingDef>
{
	std::string name;
	ZoningRestriction::Id zone;

	ProductionDef::Id production;
};
extern std::ostream& operator <<(std::ostream&, const BuildingDef&);
extern void to_json(nlohmann::json&, const BuildingDef&);
extern void from_json(const nlohmann::json&, BuildingDef&);

enum class BuildingState
{
	Constructing,
	Demolishing,
	InUse, // better name?
};

struct Building : public IIdent<Building>
{
	BuildingDef::Id definition;

	BuildingState state;
	int size;

	Production production;
	std::vector<Citizen::Id> citizensEmployed;
};
