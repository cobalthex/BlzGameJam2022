#pragma once

#include "Systems.hh"
#include "Z.hh"
#include <unordered_map>
#include <queue>

// Manages citizens
class HiveMind : public ISystem
{
	struct EmployeePriorityComparer
	{
		Z& z;

		EmployeePriorityComparer(Z& z) : z(z) { }
		bool operator()(Citizen::Id a, Citizen::Id b);
	};

public:
	HiveMind(Z& z);
	virtual ~HiveMind();
	HiveMind(const HiveMind&) = delete;
	HiveMind operator=(const HiveMind&) = delete;

	virtual void Update(const TimeStep& time) override;

private:
	void Employ(Citizen::Id citizen, Building::Id definition);

	void OnAddCitizen(Citizen& citizen);
	void OnAddBuilding(Building& definition);

	void OnRemoveCitizen(Citizen& citizen);
	void OnRemoveBuilding(Building& definition);

private:
	Z& m_z; // unsafe but ¯\_(ツ)_/¯

	EmployeePriorityComparer m_comparer;
	std::unordered_map<Discipline::Id, std::priority_queue<Citizen::Id, std::vector<Citizen::Id>, EmployeePriorityComparer>> m_joblessCitizens;

	// hacky workaround for circular references
	std::unordered_map<Citizen::Id, Building::Id> m_citizensToBuildings;
};

// some design shit

/* Hive Mind
 * The hive mind is a collection of systems that is responsible for running society
 * - Managing supply/demand by building new buildings
 * - Assigning and reassigning jobs to citizens
 */

 /* Reassignment
  * Citizens can be reassigned based on the colony needs
  * The hive mind will look for any jobs with higher production than demand (or any unassigned workers)
  * preferring citizens with jobs that have matching disciplines
  */

  /*
  updating
	  every citizen has a goal, that is calculated based on their state
	  the citizen performs the actions required to complete the goal
	  once the goal is completed, the citizen determines which goal it should do next
	  If there are no specific goals (eat/play/etc) then the citizen performs there job
	  (restrict hunger/fun to specific cycles?)
  */

  // calculating supply and demand:
  // all resource production and consumption is tracked through the hive mind
  // drained resources and excess resources take priority
  // hive mind will look at available citizens and available land 
  // to try and provision new buildings

// all jobs must have at least one employee (if available)
// jobless citizens will be given jobs if a building is created
// new citizens will go to any available building