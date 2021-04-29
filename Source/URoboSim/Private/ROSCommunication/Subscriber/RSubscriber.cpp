#include "ROSCommunication/Subscriber/RSubscriber.h"

DEFINE_LOG_CATEGORY_STATIC(LogRSubscriber, Log, All)

void URSubscriber::SetSubscriberParameters(URSubscriberParameter *&SubscriberParameters)
{
  if (SubscriberParameters)
  {
    Topic = SubscriberParameters->Topic;
    MessageType = SubscriberParameters->MessageType;
  }
}

void URSubscriber::Init()
{
  Super::Init();
  CreateSubscriber();
  AddSubscriber();
}

void URSubscriber::Tick()
{
  if (Handler.IsValid())
  {
    Handler->Process();
  }
}

void URSubscriber::AddSubscriber()
{
  if (Subscriber.IsValid())
  {
    Handler->AddSubscriber(Subscriber);
  }
}