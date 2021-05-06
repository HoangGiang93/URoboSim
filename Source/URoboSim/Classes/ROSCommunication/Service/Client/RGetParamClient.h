#pragma once

#include "ROSBridgeSrvClient.h"
#include "RServiceClient.h"
#include "srv/ConfigureJointStatePublisher.h"
// clang-format off
#include "RGetParamClient.generated.h"
// clang-format on

USTRUCT()
struct FGetParamClientParameterContainer
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FString Name;

  UPROPERTY(EditAnywhere)
  FString Default;
};

UCLASS()
class UROBOSIM_API URGetParamClientParameter : public URServiceClientParameter
{
  GENERATED_BODY()

public:
  URGetParamClientParameter()
  {
    CommonServiceClientParameters.ServiceName = TEXT("rosapi/get_param");
    CommonServiceClientParameters.ServiceType = TEXT("rosapi/GetParam");
  }

public:
  UPROPERTY(EditAnywhere)
  FGetParamClientParameterContainer GetParamClientParameters;
};

UCLASS()
class UROBOSIM_API URGetParamClient : public URServiceClient
{
  GENERATED_BODY()

public:
  URGetParamClient();

public:
  virtual void SetServiceClientParameters(URServiceClientParameter *&ServiceClientParameters) override;

  virtual void CallService() override;

protected:
  virtual void CreateServiceClient() override;

public:
  UPROPERTY(EditAnywhere)
  FGetParamClientParameterContainer GetParamClientParameters;

  TSharedPtr<class FRGetParamClientCallback> GetParamClient;

  TSharedPtr<rosapi::GetParam::Request> Request;
  
  TSharedPtr<rosapi::GetParam::Response> Response;
};

class FRGetParamClientCallback : public FROSBridgeSrvClient
{
public:
  FRGetParamClientCallback(const FString &InServiceName, const FString &InServiceType);

  void Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse) override;

protected:
  virtual void Callback(){}

protected:
  FString ParamString;
};