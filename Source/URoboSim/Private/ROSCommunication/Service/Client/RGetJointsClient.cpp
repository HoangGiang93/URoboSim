#include "ROSCommunication/Service/Client/RGetJointsClient.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogRGetJointsClient, Log, All);

URGetJointsClient::URGetJointsClient() : URGetParamClient::URGetParamClient()
{
  GetParamArguments.Name = TEXT("hardware_interface/joints");
}

void URGetJointsClient::GetJointNames(TArray<FString> *JointNames)
{
  if (GetOwner())
  {
    // Create the service client
    GetParamClient = MakeShareable<FRGetJointsClientCallback>(new FRGetJointsClientCallback(ServiceName, ServiceType, JointNames));

    FTimerHandle MyTimerHandle;
    GetOwner()->GetWorldTimerManager().SetTimer(MyTimerHandle, this, &URGetJointsClient::CallService, 1.f, false);
  }
}

FRGetJointsClientCallback::FRGetJointsClientCallback(const FString &InServiceName, const FString &InServiceType, TArray<FString> *InJointNamesPtr) : FRGetParamClientCallback::FRGetParamClientCallback(InServiceName, InServiceType)
{
  JointNamesPtr = InJointNamesPtr;
}

void FRGetJointsClientCallback::Callback()
{
  ParamString.RemoveFromStart(TEXT("["));
  ParamString.RemoveFromEnd(TEXT("]"));
  UE_LOG(LogRGetJointsClient, Log, TEXT("ParamString: %s"), *ParamString)
  
  ParamString.ParseIntoArray(*JointNamesPtr, TEXT(","), true);
  for (FString &JointsString : *JointNamesPtr)
  {
    JointsString = JointsString.TrimStartAndEnd().TrimQuotes();
  }
}