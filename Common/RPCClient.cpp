#include <string>
#include "RPCClient.h"

using namespace std;

namespace RPC
{
	RPCClient::RPCClient()
			  :m_criticalSectionIntialized(FALSE)
	{
		m_criticalSectionIntialized = InitializeCriticalSectionAndSpinCount(&m_criticalSection, CRITICAL_SECTION_SPINCOUNT);
	}

	RPCClient::~RPCClient()
	{
		size_t total = m_asyncMessageHandlers.size();

		for (int index = 0; index < total; index++)
		{
			if (NULL != m_asyncMessageHandlers[index])
				m_asyncMessageHandlers[index]->Stop();
		}

		if (m_criticalSectionIntialized)
			DeleteCriticalSection(&m_criticalSection);
	}
}