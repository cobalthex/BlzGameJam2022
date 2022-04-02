#include "Types.hh"
#include "Utils.hh"

using SecondsF = std::chrono::duration<float, std::chrono::seconds::period>;

namespace std::chrono
{
	void from_json(const nlohmann::json& json, Duration& duration)
	{
		if (json.is_number_integer() || json.is_number_float())
			duration = Duration(json.get<Duration::rep>());
		else if (json.is_string())
		{
			std::string read;
			json.get_to(read);
			const auto value(std::stoll(read)); // handle fractions
			if (read.ends_with("ms"))
				duration = std::chrono::duration_cast<Duration>(std::chrono::milliseconds(value));
			else if (read.ends_with("ns"))
				duration = std::chrono::duration_cast<Duration>(std::chrono::nanoseconds(value));
			else if (read.ends_with("m"))
				duration = std::chrono::duration_cast<Duration>(std::chrono::minutes(value));
			else if (read.ends_with("s"))
				duration = std::chrono::duration_cast<Duration>(std::chrono::seconds(value));
			else
				duration = Duration(value);
		}
	}
	void to_json(nlohmann::json& json, const Duration& duration)
	{
		// TODO
	}
}

template <typename T>
std::string _AnyToJson(const T& t)
{
	nlohmann::json json;
	to_json(json, t);
	return json.dump();
}

std::ostream& operator <<(std::ostream& os, const ResourceDef& def) { return (os << _AnyToJson(def)); }
std::ostream& operator <<(std::ostream& os, const Resource& resource) { return (os << _AnyToJson(resource)); }
std::ostream& operator <<(std::ostream& os, const Discipline& discipline) { return (os << _AnyToJson(discipline)); }
std::ostream& operator <<(std::ostream& os, const ProductionScale& scale) { return (os << _AnyToJson(scale)); }
std::ostream& operator <<(std::ostream& os, const ProductionDef& def) { return (os << _AnyToJson(def)); }
std::ostream& operator <<(std::ostream& os, const ZoningRestriction& zone) { return (os << _AnyToJson(zone)); }
std::ostream& operator <<(std::ostream& os, const BuildingDef& def) { return (os << _AnyToJson(def)); }

void to_json(nlohmann::json& json, const ResourceDef& def)
{
	json["id"] = static_cast<size_t>(def.id);
	json["name"] = def.name;
	json["description"] = def.description;
}

void to_json(nlohmann::json& json, const Resource& resource)
{
	const auto& def(ResourceDef::Get(resource.definition));
	json["resource"] = def.name;
	json["quantity"] = resource.quantity;
}

void to_json(nlohmann::json& json, const Discipline& discipline)
{
	json["id"] = static_cast<size_t>(discipline.id);
	json["name"] = discipline.name;
}

void to_json(nlohmann::json& json, const ProductionScale& scale)
{
	json["method"] = static_cast<size_t>(scale.method);
	json["coefficient"] = scale.coefficient;
}

void to_json(nlohmann::json& json, const ProductionDef& def)
{
	json["id"] = static_cast<size_t>(def.id);
	json["name"] = def.name;
	const auto discipline(Discipline::TryGet(def.discipline));
	if (discipline) json["discipline"] = discipline->name;
	json["resourceScalar"] = def.resourceScalar;
	json["timeScalar"] = def.timeScalar;
	json["duration"] = std::to_string(std::chrono::duration_cast<SecondsF>(def.duration).count()) + "s"; // determine best unit?
	json["progressPerEmployeeTick"] = def.progressPerEmployeePerTick;
	json["inputs"] = def.inputs;
	json["outputs"] = def.outputs;
}

void to_json(nlohmann::json& json, const ZoningRestriction& zone)
{
	json["id"] = static_cast<size_t>(zone.id);
	json["name"] = zone.name;
}

void to_json(nlohmann::json& json, const BuildingDef& def)
{
	json["id"] = static_cast<size_t>(def.id);
	json["name"] = def.name;
	json["zone"] = def.name;
	const auto production(ProductionDef::TryGet(def.production));
	if (production) json["production"] = production->name;
}

void from_json(const nlohmann::json& json, ResourceDef& def)
{
	if (json.contains("name")) json["name"].get_to(def.name);
	if (json.contains("description")) json["description"].get_to(def.description);
}

void from_json(const nlohmann::json& json, Resource& resource)
{
	if (json.contains("definition"))
	{
		resource.definition = ResourceDef::TryGetId(json["definition"].get<std::string>());
	}
	if (json.contains("quantity")) json["quantity"].get_to(resource.quantity);
}

void from_json(const nlohmann::json& json, Discipline& discipline)
{
	if (json.contains("name")) json["name"].get_to(discipline.name);
}

void from_json(const nlohmann::json& json, ProductionScale& scale)
{
	if (json.contains("method")) json["method"].get_to(scale.method);
	if (json.contains("coefficient")) json["coefficient"].get_to(scale.coefficient);
}

void from_json(const nlohmann::json& json, ProductionDef& def)
{
	// enumerate entries rather than search?
	if (json.contains("name")) json["name"].get_to(def.name);
	if (json.contains("discipline")) json["discipline"].get_to(def.discipline);
	if (json.contains("resourceScalar")) json["resourceScalar"].get_to(def.resourceScalar);
	if (json.contains("timeScalar")) json["timeScalar"].get_to(def.timeScalar);
	if (json.contains("duration")) json["duration"].get_to(def.duration);
	if (json.contains("progressPerEmployeePerTick")) json["progressPerEmployeePerTick"].get_to(def.progressPerEmployeePerTick);
	if (json.contains("inputs")) json["inputs"].get_to(def.inputs);
	if (json.contains("outputs")) json["outputs"].get_to(def.outputs);
}

void from_json(const nlohmann::json & json, ZoningRestriction & zone)
{
	if (json.contains("name")) json["name"].get_to(zone.name);
}

void from_json(const nlohmann::json& json, BuildingDef& def)
{
	if (json.contains("name")) json["name"].get_to(def.name);
	if (json.contains("zone")) json["zone"].get_to(def.zone);
	if (json.contains("production"))
	{
		def.production = ProductionDef::TryGetId(json["production"].get<std::string>());
	}
}
