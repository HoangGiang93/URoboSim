#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAGoalSubscriber.h"
#include "control_msgs/FollowJointTrajectoryActionGoal.h"

DEFINE_LOG_CATEGORY_STATIC(LogRFJTAGoalSubscriber, Log, All)

URFJTAGoalSubscriber::URFJTAGoalSubscriber()
{
  CommonSubscriberParameters.MessageType = TEXT("control_msgs/FollowJointTrajectoryActionGoal");
}

void URFJTAGoalSubscriber::CreateSubscriber()
{
  if (Controller)
  {
    Subscriber = MakeShareable<FRFJTAGoalSubscriberCallback>(
        new FRFJTAGoalSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, Controller));
  }
}

FRFJTAGoalSubscriberCallback::FRFJTAGoalSubscriberCallback(
    const FString &InTopic, const FString &InType, UObject *InController) : FROSBridgeSubscriber(InTopic, InType)
{
  JointTrajectoryController = Cast<URJointTrajectoryController>(InController);
}

TSharedPtr<FROSBridgeMsg> FRFJTAGoalSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<control_msgs::FollowJointTrajectoryActionGoal> JointStateMessage =
      MakeShareable<control_msgs::FollowJointTrajectoryActionGoal>(new control_msgs::FollowJointTrajectoryActionGoal());

  JointStateMessage->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(JointStateMessage);
}

void FRFJTAGoalSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (JointTrajectoryController)
  {
    TSharedPtr<control_msgs::FollowJointTrajectoryActionGoal> TrajectoryMsg = StaticCastSharedPtr<control_msgs::FollowJointTrajectoryActionGoal>(Msg);

    TArray<FString> JointNames = TrajectoryMsg->GetGoal().GetTrajectory().GetJointNames();

    JointTrajectoryController->Reset();

    actionlib_msgs::GoalID GoalID = TrajectoryMsg->GetGoalId();
    UE_LOG(LogRFJTAGoalSubscriber, Log, TEXT("%s Received Trajectory Goal ID: %s"), *FROSTime::Now().ToString(), *GoalID.GetId());

    JointTrajectoryController->AddGoalStatus(FGoalStatusInfo(GoalID.GetId(), GoalID.GetStamp().Secs, GoalID.GetStamp().NSecs));

    FROSTime ActionStart = TrajectoryMsg->GetGoal().GetTrajectory().GetHeader().GetStamp();

    for (const trajectory_msgs::JointTrajectoryPoint &JointPoint : TrajectoryMsg->GetGoal().GetTrajectory().GetPoints())
    {
      FROSTime TimeStep(JointPoint.GetTimeFromStart());
      JointTrajectoryController->DesiredTrajectory.Add(FTrajectoryPoints(TimeStep.Secs, TimeStep.NSecs, JointNames, JointPoint.GetPositions(), JointPoint.GetVelocities()));
    }

    double ActionTimeDelay = ActionStart.GetTimeAsDouble() - FROSTime::Now().GetTimeAsDouble();
    if (ActionTimeDelay > 0.)
    {
      FTimerHandle MyTimerHandle;
      JointTrajectoryController->GetOwner()->GetWorldTimerManager().SetTimer(MyTimerHandle, JointTrajectoryController, &URJointTrajectoryController::FollowJointTrajectory, ActionTimeDelay, false);
    }
    else
    {
      JointTrajectoryController->FollowJointTrajectory();
    }
  }
  else
  {
    UE_LOG(LogRFJTAGoalSubscriber, Error, TEXT("JointTrajectoryController not found"));
  }
}