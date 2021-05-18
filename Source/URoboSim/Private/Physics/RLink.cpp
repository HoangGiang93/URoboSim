// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Michael Neumann

#include "Physics/RLink.h"

DEFINE_LOG_CATEGORY_STATIC(LogRLink, Log, All);

URLink::URLink()
{
  PoseComponent = CreateDefaultSubobject<USceneComponent>(*(GetName() + TEXT("_pose")));
}

void URLink::Init()
{
  SetGravity();
  SetCollision();
}

UStaticMeshComponent *URLink::GetRootMesh() const
{
  if (CollisionMeshes.Num() > 0)
  {
    return CollisionMeshes[0];
  }
  else
  {
    return nullptr;
  }
}

void URLink::SetPose(const FTransform &Pose)
{
  if (GetRootMesh())
  {
    PoseComponent->AttachToComponent(GetRootMesh(), FAttachmentTransformRules::KeepWorldTransform);
    PoseComponent->SetWorldTransform(Pose);
  }
  else
  {
    UE_LOG(LogRLink, Error, TEXT("Link %s doesn't have any collision meshes, SetPose failed"), *GetName())
  }
}

void URLink::SetGravity()
{
  for (UStaticMeshComponent *CollisionMesh : CollisionMeshes)
  {
    CollisionMesh->SetEnableGravity(bEnableGravity);
  }
}

void URLink::DisableSimulatePhysics()
{
  // Attention: 
  // a) SetSimulatePhysics(bEnablePhysics) would detach the link from the parent component, so it should be attached again
  // b) SetSimulatePhysics(true) would break collision, so don't use it
  if (GetRootMesh())
  {
    USceneComponent *ParentMesh = GetRootMesh()->GetAttachParent();
    for (UStaticMeshComponent *&CollisionMesh : CollisionMeshes)
    {
      CollisionMesh->SetSimulatePhysics(false);
    }
    AttachToComponent(ParentMesh);
  }
}

void URLink::AttachToComponent(USceneComponent *Parent)
{
  if (GetRootMesh())
  {
    GetRootMesh()->AttachToComponent(Parent, FAttachmentTransformRules::KeepWorldTransform);
  }
  else
  {
    UE_LOG(LogRLink, Error, TEXT("Link %s doesn't have any collision meshes, AttachToComponent failed"), *GetName())
  }
}

void URLink::SetCollision()
{
  if (GetRootMesh())
  {
    if (bEnableCollision)
    {
      UE_LOG(LogRLink, Log, TEXT("Enable collision for %s"), *GetName())
      for (UStaticMeshComponent *&CollisionMesh : CollisionMeshes)
      {
        CollisionMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
        CollisionMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
      }
    }
    else
    {
      UE_LOG(LogRLink, Log, TEXT("Disable collision for %s"), *GetName())
      for (UStaticMeshComponent *&CollisionMesh : CollisionMeshes)
      {
        CollisionMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
      }
    }
  }
  else
  {
    UE_LOG(LogRLink, Error, TEXT("Link %s doesn't have any collision meshes, EnableCollision failed"), *GetName())
  }
}