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
  if (!bComponentInitialized)
  {
    Init();
  }
  
  ROSCommunication.Tick();
}

void URROSCommunicationComponent::Init()
{
  if (GetOwner()->FindComponentByClass<URControllerComponent>())
  {
    UE_LOG(LogRROSCommunication, Log, TEXT("Found ControllerComponent"));
    ROSCommunication.ControllerComponent = GetOwner()->FindComponentByClass<URControllerComponent>();
    ROSCommunication.Init();
    bComponentInitialized = true;
  }
  else
  {
    UE_LOG(LogRROSCommunication, Error, TEXT("No FROSBridgeHandler created."));
  }
}

void URROSCommunicationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  ROSCommunication.DeInit();
  bComponentInitialized = false;
  Super::EndPlay(EndPlayReason);
}