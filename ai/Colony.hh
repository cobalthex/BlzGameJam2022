#pragma once

#include "Systems.hh"
#include "Z.hh"
#include <unordered_map>

// manages buildings and productions
class Colony : ISystem
{
public:
	Colony(Z& z);
	virtual ~Colony();
	Colony(const Colony&) = delete;
	Colony operator=(const Colony&) = delete;

	Building::Id Provision(BuildingDef::Id buildingDefId);

	virtual void Update(const TimeStep& time) override;

private:
	void OnAddBuilding(Building& building);
	void OnRemoveBuilding(Building& building);

	static int CalculateScale(const ProductionScale& scalar, size_t citizensEmployed, long long value);

	BuildingDef::Id TryGetBuildingForResource(ResourceDef::Id resource) const;
	void UpdateProductions(const TimeStep& time);

private:
	Z& m_z; // unsafe but ¯\_(ツ)_/¯

	float m_provisionThreshold = 0.5f; // todo: this should be dynamic

	Duration m_nextProvisionTime;

	const Duration c_provisionDelay{ std::chrono::duration_cast<Duration>(SecondsF(3)) };
};