#include "Controller/ControllerType/JointController/RJointController.h"
#include "Conversions.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogRJointController, Log, All);

URJointController::URJointController()
{
  State = UJointControllerState::Normal;
  Mode = UJointControllerMode::Dynamic;
  bDisableCollision = true;
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
    if (!bControllAllJoints)
    {
      if (!DesiredJointStates.Contains(JointName))
      {
        DesiredJointStates.Add(JointName);
        GetOwner()->GetJoint(JointName)->SetDrive(EnableDrive);
      }
    }
  }
}

void URJointController::Reset()
{
  SetJointNames();
  TrajectoryStatusArray.Empty();
  for (const TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
  {
    TrajectoryStatusArray.Add(DesiredJointState.Key);
  }

  TrajectoryPointIndex = 0;
  DesiredTrajectory.Empty();
}

void URJointController::Init()
{
  Super::Init();

  State = UJointControllerState::Normal;
  bPublishResult = false;
  if (GetOwner())
  {
    if (bControllAllJoints)
    {
      for (URJoint *&Joint : GetOwner()->GetJoints())
      {
        JointNames.Add(Joint->GetName());
        DesiredJointStates.Add(Joint->GetName());
        Joint->SetDrive(EnableDrive);
      }
    }
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
  if (GetOwner())
  {
    switch (State)
    {
    case UJointControllerState::FollowJointTrajectory:
      ActionDuration = GetOwner()->GetGameTimeSinceCreation() - ActionStartTime;
      if (!CheckTrajectoryPoint())
      {
        SetDesiredJointState();
      }
      else
      {
        if (!CheckTrajectoryGoalReached())
        {
          SetDesiredJointState();
        }
      }
      SetJointState();
      break;

    case UJointControllerState::Normal:
      SetJointState();
      break;

    case UJointControllerState::Off:
      break;
    }
  }
}

void URJointController::SetJointState()
{
  switch (Mode)
  {
  case UJointControllerMode::Dynamic:
    for (const FString &JointName : JointNames)
    {
      if (URJoint *Joint = GetOwner()->GetJoint(JointName))
      {
        if (!DesiredJointStates.Contains(JointName))
        {
          DesiredJointStates.Add(JointName);
          Joint->SetDrive(EnableDrive);
        }
        Joint->SetTargetPosition(DesiredJointStates[JointName].JointPosition);
        Joint->SetTargetVelocity(DesiredJointStates[JointName].JointVelocity);
      }
    }
    break;

  case UJointControllerMode::Kinematic:
    for (const FString &JointName : JointNames)
    {
      if (DesiredJointStates.Contains(JointName))
      {
        if (URJoint *Joint = GetOwner()->GetJoint(JointName))
        {
          FJointState TargetJointState = DesiredJointStates.FindOrAdd(JointName);
          Joint->SetPosition(TargetJointState.JointPosition);
        }
      }
    }
    break;
  }
}

bool URJointController::CheckTrajectoryPoint()
{
  float NextTimeStep = DesiredTrajectory[TrajectoryPointIndex].GetTimeAsFloat();
  if (NextTimeStep == 0)
  {
    TrajectoryPointIndex++;
  }

  for (TPair<FString, FTrajectoryStatus> &TrajectoryStatus : TrajectoryStatusArray)
  {
    const FString JointName = TrajectoryStatus.Key;
    if (URJoint *Joint = GetOwner()->GetJoint(JointName))
    {
      TrajectoryStatus.Value.CurrentState = Joint->GetJointStateInROSUnit();
      if (DesiredTrajectory[TrajectoryPointIndex].JointStates.Contains(JointName))
      {
        TrajectoryStatus.Value.DesiredState = DesiredTrajectory[TrajectoryPointIndex].JointStates[JointName];
        TrajectoryStatus.Value.ErrorState.JointPosition = TrajectoryStatus.Value.DesiredState.JointPosition - TrajectoryStatus.Value.CurrentState.JointPosition;
        TrajectoryStatus.Value.ErrorState.JointVelocity = TrajectoryStatus.Value.DesiredState.JointVelocity - TrajectoryStatus.Value.CurrentState.JointVelocity;
      }
    }
    else
    {
      UE_LOG(LogRJointController, Error, TEXT("%s of DesiredTrajectory is not contained in %s"), *JointName, *GetOwner()->GetName());
    }
  }

  GoalStatusList.Last().Status = 1;

  if (ActionDuration > NextTimeStep)
  {
    LastTrajectoryPoints = DesiredTrajectory[TrajectoryPointIndex];
    TrajectoryPointIndex++;
    return true;
  }
  else
  {
    return false;
  }
}

void URJointController::SetDesiredJointState()
{
  if (State == UJointControllerState::FollowJointTrajectory)
  {
    float NextTimeStep = DesiredTrajectory[TrajectoryPointIndex].GetTimeAsFloat();
    float LastTimeStep = LastTrajectoryPoints.GetTimeAsFloat();
    float CurrentTimeStep = ActionDuration;
    float DiffTrajectoryTimeStep = NextTimeStep - LastTimeStep;

    // if execution is slower than the trajectory demands, use the NextTimeStep for further calculations
    // if execution is faster than the trajectory demands, use the LastTimeStep for further calculations
    // in order to cap the linear interpolation between the points
    if (ActionDuration < LastTimeStep)
    {
      CurrentTimeStep = LastTimeStep;
    }
    else if (ActionDuration > NextTimeStep)
    {
      CurrentTimeStep = NextTimeStep;
    }

    for (TPair<FString, FTrajectoryStatus> &TrajectoryStatus : TrajectoryStatusArray)
    {
      const FString JointName = TrajectoryStatus.Key;
      if (DesiredTrajectory[TrajectoryPointIndex].JointStates.Contains(JointName) && LastTrajectoryPoints.JointStates.Contains(JointName) && DesiredJointStates.Contains(JointName))
      {
        float DiffJointPosition = DesiredTrajectory[TrajectoryPointIndex].JointStates[JointName].JointPosition - LastTrajectoryPoints.JointStates[JointName].JointPosition;
        float DesiredJointPosition = DiffJointPosition / DiffTrajectoryTimeStep * (CurrentTimeStep - LastTimeStep) + LastTrajectoryPoints.JointStates[JointName].JointPosition;
        float DiffJointVelocity = DesiredTrajectory[TrajectoryPointIndex].JointStates[JointName].JointVelocity - LastTrajectoryPoints.JointStates[JointName].JointVelocity;
        float DesiredJointVelocity = DiffJointVelocity / DiffTrajectoryTimeStep * (CurrentTimeStep - LastTimeStep) + LastTrajectoryPoints.JointStates[JointName].JointVelocity;

        if (GetOwner()->GetJoint(JointName)->GetType()->GetName().Equals("revolute") || GetOwner()->GetJoint(JointName)->GetType()->GetName().Equals("continuous"))
        {
          DesiredJointStates[JointName].JointPosition = FMath::RadiansToDegrees(DesiredJointPosition);
          DesiredJointStates[JointName].JointVelocity = FMath::RadiansToDegrees(DesiredJointVelocity);
        }
        else if (GetOwner()->GetJoint(JointName)->GetType()->GetName().Equals("prismatic"))
        {
          DesiredJointStates[JointName].JointPosition = FConversions::MToCm((float)DesiredJointPosition);
          DesiredJointStates[JointName].JointVelocity = FConversions::MToCm((float)DesiredJointVelocity);
        }
      }
    }
  }
}

bool URJointController::CheckTrajectoryGoalReached()
{
  if (TrajectoryPointIndex == DesiredTrajectory.Num())
  {
    State = UJointControllerState::Normal;
    bPublishResult = true;
    GoalStatusList.Last().Status = 3;
    DesiredTrajectory.Empty();
    TrajectoryPointIndex = 0;
    return true;
  }
  else
  {
    return false;
  }
}

void URJointController::FollowJointTrajectory()
{
  TrajectoryPointIndex = 0;
  LastTrajectoryPoints.Reset();
  for (const TPair<FString, FTrajectoryStatus> &TrajectoryStatus : TrajectoryStatusArray)
  {
    if (URJoint *Joint = GetOwner()->GetJoint(TrajectoryStatus.Key))
    {
      LastTrajectoryPoints.JointStates.Add(TrajectoryStatus.Key, Joint->GetJointStateInROSUnit());
    }
  }
  ActionStartTime = GetOwner()->GetGameTimeSinceCreation();
  State = UJointControllerState::FollowJointTrajectory;
}