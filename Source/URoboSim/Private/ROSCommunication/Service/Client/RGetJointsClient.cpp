#include "ROSCommunication/Service/Client/RGetJointsClient.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogRGetJointsClient, Log, All);

URGetJointsClient::URGetJointsClient() : URGetParamClient::URGetParamClient()
{
  GetParamArguments.Name = TEXT("hardware_interface/joints");
}

void URGetJointsClient::GetJointNames(TFunction<void (const TArray<FString> &JointNames)> GetJointNamesFunction)
{
  if (GetOwner())
  {
    // Create the service client
    GetParamClient = MakeShareable<FRGetJointsClientCallback>(new FRGetJointsClientCallback(ServiceName, ServiceType, GetJointNamesFunction));

    FTimerHandle MyTimerHandle;
    GetOwner()->GetWorldTimerManager().SetTimer(MyTimerHandle, this, &URGetJointsClient::CallService, 1.f, false);
  }
}

FRGetJointsClientCallback::FRGetJointsClientCallback(const FString &InServiceName, const FString &InServiceType, TFunction<void (const TArray<FString> &JointNames)> InFunction) : FRGetParamClientCallback::FRGetParamClientCallback(InServiceName, InServiceType)
{
  Function = InFunction;
}

void FRGetJointsClientCallback::Callback()
{
  ParamString.RemoveFromStart(TEXT("["));
  ParamString.RemoveFromEnd(TEXT("]"));
  UE_LOG(LogRGetJointsClient, Log, TEXT("ParamString: %s"), *ParamString)
  
  TArray<FString> JointNames;
  ParamString.ParseIntoArray(JointNames, TEXT(","), true);
  for (FString &JointName : JointNames)
  {
    JointName = JointName.TrimStartAndEnd().TrimQuotes();
  }

  Function(JointNames);
}