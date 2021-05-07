#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAFeedbackPublisher.h"
#include "control_msgs/FollowJointTrajectoryActionFeedback.h"
#include "trajectory_msgs/JointTrajectoryPoint.h"

DEFINE_LOG_CATEGORY_STATIC(LogRFJTAFeedbackPublisher, Log, All)

URFJTAFeedbackPublisher::URFJTAFeedbackPublisher()
{
  CommonPublisherParameters.MessageType = TEXT("control_msgs/FollowJointTrajectoryActionFeedback");
}

void URFJTAFeedbackPublisher::Init()
{
  Super::Init();
  JointTrajectoryController = Cast<URJointTrajectoryController>(Controller);
}

void URFJTAFeedbackPublisher::Publish()
{
  if (JointTrajectoryController)
  {
    if (JointTrajectoryController->State == UJointTrajectoryControllerState::FollowJointTrajectory)
    {
      static int Seq = 0;
      TSharedPtr<control_msgs::FollowJointTrajectoryActionFeedback> Feedback =
          MakeShareable(new control_msgs::FollowJointTrajectoryActionFeedback());

      Feedback->SetHeader(std_msgs::Header(Seq, FROSTime(), FeedbackPublisherParameters.FrameId));

      FGoalStatusInfo GoalStatusInfo = JointTrajectoryController->GoalStatus;
      actionlib_msgs::GoalStatus GoalStatus(actionlib_msgs::GoalID(FROSTime(GoalStatusInfo.Secs, GoalStatusInfo.NSecs), GoalStatusInfo.Id), GoalStatusInfo.Status, TEXT(""));
      Feedback->SetStatus(GoalStatus);

      TArray<FString> JointNames;
      TArray<double> DesiredPositions;
      TArray<double> CurrentPositions;
      TArray<double> ErrorPositions;
      TArray<double> DesiredVelocities;
      TArray<double> CurrentVelocities;
      TArray<double> ErrorVelocities;
      for (const TPair<FString, FJointState> &DesiredJointState : JointTrajectoryController->DesiredJointStates)
      {
        if (URJoint *Joint = GetOwner()->GetJoint(DesiredJointState.Key))
        {
          JointNames.Add(Joint->GetName());

          DesiredPositions.Add(DesiredJointState.Value.JointPosition);
          DesiredVelocities.Add(DesiredJointState.Value.JointVelocity);

          CurrentPositions.Add(Joint->GetJointState().JointPosition);
          CurrentVelocities.Add(Joint->GetJointState().JointVelocity);

          ErrorPositions.Add(DesiredJointState.Value.JointPosition - Joint->GetJointState().JointPosition);
          ErrorVelocities.Add(DesiredJointState.Value.JointVelocity - Joint->GetJointState().JointVelocity);
        }
      }

      control_msgs::FollowJointTrajectoryFeedback FeedbackMsg;
      FeedbackMsg.SetHeader(std_msgs::Header(Seq, FROSTime(), FeedbackPublisherParameters.FrameId));
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

      Handler->PublishMsg(CommonPublisherParameters.Topic, Feedback);
      Handler->Process();

      Seq++;
    }
  }
  else
  {
    UE_LOG(LogRFJTAFeedbackPublisher, Error, TEXT("JointTrajectoryController not found in %s"), *GetName())
  }
}