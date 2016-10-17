#include "applicationState.h"

#include "applicationManager.h"

ApplicationState::ApplicationState()
{
	appManager_ = 0;
}

ApplicationState::~ApplicationState()
{

}

bool ApplicationState::changeState(ApplicationState* appState)
{
	if(appManager_)
	{
		return appManager_->changeState(appState);
	}

	return false;
}


