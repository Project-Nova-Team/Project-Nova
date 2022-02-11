// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNova.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDetailsModule, ProjectNova, "ProjectNova" );

#if WITH_EDITOR

#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Slate/PatrolPathDetails.h"

#endif

void FDetailsModule::StartupModule()
{
#if WITH_EDITOR
	FPropertyEditorModule& PropertyModule =
		FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomClassLayout(
		"PatrolPath",
		FOnGetDetailCustomizationInstance::CreateStatic(&FPatrolPathDetails::MakeInstance));
#endif
}

void FDetailsModule::ShutdownModule()
{

}