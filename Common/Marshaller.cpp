#include "Marshaller.h"
#include "Utils.h"
#include "Constants.h"
#include "RPC.h"
using namespace std;

#define WIRE_DELIMITER					"?"
namespace RPC
{
	Marshaller::Marshaller(const string & s)
			   :m_totalNumberOfParameters(0)
	{
		InitHelper(s);
	}

	Marshaller::Marshaller()
			   :m_totalNumberOfParameters(0)
	{
	}
	Marshaller::Marshaller(long errorCode, HANDLE objectId, int dispId)
			   : m_totalNumberOfParameters(0)
	{
		SetErrorCode(errorCode);
		SetObjectId((HANDLE)objectId);
		SetDispId(dispId);
	}

	void 
	Marshaller::SetWireFormat(const std::string & s)
	{
		InitHelper(s);
	}

	void
	Marshaller::InitHelper(const std::string & s)
	{
		string token;
		size_t start = 0;
		while (StringUtils::Tokenize(s, token, WIRE_DELIMITER, start))
		{
			m_allParameters.push_back(token);
		}
		m_totalNumberOfParameters = m_allParameters.size();
	}

	int 
	Marshaller::GetDispId()
	{
		int dispId = eInvalidDispId;
		return GetParameter<int>(eDispIdPosition, dispId);
	}

	void 
	Marshaller::SetDispId(int dispId)
	{
		SetParameter<int>(eDispIdPosition, dispId);
	}

	HANDLE 
	Marshaller::GetObjectId()
	{
		HANDLE ptr = INVALID_HANDLE_VALUE;
		return GetParameter<HANDLE>(eObjectIdPosition, ptr);
	}

	void 
	Marshaller::SetObjectId(HANDLE ptr)
	{
		SetParameter<HANDLE>(eObjectIdPosition, ptr);
	}

	long 
	Marshaller::GetErrorCode()
	{
		long errorCode = ERROR_INVALID_VALUE;
		return GetParameter<long>(eResultPosition, errorCode);
	}

	void 
	Marshaller::SetErrorCode(long errorCode)
	{
		SetParameter<long>(eResultPosition, errorCode);
	}

	size_t 
	Marshaller::GetTotalNumberOfParameters()
	{
		return m_totalNumberOfParameters;
	}

	std::string 
	Marshaller::GetWireFormat()
	{
		std::string s;
		
		for (size_t index = 0; index < m_totalNumberOfParameters; index++)
		{
			s += m_allParameters[index];
			
			// skip last parameter
			if (index != (m_totalNumberOfParameters - 1))
			{
				s += WIRE_DELIMITER;
			}
		}
		return s;
	}

	// Index == 0 means no paramter exist.
	// Index starts at 1.
	long 
	Marshaller::GetLastFunctionParameterIndex()
	{
		return (long)(m_totalNumberOfParameters - eDispIdPosition);
	}
}