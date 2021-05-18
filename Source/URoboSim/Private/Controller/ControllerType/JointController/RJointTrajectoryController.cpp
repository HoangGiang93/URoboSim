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
  TrajectoryPointIndex = 0;
  DesiredTrajectory.Empty();
}

void URJointTrajectoryController::FollowJointTrajectory()
{
  TrajectoryPointIndex = 0;
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
        CalculateDesiredJointState();
      }
    }
    SetDesiredJointState();
  }
}

bool URJointTrajectoryController::CheckTrajectoryPoint()
{
  float NextTimeStep = DesiredTrajectory[TrajectoryPointIndex].GetTimeAsFloat();
  if (NextTimeStep == 0)
  {
    TrajectoryPointIndex++;
  }

  GoalStatus.Status = 1;

  if (ActionDuration > NextTimeStep)
  {
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
    GoalStatus.Status = 3;
    DesiredTrajectory.Empty();
    TrajectoryPointIndex = 0;
    ActionFinishedDelegate.Broadcast();
    return true;
  }
  else
  {
    return false;
  }
}

void URJointTrajectoryController::CalculateDesiredJointState()
{
  if (State == UJointTrajectoryControllerState::FollowJointTrajectory)
  {
    float NextTimeStep = DesiredTrajectory[TrajectoryPointIndex].GetTimeAsFloat();
    float LastTimeStep = DesiredTrajectory[TrajectoryPointIndex-1].GetTimeAsFloat();
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

    for (const TPair<FString, FJointState> &DesiredJointState : DesiredJointStates)
    {
      const FString JointName = DesiredJointState.Key;
      if (DesiredTrajectory[TrajectoryPointIndex].JointStates.Contains(JointName) && DesiredTrajectory[TrajectoryPointIndex-1].JointStates.Contains(JointName) && DesiredJointStates.Contains(JointName))
      {
        float DiffJointPosition = DesiredTrajectory[TrajectoryPointIndex].JointStates[JointName].JointPosition - DesiredTrajectory[TrajectoryPointIndex-1].JointStates[JointName].JointPosition;
        float DesiredJointPosition = DiffJointPosition / DiffTrajectoryTimeStep * (CurrentTimeStep - LastTimeStep) + DesiredTrajectory[TrajectoryPointIndex-1].JointStates[JointName].JointPosition;
        float DiffJointVelocity = DesiredTrajectory[TrajectoryPointIndex].JointStates[JointName].JointVelocity - DesiredTrajectory[TrajectoryPointIndex-1].JointStates[JointName].JointVelocity;
        float DesiredJointVelocity = DiffJointVelocity / DiffTrajectoryTimeStep * (CurrentTimeStep - LastTimeStep) + DesiredTrajectory[TrajectoryPointIndex-1].JointStates[JointName].JointVelocity;

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