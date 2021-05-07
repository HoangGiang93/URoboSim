#include "ROSCommunication/Action/Server/PR2GripperCommandAction/PR2GCAServer.h"
#include "ROSCommunication/Action/Server/PR2GripperCommandAction/PR2GCACancelSubscriber.h"
#include "ROSCommunication/Action/Server/PR2GripperCommandAction/PR2GCAFeedbackPublisher.h"
#include "ROSCommunication/Action/Server/PR2GripperCommandAction/PR2GCAGoalSubscriber.h"
#include "ROSCommunication/Action/Server/PR2GripperCommandAction/PR2GCAResultPublisher.h"
#include "ROSCommunication/Action/Server/PR2GripperCommandAction/PR2GCAStatusPublisher.h"

URPR2GCAServer::URPR2GCAServer()
{
  CommonActionServerParameters.ActionName = TEXT("gripper_controller");
  CommonActionServerParameters.ControllerName = TEXT("GripperController");
}

void URPR2GCAServer::SetActionServerParameters(URActionServerParameter *&ActionServerParameters)
{
  if (URPR2GCAServerParameter *InPR2GCAServerParameter = Cast<URPR2GCAServerParameter>(ActionServerParameters))
  {
    Super::SetActionServerParameters(ActionServerParameters);
    PR2GCAServerParameters = InPR2GCAServerParameter->PR2GCAServerParameters;
  }
}

void URPR2GCAServer::CreateActionServer()
{
  CancelSubscriber = NewObject<URPR2GCACancelSubscriber>(GetOwner());
  StatusPublisher = NewObject<URPR2GCAStatusPublisher>(GetOwner());
  ResultPublisher = NewObject<URPR2GCAResultPublisher>(GetOwner());
  GoalSubscriber = NewObject<URPR2GCAGoalSubscriber>(GetOwner());
  FeedbackPublisher = NewObject<URPR2GCAFeedbackPublisher>(GetOwner());

  Cast<URPR2GCAFeedbackPublisher>(FeedbackPublisher)->FeedbackPublisherParameters.FrameId = PR2GCAServerParameters.FrameId;
  Cast<URPR2GCAResultPublisher>(ResultPublisher)->ResultPublisherParameters.FrameId = PR2GCAServerParameters.FrameId;
  Cast<URPR2GCAStatusPublisher>(StatusPublisher)->StatusPublisherParameters.FrameId = PR2GCAServerParameters.FrameId;
}