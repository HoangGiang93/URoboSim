#include "ROSCommunication/Service/Client/RGetParamClient.h"
#include "TimerManager.h"

URGetParamClient::URGetParamClient()
{
  CommonServiceClientParameters.ServiceName = TEXT("rosapi/get_param");
  CommonServiceClientParameters.ServiceType = TEXT("rosapi/GetParam");
}

void URGetParamClient::SetServiceClientParameters(URServiceClientParameter *&ServiceClientParameters)
{
  if (URGetParamClientParameter *InGetParamClientParameters = Cast<URGetParamClientParameter>(ServiceClientParameters))
  {
    Super::SetServiceClientParameters(ServiceClientParameters);
    GetParamClientParameters = InGetParamClientParameters->GetParamClientParameters;
  }  
}

void URGetParamClient::CreateServiceClient()
{
  if (GetOwner())
  {
    // Create a request instance with request parameters
    Request = MakeShareable(new rosapi::GetParam::Request(GetParamClientParameters.Name, GetParamClientParameters.Default));

    // Create an empty response instance
    Response = MakeShareable(new rosapi::GetParam::Response());
  }
}

void URGetParamClient::CallService()
{
  Handler->CallService(GetParamClient, Request, Response);
}

FRGetParamClientCallback::FRGetParamClientCallback(const FString &InServiceName, const FString &InServiceType) : FROSBridgeSrvClient(InServiceName, InServiceType)
{
}

void FRGetParamClientCallback::Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse)
{
  TSharedPtr<rosapi::GetParam::Response> Response =
      StaticCastSharedPtr<rosapi::GetParam::Response>(InResponse);

  ParamString = Response->GetValue();
  Callback();
}