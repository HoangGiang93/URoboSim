#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAFeedbackPublisher.h"
#include "control_msgs/FollowJointTrajectoryActionFeedback.h"
#include "trajectory_msgs/JointTrajectoryPoint.h"

DEFINE_LOG_CATEGORY_STATIC(LogRFJTAFeedbackPublisher, Log, All)

URFJTAFeedbackPublisher::URFJTAFeedbackPublisher()
{
  MessageType = TEXT("control_msgs/FollowJointTrajectoryActionFeedback");
  FrameId = TEXT("odom");
}

void URFJTAFeedbackPublisher::SetPublishParameters(URPublisherParameter *&PublisherParameters)
{
  if (URFJTAFeedbackPublisherParameter *FJTAFeedbackPublisherParameters = Cast<URFJTAFeedbackPublisherParameter>(PublisherParameters))
  {
    Super::SetPublishParameters(PublisherParameters);
    FrameId = FJTAFeedbackPublisherParameters->FrameId;
  }
}

void URFJTAFeedbackPublisher::Init()
{
  Super::Init();
  if (GetOwner())
  {
    JointController = Cast<URJointController>(Controller);
    if (!JointController)
    {
      UE_LOG(LogRFJTAFeedbackPublisher, Error, TEXT("JointController not found in %s"), *GetOwner()->GetName())
    }
  }
  else
  {
    UE_LOG(LogRFJTAFeedbackPublisher, Error, TEXT("Owner not found in %s, Outer is %s"), *GetName(), *GetOuter()->GetName())
  }
}

void URFJTAFeedbackPublisher::Publish()
{
  if (JointController)
  {
    if (JointController->State == UJointControllerState::FollowJointTrajectory)
    {
      static int Seq = 0;
      TSharedPtr<control_msgs::FollowJointTrajectoryActionFeedback> Feedback =
          MakeShareable(new control_msgs::FollowJointTrajectoryActionFeedback());

      Feedback->SetHeader(std_msgs::Header(Seq, FROSTime(), FrameId));

      FGoalStatusInfo GoalStatusInfo = JointController->GetGoalStatus();
      actionlib_msgs::GoalStatus GoalStatus(actionlib_msgs::GoalID(FROSTime(GoalStatusInfo.Secs, GoalStatusInfo.NSecs), GoalStatusInfo.Id), GoalStatusInfo.Status, TEXT(""));
      Feedback->SetStatus(GoalStatus);

      TArray<FString> JointNames;
      TArray<double> DesiredPositions;
      TArray<double> CurrentPositions;
      TArray<double> ErrorPositions;
      TArray<double> DesiredVelocities;
      TArray<double> CurrentVelocities;
      TArray<double> ErrorVelocities;
      for (const TPair<FString, FTrajectoryStatus> &TrajectoryStatus : JointController->GetTrajectoryStatusArray())
      {
        JointNames.Add(TrajectoryStatus.Key);
        DesiredPositions.Add(TrajectoryStatus.Value.DesiredState.JointPosition);
        CurrentPositions.Add(TrajectoryStatus.Value.CurrentState.JointPosition);
        ErrorPositions.Add(TrajectoryStatus.Value.ErrorState.JointPosition);
        DesiredVelocities.Add(TrajectoryStatus.Value.DesiredState.JointVelocity);
        CurrentVelocities.Add(TrajectoryStatus.Value.CurrentState.JointVelocity);
        ErrorVelocities.Add(TrajectoryStatus.Value.ErrorState.JointVelocity);
      }

      control_msgs::FollowJointTrajectoryFeedback FeedbackMsg;
      FeedbackMsg.SetHeader(std_msgs::Header(Seq, FROSTime(), FrameId));
      FeedbackMsg.SetJointNames(JointNames);

      trajectory_msgs::JointTrajectoryPoint DesiredStatesMsg;
      DesiredStatesMsg.SetPositions(DesiredPositions);
      DesiredStatesMsg.SetVelocities(DesiredVelocities);
      FeedbackMsg.SetDesired(DesiredStatesMsg);

      trajectory_msgs::JointTrajectoryPoint CurrentStatesMsg;
      CurrentStatesMsg.SetPositions(CurrentPositions);
      CurrentStatesMsg.SetVelocities(CurrentVelocities);
      FeedbackMsg.SetActual(CurrentStatesMsg);

      trajectory_msgs::JointTrajectoryPoint ErrorStatesMsg;
      ErrorStatesMsg.SetPositions(ErrorPositions);
      ErrorStatesMsg.SetVelocities(ErrorVelocities);
      FeedbackMsg.SetError(ErrorStatesMsg);

      Feedback->SetFeedback(FeedbackMsg);

      Handler->PublishMsg(Topic, Feedback);
      Handler->Process();

      Seq++;
    }
  }
  else
  {
    UE_LOG(LogRFJTAFeedbackPublisher, Error, TEXT("JointController not found in %s"), *GetName())
  }
}