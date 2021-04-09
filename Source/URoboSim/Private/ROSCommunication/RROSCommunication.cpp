#include "ROSCommunication/RROSCommunication.h"
#include "Controller/RController.h"

DEFINE_LOG_CATEGORY_STATIC(LogRROSCommunicationContainer, Log, All);

FRROSCommunicationContainer::FRROSCommunicationContainer(const FString &InWebsocketIPAddr, const uint32 &InWebsocketPort) : WebsocketIPAddr(InWebsocketIPAddr), WebsocketPort(InWebsocketPort)
{
  ControllerComponent = nullptr;
}

FRROSCommunicationContainer::FRROSCommunicationContainer() : FRROSCommunicationContainer::FRROSCommunicationContainer(TEXT("127.0.0.1"), 9393)
{
}

void FRROSCommunicationContainer::Init()
{
  if (ControllerComponent)
  {
    InitPublishers();
    InitSubscribers();
    InitServiceClients();
    InitActionServers();
  }
  else
  {
    UE_LOG(LogRROSCommunicationContainer, Error, TEXT("ControllerComponent not found"))
  }
}

void FRROSCommunicationContainer::InitPublishers()
{
  for (URPublisher *&Publisher : Publishers)
  {
    Publisher->Init(ControllerComponent->GetOwner(), WebsocketIPAddr, WebsocketPort);
  }
}

void FRROSCommunicationContainer::InitSubscribers()
{
  for (URSubscriber *&Subscriber : Subscribers)
  {
    Subscriber->Init(ControllerComponent->GetOwner(), WebsocketIPAddr, WebsocketPort);
  }
}

void FRROSCommunicationContainer::InitServiceClients()
{
  for (URServiceClient *&ServiceClient : ServiceClients)
  {
    ServiceClient->Init(ControllerComponent->GetOwner(), WebsocketIPAddr, WebsocketPort);
  }
}

void FRROSCommunicationContainer::InitActionServers()
{
  for (URActionServer *&ActionServer : ActionServers)
  {
    ActionServer->Init(ControllerComponent->GetOwner(), WebsocketIPAddr, WebsocketPort);
  }
}

void FRROSCommunicationContainer::Tick()
{
  for (URPublisher *&Publisher : Publishers)
  {
    Publisher->Tick();
  }
  for (URSubscriber *&Subscriber : Subscribers)
  {
    Subscriber->Tick();
  }
  for (URServiceClient *&ServiceClient : ServiceClients)
  {
    ServiceClient->Tick();
  }
  for (URActionServer *&ActionServer : ActionServers)
  {
    ActionServer->Tick();
  }
}

void FRROSCommunicationContainer::DeInit()
{
  UE_LOG(LogRROSCommunicationContainer, Log, TEXT("Deinitilize ROSCommunication"))
  for (URPublisher *&Publisher : Publishers)
  {
    Publisher->DeInit();
  }
  for (URSubscriber *&Subscriber : Subscribers)
  {
    Subscriber->DeInit();
  }
  for (URServiceClient *&ServiceClient : ServiceClients)
  {
    ServiceClient->DeInit();
  }
  for (URActionServer *&ActionServer : ActionServers)
  {
    ActionServer->DeInit();
  }
  if (Handler.IsValid())
  {
    Handler->Disconnect();
  }
}