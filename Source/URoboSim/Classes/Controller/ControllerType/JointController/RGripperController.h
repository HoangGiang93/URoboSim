#pragma once

#include "RJointController.h"
// clang-format off
#include "RGripperController.generated.h"
// clang-format on

UENUM()
enum class UGripperState : uint8
{
  Normal,
  Open,
  Closed,
  Grasped,
  Stop
};

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

USTRUCT()
struct FRGripperControllerParameterContainer
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TMap<FString, FGripperInformation> GripperJoints;
};

UCLASS()
class UROBOSIM_API URGripperControllerParameter : public URJointControllerParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FRGripperControllerParameterContainer GripperControllerParameters;
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
  virtual void SetGripperPosition() {}

  virtual void OpenGripper();

  virtual void CloseGripper();

  virtual void StopGripper();

  virtual void GraspObject() {}

  virtual const float GetGripperPosition() const { return 0.f; }

  const bool IsStalled() const { return bStalled; }
  
public:
  UPROPERTY(EditAnywhere)
  UGripperState GripperState = UGripperState::Normal;

  UPROPERTY(EditAnywhere)
  FRGripperControllerParameterContainer GripperControllerParameters;

  UPROPERTY(EditAnywhere)
  float DesiredPosition;

protected:
  UPROPERTY(VisibleAnywhere)
  bool bStalled;
};