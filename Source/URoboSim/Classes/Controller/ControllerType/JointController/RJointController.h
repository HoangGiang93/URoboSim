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

UCLASS()
class UROBOSIM_API URJointControllerParameter : public URControllerParameter
{
  GENERATED_BODY()

public:
  URJointControllerParameter()
  {
    Mode = UJointControllerMode::Dynamic;
    bDisableCollision = true;
    bControllAllJoints = true;
    EnableDrive.PositionStrength = 1E5;
    EnableDrive.VelocityStrength = 1E5;
    EnableDrive.MaxForce = 1E10;
  }

public:
  UPROPERTY(EditAnywhere)
  UJointControllerMode Mode;

  UPROPERTY(EditAnywhere)
  FEnableDrive EnableDrive;

  UPROPERTY(EditAnywhere)
  bool bDisableCollision;

  UPROPERTY(EditAnywhere)
  bool bControllAllJoints;
};

UCLASS()
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
  UPROPERTY(EditAnywhere)
  FEnableDrive EnableDrive;

  UPROPERTY(EditAnywhere)
  bool bDisableCollision;

  UPROPERTY(EditAnywhere)
  bool bControllAllJoints;

  UPROPERTY(EditAnywhere)
  TMap<FString, FJointState> DesiredJointStates;

  TArray<FString> JointNames;

protected:
  UPROPERTY(EditAnywhere)
  UJointControllerMode Mode;

protected:
  void SetMode();

  void SetTargetJointState();

  void SetJointNames();
};