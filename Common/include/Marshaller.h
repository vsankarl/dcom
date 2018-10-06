#pragma once

#include <windows.h>
#include <sstream>
#include <vector>
//Format

// Current:
// ReturnValue ? ObjectId ? Dispid ? Param1 ? Param2 ? Param3
//  Note: Space included for reading clarity

// TODO: Alternatively include parameter state information
//       that gets prepended to paramter. This could take the form 

//       | 4 Bytes | parameter1 | 4 bytes | paramter2 

//        '|' character included for clarity
//       Out of the 4 bytes, lower order 4 bits could capture
//       type of the parameter such as int, string, etc and
//       whether the paramter is IN, OUT or INOUT, the rest 
//       would capture the length of the parameter.

//        Once we know the paramter as IN, INOUT, OUT we can selectively
//        choose to set or fetch a value.
//        Additionally knowing type information would help to decide 
//        how to treat a value. 
//        Example for pointer types, on the client side the value is 
//		  read and passed, as opposed to passign the pointer directly. 
//        Likewise on the server side instead of reading a value a 
//        heap based allocation is made and contents are initialized 
//        with the client passed value.


//  [POSITION == 1] 
//	ReturnValue == Error information or success
// 
//  [POSITION == 2]
//		ObjectId == 0 Represent global functions
//				 == 0x000858AB ==> Some object Id

//  [POSITION == 3]
//	DispId  If ObjectId == 0 then DispId == 0 ====> CreateInstance
//									  	 == 1 ====> SendData as Int
//										 == 2 ====> SendData as string
//										 == 3 ==== > DeleteInstance
//										 
//		    if ObjectId != 0 then DispId == 0 ....N represent functions
//			 in the object

//  [POSITION == 4....N]
//  Param1, Param2, Param3 - Varied number of parameters 

// Example:
// CreateInstance Calculator object. 
// Server in - 0x0000?0x0000?0?Calculator?0
// Server out - 0x0000?0x0000?0?Calculator?0x00090749

// Calling Calculator::Add(2, 3, result) object
// Server in - 0x0000?0x000858AB?0?2?3?0
// Server out - 0x0000?0x000858AB?0?2?3?6

// DeleteInstance Calculator object. 
// Server in - 0x0000?0x0000?3?0x00090749
// Server out - 0x0000?0x0000?3?0x00090749

namespace RPC
{
	class Marshaller
	{
	public:
		Marshaller(const std::string & s);
		Marshaller();
		Marshaller(long, HANDLE, int);

		void SetWireFormat(const std::string & s);
		int GetDispId();
		void SetDispId(int);
		void * GetObjectId();
		void SetObjectId(HANDLE);
		long GetErrorCode();
		void SetErrorCode(long);

		// 1 based index, so index 0 errors out. 
		// 1st parameter takes index == 1, internally
		// it translates to 4th position in the vector.
		// This is done to abstract the notion of how its
		// stored internally and give the caller store 
		// as 1st parameter, 2nd parameter and so on.
		template<typename T>
		BOOL 
		SetFunctionParameter(long index, T t)
		{
			if (0 == index)
				return FALSE;
			// Function parameter position starts 
			// after dispid 
			index = eDispIdPosition + index;
			SetParameter<T>(index, t);
			return TRUE;
		}

		template<typename T>
		T GetFunctionParameter(long index, T t)
		{
			if (0 == index)
				return t;
			// Function parameter position starts 
			// after dispid 
			index = eDispIdPosition + index;
			return GetParameter<T>(index, t);
		}

		long GetLastFunctionParameterIndex();

		size_t GetTotalNumberOfParameters();
		std::string GetWireFormat();


	private:
		enum ePositionT
		{
			eResultPosition = 1,
			eObjectIdPosition,
			eDispIdPosition
		};
		
		// 1 based index
		template<typename T>
		T GetParameter(long index, T t)
		{
			if (index > m_totalNumberOfParameters)
				return t;

			t = ArgumentToType<T>(t, m_allParameters[index - 1]);

			return t;
		}

		// 1 based index
		template<typename T>
		void SetParameter(long index, T t)
		{
			if (index > m_totalNumberOfParameters)
			{
				// resize is 1 based while actual setting is 0 based
				m_allParameters.resize(index);
				m_totalNumberOfParameters = m_allParameters.size();
			}

			m_allParameters[index - 1] = TypeToArgument<T>(t);
		}

		// T is passed as a paramter so that we can deal with 
		// references properly. 
		template<typename T>
		T ArgumentToType(T t, std::string & s)
		{
			std::stringstream stream(s);
			stream >> t;
			return t;
		}

		// since stringstream does a space based extraction
		// jsut return the original string
		template<>
		std::string 
		ArgumentToType<std::string>(std::string, std::string & s)
		{
			return s;
		}

		template<>
		std::string & 
		ArgumentToType<std::string &>(std::string & t, std::string & s)
		{
			t = s;
			return t;
		}

		template<typename T>
		std::string TypeToArgument(T t)
		{
			std::stringstream stream;
			stream << t;
			return stream.str();
		}

		void
		InitHelper(const std::string &);

		std::vector<std::string> m_allParameters;
		size_t m_totalNumberOfParameters;
	};
}