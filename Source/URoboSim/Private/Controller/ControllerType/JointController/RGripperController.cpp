#include "Controller/ControllerType/JointController/RGripperController.h"

DEFINE_LOG_CATEGORY_STATIC(LogRGripperController, Log, All)

void URGripperController::Init()
{
  URController::Init();

  State = UJointControllerState::Normal;
  bPublishResult = false;
  bControllAllJoints = false;
  if (GetOwner())
  {
    for (const TPair<FString, FGripperInformation> &GripperJoint : GripperJoints)
    {
      if (URJoint *Joint = GetOwner()->GetJoint(GripperJoint.Key))
      {
        JointNames.Add(GripperJoint.Key);
        DesiredJointStates.Add(GripperJoint.Key, FJointState());
        Joint->SetDrive(EnableDrive);
      } 
    } 
    SetMode();
  }
  else
  {
    UE_LOG(LogRGripperController, Error, TEXT("%s is not attached to ARModel"), *GetName())
  }
}

void URGripperController::Tick(const float &InDeltaTime)
{
  switch (GripperState)
  {
    case UGripperState::Normal:
      break;

    case UGripperState::Stop:
      StopGripper();
      break;

    case UGripperState::Open:
      OpenGripper();
      break;

    case UGripperState::Closed:
      CloseGripper();
      break;

    case UGripperState::Grasped:
      GraspObject();
      break;
  }

  Super::Tick(InDeltaTime);
}

void URGripperController::SetControllerParameters(URControllerParameter *&ControllerParameters)
{
  if (URGripperControllerParameter *GripperControllerParameters = Cast<URGripperControllerParameter>(ControllerParameters))
  {
    Super::SetControllerParameters(ControllerParameters);
    GripperJoints = GripperControllerParameters->GripperJoints;
  }
}

void URGripperController::OpenGripper()
{
  for (TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
  {
    if (GripperJoints.Contains(DesiredJointState.Key))
    {
      DesiredJointState.Value.JointPosition = GripperJoints[DesiredJointState.Key].OpenPosition;
    }
    else
    {
      UE_LOG(LogRGripperController, Error, TEXT("%s in GripperJoints not found"), *DesiredJointState.Key)
    }
  }
}

void URGripperController::CloseGripper()
{
  for (TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
  {
    if (GripperJoints.Contains(DesiredJointState.Key))
    {
      DesiredJointState.Value.JointPosition = GripperJoints[DesiredJointState.Key].ClosedPosition;
    }
    else
    {
      UE_LOG(LogRGripperController, Error, TEXT("%s in GripperJoints not found"), *DesiredJointState.Key)
    }
  }
}

void URGripperController::StopGripper()
{
  for (TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
  {
    if (GetOwner()->GetJoint(DesiredJointState.Key))
    {
      DesiredJointState.Value = GetOwner()->GetJoint(DesiredJointState.Key)->GetJointState();
    }
    else
    {
      UE_LOG(LogRGripperController, Error, TEXT("%s of %s not found"), *DesiredJointState.Key, *GetOwner()->GetName())
    }
  }
  GripperState = UGripperState::Normal;
}