#include "Controller/ControllerType/JointController/RFingerGripperController.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogRFingerGripperController, Log, All)

void URFingerGripperController::Init()
{
  Super::Init();

  if (GetOwner())
  {
    for (const TPair<FString, FGripperInformation> &GripperJoint : GripperJoints)
    {
      URJoint *Joint = GetOwner()->GetJoint(GripperJoint.Key);
      if (Joint && Joint->GetChild())
      {
        FingerMeshes.Emplace(Joint->GetChild()->GetName());
        for (UStaticMeshComponent *&CollisionMesh : Joint->GetChild()->GetCollisionMeshes())
        {
          UE_LOG(LogRFingerGripperController, Log, TEXT("Generate Hit Event for %s"), *CollisionMesh->GetName())
          CollisionMesh->SetNotifyRigidBodyCollision(true);
          FingerMeshes.Last().FingerMeshNames.Add(CollisionMesh->GetName());
        }
      }
      else
      {
        UE_LOG(LogRFingerGripperController, Error, TEXT("%s of %s not found"), *GripperJoint.Key, *GetOwner()->GetName())
      }
    }
  }
}

void URFingerGripperController::Tick(const float &InDeltaTime)
{
  Super::Tick(InDeltaTime);
  CheckFingerHitEvents();
}

void URFingerGripperController::SetControllerParameters(URControllerParameter *&ControllerParameters)
{
  URFingerGripperControllerParameter *FingerGripperControllerParameters = Cast<URFingerGripperControllerParameter>(ControllerParameters);
  if (FingerGripperControllerParameters)
  {
    Super::SetControllerParameters(ControllerParameters);
    GripperHitDistance = FingerGripperControllerParameters->GripperHitDistance;
  }
}

void URFingerGripperController::CheckFingerHitEvents()
{
  HitFingerCount.Empty();
  if (GetOwner())
  {
    for (const FHitComponent &HitComponent : GetOwner()->GetHitComponents())
    {
      HitFingerCount.FindOrAdd(HitComponent.OtherComponent->GetName());
      for (const FFingerMesh &FingerMesh : FingerMeshes)
      {
        if (FingerMesh.FingerMeshNames.Contains(HitComponent.MyComponent->GetName()))
        {
          HitFingerCount[HitComponent.OtherComponent->GetName()] += 1;
          if (HitFingerCount[HitComponent.OtherComponent->GetName()] == 2)
          {
            UE_LOG(LogRFingerGripperController, Log, TEXT("%s is grasped by %s"), *HitComponent.OtherComponent->GetName(), *GetOwner()->GetName())
            if (GripperState == UGripperState::Closed)
            {
              GripperState = UGripperState::Grasped;
              return;
            }
          }
        }
      }
    }
  }
}

void URFingerGripperController::GraspObject()
{
  for (TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
  {
    if (GetOwner()->GetJoint(DesiredJointState.Key))
    {
      DesiredJointState.Value.JointPosition = GetOwner()->GetJoint(DesiredJointState.Key)->GetJointState().JointPosition * GripperHitDistance;
      DesiredJointState.Value.JointVelocity = 0.f;
    }
    else
    {
      UE_LOG(LogRFingerGripperController, Error, TEXT("%s of %s not found"), *DesiredJointState.Key, *GetOwner()->GetName())
    }
  }
}