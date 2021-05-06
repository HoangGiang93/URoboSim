#pragma once

#include "RPublisher.h"
#include "ROSCommunication/Service/Client/RGetJointsClient.h"
// clang-format off
#include "RJointStatePublisher.generated.h"
// clang-format on

USTRUCT()
struct FRJointStatePublisherParameterContainer
{
  GENERATED_BODY()

public:
  FRJointStatePublisherParameterContainer()
  {
    FrameId = TEXT("");
    JointParamPath = TEXT("hardware_interface/joints");
  }

public:
  UPROPERTY(EditAnywhere)
  FString FrameId; 

  UPROPERTY(EditAnywhere)
  FString JointParamPath; 
};

UCLASS()
class UROBOSIM_API URJointStatePublisherParameter : public URPublisherParameter
{
  GENERATED_BODY()

public:
  URJointStatePublisherParameter()
  {
    CommonPublisherParameters.Topic = TEXT("body/joint_states");
    CommonPublisherParameters.MessageType = TEXT("sensor_msgs/JointState");
  }

public:
  UPROPERTY(EditAnywhere)
  FRJointStatePublisherParameterContainer JointStatePublisherParameters;
};

UCLASS()
class UROBOSIM_API URJointStatePublisher final : public URPublisher
{
  GENERATED_BODY()

public:
  URJointStatePublisher();

public:
  void Publish() override;

  void SetPublishParameters(URPublisherParameter *&PublisherParameters) override;

protected:
  void Init() override;

  void SetJointNames(TArray<FString> InJointNames) { JointNames = InJointNames; }

public:
  UPROPERTY(EditAnywhere)
  FRJointStatePublisherParameterContainer JointStatePublisherParameters;

private:
  URGetJointsClient *GetJointsClient;

  TArray<FString> JointNames;
};