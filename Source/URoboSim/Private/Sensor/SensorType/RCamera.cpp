#include "Sensor/SensorType/RCamera.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogRCamera, Log, All);

URCamera::URCamera()
{

}

void URCamera::Init()
{
  Super::Init();

  TArray<UStaticMeshComponent *> ActorComponents;
  if (GetOwner())
  {
    GetOwner()->GetComponents(ActorComponents);

    for (UStaticMeshComponent *&ActorComponent : ActorComponents)
    {
      if (ActorComponent->GetName().Equals(CameraParameters.ReferenceLinkName))
      {
        ReferenceLink = ActorComponent;
        break;
      }
    }
    if (ReferenceLink)
    {
      TArray<AActor *> Actors;
      UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARGBDCamera::StaticClass(), Actors);
      for (AActor *&Actor : Actors)
      {
        UE_LOG(LogRCamera, Log, TEXT("Found Camera %s"), *Actor->GetName())
        if (Actor->GetName().Equals(CameraParameters.CameraName))
        {
          Actor->AttachToComponent(ReferenceLink, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
          Actor->AddActorLocalOffset(CameraParameters.CameraPoseOffset.GetLocation());
          Actor->AddActorLocalRotation(CameraParameters.CameraPoseOffset.GetRotation());
          return;
        }
      }
      UE_LOG(LogRCamera, Error, TEXT("%s of %s not found"), *CameraParameters.CameraName, *GetOwner()->GetName())
    }
    else
    {
      UE_LOG(LogRCamera, Error, TEXT("%s of %s not found"), *CameraParameters.ReferenceLinkName, *GetOwner()->GetName());
    }
  }
  else
  {
    UE_LOG(LogRCamera, Error, TEXT("Owner of %s not found"), *GetName());
  }
}

void URCamera::SetSensorParameters(URSensorParameter *&SensorParameters)
{
  URCameraParameter *InCameraParameters = Cast<URCameraParameter>(SensorParameters);
  if (InCameraParameters)
  {
    Super::SetSensorParameters(SensorParameters);
    CameraParameters = InCameraParameters->CameraParameters;
  }
}

void URCamera::Tick(const float &InDeltaTime)
{
}