#pragma once

#include "ROSCommunication/Service/Client/RGetParamClient.h"
// clang-format off
#include "RGetJointsClient.generated.h"
// clang-format on

UCLASS()
class UROBOSIM_API URGetJointsClientParameter : public URGetParamClientParameter
{
  GENERATED_BODY()

public:
  URGetJointsClientParameter()
  {
    GetParamArguments.Name = TEXT("/hardware_interface/joints");
  }
};

UCLASS()
class UROBOSIM_API URGetJointsClient final : public URGetParamClient
{
  GENERATED_BODY()

public:
  URGetJointsClient();

public:
  void GetJointNames(TArray<FString> *JointNamesPtr);
};

class FRGetJointsClientCallback final : public FRGetParamClientCallback
{
public:
  FRGetJointsClientCallback(const FString &InServiceName, const FString &InServiceType, TArray<FString> *InJointNamesPtr);

protected:
  void Callback() override;

private:
  TArray<FString> *JointNamesPtr;
};