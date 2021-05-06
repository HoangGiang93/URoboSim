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
}

void URJointController::Init()
{
  Super::Init();

  bPublishResult = false;
  if (GetOwner())
  {
    SetMode();
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
      for (URLink *&Link : GetOwner()->GetLinks())
      {
        Link->DisableSimulatePhysics();
        Link->DisableCollision();
      }
      break;

    case UJointControllerMode::Dynamic:
      GetOwner()->EnableGravity.bLinks = false;
      JointControllerParameters.EnableDrive.bPositionDrive = true;
      JointControllerParameters.EnableDrive.bVelocityDrive = true;
      if (JointControllerParameters.bDisableCollision)
      {
        for (URLink *&Link : GetOwner()->GetLinks())
        {
          Link->DisableCollision();
        }
      }
      else
      {
        for (URLink *&Link : GetOwner()->GetLinks())
        {
          Link->EnableCollision();
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