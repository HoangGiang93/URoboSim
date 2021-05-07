#include "ROSCommunication/Action/Server/RActionServer.h"

DEFINE_LOG_CATEGORY_STATIC(LogRActionServer, Log, All)

void URActionServer::SetActionServerParameters(URActionServerParameter *&ActionServerParameters)
{
  if (ActionServerParameters)
  {
    CommonActionServerParameters = ActionServerParameters->CommonActionServerParameters;
  }
}

void URActionServer::Init()
{
  CreateActionServer();
  if (GetOwner())
  {
    URController *Controller = GetOwner()->GetController(CommonActionServerParameters.ControllerName);
    if (!Controller)
    {
      UE_LOG(LogRActionServer, Error, TEXT("%s not found in %s"), *CommonActionServerParameters.ControllerName, *GetName())
      return;
    }
    
    if (GoalSubscriber)
    {
      UE_LOG(LogRActionServer, Log, TEXT("Initialize %s of %s"), *GoalSubscriber->GetName(), *GetName())
      GoalSubscriber->SetController(Controller);
      GoalSubscriber->CommonSubscriberParameters.Topic = CommonActionServerParameters.ActionName + TEXT("/goal");
      GoalSubscriber->Connect(Handler);
    }
    if (CancelSubscriber)
    {
      UE_LOG(LogRActionServer, Log, TEXT("Initialize %s of %s"), *CancelSubscriber->GetName(), *GetName())
      CancelSubscriber->SetController(Controller);
      CancelSubscriber->CommonSubscriberParameters.Topic = CommonActionServerParameters.ActionName + TEXT("/cancel");
      CancelSubscriber->Connect(Handler);
    }
    if (StatusPublisher)
    {
      UE_LOG(LogRActionServer, Log, TEXT("Initialize %s of %s"), *StatusPublisher->GetName(), *GetName())
      StatusPublisher->SetController(Controller);
      StatusPublisher->CommonPublisherParameters.Topic = CommonActionServerParameters.ActionName + TEXT("/status");
      StatusPublisher->Connect(Handler);
    }
    if (FeedbackPublisher)
    {
      UE_LOG(LogRActionServer, Log, TEXT("Initialize %s of %s"), *FeedbackPublisher->GetName(), *GetName())
      FeedbackPublisher->SetController(Controller);
      FeedbackPublisher->CommonPublisherParameters.Topic = CommonActionServerParameters.ActionName + TEXT("/feedback");
      FeedbackPublisher->Connect(Handler);
    }
    if (ResultPublisher)
    {
      UE_LOG(LogRActionServer, Log, TEXT("Initialize %s of %s"), *ResultPublisher->GetName(), *GetName())
      ResultPublisher->SetController(Controller);
      ResultPublisher->CommonPublisherParameters.Topic = CommonActionServerParameters.ActionName + TEXT("/result");
      ResultPublisher->Connect(Handler);
    }
  }
  else
  {
    UE_LOG(LogRActionServer, Error, TEXT("Owner of %s not found, Outer is %s"), *GetName(), *GetOuter()->GetName())
  }
}

void URActionServer::Tick()
{
  if (Handler.IsValid())
  {
    Handler->Process();
  }
  if (StatusPublisher)
  {
    StatusPublisher->Tick();
  }
  if (FeedbackPublisher)
  {
    FeedbackPublisher->Tick();
  }
  if (ResultPublisher)
  {
    ResultPublisher->Tick();
  }
}