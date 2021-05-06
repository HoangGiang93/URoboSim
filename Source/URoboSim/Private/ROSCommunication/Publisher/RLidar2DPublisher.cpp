#include "ROSCommunication/Publisher/RLidar2DPublisher.h"
#include "sensor_msgs/LaserScan.h"

DEFINE_LOG_CATEGORY_STATIC(LogRLidar2DPublisher, Log, All);

URLidar2DPublisher::URLidar2DPublisher()
{
  CommonPublisherParameters.Topic = TEXT("base_scan");
  CommonPublisherParameters.MessageType = TEXT("sensor_msgs/LaserScan");
}

void URLidar2DPublisher::SetPublishParameters(URPublisherParameter *&PublisherParameters)
{
  if (URLidar2DPublisherParameter *InLidarPublisherParameters = Cast<URLidar2DPublisherParameter>(PublisherParameters))
  {
    Super::SetPublishParameters(PublisherParameters);
    Lidar2DPublisherParameters = InLidarPublisherParameters->Lidar2DPublisherParameters;
  }
}

void URLidar2DPublisher::Init()
{
  Super::Init();
  if (!Lidar)
  {
    SetLidar();
  }
}

void URLidar2DPublisher::Publish()
{
  if (Lidar && Lidar->bPublishResult)
  {
    static int Seq = 0;
    static TSharedPtr<sensor_msgs::LaserScan> ScanData = MakeShareable(new sensor_msgs::LaserScan());

    ScanData->SetHeader(std_msgs::Header(Seq++, FROSTime(), Lidar2DPublisherParameters.LidarReferenceROSLinkName));
    ScanData->SetAngleMin(Lidar->ScanAngleMin);
    ScanData->SetAngleMax(Lidar->ScanAngleMax);
    ScanData->SetAngleIncrement(Lidar->AngularIncrement);
    ScanData->SetRangeMin(Lidar->MinimumDistance / 100.);
    ScanData->SetRangeMax(Lidar->MaximumDistance / 100.);
    ScanData->SetTimeIncrement(Lidar->TimeIncrement);
    ScanData->SetScanTime(Lidar->ScanTime);

    TArray<float> Measurements = Lidar->GetMeasuredData();
    ScanData->SetRanges(Measurements);

    Handler->PublishMsg(CommonPublisherParameters.Topic, ScanData);

    Handler->Process();

    Lidar->bPublishResult = false;
  }
}

void URLidar2DPublisher::SetLidar()
{
  if (GetOwner())
  {
    Lidar = Cast<URLidar2D>(GetOwner()->GetSensor(Lidar2DPublisherParameters.LidarName));
    if (!Lidar)
    {
      UE_LOG(LogRLidar2DPublisher, Error, TEXT("%s not found in %s"), *Lidar2DPublisherParameters.LidarName, *GetOwner()->GetName())
    }
  }
}