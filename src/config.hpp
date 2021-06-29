#include <chrono>
#include <cstdint>
#include <vector>

///
/// Dimensions of the screenshots that will be displayed.
///
/// Specifying width will stack desktops vertically.
/// Specifying height will stack desktops horizontally.
///
/// One dimension *must* be set to zero and will be calculated at runtime.
///
/// Actual window width will be equal to dexpo_width + (dexpo_padding * 2)
///
const int dexpo_width = 200;
const int dexpo_height = 0;

const uint16_t dexpo_padding = 5; ///< Padding around the screenshots

const uint16_t dexpo_hlwidth = 4; ///< Width of the preselected desktop border
const uint32_t dexpo_bgcolor = 0xAA525252; ///< Background color. TODO Alpha and document properly
const uint32_t dexpo_preselected_desktop_color = 0Xc20a66;///< Color of the preselection border
const uint32_t dexpo_desktop_color =   0X0066cc;///<Color of the unselected desktops borders

const int16_t dexpo_x = 0; ///< X coordinate of window's top left corner
const int16_t dexpo_y = 0; ///< Y coordinate of window's top left corner

const unsigned int dexpo_outer_border = 0; ///< TODO Window border

///
/// How often to take screenshots.
///
/// Setting timeout to values below 0.1 second may severely increase cpu load.
///
const auto dexpo_screenshot_timeout = std::chrono::seconds (1);

///
/// Desktop viewport:
/// Top left coordinates of each of your desktops in the format
/// { x1, y1, x2, y2, x3, y3 } and so on.
///
/// Should be set only if you have a DE and
/// `xprop -root _NET_DESKOP_VIEWPORT`
/// returns `0, 0`
///
/// Otherwise leave empty.
///
const std::vector<uint32_t> dexpo_viewport = {0,0,0,0,0,0,0,0};
