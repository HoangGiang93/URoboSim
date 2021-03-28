#pragma once

#include "ROSCommunication/Publisher/RPublisher.h"
#include "Controller/RController.h"
// clang-format off
#include "RActionStatusPublisher.generated.h"
// clang-format on

UCLASS()
class UROBOSIM_API URActionStatusPublisher : public URPublisher
{
  GENERATED_BODY()

public:
  URActionStatusPublisher();

public:
  virtual void Publish() override;

protected:
  URController *Controller;

  UPROPERTY(EditAnywhere)
  FString FrameId;
};
