#include "ROSCommunication/Action/Server/PR2GripperCommandAction/PR2GCAResultPublisher.h"
#include "pr2_controllers_msgs/PR2GripperCommandActionResult.h"
#include "pr2_controllers_msgs/PR2GripperCommandResult.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPR2GCAResultPublisher, Log, All);

URPR2GCAResultPublisher::URPR2GCAResultPublisher()
{
  CommonPublisherParameters.MessageType = TEXT("pr2_controllers_msgs/Pr2GripperCommandActionResult");
}

void URPR2GCAResultPublisher::Init()
{
  Super::Init();
  GripperController = Cast<URGripperController>(Controller);
}

void URPR2GCAResultPublisher::Publish()
{
  if (GetOwner() && GripperController && GripperController->bPublishResult)
  {
    static int Seq = 0;
    TSharedPtr<pr2_controllers_msgs::PR2GripperCommandActionResult> ActionResult =
        MakeShareable(new pr2_controllers_msgs::PR2GripperCommandActionResult());

    ActionResult->SetHeader(std_msgs::Header(Seq, FROSTime(), ResultPublisherParameters.FrameId));

    // uint8 status = Owner->Status;
    FGoalStatusInfo StatusInfo = GripperController->GoalStatus;
    actionlib_msgs::GoalStatus GS(actionlib_msgs::GoalID(FROSTime(StatusInfo.Secs, StatusInfo.NSecs), StatusInfo.Id), StatusInfo.Status, "");
    ActionResult->SetStatus(GS);

    pr2_controllers_msgs::PR2GripperCommandResult Result(GripperController->GetGripperPosition(), 0.f, GripperController->IsStalled(), GripperController->IsStalled());
    ActionResult->SetResult(Result);

    Handler->PublishMsg(CommonPublisherParameters.Topic, ActionResult);
    Handler->Process();

    Seq++;
    GripperController->bPublishResult = false;
  }
}
