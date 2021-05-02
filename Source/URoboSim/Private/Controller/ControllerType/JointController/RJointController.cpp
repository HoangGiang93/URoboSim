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

void URJointController::SetJointNames()
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

void URJointController::Init()
{
  Super::Init();

  bPublishResult = false;
  if (GetOwner())
  {
    if (bControllAllJoints)
    {
      for (URJoint *&Joint : GetOwner()->GetJoints())
      {
        JointNames.Add(Joint->GetName());
      }
    }
    SetJointNames();
    SetMode();
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
    for (const FString &JointName : JointNames)
    {
      if (URJoint *Joint = GetOwner()->GetJoint(JointName))
      {
        switch (Mode)
        {
        case UJointControllerMode::Dynamic:
          if (!DesiredJointStates.Contains(JointName))
          {
            Joint->SetDrive(EnableDrive);
          }
          Joint->SetTargetState(DesiredJointStates.FindOrAdd(JointName));
          break;

        case UJointControllerMode::Kinematic:
          Joint->SetState(DesiredJointStates.FindOrAdd(JointName));
          break;
        }
      }
      else
      {
        UE_LOG(LogRJointController, Error, TEXT("%s of %s not found"), *JointName, *GetOwner()->GetName())
      }
    }
  }
}