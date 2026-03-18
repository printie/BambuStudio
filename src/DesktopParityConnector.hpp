#ifndef SLIC3R_DESKTOP_PARITY_CONNECTOR_HPP
#define SLIC3R_DESKTOP_PARITY_CONNECTOR_HPP

#include <map>
#include <string>
#include <vector>

#include "libslic3r/Config.hpp"
#include "libslic3r/PrintConfig.hpp"

namespace Slic3r::DesktopParityConnector {

bool is_enabled(const DynamicPrintAndCLIConfig &config);

bool should_retry_project_as_geometry_only(bool enabled, const std::string &file);
void set_project_load_fallback_markers(std::map<std::string, std::string> &markers, const std::string &file, const std::string &reason);

bool allow_variant_metadata_fallback(bool enabled, const std::string &scope, std::map<std::string, std::string> &markers);

NozzleVolumeType infer_nozzle_volume_type_from_variant(const std::string &variant, NozzleVolumeType fallback = nvtStandard);
bool recover_nozzle_volume_types_from_variants(int extruder_count, const std::vector<std::string> &variant_candidates, std::vector<NozzleVolumeType> &nozzle_volume_types, std::map<std::string, std::string> &markers);

bool should_continue_plate_failures(bool enabled, int plate_to_slice);

std::string join_int_values(const std::vector<int> &values, const std::string &delimiter = ",");
std::string join_string_values(const std::vector<std::string> &values, const std::string &delimiter);

} // namespace Slic3r::DesktopParityConnector

#endif // SLIC3R_DESKTOP_PARITY_CONNECTOR_HPP
