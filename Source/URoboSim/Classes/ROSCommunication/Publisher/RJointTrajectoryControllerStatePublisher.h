#pragma once

#include "RPublisher.h"
#include "Controller/ControllerType/JointController/RJointTrajectoryController.h"
// clang-format off
#include "RJointTrajectoryControllerStatePublisher.generated.h"
// clang-format on

USTRUCT()
struct FRJointTrajectoryControllerStatePublisherParameterContainer
{
  GENERATED_BODY()

public:
  FRJointTrajectoryControllerStatePublisherParameterContainer()
  {
    FrameId = TEXT("odom");
    JointTrajectoryControllerName = TEXT("JointTrajectoryController");
  }

public:
  UPROPERTY(EditAnywhere)
  FString FrameId;

  UPROPERTY(EditAnywhere)
  FString JointTrajectoryControllerName;
};

UCLASS()
class UROBOSIM_API URJointTrajectoryControllerStatePublisherParameter : public URPublisherParameter
{
  GENERATED_BODY()

public:
  URJointTrajectoryControllerStatePublisherParameter()
  {
    CommonPublisherParameters.Topic = TEXT("whole_body_controller/body/state");
    CommonPublisherParameters.MessageType = TEXT("control_msgs/JointTrajectoryControllerState");
  }

public:
  UPROPERTY(EditAnywhere)
  FRJointTrajectoryControllerStatePublisherParameterContainer JointTrajectoryControllerStatePublisherParameters;
};

UCLASS()
class UROBOSIM_API URJointTrajectoryControllerStatePublisher final : public URPublisher
{
  GENERATED_BODY()

public:
  URJointTrajectoryControllerStatePublisher();

public:
  void Publish() override;

  void SetPublishParameters(URPublisherParameter *&PublisherParameters) override;

protected:
  void Init() override;

public:
  UPROPERTY(EditAnywhere)
  FRJointTrajectoryControllerStatePublisherParameterContainer JointTrajectoryControllerStatePublisherParameters;

private:
  URJointTrajectoryController *JointTrajectoryController;
};
