#include "Sensor/SensorType/RLidar2D.h"

DEFINE_LOG_CATEGORY_STATIC(LogRLidar2D, Log, All);

URLidar2D::URLidar2D()
{

}

void URLidar2D::Init()
{
  Super::Init();
  ScanTime = 1.f / Lidar2DParameters.UpdateRate;
  TimeIncrement = ScanTime / Lidar2DParameters.SampleNumber;
  AngularIncrement = FMath::Abs((Lidar2DParameters.ScanAngleMax - Lidar2DParameters.ScanAngleMin) / Lidar2DParameters.SampleNumber);
  SCSResolution = FMath::CeilToInt(360.f / FMath::RadiansToDegrees(AngularIncrement));
  MeasuredDistance.AddZeroed(SCSResolution);
  TArray<UStaticMeshComponent *> ActorComponents;

  if (GetOwner())
  {
    GetOwner()->GetComponents(ActorComponents);

    for (UStaticMeshComponent *&ActorComponent : ActorComponents)
    {
      if (ActorComponent->GetName().Equals(Lidar2DParameters.ReferenceLinkName))
      {
        ReferenceLink = ActorComponent;
        break;
      }
    }
    if (!ReferenceLink)
    {
      UE_LOG(LogRLidar2D, Error, TEXT("%s of %s not found"), *Lidar2DParameters.ReferenceLinkName, *GetOwner()->GetName());
    }
  }
}

void URLidar2D::SetSensorParameters(URSensorParameter *&SensorParameters)
{
  URLidar2DParameter *InLidar2DParameters = Cast<URLidar2DParameter>(SensorParameters);
  if (InLidar2DParameters)
  {
    Lidar2DParameters = InLidar2DParameters->Lidar2DParameters;
  }
}

void URLidar2D::Tick(const float &InDeltaTime)
{
  static float PassedTime = 0.f;
  PassedTime += InDeltaTime;
  if (PassedTime < ScanTime)
  {
    return;
  }
  else
  {
    PassedTime = 0.f;
    FVector LidarBodyLocation;
    FRotator LidarBodyRotation;
    if (ReferenceLink)
    {
      LidarBodyLocation = ReferenceLink->GetComponentLocation() + Lidar2DParameters.LidarBodyOffset;
      LidarBodyRotation = ReferenceLink->GetComponentRotation();
    }

    for (uint32 i = 0; i < SCSResolution; i++)
    {
      float Angle = i * FMath::RadiansToDegrees(AngularIncrement);
      FRotator ResultRot(0, 180 - Angle, 0);
      FVector EndTrace = Lidar2DParameters.MaximumDistance * LidarBodyRotation.RotateVector(ResultRot.Vector()) + LidarBodyLocation;
      FCollisionQueryParams TraceParams = FCollisionQueryParams(TEXT("TraceLaser"), true, GetOwner());
      TraceParams.bTraceComplex = true;
      TraceParams.bReturnPhysicalMaterial = false;

      FHitResult HitInfo(EForceInit::ForceInit);
      GetWorld()->LineTraceSingleByChannel(
          HitInfo,
          LidarBodyLocation,
          EndTrace,
          ECollisionChannel::ECC_MAX,
          TraceParams,
          FCollisionResponseParams::DefaultResponseParam);
      MeasuredDistance[i] = HitInfo.Distance / 100.;
    }
    bPublishResult = true;
  }
}

TArray<float> URLidar2D::GetMeasuredData() const
{
  TArray<float> MeasuredData;
  int Index = SCSResolution / 2 + Lidar2DParameters.ScanAngleMin / AngularIncrement;
  MeasuredData.Append(&MeasuredDistance[Index], Lidar2DParameters.SampleNumber);

  return MeasuredData;
}