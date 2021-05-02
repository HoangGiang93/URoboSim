#include "Controller/ControllerType/JointController/RJointTrajectoryController.h"
#include "Conversions.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogRJointTrajectoryController, Log, All);

URJointTrajectoryController::URJointTrajectoryController()
{
  State = UJointTrajectoryControllerState::Normal;
}

void URJointTrajectoryController::Reset()
{
  DesiredJointStates.Empty();
  TrajectoryStatusMap.Empty();
  TrajectoryPointIndex = 0;
  DesiredTrajectory.Empty();

  SetJointNames();
  for (const FString &JointName : JointNames)
  {
    TrajectoryStatusMap.Add(JointName);
  }
}

void URJointTrajectoryController::FollowJointTrajectory()
{
  TrajectoryPointIndex = 0;
  LastTrajectoryPoints.Reset();
  for (const FString &JointName : JointNames)
  {
    if (URJoint *Joint = GetOwner()->GetJoint(JointName))
    {
      LastTrajectoryPoints.JointStates.Add(JointName, Joint->GetJointStateInROSUnit());
    }
  }
  ActionStartTime = GetOwner()->GetGameTimeSinceCreation();
  State = UJointTrajectoryControllerState::FollowJointTrajectory;
}

void URJointTrajectoryController::Tick(const float &InDeltaTime)
{
  if (GetOwner())
  {
    if (State == UJointTrajectoryControllerState::FollowJointTrajectory)
    {
      ActionDuration = GetOwner()->GetGameTimeSinceCreation() - ActionStartTime;
      if (!CheckTrajectoryPoint() || !IsTrajectoryGoalReached())
      {
        SetDesiredJointState();
      }
    }
    SetTargetJointState();
  }
}

bool URJointTrajectoryController::CheckTrajectoryPoint()
{
  float NextTimeStep = DesiredTrajectory[TrajectoryPointIndex].GetTimeAsFloat();
  if (NextTimeStep == 0)
  {
    TrajectoryPointIndex++;
  }

  for (const FString &JointName : JointNames)
  {
    if (URJoint *Joint = GetOwner()->GetJoint(JointName))
    {
      TrajectoryStatusMap[JointName].CurrentState = Joint->GetJointStateInROSUnit();
      if (DesiredTrajectory[TrajectoryPointIndex].JointStates.Contains(JointName))
      {
        TrajectoryStatusMap[JointName].DesiredState = DesiredTrajectory[TrajectoryPointIndex].JointStates[JointName];
        TrajectoryStatusMap[JointName].ErrorState.JointPosition = TrajectoryStatusMap[JointName].DesiredState.JointPosition - TrajectoryStatusMap[JointName].CurrentState.JointPosition;
        TrajectoryStatusMap[JointName].ErrorState.JointVelocity = TrajectoryStatusMap[JointName].DesiredState.JointVelocity - TrajectoryStatusMap[JointName].CurrentState.JointVelocity;
      }
    }
    else
    {
      UE_LOG(LogRJointTrajectoryController, Error, TEXT("%s of DesiredTrajectory is not contained in %s"), *JointName, *GetOwner()->GetName());
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

bool URJointTrajectoryController::IsTrajectoryGoalReached()
{
  if (TrajectoryPointIndex == DesiredTrajectory.Num())
  {
    State = UJointTrajectoryControllerState::Normal;
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

void URJointTrajectoryController::SetDesiredJointState()
{
  if (State == UJointTrajectoryControllerState::FollowJointTrajectory)
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

    for (TPair<FString, FTrajectoryStatus> &TrajectoryStatus : TrajectoryStatusMap)
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