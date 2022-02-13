#pragma once
#include <wrl.h>
using namespace Microsoft::WRL;

namespace Initializers {
	class Com final {
		HRESULT m_hr;
	public:
		Com();

		operator HRESULT() const;

		~Com();
	};
}