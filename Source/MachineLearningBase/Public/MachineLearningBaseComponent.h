// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/LatentActionManager.h"
#include "MachineLearningBaseComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMLConnectionSignature, FString, Endpoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMLResultSignature, FString, ResultData, FString, CallingFunctionName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMLRawResultSignature, TArray<float>, ResultData, FString, CallingFunctionName);

/** 
*	Base component for both remote and standard MachineLearning components.
*	Considered abstract, ensures all MachineLearning components have same base API
*/
UCLASS(ClassGroup = Computing)
class MACHINELEARNINGBASE_API UMachineLearningBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UMachineLearningBaseComponent();

	/** 
	* When the given setup gets a connection to a backend (e.g. server for remote, library for native).
	* This is a good place to do initialization type calls instead of BeginPlay.
	*/
	UPROPERTY(BlueprintAssignable, Category = TensorflowEvents)
	FMLConnectionSignature OnConnectedToBackend;

	UPROPERTY(BlueprintAssignable, Category = TensorflowEvents)
	FMLConnectionSignature OnDisconnectedFromBackend;

	/** SendInput variant will return data to this event. */
	UPROPERTY(BlueprintAssignable, Category = TensorflowEvents)
	FMLResultSignature OnInputResult;

	UPROPERTY(BlueprintAssignable, Category = TensorflowEvents)
	FMLRawResultSignature OnRawInputResult;

	/** What this means is different for each sub-type */
	UPROPERTY(BlueprintReadOnly, Category = TensorflowRemoteProperties)
	bool bIsConnectedToBackend;

	/** Abstract Method: Send input to ML side result comes back to the OnResult event. Optionally re-target to another function name. */
	UFUNCTION(BlueprintCallable, Category = TensorflowFunctions)
	virtual void SendInput(const FString& InputData, const FString& FunctionName = TEXT("onJsonInput"));

	/** Abstract Method: Send float array input, bypasses encoding. Useful for large data/native inference. Result comes back to the OnRawResult event*/
	UFUNCTION(BlueprintCallable, Category = TensorflowFunctions)
	virtual void SendRawInput(const TArray<float>& InputData, const FString& FunctionName = TEXT("onFloatArrayInput"));

	/** Abstract Method: Send input to ML side result will come back as a latent action in the graph. Recommended method. Optionally re-target to another function name. */
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"), Category = TensorflowFunctions)
	virtual void SendInputGraphResult(const FString& InputData, FString& ResultData, struct FLatentActionInfo LatentInfo, const FString& FunctionName = TEXT("onJsonInput"));

	/** Abstract Method: Send float array input, bypasses encoding. Useful for large data/native inference. Result will come back as a latent action in the graph.*/
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo"), Category = TensorflowFunctions)
	virtual void SendRawInputGraphResult(const TArray<float>& InputData, TArray<float>& ResultData, struct FLatentActionInfo LatentInfo, const FString& FunctionName = TEXT("onJsonInput"));

private:
	void ImmediateLatentResponse(struct FLatentActionInfo LatentInfo);

};
