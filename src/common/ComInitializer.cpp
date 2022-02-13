#include "ComInitializer.h"

namespace Initializers
{
	Com::Com() 
		: m_hr(S_FALSE) {
		m_hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	}

	Com::operator HRESULT() const {
		return m_hr;
	}

	Com::~Com() {
		if (SUCCEEDED(m_hr))
		{
			CoUninitialize();
		}
	}

}