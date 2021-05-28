#include "Controller/ControllerType/JointController/RFingerGripperController.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogRFingerGripperController, Log, All)

void URFingerGripperController::Init()
{
  Super::Init();

  if (ARModel *Owner = GetOwner())
  {
    Owner->NotifyHitDelegate.AddDynamic(this, &URFingerGripperController::AddHitComponent);
    for (const TPair<FString, FGripperInformation> &GripperJoint : GripperControllerParameters.GripperJoints)
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
  CheckFingerHitEvents();
  Super::Tick(InDeltaTime);
  HitComponents.Empty();
  HitFingerCount.Empty();
}

void URFingerGripperController::SetGripperPosition()
{
  for (TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
  {
    const FString JointName = DesiredJointState.Key;
    if (GripperControllerParameters.GripperJoints.Contains(JointName))
    {
      const float FingerDesiredPosition = DesiredPosition * FMath::Sign(GripperControllerParameters.GripperJoints[JointName].OpenPosition);
      DesiredJointState.Value.JointPosition = FingerDesiredPosition;
      float Error = FMath::Abs(FingerDesiredPosition - GetOwner()->GetJoint(JointName)->GetJointState().JointPosition);
      if (Error < 1E-1)
      {
        ActionFinishedDelegate.Broadcast();
      }
    }
    else
    {
      UE_LOG(LogRFingerGripperController, Error, TEXT("%s of %s not found"), *JointName, *GetOwner()->GetName())
    }
  }
}

const float URFingerGripperController::GetGripperPosition() const
{
  float GripperPosition = 0.f;
  for (const TPair<FString, FGripperInformation> &Gripper : GripperControllerParameters.GripperJoints)
  {
    const FString JointName = Gripper.Key;
    if (const URJoint *Joint = GetOwner()->GetJoint(JointName))
    {
      GripperPosition += FMath::Abs(Joint->GetJointState().JointPosition);
    }
    else
    {
      UE_LOG(LogRFingerGripperController, Error, TEXT("%s of %s not found"), *JointName, *GetOwner()->GetName())
    }
  }
  return GripperPosition / GripperControllerParameters.GripperJoints.Num();
}

void URFingerGripperController::SetControllerParameters(URControllerParameter *&ControllerParameters)
{
  if (URFingerGripperControllerParameter *InFingerGripperControllerParameters = Cast<URFingerGripperControllerParameter>(ControllerParameters))
  {
    Super::SetControllerParameters(ControllerParameters);
    FingerGripperControllerParameters = InFingerGripperControllerParameters->FingerGripperControllerParameters;
  }
}

void URFingerGripperController::AddHitComponent(FHitComponent HitComponent)
{
  HitComponents.Add(HitComponent);
}

void URFingerGripperController::CheckFingerHitEvents()
{
  if (GetOwner())
  {
    bStalled = false;
    for (const FHitComponent &HitComponent : HitComponents)
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
            bStalled = true;
            ActionFinishedDelegate.Broadcast();
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
  else
  {
    UE_LOG(LogRFingerGripperController, Error, TEXT("Owner of %s not found"), *GetName())
  }
}

void URFingerGripperController::GraspObject()
{
  if (ARModel *Owner = GetOwner())
  {
    for (const TPair<FString, FGripperInformation> &GripperJoint : GripperControllerParameters.GripperJoints)
    {
      if (URJoint *Joint = Owner->GetJoint(GripperJoint.Key))
      {
        switch (JointControllerParameters.Mode)
        {
        case UJointControllerMode::Dynamic:
          if (URLink *Child = Joint->GetChild())
          {
            if (UStaticMeshComponent *ChildMesh = Child->GetRootMesh())
            {
              const FVector FingerForce = FingerGripperControllerParameters.GraspingStrength * ChildMesh->GetComponentRotation().RotateVector(Joint->GetType()->Axis);
              ChildMesh->AddForce(FingerForce, NAME_None, true);
            }
          }
          break;
        
        default:
          break;
        }
      } 
    }
  }
  else
  {
    UE_LOG(LogRFingerGripperController, Error, TEXT("%s is not attached to ARModel"), *GetName())
  }
}