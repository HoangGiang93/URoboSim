#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAServer.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTACancelSubscriber.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAFeedbackPublisher.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAGoalSubscriber.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAResultPublisher.h"
#include "ROSCommunication/Action/Server/FollowJointTrajectoryAction/FJTAStatusPublisher.h"

URFJTAServer::URFJTAServer()
{
  ActionName = TEXT("/whole_body_controller/body/follow_joint_trajectory");
  FrameId = TEXT("odom");
  JointParamPath = TEXT("whole_body_controller/body/joints");
  ControllerName = TEXT("JointController");
  CancelSubscriber = CreateDefaultSubobject<URFJTACancelSubscriber>(TEXT("FJTACancelSubscriber"));
  StatusPublisher = CreateDefaultSubobject<URFJTAStatusPublisher>(TEXT("FJTAStatusPublisher"));
  ResultPublisher = CreateDefaultSubobject<URFJTAResultPublisher>(TEXT("FJTAResultPublisher"));
  GoalSubscriber = CreateDefaultSubobject<URFJTAGoalSubscriber>(TEXT("FJTAGoalSubscriber"));
  FeedbackPublisher = CreateDefaultSubobject<URFJTAFeedbackPublisher>(TEXT("FJTAFeedbackPublisher"));
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
  Cast<URFJTAFeedbackPublisher>(FeedbackPublisher)->FrameId = FrameId;
  Cast<URFJTAResultPublisher>(ResultPublisher)->FrameId = FrameId;
  Cast<URFJTAStatusPublisher>(StatusPublisher)->FrameId = FrameId;

  Super::Init();

  GetJointsClient = NewObject<URGetJointsClient>(GetOwner());
  GetJointsClient->GetParamArguments.Name = JointParamPath;
  GetJointsClient->Connect(Handler);
  if (URJointController *JointController = Cast<URJointController>(GetOwner()->GetController(ControllerName)))
  {
    JointController->bControllAllJoints = false;
    GetJointsClient->GetJointNames(&JointController->JointNames);
  }
}