#include "ROSCommunication/Publisher/RJointTrajectoryControllerStatePublisher.h"
#include "Controller/RControllerComponent.h"
#include "control_msgs/JointTrajectoryControllerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogRJointTrajectoryControllerStatePublisher, Log, All);

URJointTrajectoryControllerStatePublisher::URJointTrajectoryControllerStatePublisher()
{
  CommonPublisherParameters.Topic = TEXT("whole_body_controller/body/state");
  CommonPublisherParameters.MessageType = TEXT("control_msgs/JointTrajectoryControllerState");
}

void URJointTrajectoryControllerStatePublisher::SetPublishParameters(URPublisherParameter *&PublisherParameters)
{
  if (URJointTrajectoryControllerStatePublisherParameter *InJointTrajectoryControllerStatePublisherParameters = Cast<URJointTrajectoryControllerStatePublisherParameter>(PublisherParameters))
  {
    Super::SetPublishParameters(PublisherParameters);
    JointTrajectoryControllerStatePublisherParameters = InJointTrajectoryControllerStatePublisherParameters->JointTrajectoryControllerStatePublisherParameters;
  }
}

void URJointTrajectoryControllerStatePublisher::Init()
{
  Super::Init();
  if (GetOwner())
  {
    if (!(JointTrajectoryController = Cast<URJointTrajectoryController>(GetOwner()->GetController(JointTrajectoryControllerStatePublisherParameters.JointTrajectoryControllerName))))
    {
      UE_LOG(LogRJointTrajectoryControllerStatePublisher, Error, TEXT("%s not found in %s"), *JointTrajectoryControllerStatePublisherParameters.JointTrajectoryControllerName, *GetName())
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

    State->SetHeader(std_msgs::Header(Seq, FROSTime(), JointTrajectoryControllerStatePublisherParameters.FrameId));

    TArray<FString> JointNames;
    TArray<double> DesiredPositions;
    TArray<double> CurrentPositions;
    TArray<double> ErrorPositions;
    TArray<double> DesiredVelocities;
    TArray<double> CurrentVelocities;
    TArray<double> ErrorVelocities;
    for (const TPair<FString, FJointState> &DesiredJointState : JointTrajectoryController->DesiredJointStates)
    {
      const FString JointName = DesiredJointState.Key;
      if (URJoint *Joint = GetOwner()->GetJoint(JointName))
      {
        JointNames.Add(JointName);

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

    Handler->PublishMsg(CommonPublisherParameters.Topic, State);
    Handler->Process();

    Seq++;
  }
  else
  {
    UE_LOG(LogRJointTrajectoryControllerStatePublisher, Error, TEXT("%s not found in %s"), *JointTrajectoryControllerStatePublisherParameters.JointTrajectoryControllerName, *GetName())
  }
}
