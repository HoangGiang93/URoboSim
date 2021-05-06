#include "ROSCommunication/Subscriber/RVelocityCommandSubscriber.h"
#include "Conversions.h"
#include "geometry_msgs/Twist.h"

DEFINE_LOG_CATEGORY_STATIC(LogRVelocityCommandSubscriber, Log, All)

URVelocityCommandSubscriber::URVelocityCommandSubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/base_controller/command");
  CommonSubscriberParameters.MessageType = TEXT("geometry_msgs/Twist");
}

void URVelocityCommandSubscriber::SetSubscriberParameters(URSubscriberParameter *&SubscriberParameters)
{
  URVelocityCommandSubscriberParameter *InVelocityCommandSubscriberParameters = Cast<URVelocityCommandSubscriberParameter>(SubscriberParameters);
  if (InVelocityCommandSubscriberParameters)
  {
    Super::SetSubscriberParameters(SubscriberParameters);
    VelocityCommandSubscriberParameters = InVelocityCommandSubscriberParameters->VelocityCommandSubscriberParameters;
  }  
}

void URVelocityCommandSubscriber::CreateSubscriber()
{
  if (GetOwner())
  {
    Subscriber = MakeShareable<FRVelocityCommandSubscriberCallback>(
        new FRVelocityCommandSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, GetOwner()->GetController(VelocityCommandSubscriberParameters.BaseControllerName)));
  }
}

FRVelocityCommandSubscriberCallback::FRVelocityCommandSubscriberCallback(
    const FString &InTopic, const FString &InType, UObject *InController) : FROSBridgeSubscriber(InTopic, InType)
{
  BaseController = Cast<URBaseController>(InController);
}

TSharedPtr<FROSBridgeMsg> FRVelocityCommandSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<geometry_msgs::Twist> VelocityCommand =
      MakeShareable<geometry_msgs::Twist>(new geometry_msgs::Twist());

  VelocityCommand->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(VelocityCommand);
}

void FRVelocityCommandSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (BaseController)
  {
    TSharedPtr<geometry_msgs::Twist> VelocityCommand = StaticCastSharedPtr<geometry_msgs::Twist>(Msg);
    FVector LinearVelocity = FConversions::ROSToU(VelocityCommand->GetLinear().GetVector());
    FVector AngularVelocity = VelocityCommand->GetAngular().GetVector();
    BaseController->MoveLinear(LinearVelocity);
    BaseController->MoveAngular(-FMath::RadiansToDegrees(AngularVelocity.Z));
  }
}