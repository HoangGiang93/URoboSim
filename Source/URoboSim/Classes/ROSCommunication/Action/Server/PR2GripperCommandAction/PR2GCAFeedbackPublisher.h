#pragma once

#include "Controller/ControllerType/JointController/RGripperController.h"
#include "ROSCommunication/Action/Server/RActionServer.h"
// clang-format off
#include "PR2GCAFeedbackPublisher.generated.h"
// clang-format on

USTRUCT()
struct FRPR2GCAFeedbackPublisherParameterContainer
{
  GENERATED_BODY()

public:
  FRPR2GCAFeedbackPublisherParameterContainer()
  {
    FrameId = TEXT("");
  }

public:
  UPROPERTY(EditAnywhere)
  FString FrameId;
};

UCLASS()
class UROBOSIM_API URPR2GCAFeedbackPublisher final : public URActionPublisher
{
  GENERATED_BODY()

public:
  URPR2GCAFeedbackPublisher();

public:
  void Publish() override;

public:
  UPROPERTY(EditAnywhere)
  FRPR2GCAFeedbackPublisherParameterContainer FeedbackPublisherParameters;

protected:
  void Init() override;

private:
  URGripperController *GripperController;
};
