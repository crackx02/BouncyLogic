#pragma once

#include <vector>
#include <cstdint>

#include "Vec3.hpp"

namespace SM {
	class Visualization {
		public:
			static constexpr uintptr_t Offset = 0x12675a0;
			struct ConnectionLine {
				Vec3 start;
				Vec3 end;
				uint32_t color;
			};

			inline std::vector<ConnectionLine>& getConnectionLines() {return m_vecConnectionLines;};

		private:
			char _pad0[0x640];
			std::vector<ConnectionLine> m_vecConnectionLines;
			char _pad1[0x128];
	};
}
