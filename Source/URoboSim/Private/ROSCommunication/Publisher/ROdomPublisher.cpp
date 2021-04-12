#include "ROSCommunication/Publisher/ROdomPublisher.h"
#include "Conversions.h"
#include "sensor_msgs/JointState.h"

UROdomPublisher::UROdomPublisher()
{
  Topic = TEXT("/base/joint_states");
  MessageType = TEXT("sensor_msgs/JointState");
  FrameId = TEXT("odom");
  FrameNames.Add(TEXT("odom_x_joint"));
  FrameNames.Add(TEXT("odom_y_joint"));
  FrameNames.Add(TEXT("odom_z_joint"));
}

void UROdomPublisher::Init()
{
  Super::Init();
  OdomPosition.Init(0., FrameNames.Num());
  OdomVelocity.Init(0., FrameNames.Num());
}

void UROdomPublisher::SetPublishParameters(URPublisherParameter *&PublisherParameters)
{
  UROdomPublisherParameter *OdomPublisherParameters = Cast<UROdomPublisherParameter>(PublisherParameters);
  if (OdomPublisherParameters)
  {
    Super::SetPublishParameters(PublisherParameters);
    FrameId = OdomPublisherParameters->FrameId;
    FrameNames = OdomPublisherParameters->FrameNames;
  }
}

void UROdomPublisher::Publish()
{
  static int Seq = 0;
  static TSharedPtr<sensor_msgs::JointState> JointStateMsg =
      MakeShareable(new sensor_msgs::JointState());

  JointStateMsg->SetHeader(std_msgs::Header(Seq++, FROSTime(), FrameId));
  JointStateMsg->SetName(FrameNames);
  CalculateOdomStates();
  JointStateMsg->SetPosition(OdomPosition);
  JointStateMsg->SetVelocity(OdomVelocity);

  Handler->PublishMsg(Topic, JointStateMsg);

  Handler->Process();
}

void UROdomPublisher::CalculateOdomStates()
{
  if (GetOwner() && OdomPosition.Num() == 3 && OdomVelocity.Num() == 3)
  {
    FTransform BasePose = GetOwner()->GetBaseLink()->GetCollisionMeshes()[0]->GetComponentTransform();
    FVector BasePosition = FConversions::UToROS(BasePose.GetLocation());
    FQuat BaseQuaternion = FConversions::UToROS(BasePose.GetRotation());
    OdomPosition[0] = BasePosition.X;
    OdomPosition[1] = BasePosition.Y;
    OdomPosition[2] = FMath::DegreesToRadians(BaseQuaternion.Rotator().Yaw);

    FVector BaseLinearVelocity = FConversions::UToROS(GetOwner()->GetBaseLink()->GetCollisionMeshes()[0]->GetComponentVelocity());
    FVector BaseAngularVelocity = GetOwner()->GetBaseLink()->GetCollisionMeshes()[0]->GetPhysicsAngularVelocityInDegrees();
    OdomVelocity[0] = BaseLinearVelocity.X;
    OdomVelocity[1] = BaseLinearVelocity.Y;
    OdomVelocity[2] = -FMath::DegreesToRadians(BaseAngularVelocity.Z);
  }
}