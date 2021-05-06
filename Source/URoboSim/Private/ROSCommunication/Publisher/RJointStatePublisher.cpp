#include "ROSCommunication/Publisher/RJointStatePublisher.h"
#include "Physics/RJoint.h"
#include "sensor_msgs/JointState.h"

DEFINE_LOG_CATEGORY_STATIC(LogRJointStatePublisher, Log, All);

static void GenerateMsg(const TMap<FString, FJointState> &JointStates, TArray<double> &JointPositions, TArray<double> &JointVelocities)
{
  for (const TPair<FString, FJointState> &JointState : JointStates)
  {
    JointPositions.Add(JointState.Value.JointPosition);
    JointVelocities.Add(JointState.Value.JointVelocity);
  }
}

URJointStatePublisher::URJointStatePublisher()
{
  CommonPublisherParameters.Topic = TEXT("body/joint_states");
  CommonPublisherParameters.MessageType = TEXT("sensor_msgs/JointState");
}

void URJointStatePublisher::SetPublishParameters(URPublisherParameter *&PublisherParameters)
{
  if (URJointStatePublisherParameter *InJointStatePublisherParameters = Cast<URJointStatePublisherParameter>(PublisherParameters))
  {
    Super::SetPublishParameters(PublisherParameters);
    JointStatePublisherParameters = InJointStatePublisherParameters->JointStatePublisherParameters;
  }
}

void URJointStatePublisher::Init()
{
  Super::Init();
  if (GetOwner())
  {
    GetJointsClient = NewObject<URGetJointsClient>(GetOwner());
    GetJointsClient->GetParamClientParameters.Name = JointStatePublisherParameters.JointParamPath;
    GetJointsClient->Connect(Handler);
    GetJointsClient->GetJointNames([this](const TArray<FString> &InJointNames){ JointNames = InJointNames; });
  }
}

void URJointStatePublisher::Publish()
{
  if (GetOwner())
  {
    TMap<FString, FJointState> JointStates;
    for (const URJoint *Joint : GetOwner()->GetJoints())
    {
      if (JointNames.Contains(Joint->GetName()))
      {
        JointStates.Add(Joint->GetName(), Joint->GetJointStateInROSUnit());
      } 
    }

    static int Seq = 0;
    TSharedPtr<sensor_msgs::JointState> JointStateMsg = MakeShareable(new sensor_msgs::JointState());
    JointStateMsg->SetHeader(std_msgs::Header(Seq++, FROSTime(), JointStatePublisherParameters.FrameId));
    JointStateMsg->SetName(JointNames);

    TArray<double> JointPositions;
    TArray<double> JointVelocities;
    GenerateMsg(JointStates, JointPositions, JointVelocities);
    JointStateMsg->SetPosition(JointPositions);
    JointStateMsg->SetVelocity(JointVelocities);

    Handler->PublishMsg(CommonPublisherParameters.Topic, JointStateMsg);

    Handler->Process();
  }
}