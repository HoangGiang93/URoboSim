#pragma once

#include "Physics/RModel.h"
#include "ROSBridgeHandler.h"
#include "ROSBridgePublisher.h"
#include "RPublisherParameter.h"
// clang-format off
#include "RPublisher.generated.h"
// clang-format on

UCLASS(Blueprintable, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOSIM_API URPublisher : public UObject
{
  GENERATED_BODY()

public:
  void Init(const TSharedPtr<FROSBridgeHandler> &InHandler, const FString &InTopic = TEXT(""));

  void Init(const FString &WebsocketIPAddr, const uint32 &WebsocketPort, const FString &InTopic = TEXT(""));

  void DeInit();

  void Tick();

public:
  ARModel *GetOwner() const { return Owner; }

  void SetOwner(UObject *InOwner){ Owner = Cast<ARModel>(InOwner); }

  virtual void Publish() {}

  virtual void SetPublishParameters(URPublisherParameter *&PublisherParameters);

protected:
  void Init(const FString &InTopic);

  virtual void Init();

  virtual void CreatePublisher();

public:
  UPROPERTY(EditAnywhere)
  FString Topic;

protected:
  UPROPERTY(VisibleAnywhere)
  FString MessageType;

  TSharedPtr<FROSBridgePublisher> Publisher;

  TSharedPtr<FROSBridgeHandler> Handler;

private:
  ARModel *Owner;
};