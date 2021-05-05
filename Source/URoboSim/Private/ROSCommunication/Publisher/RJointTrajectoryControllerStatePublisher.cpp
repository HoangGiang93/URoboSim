#include "ROSCommunication/Publisher/RJointTrajectoryControllerStatePublisher.h"
#include "Controller/RControllerComponent.h"
#include "control_msgs/JointTrajectoryControllerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogRJointTrajectoryControllerStatePublisher, Log, All);

URJointTrajectoryControllerStatePublisher::URJointTrajectoryControllerStatePublisher()
{
  Topic = TEXT("/whole_body_controller/body/state");
  MessageType = TEXT("control_msgs/JointTrajectoryControllerState");
  FrameId = TEXT("odom");
  JointTrajectoryControllerName = TEXT("JointTrajectoryController");
}

void URJointTrajectoryControllerStatePublisher::SetPublishParameters(URPublisherParameter *&PublisherParameters)
{
  if (URJointTrajectoryControllerStatePublisherParameter *JointTrajectoryControllerStatePublisherParameters = Cast<URJointTrajectoryControllerStatePublisherParameter>(PublisherParameters))
  {
    Super::SetPublishParameters(PublisherParameters);
    FrameId = JointTrajectoryControllerStatePublisherParameters->FrameId;
    JointTrajectoryControllerName = JointTrajectoryControllerStatePublisherParameters->JointTrajectoryControllerName;
  }
}

void URJointTrajectoryControllerStatePublisher::Init()
{
  Super::Init();
  if (GetOwner())
  {
    if (!(JointTrajectoryController = Cast<URJointTrajectoryController>(GetOwner()->GetController(JointTrajectoryControllerName))))
    {
      UE_LOG(LogRJointTrajectoryControllerStatePublisher, Error, TEXT("%s not found in %s"), *JointTrajectoryControllerName, *GetName())
    }
  }
}

void URJointTrajectoryControllerStatePublisher::Publish()
{
  if (JointTrajectoryController)
  {
    static int Seq = 0;
    static TSharedPtr<control_msgs::JointTrajectoryControllerState> State =
        MakeShareable(new control_msgs::JointTrajectoryControllerState());

    State->SetHeader(std_msgs::Header(Seq, FROSTime(), FrameId));

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
    UE_LOG(LogRJointTrajectoryControllerStatePublisher, Error, TEXT("%s not found in %s"), *JointTrajectoryControllerName, *GetName())
  }
}
