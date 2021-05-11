#include "ROSCommunication/Action/Server/PR2GripperCommandAction/PR2GCAGoalSubscriber.h"
#include "Conversions.h"
#include "pr2_controllers_msgs/PR2GripperCommandActionGoal.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPR2GCAGoalSubscriber, Log, All)

URPR2GCAGoalSubscriber::URPR2GCAGoalSubscriber()
{
  CommonSubscriberParameters.MessageType = TEXT("pr2_controllers_msgs/Pr2GripperCommandActionGoal");
}

void URPR2GCAGoalSubscriber::CreateSubscriber()
{
  if (Controller)
  {
    Subscriber = MakeShareable<FRPR2GCAGoalSubscriberCallback>(
        new FRPR2GCAGoalSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, Controller));
  }
}

FRPR2GCAGoalSubscriberCallback::FRPR2GCAGoalSubscriberCallback(
    const FString &InTopic, const FString &InType, UObject *InController) : FROSBridgeSubscriber(InTopic, InType)
{
  GripperController = Cast<URGripperController>(InController);
}

TSharedPtr<FROSBridgeMsg> FRPR2GCAGoalSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<pr2_controllers_msgs::PR2GripperCommandActionGoal> Message =
      MakeShareable<pr2_controllers_msgs::PR2GripperCommandActionGoal>(new pr2_controllers_msgs::PR2GripperCommandActionGoal());

  Message->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(Message);
}

void FRPR2GCAGoalSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (GripperController)
  {
    TSharedPtr<pr2_controllers_msgs::PR2GripperCommandActionGoal> Command = StaticCastSharedPtr<pr2_controllers_msgs::PR2GripperCommandActionGoal>(Msg);

    actionlib_msgs::GoalID Id = Command->GetGoalId();

    AsyncTask(ENamedThreads::GameThread, [this, Id, Command]() {
      const float DesiredPosition = FConversions::MToCm(Command->GetGoal().GetCommand().GetPosition());
      GripperController->GripperState = UGripperState::Active;
      GripperController->DesiredPosition = DesiredPosition;

      UE_LOG(LogRPR2GCAGoalSubscriber, Log, TEXT("Received GripperCommand Id: %s Goal: %f"), *Id.GetId(), DesiredPosition);
      GripperController->GoalStatus = FGoalStatusInfo(Id.GetId(), Id.GetStamp().Secs, Id.GetStamp().NSecs);
    });
  }
}