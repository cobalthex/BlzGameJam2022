#pragma once

#include "Systems.hh"
#include <unordered_map>

// Manages citizens
class HiveMind : public ISystem
{
public:
	static HiveMind Default; // todo: not this

	HiveMind() = default;
	virtual ~HiveMind() = default;
	HiveMind(const HiveMind&) = delete;
	HiveMind operator=(const HiveMind&) = delete;

	virtual void Update(const TimeStep& time) override;

	Citizen::Id CreateCitizen();

	void AssignToBuilding(std::chrono::milliseconds now, Citizen::Id citizen, BuildingDef ::Id building); // make private

private:

private:
	std::unordered_map<Citizen::Id, Citizen> m_allCitizens;
	Citizen::Id m_nextCitizenId{};
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