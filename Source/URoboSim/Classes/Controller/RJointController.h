#pragma once

#include "Controller/RController.h"
#include "RJointController.generated.h"


UENUM()
enum class UJointControllerState : uint8
{
  Normal,
  FollowJointTrajectory,
  Off
};

UENUM()
enum class UJointControllerMode : uint8
{
  Dynamic,
  Kinematic
};

UCLASS(Blueprintable, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URJointController : public URController
{
  GENERATED_BODY()
    public:
    URJointController();

  virtual void Init(ARModel* InModel) override;
  virtual void Tick(float InDeltaTime);

  UPROPERTY(VisibleAnywhere)
    UJointControllerState State;

  UPROPERTY(EditAnywhere)
    FString BaseLink;

  UPROPERTY()
    TArray<FTrajectoryPoints> Trajectory;

  UPROPERTY(EditAnywhere)
    TMap<FString,float> DesiredJointState;

  UPROPERTY()
    TArray<bool> bTrajectoryPointsReached;


  UPROPERTY(EditAnywhere)
    float MaxJointAngularVel;

  UPROPERTY()
    FTrajectoryStatus TrajectoryStatus;

  virtual void FollowTrajectory();
  virtual void SetJointNames(TArray<FString> InNames);
  virtual void SwitchMode(UJointControllerMode InMode, bool IsInit = false);

  UJointControllerState GetState();
 protected:
  UPROPERTY(EditAnywhere)
    UJointControllerMode Mode;

  virtual void MoveJoints(float InDeltaTime);
  virtual void MoveJointsDynamic(float InDeltaTime);
  virtual void MoveJointsKinematic();

  UPROPERTY()
    ARModel* Model;

  UPROPERTY()
    uint32 TrajectoryPointIndex;

  void UpdateDesiredJointAngle(float InDeltaTime);
  bool CheckTrajectoryStatus();
  void CallculateJointVelocities(float InDeltaTime);
};