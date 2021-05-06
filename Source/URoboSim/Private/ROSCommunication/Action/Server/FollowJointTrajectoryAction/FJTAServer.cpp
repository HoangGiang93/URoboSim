#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAServer.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTACancelSubscriber.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAFeedbackPublisher.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAGoalSubscriber.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAResultPublisher.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAStatusPublisher.h"

URFJTAServer::URFJTAServer()
{
  CommonActionServerParameters.ActionName = TEXT("whole_body_controller/body/follow_joint_trajectory");
  CommonActionServerParameters.ControllerName = TEXT("JointTrajectoryController");
}

void URFJTAServer::SetActionServerParameters(URActionServerParameter *&ActionServerParameters)
{
  if (URFJTAServerParameter *InFJTAServerParameters = Cast<URFJTAServerParameter>(ActionServerParameters))
  {
    Super::SetActionServerParameters(ActionServerParameters);
    FJTAServerParameters = InFJTAServerParameters->FJTAServerParameters;
  }
}

void URFJTAServer::Init()
{
  Super::Init();

  GetJointsClient = NewObject<URGetJointsClient>(GetOwner());
  GetJointsClient->GetParamClientParameters.Name = FJTAServerParameters.JointParamPath;
  GetJointsClient->Connect(Handler);
  if (URJointTrajectoryController *JointTrajectoryController = Cast<URJointTrajectoryController>(GetOwner()->GetController(CommonActionServerParameters.ControllerName)))
  {
    JointTrajectoryController->JointControllerParameters.bControllAllJoints = false;
    GetJointsClient->GetJointNames([JointTrajectoryController](const TArray<FString> &InJointNames) { JointTrajectoryController->SetJointNames(InJointNames); });
  }
}

void URFJTAServer::CreateActionServer()
{
  CancelSubscriber = NewObject<URFJTACancelSubscriber>(GetOwner());
  StatusPublisher = NewObject<URFJTAStatusPublisher>(GetOwner());
  ResultPublisher = NewObject<URFJTAResultPublisher>(GetOwner());
  GoalSubscriber = NewObject<URFJTAGoalSubscriber>(GetOwner());
  FeedbackPublisher = NewObject<URFJTAFeedbackPublisher>(GetOwner());

  Cast<URFJTAFeedbackPublisher>(FeedbackPublisher)->FeedbackPublisherParameters.FrameId = FJTAServerParameters.FrameId;
  Cast<URFJTAResultPublisher>(ResultPublisher)->ResultPublisherParameters.FrameId = FJTAServerParameters.FrameId;
  Cast<URFJTAStatusPublisher>(StatusPublisher)->ActionStatusPublisherParameters.FrameId = FJTAServerParameters.FrameId;
}