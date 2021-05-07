#include "Controller/ControllerType/JointController/RJointController.h"

DEFINE_LOG_CATEGORY_STATIC(LogRJointController, Log, All);

URJointController::URJointController()
{
}

void URJointController::SetControllerParameters(URControllerParameter *&ControllerParameters)
{
  URJointControllerParameter *InJointControllerParameters = Cast<URJointControllerParameter>(ControllerParameters);
  if (InJointControllerParameters)
  {
    JointControllerParameters = InJointControllerParameters->JointControllerParameters;
  }
}

void URJointController::SetJointNames(const TArray<FString> &JointNames)
{
  for (const FString &JointName : JointNames)
  {
    if (!DesiredJointStates.Contains(JointName) && GetOwner()->GetJoint(JointName))
    {
      DesiredJointStates.Add(JointName);
      GetOwner()->GetJoint(JointName)->SetDrive(JointControllerParameters.EnableDrive);
    }
  }
  SetMode();
}

void URJointController::Init()
{
  Super::Init();

  if (GetOwner())
  {
    if (JointControllerParameters.bControllAllJoints)
    {
      TArray<FString> JointNames;
      for (URJoint *&Joint : GetOwner()->GetJoints())
      {
        JointNames.Add(Joint->GetName());
      }
      SetJointNames(JointNames);
    }
  }
  else
  {
    UE_LOG(LogRJointController, Error, TEXT("%s is not attached to ARModel"), *GetName())
  }
}

void URJointController::SetMode()
{
  if (GetOwner())
  {
    switch (JointControllerParameters.Mode)
    {
    case UJointControllerMode::Kinematic:
      JointControllerParameters.EnableDrive.bPositionDrive = false;
      JointControllerParameters.EnableDrive.bVelocityDrive = false;
      for (const TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
      {
        if (URLink *Link = GetOwner()->GetJoint(DesiredJointState.Key)->GetChild())
        {
          Link->DisableSimulatePhysics();
          Link->DisableCollision();
        }
      }
      break;

    case UJointControllerMode::Dynamic:
      JointControllerParameters.EnableDrive.bPositionDrive = true;
      JointControllerParameters.EnableDrive.bVelocityDrive = true;
      for (const TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
      {
        if (JointControllerParameters.bDisableCollision)
        {
          if (URLink *Link = GetOwner()->GetJoint(DesiredJointState.Key)->GetChild())
          {
            Link->DisableCollision();
          }
        }
        else
        {
          if (URLink *Link = GetOwner()->GetJoint(DesiredJointState.Key)->GetChild())
          {
            Link->EnableCollision();
          }
        }
      }
      break;
    }
  }
  else
  {
    UE_LOG(LogRJointController, Error, TEXT("Owner of %s not found"), *GetName())
  }
}

void URJointController::Tick(const float &InDeltaTime)
{
  SetTargetJointState();
}

void URJointController::SetTargetJointState()
{
  if (GetOwner())
  {
    for (const TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
    {
      if (URJoint *Joint = GetOwner()->GetJoint(DesiredJointState.Key))
      {
        switch (JointControllerParameters.Mode)
        {
        case UJointControllerMode::Dynamic:
          Joint->SetDrive(JointControllerParameters.EnableDrive);
          Joint->SetTargetState(DesiredJointState.Value);
          break;

        case UJointControllerMode::Kinematic:
          Joint->SetState(DesiredJointState.Value);
          break;
        }
      }
      else
      {
        UE_LOG(LogRJointController, Error, TEXT("%s of %s not found"), *DesiredJointState.Key, *GetOwner()->GetName())
      }
    }
  }
}