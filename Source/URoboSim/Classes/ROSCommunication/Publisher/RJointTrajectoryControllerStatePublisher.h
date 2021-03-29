#pragma once

#include "ROSCommunication/Publisher/RPublisher.h"
#include "Controller/ControllerType/RJointController.h"
// clang-format off
#include "RJointTrajectoryControllerStatePublisher.generated.h"
// clang-format on

UCLASS()
class UROBOSIM_API URJointTrajectoryControllerStatePublisher final : public URPublisher
{
  GENERATED_BODY()

public:
  URJointTrajectoryControllerStatePublisher();

  void Publish() override;

public:
  UPROPERTY(EditAnywhere)
  FString FrameId;

protected:
  void Init() override;

private:
  URJointController *JointController;
};