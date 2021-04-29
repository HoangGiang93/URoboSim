#pragma once

#include "Controller/ControllerType/JointController/RJointController.h"
#include "ROSCommunication/Action/Server/RActionServer.h"
// clang-format off
#include "FJTAFeedbackPublisher.generated.h"
// clang-format on

UCLASS()
class UROBOSIM_API URFJTAFeedbackPublisherParameter : public URPublisherParameter
{
  GENERATED_BODY()

public:
  URFJTAFeedbackPublisherParameter()
  {
    MessageType = TEXT("control_msgs/FollowJointTrajectoryActionFeedback");
    FrameId = TEXT("odom");
  }

public:
  UPROPERTY(EditAnywhere)
  FString FrameId; 

  UPROPERTY(EditAnywhere)
  FString JointParamPath; 
};

UCLASS()
class UROBOSIM_API URFJTAFeedbackPublisher final : public URActionPublisher
{
  GENERATED_BODY()

public:
  URFJTAFeedbackPublisher();

public:
  void Publish() override;

  void SetPublishParameters(URPublisherParameter *&PublisherParameters) override;

protected:
  void Init() override;

public:
  UPROPERTY(EditAnywhere)
  FString FrameId; 

private:
  URJointController *JointController;
};
