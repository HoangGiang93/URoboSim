#include "ROSCommunication/Publisher/RJointTrajectoryControllerStatePublisher.h"
#include "Controller/RControllerComponent.h"
#include "control_msgs/JointTrajectoryControllerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogRJointTrajectoryControllerStatePublisher, Log, All);

URJointTrajectoryControllerStatePublisher::URJointTrajectoryControllerStatePublisher()
{
  Topic = TEXT("/whole_body_controller/body/state");
  MessageType = TEXT("control_msgs/JointTrajectoryControllerState");
  FrameId = TEXT("odom");
  JointControllerName = TEXT("JointController");
}

void URJointTrajectoryControllerStatePublisher::SetPublishParameters(URPublisherParameter *&PublisherParameters)
{
  if (URJointTrajectoryControllerStatePublisherParameter *JointTrajectoryControllerStatePublisherParameters = Cast<URJointTrajectoryControllerStatePublisherParameter>(PublisherParameters))
  {
    Super::SetPublishParameters(PublisherParameters);
    FrameId = JointTrajectoryControllerStatePublisherParameters->FrameId;
    JointControllerName = JointTrajectoryControllerStatePublisherParameters->JointControllerName;
  }
}

void URJointTrajectoryControllerStatePublisher::Init()
{
  Super::Init();
  if (GetOwner())
  {
    if (!(JointController = Cast<URJointController>(GetOwner()->GetController(JointControllerName))))
    {
      UE_LOG(LogRJointTrajectoryControllerStatePublisher, Error, TEXT("%s not found in %s"), *JointControllerName, *GetName())
    }
  }
}

void URJointTrajectoryControllerStatePublisher::Publish()
{
  if (JointController)
  {
    static int Seq = 0;
    static TSharedPtr<control_msgs::JointTrajectoryControllerState> State =
        MakeShareable(new control_msgs::JointTrajectoryControllerState());

    State->SetHeader(std_msgs::Header(Seq, FROSTime(), FrameId));

    TArray<FString> JointNames = JointController->JointNames;
    TArray<double> DesiredPositions;
    TArray<double> CurrentPositions;
    TArray<double> ErrorPositions;
    TArray<double> DesiredVelocities;
    TArray<double> CurrentVelocities;
    TArray<double> ErrorVelocities;
    for (const TPair<FString, FTrajectoryStatus> &TrajectoryStatus : JointController->GetTrajectoryStatusArray())
    {
      DesiredPositions.Add(TrajectoryStatus.Value.DesiredState.JointPosition);
      CurrentPositions.Add(TrajectoryStatus.Value.CurrentState.JointPosition);
      ErrorPositions.Add(TrajectoryStatus.Value.ErrorState.JointPosition);
      DesiredVelocities.Add(TrajectoryStatus.Value.DesiredState.JointVelocity);
      CurrentVelocities.Add(TrajectoryStatus.Value.CurrentState.JointVelocity);
      ErrorVelocities.Add(TrajectoryStatus.Value.ErrorState.JointVelocity);
    }

    State->SetJointNames(JointNames);

    trajectory_msgs::JointTrajectoryPoint DesiredStatesMsg;
    DesiredStatesMsg.SetPositions(DesiredPositions);
    DesiredStatesMsg.SetVelocities(DesiredVelocities);
    State->SetDesired(DesiredStatesMsg);

    trajectory_msgs::JointTrajectoryPoint CurrentStatesMsg;
    CurrentStatesMsg.SetPositions(CurrentPositions);
    CurrentStatesMsg.SetVelocities(CurrentVelocities);
    State->SetActual(CurrentStatesMsg);

    trajectory_msgs::JointTrajectoryPoint ErrorStatesMsg;
    ErrorStatesMsg.SetPositions(ErrorPositions);
    ErrorStatesMsg.SetVelocities(ErrorVelocities);
    State->SetError(ErrorStatesMsg);

    Handler->PublishMsg(Topic, State);
    Handler->Process();

    Seq++;
  }
  else
  {
    UE_LOG(LogRJointTrajectoryControllerStatePublisher, Error, TEXT("%s not found in %s"), *JointControllerName, *GetName())
  }
}
