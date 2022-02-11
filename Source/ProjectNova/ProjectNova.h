#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FDetailsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool IsGameModule() const override { return true; }

};