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
  void Init(UObject *InOwner, const TSharedPtr<FROSBridgeHandler> &InHandler, const FString &InTopic = TEXT(""));

  void Init(UObject *InOwner, const FString &WebsocketIPAddr, const uint32 &WebsocketPort, const FString &InTopic = TEXT(""));

  void DeInit()
  {
    if (Handler.IsValid())
    {
      Handler->Disconnect();
    }
  }

  void Tick();

public:
  ARModel *GetOwner() const { return Owner; }

  virtual void Publish() {}

public:
  UPROPERTY()
  URPublisherParameter *PublisherParameters;

protected:
  void Init(UObject *InOwner, const FString &InTopic);

  virtual void Init() {}

  virtual void SetOwner(UObject *&InOwner) { Owner = Cast<ARModel>(InOwner); }

  virtual void CreatePublisher();

protected:
  TSharedPtr<FROSBridgePublisher> Publisher;

  TSharedPtr<FROSBridgeHandler> Handler;

private:
  UPROPERTY()
  ARModel *Owner;
};