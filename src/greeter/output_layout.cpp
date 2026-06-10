#include "greeter/output_layout.h"

// wlr-randr requires zwlr_output_management_v1, which Cage 0.3.0 does not
// implement. Output isolation is handled via Wayland viewport in
// syncOutputViewport() (greeter.cpp). This function is a documented no-op
// until the compositor exposes the required protocol.

namespace greeter {

bool disableNonPreferredOutputs(std::string_view /*preferredOutput*/) {
  return false;
}

} // namespace greeter
