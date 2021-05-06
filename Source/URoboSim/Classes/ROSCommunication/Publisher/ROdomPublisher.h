#pragma once

#include "RPublisher.h"
// clang-format off
#include "ROdomPublisher.generated.h"
// clang-format on

USTRUCT()
struct FROdomPublisherParameterContainer
{
  GENERATED_BODY()

public:
  FROdomPublisherParameterContainer()
  {
    FrameId = TEXT("odom");
    FrameNames.Add(TEXT("odom_x_joint"));
    FrameNames.Add(TEXT("odom_y_joint"));
    FrameNames.Add(TEXT("odom_z_joint"));
    bProjectToGround = true;
  }

public:
  UPROPERTY(EditAnywhere)
  FString FrameId;

  UPROPERTY(EditAnywhere)
	TArray<FString> FrameNames;

  UPROPERTY(EditAnywhere)
  bool bProjectToGround;
};

UCLASS()
class UROBOSIM_API UROdomPublisherParameter : public URPublisherParameter
{
  GENERATED_BODY()

public:
  UROdomPublisherParameter()
  {
    CommonPublisherParameters.Topic = TEXT("base/joint_states");
    CommonPublisherParameters.MessageType = TEXT("sensor_msgs/JointState");
  }

public:
  UPROPERTY(EditAnywhere)
  FROdomPublisherParameterContainer OdomPublisherParameters;
};

UCLASS()
class UROBOSIM_API UROdomPublisher final : public URPublisher
{
  GENERATED_BODY()

public:
  UROdomPublisher();

public:
  void Publish() override;

  void SetPublishParameters(URPublisherParameter *&PublisherParameters) override;

protected:
  void Init() override;

public:
  UPROPERTY(EditAnywhere)
  FROdomPublisherParameterContainer OdomPublisherParameters;

private:
	void CalculateOdomStates();

private:
	TArray<double> OdomPosition;

	TArray<double> OdomVelocity;
};
