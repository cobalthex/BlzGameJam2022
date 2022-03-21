#pragma once

#include "Systems.hh"
#include <unordered_map>

// manages buildings and productions
class Colony : ISystem
{
public:
	static Colony Default; // todo: not this

	Colony() = default;
	virtual ~Colony() = default;
	Colony(const Colony&) = delete;
	Colony operator=(const Colony&) = delete;

	Building::Id Provision(BuildingDef::Id buildingDefId);

	void Remove(Building::Id buildingId);

	virtual void Update(const TimeStep& time) override;

	void PrintBuildings() const;

private:
	Building::Id Add(Building building);

private:
	std::unordered_map<Building::Id, Building> m_allBuildings;
	Building::Id m_nextBuildingId{ 0 };
};