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
  void GetJointNames(TFunction<void (const TArray<FString> &JointNames)> GetJointNamesFunction);
};

class FRGetJointsClientCallback final : public FRGetParamClientCallback
{
public:
  FRGetJointsClientCallback(const FString &InServiceName, const FString &InServiceType, TFunction<void (const TArray<FString> &JointNames)> InFunction);

protected:
  void Callback() override;

private:
  TFunction<void (const TArray<FString> &JointNames)> Function;
};