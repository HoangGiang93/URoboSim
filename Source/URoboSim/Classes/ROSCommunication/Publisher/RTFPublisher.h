#pragma once

#include "RPublisher.h"
#include "Engine/StaticMeshActor.h"
// clang-format off
#include "RTFPublisher.generated.h"
// clang-format on

USTRUCT()
struct FRTFPublisherParameterContainer
{
  GENERATED_BODY()

public:
  FRTFPublisherParameterContainer()
  {
    FrameId = TEXT("map");
  }

public:
  UPROPERTY(EditAnywhere)
  FString FrameId;
};

UCLASS()
class UROBOSIM_API URTFPublisherParameter : public URPublisherParameter
{
  GENERATED_BODY()

public:
  URTFPublisherParameter()
  {
    CommonPublisherParameters.Topic = TEXT("/tf");
    CommonPublisherParameters.MessageType = TEXT("tf2_msgs/TFMessage");
  }

public:
  UPROPERTY(EditAnywhere)
  FRTFPublisherParameterContainer TFPublisherParameters;
};

UCLASS()
class UROBOSIM_API URTFPublisher : public URPublisher
{
  GENERATED_BODY()

public:
  URTFPublisher();

public:
  virtual void Publish();

  virtual void SetPublishParameters(URPublisherParameter *&PublisherParameters) override;

public:
  UPROPERTY(EditAnywhere)
  FRTFPublisherParameterContainer TFPublisherParameters;

  UPROPERTY(EditAnywhere)
  TArray<FString> ObjectNames;

protected:
  virtual void Init() override;
  
  virtual void AddObject(AActor *&InObject);

  virtual void SetObjects(const TArray<AActor *> &InObjects);

private:
  UPROPERTY(VisibleAnywhere)
  TArray<AActor *> Objects;
};
