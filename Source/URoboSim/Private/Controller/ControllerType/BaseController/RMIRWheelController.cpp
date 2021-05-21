#include "Controller/ControllerType/BaseController/RMIRWheelController.h"

DEFINE_LOG_CATEGORY_STATIC(LogRMIRWheelController, Log, All)

URMIRWheelController::URMIRWheelController()
{
  WheelVelocities.Init(0.0, 5);
}

void URMIRWheelController::SetControllerParameters(URControllerParameter *&ControllerParameters)
{
  URMIRWheelControllerParameter *MIRWheelControllerParameters = Cast<URMIRWheelControllerParameter>(ControllerParameters);
  if (MIRWheelControllerParameters)
  {
    Super::SetControllerParameters(ControllerParameters);
    WheelSetting = MIRWheelControllerParameters->WheelSetting;
  }
}

void URMIRWheelController::Tick(const float &InDeltaTime)
{
  Super::Tick(InDeltaTime);
  MoveWheelTick(InDeltaTime);
}

void SetAngularVelocityY(UStaticMeshComponent *Link, const double &AngularVelocity)
{
  Link->SetPhysicsAngularVelocityInRadians(Link->GetComponentQuat().GetAxisY() * AngularVelocity);
}

void SetAngularVelocityZ(UStaticMeshComponent *Link, const double &AngularVelocity)
{
  Link->SetPhysicsAngularVelocityInRadians(Link->GetComponentQuat().GetAxisZ() * AngularVelocity);
}

void URMIRWheelController::MoveWheelTick(const float &InDeltaTime)
{
  if (GetOwner() && BaseMesh)
  {
    if (GetOwner()->GetLink(WheelSetting.WheelLeft) &&
        GetOwner()->GetLink(WheelSetting.WheelRight) &&
        GetOwner()->GetLink(WheelSetting.WheelFrontLeftRotation) &&
        GetOwner()->GetLink(WheelSetting.WheelFrontLeftTranslation) &&
        GetOwner()->GetLink(WheelSetting.WheelFrontRightRotation) &&
        GetOwner()->GetLink(WheelSetting.WheelFrontRightTranslation) &&
        GetOwner()->GetLink(WheelSetting.WheelBackLeftRotation) &&
        GetOwner()->GetLink(WheelSetting.WheelBackLeftTranslation) &&
        GetOwner()->GetLink(WheelSetting.WheelBackRightRotation) &&
        GetOwner()->GetLink(WheelSetting.WheelBackRightTranslation))
    {
      WheelVelocities[0] = LinearVelocity.X / (2 * PI * WheelSetting.WheelRadius);
      SetAngularVelocityY(GetOwner()->GetLink(WheelSetting.WheelLeft)->GetRootMesh(), WheelVelocities[0]);
      SetAngularVelocityY(GetOwner()->GetLink(WheelSetting.WheelRight)->GetRootMesh(), WheelVelocities[0]);
      SetAngularVelocityY(GetOwner()->GetLink(WheelSetting.WheelFrontLeftTranslation)->GetRootMesh(), FMath::Abs(WheelVelocities[0]));
      SetAngularVelocityY(GetOwner()->GetLink(WheelSetting.WheelFrontRightTranslation)->GetRootMesh(), FMath::Abs(WheelVelocities[0]));
      SetAngularVelocityY(GetOwner()->GetLink(WheelSetting.WheelBackLeftTranslation)->GetRootMesh(), FMath::Abs(WheelVelocities[0]));
      SetAngularVelocityY(GetOwner()->GetLink(WheelSetting.WheelBackRightTranslation)->GetRootMesh(), FMath::Abs(WheelVelocities[0]));
      float WheelSteeringAngle = BaseMesh->GetComponentRotation().Yaw;
      float DeltaWheelSteeringAngle = 0.0;
      if (LinearVelocity.X > 1E-4)
      {
        DeltaWheelSteeringAngle = FMath::RadiansToDegrees(FMath::Asin((FMath::DegreesToRadians(AngularVelocity)  * WheelSetting.WheelDistanceLength) / (2 * LinearVelocity.X)));
      }
      else if (LinearVelocity.X < -1E-4)
      {
        DeltaWheelSteeringAngle = 180.0 + FMath::RadiansToDegrees(FMath::Asin((FMath::DegreesToRadians(AngularVelocity) * WheelSetting.WheelDistanceLength) / (2 * LinearVelocity.X)));
      }
      else
      {
        DeltaWheelSteeringAngle = 90.0 * FMath::Sign(AngularVelocity);
      }
      FRotator WheelSteeringAngleFront = BaseMesh->GetComponentRotation().Add(0.f, DeltaWheelSteeringAngle, 0.f);
      FRotator WheelSteeringAngleBack = BaseMesh->GetComponentRotation().Add(0.f, -DeltaWheelSteeringAngle, 0.f);
      WheelVelocities[1] = 0.1 * ((WheelSteeringAngleFront - GetOwner()->GetLink(WheelSetting.WheelFrontLeftRotation)->GetRootMesh()->GetComponentRotation()).GetNormalized().Yaw) / InDeltaTime;
      WheelVelocities[2] = 0.1 * ((WheelSteeringAngleFront - GetOwner()->GetLink(WheelSetting.WheelFrontRightRotation)->GetRootMesh()->GetComponentRotation()).GetNormalized().Yaw) / InDeltaTime;
      WheelVelocities[3] = 0.1 * ((WheelSteeringAngleBack - GetOwner()->GetLink(WheelSetting.WheelBackLeftRotation)->GetRootMesh()->GetComponentRotation()).GetNormalized().Yaw) / InDeltaTime;
      WheelVelocities[4] = 0.1 * ((WheelSteeringAngleBack - GetOwner()->GetLink(WheelSetting.WheelBackRightRotation)->GetRootMesh()->GetComponentRotation()).GetNormalized().Yaw) / InDeltaTime;
      SetAngularVelocityZ(GetOwner()->GetLink(WheelSetting.WheelFrontLeftRotation)->GetRootMesh(), WheelVelocities[1]);
      SetAngularVelocityZ(GetOwner()->GetLink(WheelSetting.WheelFrontRightRotation)->GetRootMesh(), WheelVelocities[2]);
      SetAngularVelocityZ(GetOwner()->GetLink(WheelSetting.WheelBackLeftRotation)->GetRootMesh(), WheelVelocities[3]);
      SetAngularVelocityZ(GetOwner()->GetLink(WheelSetting.WheelBackRightRotation)->GetRootMesh(), WheelVelocities[4]);
    }
    else
    {
      UE_LOG(LogRMIRWheelController, Error, TEXT("Wheels in %s not found"), *GetOwner()->GetName())
    }
  }
}
