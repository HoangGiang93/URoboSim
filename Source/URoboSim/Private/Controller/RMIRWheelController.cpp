#include "Controller/RMIRWheelController.h"

URMIRWheelController::URMIRWheelController()
{
  BaseName = TEXT("base_footprint");

  OdomPositionStates.Init(0.0, 3);
  WheelSetting.WheelVelocities.Init(0.0, 5);
  OdomVelocityStates.Init(0.0, 3);
}

void URMIRWheelController::Tick(float InDeltaTime)
{
  MoveLinearTick(InDeltaTime);
  TurnTick(InDeltaTime);
  CalculateOdomStates(InDeltaTime);
  MoveWheelTick(InDeltaTime);
}

void SetAngularVelocityY(UStaticMeshComponent* Link, const double &AngularVelocity)
{
  Link->SetPhysicsAngularVelocityInRadians(Link->GetComponentQuat().GetAxisY() * AngularVelocity);
}

void SetAngularVelocityZ(UStaticMeshComponent* Link, const double &AngularVelocity)
{
  Link->SetPhysicsAngularVelocityInRadians(Link->GetComponentQuat().GetAxisZ() * AngularVelocity);
}

void URMIRWheelController::MoveWheelTick(const float &InDeltaTime)
{
  if (GetOwner()->Links.Contains(WheelSetting.WheelLeft) &&
      GetOwner()->Links.Contains(WheelSetting.WheelRight) &&
      GetOwner()->Links.Contains(WheelSetting.WheelFrontLeftRotation) &&
      GetOwner()->Links.Contains(WheelSetting.WheelFrontLeftTranslation) &&
      GetOwner()->Links.Contains(WheelSetting.WheelFrontRightRotation) &&
      GetOwner()->Links.Contains(WheelSetting.WheelFrontRightTranslation) &&
      GetOwner()->Links.Contains(WheelSetting.WheelBackLeftRotation) &&
      GetOwner()->Links.Contains(WheelSetting.WheelBackLeftTranslation) &&
      GetOwner()->Links.Contains(WheelSetting.WheelBackRightRotation) &&
      GetOwner()->Links.Contains(WheelSetting.WheelBackRightTranslation))
      {
        WheelSetting.WheelVelocities[0] = LinearVelocity.X / (2 * PI * WheelSetting.WheelRadius);
        SetAngularVelocityY(GetOwner()->Links[WheelSetting.WheelLeft], WheelSetting.WheelVelocities[0]);
        SetAngularVelocityY(GetOwner()->Links[WheelSetting.WheelRight], WheelSetting.WheelVelocities[0]);
        SetAngularVelocityY(GetOwner()->Links[WheelSetting.WheelFrontLeftTranslation], FMath::Abs(WheelSetting.WheelVelocities[0]));
        SetAngularVelocityY(GetOwner()->Links[WheelSetting.WheelFrontRightTranslation], FMath::Abs(WheelSetting.WheelVelocities[0]));
        SetAngularVelocityY(GetOwner()->Links[WheelSetting.WheelBackLeftTranslation], FMath::Abs(WheelSetting.WheelVelocities[0]));
        SetAngularVelocityY(GetOwner()->Links[WheelSetting.WheelBackRightTranslation], FMath::Abs(WheelSetting.WheelVelocities[0]));
        float WheelSteeringAngle = GetOwner()->Links[BaseName]->GetComponentRotation().Yaw;
        float DeltaWheelSteeringAngle = 0.0;
        if (LinearVelocity.X > 1E-4)
        {
          DeltaWheelSteeringAngle = FMath::RadiansToDegrees(FMath::Asin((AngularVelocity * WheelSetting.WheelDistanceLength) / (2 * LinearVelocity.X)));
        }
        else if (LinearVelocity.X < -1E-4)
        {
          DeltaWheelSteeringAngle = 180.0 + FMath::RadiansToDegrees(FMath::Asin((AngularVelocity * WheelSetting.WheelDistanceLength) / (2 * LinearVelocity.X)));
        }
        else
        {
          DeltaWheelSteeringAngle = 90.0 * FMath::Sign(AngularVelocity);
        }
        FRotator WheelSteeringAngleFront = GetOwner()->Links[BaseName]->GetComponentRotation().Add(0.f, DeltaWheelSteeringAngle, 0.f);
        FRotator WheelSteeringAngleBack = GetOwner()->Links[BaseName]->GetComponentRotation().Add(0.f, -DeltaWheelSteeringAngle, 0.f);
        WheelSetting.WheelVelocities[1] = 0.1 * ((WheelSteeringAngleFront - GetOwner()->Links[WheelSetting.WheelFrontLeftRotation]->GetComponentRotation()).GetNormalized().Yaw) / InDeltaTime;
        WheelSetting.WheelVelocities[2] = 0.1 * ((WheelSteeringAngleFront - GetOwner()->Links[WheelSetting.WheelFrontRightRotation]->GetComponentRotation()).GetNormalized().Yaw) / InDeltaTime;
        WheelSetting.WheelVelocities[3] = 0.1 * ((WheelSteeringAngleBack - GetOwner()->Links[WheelSetting.WheelBackLeftRotation]->GetComponentRotation()).GetNormalized().Yaw) / InDeltaTime;
        WheelSetting.WheelVelocities[4] = 0.1 * ((WheelSteeringAngleBack - GetOwner()->Links[WheelSetting.WheelBackRightRotation]->GetComponentRotation()).GetNormalized().Yaw) / InDeltaTime;
        SetAngularVelocityZ(GetOwner()->Links[WheelSetting.WheelFrontLeftRotation], WheelSetting.WheelVelocities[1]);
        SetAngularVelocityZ(GetOwner()->Links[WheelSetting.WheelFrontRightRotation], WheelSetting.WheelVelocities[2]);
        SetAngularVelocityZ(GetOwner()->Links[WheelSetting.WheelBackLeftRotation], WheelSetting.WheelVelocities[3]);
        SetAngularVelocityZ(GetOwner()->Links[WheelSetting.WheelBackRightRotation], WheelSetting.WheelVelocities[4]);
      }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("RMIRWheelController can not find wheels in the GetOwner()"))
  }
}
