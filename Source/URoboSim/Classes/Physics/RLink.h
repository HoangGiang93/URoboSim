// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Michael Neumann

#pragma once

#include "CoreMinimal.h"
#include "RStaticMeshComponent.h"
#include "RLink.generated.h"

class ARModel;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UROBOSIM_API URLink : public URStaticMeshComponent
{
	GENERATED_BODY()

public:
	URLink();


protected:

	UPROPERTY()
	TArray<class URJoint*> Joints;

public:

	UPROPERTY()
	ARModel* Model;

	UPROPERTY(EditAnywhere)
	TArray<class URStaticMeshComponent*> Visuals;
	UPROPERTY(EditAnywhere)
	TArray<class URStaticMeshComponent*> Collisions;

	virtual void SetPose(FTransform InPose);
	virtual void SetPose(FVector InLocation, FQuat InRotation);

	virtual void DisableCollision();
	virtual void EnableCollision();

	URStaticMeshComponent* GetVisual();
	URStaticMeshComponent* GetCollision();
	URStaticMeshComponent* GetCollision(FString InCollisionName, bool bExactMatch = false);
        TArray<class URJoint*> GetJoints();

	float GetNumCollisions();

	void AddJoint(class URJoint* InJoint);

	virtual void UpdateVelocity(float InDeltaTime);
	// virtual void SetNextVelocities();

	UPROPERTY(EditAnywhere)
	FTransform Pose;

        UPROPERTY()
        bool bAttachedToParent = false;
};

USTRUCT()
struct FLinkInformation
{
  GENERATED_BODY()
  public:

  TArray<URLink*> Childs;
};
