#include "ROSCommunication/Publisher/RTFPublisher.h"
#include "Conversions.h"
#include "Kismet/GameplayStatics.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/Quaternion.h"
#include "geometry_msgs/TransformStamped.h"
#include "tf2_msgs/TFMessage.h"

DEFINE_LOG_CATEGORY_STATIC(LogRTFPublisher, Log, All)

URTFPublisher::URTFPublisher()
{
  CommonPublisherParameters.Topic = TEXT("/tf");
  CommonPublisherParameters.MessageType = TEXT("tf2_msgs/TFMessage");
}

void URTFPublisher::SetPublishParameters(URPublisherParameter *&PublisherParameters)
{
  if (URTFPublisherParameter *InTFPublisherParameters = Cast<URTFPublisherParameter>(PublisherParameters))
  {
    Super::SetPublishParameters(PublisherParameters);
    TFPublisherParameters = InTFPublisherParameters->TFPublisherParameters;
  }
}

void URTFPublisher::Init()
{
  Super::Init();
  TArray<AActor *> Actors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), Actors);
  for (const FString &ObjectName : ObjectNames)
  {
    for (AActor *&Actor : Actors)
    {
      if (Actor->GetName().Contains(ObjectName))
      {
        UE_LOG(LogRTFPublisher, Log, TEXT("Add %s to %s"), *ObjectName, *GetName())
        AddObject(Actor);
        break;
      }
    }
    UE_LOG(LogRTFPublisher, Error, TEXT("%s not found"), *ObjectName)
  }
}

void URTFPublisher::AddObject(AActor *&InObject)
{
  if (!Objects.Contains(InObject))
  {
    Objects.Add(InObject);
  }
  else
  {
    UE_LOG(LogRTFPublisher, Error, TEXT("Object %s is already in list"), *InObject->GetName());
  }
}

void URTFPublisher::SetObjects(const TArray<AActor *> &InObjects)
{
  Objects = InObjects;
}

void URTFPublisher::Publish()
{
  if (Objects.Num() > 0)
  {
    static int Seq = 0;
    TSharedPtr<tf2_msgs::TFMessage> TfMessage =
        MakeShareable(new tf2_msgs::TFMessage());

    geometry_msgs::Transform ObjectTransfMsg;
    geometry_msgs::TransformStamped ObjectFrame;
    for (AActor *&Object : Objects)
    {
      ObjectFrame.SetHeader(std_msgs::Header(Seq, FROSTime(), TFPublisherParameters.FrameId));
      ObjectFrame.SetChildFrameId(Object->GetName());

      geometry_msgs::Transform TransfMsgTemp(
          geometry_msgs::Vector3(FConversions::UToROS(Object->GetActorLocation())),
          geometry_msgs::Quaternion(FConversions::UToROS(Object->GetActorQuat())));
      ObjectTransfMsg = TransfMsgTemp;
      ObjectFrame.SetTransform(ObjectTransfMsg);
      TfMessage->AddTransform(ObjectFrame);
    }

    Handler->PublishMsg(CommonPublisherParameters.Topic, TfMessage);
    Handler->Process();

    Seq++;
  }
}
