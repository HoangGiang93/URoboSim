#pragma once

#include "Controller/ControllerType/JointController/RGripperController.h"
#include "ROSCommunication/Action/Server/RActionServer.h"
// clang-format off
#include "PR2GCAResultPublisher.generated.h"
// clang-format on

USTRUCT()
struct FRPR2GCAResultPublisherParameterContainer
{
  GENERATED_BODY()

public:
  FRPR2GCAResultPublisherParameterContainer()
  {
    FrameId = TEXT("");
  }

public:
  UPROPERTY(EditAnywhere)
  FString FrameId;
};

UCLASS()
class UROBOSIM_API URPR2GCAResultPublisher final : public URActionPublisher
{
  GENERATED_BODY()

public:
  URPR2GCAResultPublisher();

public:
  void Publish() override;

public:
  UPROPERTY(EditAnywhere)
  FRPR2GCAResultPublisherParameterContainer ResultPublisherParameters;

protected:
  void Init() override;

private:
  URGripperController *GripperController;
};
