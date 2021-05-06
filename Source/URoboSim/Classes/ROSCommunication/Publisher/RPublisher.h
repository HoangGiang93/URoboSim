#pragma once

#include "ROSCommunication/RROSCommunication.h"
#include "ROSBridgePublisher.h"
// clang-format off
#include "RPublisher.generated.h"
// clang-format on

USTRUCT()
struct FRPublisherParameterContainer
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FString Topic;

  UPROPERTY(VisibleAnywhere)
  FString MessageType;
};

UCLASS(BlueprintType, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URPublisherParameter : public UObject
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FRPublisherParameterContainer CommonPublisherParameters;
};

UCLASS()
class UROBOSIM_API URPublisher : public URROSCommunication
{
  GENERATED_BODY()

public:
  void Tick() override;

public:
  virtual void Publish() {}

  virtual void SetPublishParameters(URPublisherParameter *&PublisherParameters);

protected:
  virtual void Init() override;

  virtual void CreatePublisher();

public:
  UPROPERTY(EditAnywhere)
  FRPublisherParameterContainer CommonPublisherParameters;

protected:
  TSharedPtr<FROSBridgePublisher> Publisher;
};