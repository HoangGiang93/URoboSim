#pragma once

#include "RActionServer.h"
// clang-format off
#include "RActionStatusPublisher.generated.h"
// clang-format on

USTRUCT()
struct FRActionStatusPublisherContainer
{
  GENERATED_BODY()

public:
  FRActionStatusPublisherContainer()
  {
    FrameId = TEXT("");
  }

public:
  UPROPERTY(EditAnywhere)
  FString FrameId;
};

UCLASS()
class UROBOSIM_API URActionStatusPublisher : public URActionPublisher
{
  GENERATED_BODY()

public:
  URActionStatusPublisher();

public:
  void Publish() override;

public:
  UPROPERTY(EditAnywhere)
  FRActionStatusPublisherContainer StatusPublisherParameters;
};
