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

USTRUCT()
struct FRJointControllerParameterContainer
{
  GENERATED_BODY()

public:
  FRJointControllerParameterContainer()
  {
    Mode = UJointControllerMode::Dynamic;
    bDisableCollision = false;
    bControllAllJoints = false;
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
class UROBOSIM_API URJointControllerParameter : public URControllerParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FRJointControllerParameterContainer JointControllerParameters;
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

  void SetJointNames(const TArray<FString> &JointNames);

public:
  UPROPERTY(EditAnywhere)
  FRJointControllerParameterContainer JointControllerParameters;

  UPROPERTY(EditAnywhere)
  TMap<FString, FJointState> DesiredJointStates;

protected:
  void SetMode();

  void SetTargetJointState();
};