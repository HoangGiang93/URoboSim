#include "ROSCommunication/Action/Server/RActionServer.h"

DEFINE_LOG_CATEGORY_STATIC(LogRActionServer, Log, All)

void URActionServer::Init(UObject *InOwner, const TSharedPtr<FROSBridgeHandler> &InHandler, const FString &InActionName)
{
  Handler = InHandler;
  if (Handler.IsValid())
  {
    Init(InOwner, InActionName);
  }
  else
  {
    UE_LOG(LogRActionServer, Error, TEXT("No FROSBridgeHandler created in %s"), *GetName())
  }
}

void URActionServer::Init(UObject *InOwner, const FString &WebsocketIPAddr, const uint32 &WebsocketPort, const FString &InActionName)
{
  Handler = MakeShareable<FROSBridgeHandler>(new FROSBridgeHandler(WebsocketIPAddr, WebsocketPort));
  if (Handler.IsValid())
  {
    Handler->Connect();
    Init(InOwner, InActionName);
  }
  else
  {
    UE_LOG(LogRActionServer, Error, TEXT("No FROSBridgeHandler created in %s"), *GetName())
  }
}

void URActionServer::Init(UObject *InOwner, const FString &InActionName)
{
  if (!ActionServerParameters)
  {
    ActionServerParameters = CreateDefaultSubobject<URActionServerParameter>(TEXT("ActionServerParameters"));
  }
  
  if (!InActionName.Equals(TEXT("")))
  {
    ActionServerParameters->ActionName = InActionName;
  }
  Init(InOwner);
}

void URActionServer::Init(UObject *&InOwner)
{
  Init();
  if (GoalSubscriber)
  {
    GoalSubscriber->Init(InOwner, Handler, ActionServerParameters->ActionName + TEXT("/goal"));
  }
  if (CancelSubscriber)
  {
    CancelSubscriber->Init(InOwner, Handler, ActionServerParameters->ActionName + TEXT("/cancel"));
  }
  if (StatusPublisher)
  {
    StatusPublisher->Init(InOwner, Handler, ActionServerParameters->ActionName + TEXT("/status"));
  }
  if (FeedbackPublisher)
  {
    FeedbackPublisher->Init(InOwner, Handler, ActionServerParameters->ActionName + TEXT("/feedback"));
  }
  if (ResultPublisher)
  {
    ResultPublisher->Init(InOwner, Handler, ActionServerParameters->ActionName + TEXT("/result"));
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
    StatusPublisher->Publish();
  }
  if (FeedbackPublisher)
  {
    FeedbackPublisher->Publish();
  }
  if (ResultPublisher)
  {
    ResultPublisher->Publish();
  }
}