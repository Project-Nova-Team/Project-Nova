#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DelayedAction.h"
#include "DelayedActionManager.generated.h"

UCLASS()
class UNREALPLAYGROUND_API UDelayedActionManager : public UObject
{
	GENERATED_BODY()

public:
	UDelayedActionManager() { }
	~UDelayedActionManager() { }

	/** Initializes the action manager */
	void Initialize();

	/**
	 *  Calls the action function for every DelayedAction in RunningActions
	 *  @param	DeltaTime					Time between last frame
	 */
	void TickRunningActions(const float DeltaTime);

	/**
	 *	Ends execution of the handle early and cleans up the delayed action for immediate use
	 *
	 * @param	ActionHandle				Handle to the action that is to be cancelled
	 */
	void StopDelayedAction(FDelayedActionHandle* ActionHandle);

	/**
	 *	Ends execution of the handle early and cleans up the delayed action for immediate use
	 *
	 * @param	ActionHandle				Handle to the action that is to be cancelled
	 */
	void StopDelayedAction(FDelayedAction* Action);

	/** 
	 *  Creates a DelayedAction that handles running a function every frame over a period of time
	 *  @param	InObj						The reference object that is calling the ActionFunction
	 *  @param	ActionFunction				The function that will be called every tick for this action
	 *	@param	CompletionTime				The amount of time this function will run every tick for
	 */	
	template<class T>					FDelayedActionHandle* StartOverTimeAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate					<T>::FMethodPtr ActionFunction,			const float CompletionTime)				 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction), CompletionTime, DAT_OverTime); }
	template<class T>					FDelayedActionHandle* StartOverTimeAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_Const				<T>::FMethodPtr ActionFunction,			const float CompletionTime)				 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction), CompletionTime, DAT_OverTime); }
	/**
	 *  Creates a DelayedAction that handles running a function every frame over a period of time
	 *  @param	InObj						The reference object that is calling the ActionFunction
	 *  @param	ActionFunction				The function that will be called every tick for this action
	 *	@param	CompletionTime				The amount of time this function will run every tick for
	 *	@param	Args						Additional parameters you want to pass to ActionFunction
	 */
	template<class T, typename ...A>	FDelayedActionHandle* StartOverTimeAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_OneVar				<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_OverTime); }
	template<class T, typename ...A>	FDelayedActionHandle* StartOverTimeAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_OneVar_Const		<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_OverTime); }
	template<class T, typename ...A>	FDelayedActionHandle* StartOverTimeAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_TwoVars			<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_OverTime); }
	template<class T, typename ...A>	FDelayedActionHandle* StartOverTimeAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_TwoVars_Const		<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_OverTime); }
	template<class T, typename ...A>	FDelayedActionHandle* StartOverTimeAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_ThreeVars			<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_OverTime); }
	template<class T, typename ...A>	FDelayedActionHandle* StartOverTimeAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_ThreeVars_Const	<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_OverTime); }
	template<class T, typename ...A>	FDelayedActionHandle* StartOverTimeAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_FourVars			<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_OverTime); }
	template<class T, typename ...A>	FDelayedActionHandle* StartOverTimeAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_FourVars_Const		<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_OverTime); }
	
	/**
	 *  Creates a DelayedAction that handles calling a function after a defined amount of time
	 *  @param	InObj						The reference object that is calling the ActionFunction
	 *  @param	ActionFunction				The function that will be called every tick for this action
	 *	@param	CompletionTime				The amount of time that will pass before this function is executed
	 */
	template<class T>					FDelayedActionHandle* StartDelayedAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate						<T>::FMethodPtr ActionFunction,			const float CompletionTime)				 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction), CompletionTime, DAT_Delay); }
	template<class T>					FDelayedActionHandle* StartDelayedAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_Const				<T>::FMethodPtr ActionFunction,			const float CompletionTime)				 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction), CompletionTime, DAT_Delay); }
	/**
	 *  Creates a DelayedAction that handles calling a function after a defined amount of time
	 *  @param	InObj						The reference object that is calling the ActionFunction
	 *  @param	ActionFunction				The function that will be called every tick for this action
	 *	@param	CompletionTime				The amount of time that will pass before this function is executed
	 *	@param	Args						(Optional) Additional parameters you want to pass to ActionFunction
	 */
	template<class T, typename ...A>	FDelayedActionHandle* StartDelayedAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_OneVar				<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_Delay); }
	template<class T, typename ...A>	FDelayedActionHandle* StartDelayedAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_OneVar_Const		<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_Delay); }
	template<class T, typename ...A>	FDelayedActionHandle* StartDelayedAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_TwoVars				<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_Delay); }
	template<class T, typename ...A>	FDelayedActionHandle* StartDelayedAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_TwoVars_Const		<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_Delay); }
	template<class T, typename ...A>	FDelayedActionHandle* StartDelayedAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_ThreeVars			<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_Delay); }
	template<class T, typename ...A>	FDelayedActionHandle* StartDelayedAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_ThreeVars_Const		<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_Delay); }
	template<class T, typename ...A>	FDelayedActionHandle* StartDelayedAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_FourVars			<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_Delay); }
	template<class T, typename ...A>	FDelayedActionHandle* StartDelayedAction(T* InObj, typename FActionDelegate::TUObjectMethodDelegate_FourVars_Const		<T, A...>::FMethodPtr ActionFunction,	const float CompletionTime, A... Args)	 { return Actions[GetAvailableAction()]->StartAction(FActionDelegate::CreateUObject(InObj, ActionFunction, Args...), CompletionTime, DAT_Delay); }

private:
	TArray<TUniquePtr<FDelayedAction>> Actions;

	/** 
	 * Gets the index of a delayed action that is not in use
	 * If there are no available delayed actions a new one will be created and that index will be returned
	 */
	int32 GetAvailableAction();

	/** Creates a new instance of DelayedAction and adds it to the available action collection*/
	void AddNewDelayedActionInstance();
		
};
