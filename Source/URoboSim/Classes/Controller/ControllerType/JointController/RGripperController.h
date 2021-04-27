#pragma once

#include "RJointController.h"
// clang-format off
#include "RGripperController.generated.h"
// clang-format on

USTRUCT()
struct FGripperInformation
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  float OpenPosition;

  UPROPERTY(EditAnywhere)
  float ClosedPosition;
};

UENUM()
enum class UGripperState : uint8
{
  Normal,
  Open,
  Closed,
  Grasped,
  Stop
};

UCLASS()
class UROBOSIM_API URGripperControllerParameter : public URJointControllerParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TMap<FString, FGripperInformation> GripperJoints;
};

UCLASS()
class UROBOSIM_API URGripperController : public URJointController
{
  GENERATED_BODY()

public:
  virtual void Tick(const float &InDeltaTime) override;

  virtual void Init() override;

  virtual void SetControllerParameters(URControllerParameter *&ControllerParameters) override;

public:
  virtual void OpenGripper();

  virtual void CloseGripper();

  virtual void StopGripper();

  virtual void GraspObject() {}

public:
  UPROPERTY(EditAnywhere)
  UGripperState GripperState = UGripperState::Normal;

protected:
  UPROPERTY(EditAnywhere)
  TMap<FString, FGripperInformation> GripperJoints;
};