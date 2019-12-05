// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SDF/SDFJointAxis.h"
#include "SDFJoint.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Category = "SDF")
class UROBOSIM_API USDFJoint : public UObject
{
	GENERATED_BODY()
	
public:
	//Constructor
	USDFJoint();

	UPROPERTY(EditAnywhere, Category = "Joint")
	FString Name;

	UPROPERTY(EditAnywhere, Category = "Joint")
	FString Type;

	UPROPERTY(EditAnywhere, Category = "Joint")
	FTransform Pose;

	UPROPERTY(EditAnywhere, Category = "Joint")
	FString Parent;

	UPROPERTY(EditAnywhere, Category = "Joint")
	FString Child;

	UPROPERTY(EditAnywhere, Category = "Joint")
	USDFJointAxis* Axis;
};