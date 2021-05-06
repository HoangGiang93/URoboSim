#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAResultPublisher.h"
#include "control_msgs/FollowJointTrajectoryActionResult.h"
#include "control_msgs/FollowJointTrajectoryResult.h"

DEFINE_LOG_CATEGORY_STATIC(LogRFJTAResultPublisher, Log, All);

URFJTAResultPublisher::URFJTAResultPublisher()
{
  CommonPublisherParameters.MessageType = TEXT("control_msgs/FollowJointTrajectoryActionResult");
}

void URFJTAResultPublisher::Init()
{
  Super::Init();
  JointTrajectoryController = Cast<URJointTrajectoryController>(Controller);
}

void URFJTAResultPublisher::Publish()
{
  if (JointTrajectoryController && JointTrajectoryController->bPublishResult)
  {
    static int Seq = 0;

    UE_LOG(LogRFJTAResultPublisher, Log, TEXT("The goal of %s has been reached, publishing results"), *GetOwner()->GetName());
    TSharedPtr<control_msgs::FollowJointTrajectoryActionResult> ActionResult =
        MakeShareable(new control_msgs::FollowJointTrajectoryActionResult());

    ActionResult->SetHeader(std_msgs::Header(Seq, FROSTime(), ResultPublisherParameters.FrameId));

    FGoalStatusInfo GoalStatusInfo = JointTrajectoryController->GetGoalStatus();
    actionlib_msgs::GoalStatus GoalStatus(actionlib_msgs::GoalID(FROSTime(GoalStatusInfo.Secs, GoalStatusInfo.NSecs), GoalStatusInfo.Id), GoalStatusInfo.Status, TEXT(""));
    ActionResult->SetStatus(GoalStatus);

    control_msgs::FollowJointTrajectoryResult Result(0);
    ActionResult->SetResult(Result);

    Handler->PublishMsg(CommonPublisherParameters.Topic, ActionResult);
    Handler->Process();

    JointTrajectoryController->bPublishResult = false;
    Seq++;
  }
}
