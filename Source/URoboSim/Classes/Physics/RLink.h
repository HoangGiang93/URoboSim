// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Michael Neumann

#pragma once

#include "Components/StaticMeshComponent.h"
// clang-format off
#include "RLink.generated.h"
// clang-format on

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UROBOSIM_API URLink : public UObject
{
	GENERATED_BODY()

public:
	URLink();

public:
	void Init();

	const FTransform GetPose() const { return PoseComponent->GetComponentTransform(); }

	void SetPose(const FTransform &Pose);

	const USceneComponent *GetPoseComponent() const { return PoseComponent; }

	TArray<UStaticMeshComponent *> GetVisualMeshes() const { return VisualMeshes; }

	void AddVisualMesh(UStaticMeshComponent *&VisualMesh) { VisualMeshes.Add(VisualMesh); }

	TArray<UStaticMeshComponent *> GetCollisionMeshes() const { return CollisionMeshes; }

	void AddCollisionMesh(UStaticMeshComponent *&CollisionMesh) { CollisionMeshes.Add(CollisionMesh); }

	UStaticMeshComponent *GetRootMesh() const;

	void AttachToComponent(USceneComponent *Parent);

	/**
	 * Set physics of the link (disable to neglect oscillate when using kinematic control, enable to move link using dynamic control)
	 * Attention: This function would detach the link from the parent component, so please attach the parent again after this function
	 */
	void SetSimulatePhysics();

	void SetCollision();

	void SetGravity();

public:
	UPROPERTY(EditAnywhere)
	bool bEnableCollision = true;

	UPROPERTY(EditAnywhere)
	bool bEnablePhysics = true;

	UPROPERTY(EditAnywhere)
	bool bEnableGravity = true;

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent *PoseComponent;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMeshComponent *> VisualMeshes;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMeshComponent *> CollisionMeshes;
};