#pragma once

#include "Controller/RController.h"
// clang-format off
#include "RJointController.generated.h"
// clang-format on

UENUM()
enum class UJointControllerMode : uint8
{
  Dynamic,
  Kinematic
};

UENUM()
enum class UJointControllerState : uint8
{
  Normal,
  FollowJointTrajectory,
  Off
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

struct FTrajectoryStatus
{
public:
  FJointState CurrentState;

  FJointState DesiredState;

  FJointState ErrorState;
};

UCLASS()
class UROBOSIM_API URJointControllerParameter : public URControllerParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  UJointControllerMode Mode = UJointControllerMode::Dynamic;

  UPROPERTY(EditAnywhere)
  FEnableDrive EnableDrive;

  UPROPERTY(EditAnywhere)
  bool bDisableCollision;

  UPROPERTY(EditAnywhere)
  bool bControllAllJoints;
};

UCLASS(Blueprintable, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URJointController : public URController
{
  GENERATED_BODY()

public:
  URJointController();

public:
  virtual void Tick(const float &InDeltaTime) override;

  virtual void Init() override;

  virtual void SetControllerParameters(URControllerParameter *&ControllerParameters) override;

public:
  void Reset();

  const TMap<FString, FTrajectoryStatus> GetTrajectoryStatusArray() const { return TrajectoryStatusArray; }

  void FollowJointTrajectory();

public:
  UPROPERTY(VisibleAnywhere)
  UJointControllerState State;

  UPROPERTY(EditAnywhere)
  FEnableDrive EnableDrive;

  UPROPERTY(EditAnywhere)
  bool bDisableCollision;

  UPROPERTY(EditAnywhere)
  bool bControllAllJoints;

  UPROPERTY(EditAnywhere)
  TMap<FString, FJointState> DesiredJointStates;

  TArray<FTrajectoryPoints> DesiredTrajectory;

  TArray<FString> JointNames;

protected:
  UPROPERTY(EditAnywhere)
  UJointControllerMode Mode;

  TMap<FString, FTrajectoryStatus> TrajectoryStatusArray;

  uint32 TrajectoryPointIndex;

  float ActionStartTime;

  float ActionDuration;

protected:
  void SetMode();

  void SetJointState();

  void SetJointNames();

  bool CheckTrajectoryPoint();

  bool CheckTrajectoryGoalReached();

  void SetDesiredJointState();

private:
  FTrajectoryPoints LastTrajectoryPoints;
};