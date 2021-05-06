#pragma once

#include "Controller/ControllerType/BaseController/RBaseController.h"
#include "Controller/RControllerComponent.h"
#include "RSubscriber.h"
// clang-format off
#include "RVelocityCommandSubscriber.generated.h"
// clang-format on

USTRUCT()
struct FRVelocityCommandSubscriberParameterContainer
{
  GENERATED_BODY()

public:
  FRVelocityCommandSubscriberParameterContainer()
  {
    BaseControllerName = TEXT("BaseController");
  }

public:
  UPROPERTY(EditAnywhere)
  FString BaseControllerName;
};

UCLASS()
class UROBOSIM_API URVelocityCommandSubscriberParameter : public URSubscriberParameter
{
  GENERATED_BODY()

public:
  URVelocityCommandSubscriberParameter()
  {
    CommonSubscriberParameters.Topic = TEXT("/base_controller/command");
    CommonSubscriberParameters.MessageType = TEXT("geometry_msgs/Twist");
  }

public:
  UPROPERTY(EditAnywhere)
  FRVelocityCommandSubscriberParameterContainer VelocityCommandSubscriberParameters;
};

UCLASS()
class UROBOSIM_API URVelocityCommandSubscriber final : public URSubscriber
{
  GENERATED_BODY()

public:
  URVelocityCommandSubscriber();

public:
  void SetSubscriberParameters(URSubscriberParameter *&SubscriberParameters) override;

protected:
  void CreateSubscriber() override;

public:
  UPROPERTY(EditAnywhere)
  FRVelocityCommandSubscriberParameterContainer VelocityCommandSubscriberParameters;
};

class UROBOSIM_API FRVelocityCommandSubscriberCallback final : public FROSBridgeSubscriber
{
public:
  FRVelocityCommandSubscriberCallback(const FString &InTopic, const FString &InType, UObject *InController);

  TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

  void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:
  URBaseController *BaseController;
};