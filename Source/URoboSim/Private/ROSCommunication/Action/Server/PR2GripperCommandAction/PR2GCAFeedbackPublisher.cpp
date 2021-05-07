#include "ROSCommunication/Action/Server/PR2GripperCommandAction/PR2GCAFeedbackPublisher.h"
#include "pr2_controllers_msgs/PR2GripperCommandActionFeedback.h"

URPR2GCAFeedbackPublisher::URPR2GCAFeedbackPublisher()
{
  CommonPublisherParameters.MessageType = TEXT("pr2_controllers_msgs/Pr2GripperCommandActionFeedback");
}

void URPR2GCAFeedbackPublisher::Init()
{
  Super::Init();
  GripperController = Cast<URGripperController>(Controller);
}

void URPR2GCAFeedbackPublisher::Publish()
{
  if (GripperController)
  {
    static int Seq = 0;

    TSharedPtr<pr2_controllers_msgs::PR2GripperCommandActionFeedback> Feedback =
        MakeShareable(new pr2_controllers_msgs::PR2GripperCommandActionFeedback());

    FGoalStatusInfo StatusInfo = GripperController->GoalStatus;
    actionlib_msgs::GoalStatus GS(actionlib_msgs::GoalID(FROSTime(StatusInfo.Secs, StatusInfo.NSecs), StatusInfo.Id), StatusInfo.Status, "");
    Feedback->SetStatus(GS);
    Feedback->SetHeader(std_msgs::Header(Seq, FROSTime(), FeedbackPublisherParameters.FrameId));

    pr2_controllers_msgs::PR2GripperCommandFeedback FeedbackMsg(GripperController->GetGripperPosition(), 0.f, GripperController->IsStalled(), GripperController->IsStalled());

    Feedback->SetFeedback(FeedbackMsg);
    Handler->PublishMsg(CommonPublisherParameters.Topic, Feedback);
    Handler->Process();

    Seq++;
  }
}