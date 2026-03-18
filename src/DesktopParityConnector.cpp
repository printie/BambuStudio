#include "DesktopParityConnector.hpp"

#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace Slic3r::DesktopParityConnector {

bool is_enabled(const DynamicPrintAndCLIConfig &config)
{
    if (config.opt_bool("legacy_cli"))
        return false;
    return config.opt_bool("desktop_parity");
}

bool should_retry_project_as_geometry_only(bool enabled, const std::string &file)
{
    return enabled && boost::algorithm::iends_with(file, ".3mf");
}

void set_project_load_fallback_markers(std::map<std::string, std::string> &markers, const std::string &file, const std::string &reason)
{
    markers["project_load_fallback"]        = "geometry_only";
    markers["project_load_fallback_file"]   = file;
    markers["project_load_fallback_reason"] = reason;
}

bool allow_variant_metadata_fallback(bool enabled, const std::string &scope, std::map<std::string, std::string> &markers)
{
    if (!enabled)
        return false;

    const std::string key_prefix = scope.empty() ? "variant" : scope + "_variant";
    markers[key_prefix + "_fallback"]        = "true";
    markers[key_prefix + "_fallback_reason"] = "missing_variant_id_pair";
    return true;
}

NozzleVolumeType infer_nozzle_volume_type_from_variant(const std::string &variant, NozzleVolumeType fallback)
{
    if (variant.empty())
        return fallback;

    if (boost::algorithm::icontains(variant, "high flow") || boost::algorithm::icontains(variant, "hf"))
        return nvtHighFlow;
    if (boost::algorithm::icontains(variant, "hybrid"))
        return nvtHybrid;
    if (boost::algorithm::icontains(variant, "standard"))
        return nvtStandard;

    return fallback;
}

bool recover_nozzle_volume_types_from_variants(
    int                               extruder_count,
    const std::vector<std::string>   &variant_candidates,
    std::vector<NozzleVolumeType>    &nozzle_volume_types,
    std::map<std::string, std::string> &markers)
{
    std::vector<std::string> normalized_variants = variant_candidates;
    if (normalized_variants.size() < static_cast<size_t>(extruder_count))
        normalized_variants.resize(extruder_count, "");

    nozzle_volume_types.resize(extruder_count, nvtStandard);
    bool inferred_any = false;
    for (int index = 0; index < extruder_count; ++index) {
        const NozzleVolumeType inferred = infer_nozzle_volume_type_from_variant(normalized_variants[index], nvtStandard);
        nozzle_volume_types[index]      = inferred;
        if (inferred != nvtStandard)
            inferred_any = true;
    }

    markers["nozzle_volume_recovery"] = inferred_any ? "derived_from_variant_then_fallback_standard" : "fallback_standard";
    return inferred_any;
}

bool should_continue_plate_failures(bool enabled, int plate_to_slice)
{
    return enabled && plate_to_slice == 0;
}

std::string join_int_values(const std::vector<int> &values, const std::string &delimiter)
{
    std::ostringstream oss;
    for (size_t index = 0; index < values.size(); ++index) {
        if (index > 0)
            oss << delimiter;
        oss << values[index];
    }
    return oss.str();
}

std::string join_string_values(const std::vector<std::string> &values, const std::string &delimiter)
{
    std::ostringstream oss;
    for (size_t index = 0; index < values.size(); ++index) {
        if (index > 0)
            oss << delimiter;
        oss << values[index];
    }
    return oss.str();
}

} // namespace Slic3r::DesktopParityConnector
