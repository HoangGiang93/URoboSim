#pragma once

#include "RJointController.h"
// clang-format off
#include "RJointTrajectoryController.generated.h"
// clang-format on

UENUM()
enum class UJointTrajectoryControllerState : uint8
{
  Normal,
  FollowJointTrajectory,
};

struct FTrajectoryPoints
{
public:
  FTrajectoryPoints() {}

  FTrajectoryPoints(const float &InSecs, const float &InNSecs, const TArray<FString> &InJointNames, const TArray<double> &InPositions, const TArray<double> &InVelocities)
  {
    SetTimeStep(InSecs, InNSecs);
    for (int32 i = 0; i < InJointNames.Num(); i++)
    {
      JointStates.Add(InJointNames[i], FJointState(InPositions[i], InVelocities[i]));
    }
  }

public:
  void SetTimeStep(const float &InSecs, const float &InNSecs)
  {
    Secs = InSecs;
    NSecs = InNSecs;
  }

  const float GetTimeAsFloat() const
  {
    return Secs + NSecs / 1000000000;
  }

  void Reset()
  {
    StartTime = 0.f;
    Secs = 0;
    NSecs = 0;
    JointStates.Empty();
  }

public:
  float StartTime;

  float Secs;

  float NSecs;

  TMap<FString, FJointState> JointStates;
};

UCLASS()
class UROBOSIM_API URJointTrajectoryControllerParameter : public URJointControllerParameter
{
  GENERATED_BODY()

};

UCLASS()
class UROBOSIM_API URJointTrajectoryController : public URJointController
{
  GENERATED_BODY()

public:
  URJointTrajectoryController();

public:
  void Tick(const float &InDeltaTime) override;

public:
  void Reset();

  void FollowJointTrajectory();

public:
  UPROPERTY(VisibleAnywhere)
  UJointTrajectoryControllerState State;

  TArray<FTrajectoryPoints> DesiredTrajectory;

protected:
  uint32 TrajectoryPointIndex;

  float ActionStartTime;

  float ActionDuration;

protected:
  void SetMode();

  bool CheckTrajectoryPoint();

  bool IsTrajectoryGoalReached();

  void CalculateDesiredJointState();
};