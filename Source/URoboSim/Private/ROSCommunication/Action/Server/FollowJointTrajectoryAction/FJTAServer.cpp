#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAServer.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTACancelSubscriber.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAFeedbackPublisher.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAGoalSubscriber.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAResultPublisher.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAStatusPublisher.h"

URFJTAServer::URFJTAServer()
{
  ActionName = TEXT("/whole_body_controller/body/follow_joint_trajectory");
  ControllerName = TEXT("JointTrajectoryController");
  FrameId = TEXT("odom");
  JointParamPath = TEXT("whole_body_controller/body/joints");
}

void URFJTAServer::SetActionServerParameters(URActionServerParameter *&ActionServerParameters)
{
  if (URFJTAServerParameter *FJTAServerParameter = Cast<URFJTAServerParameter>(ActionServerParameters))
  {
    Super::SetActionServerParameters(ActionServerParameters);
    FrameId = FJTAServerParameter->FrameId;
    JointParamPath = FJTAServerParameter->JointParamPath;
  }
}

void URFJTAServer::Init()
{
  Super::Init();

  GetJointsClient = NewObject<URGetJointsClient>(GetOwner());
  GetJointsClient->GetParamArguments.Name = JointParamPath;
  GetJointsClient->Connect(Handler);
  if (URJointTrajectoryController *JointTrajectoryController = Cast<URJointTrajectoryController>(GetOwner()->GetController(ControllerName)))
  {
    JointTrajectoryController->bControllAllJoints = false;
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

  Cast<URFJTAFeedbackPublisher>(FeedbackPublisher)->FrameId = FrameId;
  Cast<URFJTAResultPublisher>(ResultPublisher)->FrameId = FrameId;
  Cast<URFJTAStatusPublisher>(StatusPublisher)->FrameId = FrameId;
}