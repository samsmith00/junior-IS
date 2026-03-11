#include <tremolo_plugin/tremolo_plugin.h>
#include <gtest/gtest.h>

namespace tremolo {
/** This test doesn't test anything but shows that you can instantiate and use a
 * plugin processor in test */
TEST(PluginProcessor, PlaceholderTest) {
  PluginProcessor processor{};
}
}  // namespace tremolo
