#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTACancelSubscriber.h"
#include "actionlib_msgs/GoalID.h"

DEFINE_LOG_CATEGORY_STATIC(LogRFJTACancelSubscriber, Log, All)

URFJTACancelSubscriber::URFJTACancelSubscriber()
{
  
}

void URFJTACancelSubscriber::CreateSubscriber()
{
  if (Controller)
  {
    Subscriber = MakeShareable<FRFJTACancelSubscriberCallback>(
        new FRFJTACancelSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, Controller));
  }
}

FRFJTACancelSubscriberCallback::FRFJTACancelSubscriberCallback(
    const FString &InTopic, const FString &InType, UObject *InController) : FROSBridgeSubscriber(InTopic, InType)
{
  JointTrajectoryController = Cast<URJointTrajectoryController>(InController);
}

void FRFJTACancelSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (JointTrajectoryController)
  {
    JointTrajectoryController->bCancel = true;
  }
  else
  {
    UE_LOG(LogRFJTACancelSubscriber, Error, TEXT("JointTrajectoryController not found"))
  }
}

TSharedPtr<FROSBridgeMsg> FRFJTACancelSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<actionlib_msgs::GoalID> GoalId =
      MakeShareable<actionlib_msgs::GoalID>(new actionlib_msgs::GoalID());

  GoalId->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(GoalId);
}