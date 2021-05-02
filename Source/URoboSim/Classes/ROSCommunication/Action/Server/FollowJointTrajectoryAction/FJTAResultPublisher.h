#pragma once

#include "Controller/ControllerType/JointController/RJointTrajectoryController.h"
#include "ROSCommunication/Action/Server/RActionServer.h"
// clang-format off
#include "FJTAResultPublisher.generated.h"
// clang-format on

UCLASS()
class UROBOSIM_API URFJTAResultPublisher final : public URActionPublisher
{
  GENERATED_BODY()

public:
  URFJTAResultPublisher();

public:
  void Publish() override;

protected:
  void Init() override;

public:
  UPROPERTY(EditAnywhere)
  FString FrameId; 

private:
  URJointTrajectoryController *JointTrajectoryController;
};
