#include "Controller/ControllerType/JointController/RJointController.h"

DEFINE_LOG_CATEGORY_STATIC(LogRJointController, Log, All);

URJointController::URJointController()
{
  Mode = UJointControllerMode::Dynamic;
  bDisableCollision = true;
  bControllAllJoints = true;
  EnableDrive.PositionStrength = 1E5;
  EnableDrive.VelocityStrength = 1E5;
  EnableDrive.MaxForce = 1E10;
}

void URJointController::SetControllerParameters(URControllerParameter *&ControllerParameters)
{
  URJointControllerParameter *JointControllerParameters = Cast<URJointControllerParameter>(ControllerParameters);
  if (JointControllerParameters)
  {
    Mode = JointControllerParameters->Mode;
    EnableDrive = JointControllerParameters->EnableDrive;
    bDisableCollision = JointControllerParameters->bDisableCollision;
    bControllAllJoints = JointControllerParameters->bControllAllJoints;
  }
}

void URJointController::SetJointNames(const TArray<FString> &JointNames)
{
  for (const FString &JointName : JointNames)
  {
    if (!DesiredJointStates.Contains(JointName) && GetOwner()->GetJoint(JointName))
    {
      DesiredJointStates.Add(JointName);
      GetOwner()->GetJoint(JointName)->SetDrive(EnableDrive);
    }
  }
}

TFunction<void (const TArray<FString> &JointNames)> URJointController::SetJointNamesFunction()
{
  return [this](const TArray<FString> &JointNames){ SetJointNames(JointNames); };
}

void URJointController::Init()
{
  Super::Init();

  bPublishResult = false;
  if (GetOwner())
  {
    SetMode();
    if (bControllAllJoints)
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
    switch (Mode)
    {
    case UJointControllerMode::Kinematic:
      EnableDrive.bPositionDrive = false;
      EnableDrive.bVelocityDrive = false;
      for (URLink *&Link : GetOwner()->GetLinks())
      {
        Link->DisableSimulatePhysics();
        Link->DisableCollision();
      }
      break;

    case UJointControllerMode::Dynamic:
      GetOwner()->EnableGravity.bLinks = false;
      EnableDrive.bPositionDrive = true;
      EnableDrive.bVelocityDrive = true;
      if (bDisableCollision)
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
        switch (Mode)
        {
        case UJointControllerMode::Dynamic:
          Joint->SetDrive(EnableDrive);
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