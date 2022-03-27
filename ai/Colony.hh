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

	virtual void Update(const TimeStep& time) override;
};