#include "ROSCommunication/Publisher/RPublisher.h"
#include "ROSCommunication/RROSCommunicationComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPublisher, Log, All)

void URPublisher::SetPublishParameters(URPublisherParameter *&PublisherParameters)
{
  if (PublisherParameters)
  {
    CommonPublisherParameters = PublisherParameters->CommonPublisherParameters;
  }
}

void URPublisher::Init()
{
  CreatePublisher();
  if (Publisher.IsValid())
  {
    Handler->AddPublisher(Publisher);
  }
}

void URPublisher::Tick()
{
  if (Handler.IsValid())
  {
    Publish();
  }
}

void URPublisher::CreatePublisher()
{
  Publisher = MakeShareable<FROSBridgePublisher>(new FROSBridgePublisher(CommonPublisherParameters.Topic, CommonPublisherParameters.MessageType));
}