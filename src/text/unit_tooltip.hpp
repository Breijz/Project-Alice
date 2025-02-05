#pragma once

#include "system_state.hpp"

namespace text {

struct unitamounts {
	uint16_t type1 = 0;
	uint32_t type1_pop = 0;

	uint16_t type2 = 0;
	uint32_t type2_pop = 0;

	uint16_t type3 = 0;
	uint32_t type3_pop = 0;
};

void populate_unit_tooltip(sys::state& state, int16_t x, int16_t y, text::columnar_layout& contents, dcon::province_id prov);

}
