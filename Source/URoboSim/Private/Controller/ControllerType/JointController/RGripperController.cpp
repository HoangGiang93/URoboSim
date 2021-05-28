#include "Controller/ControllerType/JointController/RGripperController.h"

DEFINE_LOG_CATEGORY_STATIC(LogRGripperController, Log, All)

void URGripperController::Init()
{
  URController::Init();

  bStalled = false;
  JointControllerParameters.bControllAllJoints = false;
  if (GetOwner())
  {
    for (const TPair<FString, FGripperInformation> &GripperJoint : GripperControllerParameters.GripperJoints)
    {
      if (URJoint *Joint = GetOwner()->GetJoint(GripperJoint.Key))
      {
        DesiredJointStates.Add(GripperJoint.Key, FJointState());
        Joint->SetDrive(JointControllerParameters.EnableDrive);
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
      SetGripperPosition();
      Super::Tick(InDeltaTime);
      break;

    case UGripperState::Active:
      SetGripperPosition();
      Super::Tick(InDeltaTime);
      break;

    case UGripperState::Stop:
      StopGripper();
      Super::Tick(InDeltaTime);
      break;

    case UGripperState::Open:
      OpenGripper();
      Super::Tick(InDeltaTime);
      break;

    case UGripperState::Closed:
      CloseGripper();
      Super::Tick(InDeltaTime);
      break;

    case UGripperState::Grasped:
      GraspObject();
      break;
  }
}

void URGripperController::SetControllerParameters(URControllerParameter *&ControllerParameters)
{
  if (URGripperControllerParameter *InGripperControllerParameters = Cast<URGripperControllerParameter>(ControllerParameters))
  {
    Super::SetControllerParameters(ControllerParameters);
    GripperControllerParameters = InGripperControllerParameters->GripperControllerParameters;
  }
}

void URGripperController::OpenGripper()
{
  for (TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
  {
    if (GripperControllerParameters.GripperJoints.Contains(DesiredJointState.Key))
    {
      DesiredJointState.Value.JointPosition = GripperControllerParameters.GripperJoints[DesiredJointState.Key].OpenPosition;
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
    if (GripperControllerParameters.GripperJoints.Contains(DesiredJointState.Key))
    {
      DesiredJointState.Value.JointPosition = GripperControllerParameters.GripperJoints[DesiredJointState.Key].ClosedPosition;
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