////////////////////////////////////////////////
//	File	:	"CSGD_ObjectManager.cpp"
//
//	Author	:	David Brown (DB)
//
//	Purpose	:	To contain and manage all of our game objects.
/////////////////////////////////////////////////
#include "Macros.h"
#include "CSGD_ObjectManager.h"
#include "messaging\CSGD_EventSystem.h"
#include "models/CBase.h"
#include <vector>
using std::vector;

CSGD_ObjectManager* CSGD_ObjectManager::sm_pInstance = NULL;

CSGD_ObjectManager::CSGD_ObjectManager(void)
{
}

CSGD_ObjectManager::~CSGD_ObjectManager(void)
{
}

void CSGD_ObjectManager::UpdateObjects(float fElapsedTime)
{
	vector<IBaseInterface*>::iterator iter = m_vObjectList.begin();

	// Dead objects
	vector<IBaseInterface*> deadObjects;

	while(iter != m_vObjectList.end())
	{
		if(((CBase*)(*iter))->GetIsActive())		
			(*iter)->Update(fElapsedTime);			
		else
			deadObjects.push_back(*iter);

		iter++;
	}

	for(unsigned int i = 0; i < deadObjects.size(); ++i)
		RemoveObject(deadObjects[i]);
}

void CSGD_ObjectManager::CheckCollisions(void)
{
	for(unsigned int i = 0; i < m_vObjectList.size(); ++i)
		for(unsigned int j = 0; j < m_vObjectList.size();++j)
		{
			if(m_vObjectList[i]->GetType() != m_vObjectList[j]->GetType())
			{
				if(m_vObjectList[i]->CheckCollision(m_vObjectList[j]))					
					break;
			}
		}
}

void CSGD_ObjectManager::RenderObjects(void)
{
	for (unsigned int i=0; i < m_vObjectList.size(); i++)
		m_vObjectList[i]->Render();
}

void CSGD_ObjectManager::AddObject(IBaseInterface* pObject)
{
	//	Check for valid object pointer
	if (pObject == NULL)
		return;

	//	Add object to object list
	m_vObjectList.push_back(pObject);

	//	Add my reference to it.
	pObject->AddRef();
}

void CSGD_ObjectManager::RemoveObject(IBaseInterface* pObject)
{
	//	Check for valid object pointer
	if (pObject == NULL)
		return;

	for (vector<IBaseInterface*>::iterator iter = m_vObjectList.begin();
		 iter != m_vObjectList.end();
		 iter++)
	{
		// if this is the object we are looking for.
		if ((*iter) == pObject)
		{
			// Remove my reference to this object.
			(*iter)->Release();

			// Remove the object from the list
			iter = m_vObjectList.erase(iter);
			break;
		}
	}
}

void CSGD_ObjectManager::RemoveAllObjects(void)
{
	//	Call Release() on all objects.
	for (unsigned int i=0; i < m_vObjectList.size(); i++)
	{
		m_vObjectList[i]->Release();
	}

	//	Collapse the vector
	m_vObjectList.clear();
}
CSGD_ObjectManager* CSGD_ObjectManager::GetInstance()
{
	if(sm_pInstance == NULL)
		sm_pInstance = new CSGD_ObjectManager();

	return sm_pInstance;
}

void CSGD_ObjectManager::DeleteInstance()
{
	if(sm_pInstance)
	{
		delete sm_pInstance;
		sm_pInstance = NULL;
	}
}
