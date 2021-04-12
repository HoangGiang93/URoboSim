#include "ROSCommunication/RROSCommunicationComponent.h"
#include "Controller/RControllerComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRROSCommunication, Log, All);

URROSCommunicationComponent::URROSCommunicationComponent(const FString &InWebsocketIPAddr, const uint32 &InWebsocketPort) : URROSCommunicationComponent::URROSCommunicationComponent()
{
  ROSCommunication = FRROSCommunicationContainer(InWebsocketIPAddr, InWebsocketPort);
}

URROSCommunicationComponent::URROSCommunicationComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
  PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void URROSCommunicationComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
  ROSCommunication.Tick();
}

void URROSCommunicationComponent::Init()
{
  ROSCommunication.Init();
}

void URROSCommunicationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  ROSCommunication.DeInit();
  Super::EndPlay(EndPlayReason);
}