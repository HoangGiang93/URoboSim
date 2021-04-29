#pragma once

#include "Controller/ControllerType/JointController/RJointController.h"
#include "ROSCommunication/Action/Server/RActionServer.h"
// clang-format off
#include "FJTAGoalSubscriber.generated.h"
// clang-format on

UCLASS()
class UROBOSIM_API URFJTAGoalSubscriberParameter : public URSubscriberParameter
{
  GENERATED_BODY()

public:
  URFJTAGoalSubscriberParameter()
  {
    MessageType = TEXT("control_msgs/FollowJointTrajectoryActionGoal");
  }
};

UCLASS()
class UROBOSIM_API URFJTAGoalSubscriber final : public URActionSubscriber
{
  GENERATED_BODY()

public:
  URFJTAGoalSubscriber();

protected:
  void CreateSubscriber() override;
};

class UROBOSIM_API FRFJTAGoalSubscriberCallback final : public FROSBridgeSubscriber
{
public:
  FRFJTAGoalSubscriberCallback(const FString &InTopic, const FString &InType, UObject *InController);

  TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

  void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:
  URJointController *JointController;
};