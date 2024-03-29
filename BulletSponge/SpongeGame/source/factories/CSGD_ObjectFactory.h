/////////////////////////////////////////////////
//	File	:	"CSGD_ObjectFactory.h"
//
//	Author	:	David Brown (DB)
//
//	Purpose	:	A templated class that contains function pointers
//				for the creation of objects.
/////////////////////////////////////////////////

#pragma once

#include <map>
using std::map;

template <typename ClassIDType, typename BaseClassType>
class CSGD_ObjectFactory
{
private:
	//			Function pointer declaration.
	typedef BaseClassType* (*ObjectCreator)(void);

	//			Database of function Pointer to create my objects
	map<ClassIDType, ObjectCreator> m_ObjectCreators;

	//		Singleton Instance.
	static CSGD_ObjectFactory<ClassIDType, BaseClassType> sm_Instance;

	//		Trilogy of EVIL!
	//	Constructor.
	CSGD_ObjectFactory(void) {}
	//	Copy constructor.
	CSGD_ObjectFactory(const CSGD_ObjectFactory&);
	//	Assignment Operator
	CSGD_ObjectFactory& operator=(const CSGD_ObjectFactory&);

	//	And destructor!
	~CSGD_ObjectFactory(void) {}

	//	Private function to Create an object.
	template<typename DerivedClassType>
	static BaseClassType* ConstructObject(void)
	{
		return new DerivedClassType;
	}

public:

	//	GetInstance
	static CSGD_ObjectFactory<ClassIDType, BaseClassType>* GetInstance(void)
	{
		return &sm_Instance;
	}

	//	Build the map data of function pointers
	template<typename DerivedClassType>
	bool RegisterClassType(ClassIDType id)
	{
		//	Create the database definition
		std::pair<ClassIDType, ObjectCreator> objTypeDef;

		objTypeDef.first	=	id;
		objTypeDef.second	=	ConstructObject<DerivedClassType>;

		//	Add this information into my database.
		m_ObjectCreators.insert(objTypeDef);

		//	Success.
		return true;
	}

	//	Remove information from our database
	bool UnregisterClassType(ClassIDType id)
	{
		//	Find the given key inside of the database (map).
		map<ClassIDType, ObjectCreator>::iterator iter = m_ObjectCreators.find(id);

		//	Error check to make sure we found it.
		if (iter == m_ObjectCreators.end())
			return false;

		//	Remove the database information.
		m_ObjectCreators.erase(iter);

		//	Success.
		return true;
	}

	//	Creates an object of the given id.
	BaseClassType* CreateObject(ClassIDType id)
	{
		map<ClassIDType, ObjectCreator>::iterator iter = m_ObjectCreators.find(id);

		//	Error check to make sure we found it
		if (iter == m_ObjectCreators.end())
			return NULL;

		//	Create the class type and return it.
		return (*iter).second();
	}

	void ShutdownObjectFactory(void)
	{
		//	Clear the database.
		m_ObjectCreators.clear();
	}
}; // end CSGD_ObjectFactory

//	Declare static member instance at global scope
template<typename ClassIDType, typename BaseClassType>
CSGD_ObjectFactory<ClassIDType, BaseClassType> 
CSGD_ObjectFactory<ClassIDType, BaseClassType>::sm_Instance;


