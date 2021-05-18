#include "Controller/ControllerType/BaseController/RBaseController.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogRBaseController, Log, All)

void URBaseController::SetControllerParameters(URControllerParameter *&ControllerParameters)
{
  URBaseControllerParameter *InBaseControllerParameters = Cast<URBaseControllerParameter>(ControllerParameters);
  if (InBaseControllerParameters)
  {
    BaseControllerParameters = InBaseControllerParameters->BaseControllerParameters;
  }
}

void URBaseController::Init()
{
  Super::Init();

  if (GetOwner())
  {
    URLink *BaseLink = GetOwner()->GetBaseLink();
    if (BaseLink)
    {
      BaseMesh = BaseLink->GetRootMesh();
      if (BaseMesh)
      {
        switch (BaseControllerParameters.Mode)
        {
        case UBaseControllerMode::Dynamic:
          BaseLink->bEnablePhysics = true;
          BaseMesh->SetConstraintMode(EDOFMode::XYPlane);
          break;
        
        case UBaseControllerMode::Kinematic:
          BaseLink->bEnablePhysics = false;
          break;
        }
        TargetPose = BaseMesh->GetComponentTransform();
      }
    }
  }
  else
  {
    UE_LOG(LogRBaseController, Error, TEXT("%s is not attached to ARModel"), *GetName())
  }
}

void URBaseController::MoveLinear(const FVector &InVelocity)
{
  LinearVelocity = InVelocity;
}

void URBaseController::MoveAngular(const float &InVelocity)
{
  AngularVelocity = InVelocity;
}

void URBaseController::Tick(const float &DeltaTime)
{
  if (BaseMesh)
  {
    BasePose = BaseMesh->GetComponentTransform();
    MoveLinearTick(DeltaTime);
    MoveAngularTick(DeltaTime);
  }
}

void URBaseController::MoveLinearTick(const float &DeltaTime)
{
  // Check if AngularVelocity is 0 to avoid division by 0
  if (FMath::Abs(AngularVelocity) > 1E-6f)
  {
    // Calculate the resulting position after one tick by using the Integral of Rx (R = rotation matrix, x = Position https://en.wikipedia.org/wiki/Rotation_matrix)
    // in the interval of 0 to DeltaTime
    float Theta0 = FMath::DegreesToRadians(BasePose.GetRotation().Rotator().Yaw);
    float Theta1 = Theta0 + AngularVelocity * DeltaTime;
    float VelX = (FMath::Sin(Theta1) * LinearVelocity.X + FMath::Cos(Theta1) * LinearVelocity.Y) / AngularVelocity - (FMath::Sin(Theta0) * LinearVelocity.X + FMath::Cos(Theta0) * LinearVelocity.Y) / AngularVelocity;
    float VelY = (-FMath::Cos(Theta1) * LinearVelocity.X + FMath::Sin(Theta1) * LinearVelocity.Y) / AngularVelocity - (-FMath::Cos(Theta0) * LinearVelocity.X + FMath::Sin(Theta0) * LinearVelocity.Y) / AngularVelocity;
    TargetPose.AddToTranslation(FVector(VelX, VelY, 0.f));
  }
  else
  {
    // if the AngularVelocity == 0, rotate the LinearVelocity from Base to World frame
    FVector VelocityInBaseFrame = BasePose.GetRotation().RotateVector(LinearVelocity);
    TargetPose.AddToTranslation(VelocityInBaseFrame * DeltaTime);
  }
  
  switch (BaseControllerParameters.Mode)
  {
  case UBaseControllerMode::Dynamic:
    // Calculate velocity in order to move from current position to the target position
    BaseMesh->SetPhysicsLinearVelocity((TargetPose.GetLocation() - BasePose.GetLocation()) / DeltaTime);
    break;
  
  case UBaseControllerMode::Kinematic:
    BaseMesh->AddWorldOffset(TargetPose.GetLocation() - BasePose.GetLocation());
    break;
  }
}

void URBaseController::MoveAngularTick(const float &DeltaTime)
{
  FRotator DeltaRotation = UKismetMathLibrary::RotatorFromAxisAndAngle(BasePose.GetRotation().GetAxisZ(), AngularVelocity * DeltaTime);
  TargetPose.ConcatenateRotation(DeltaRotation.Quaternion());
  float TargetAngle = FRotator::NormalizeAxis(TargetPose.GetRotation().Rotator().Yaw);
  float CurrentAngle = FRotator::NormalizeAxis(BasePose.GetRotation().Rotator().Yaw);

  switch (BaseControllerParameters.Mode)
  {
  case UBaseControllerMode::Dynamic:
    // Calculate velocity in order to move from current position to the target position
    BaseMesh->SetPhysicsAngularVelocityInDegrees(FVector::UpVector * FRotator::NormalizeAxis(TargetAngle - CurrentAngle) / DeltaTime);
    break;
  
  case UBaseControllerMode::Kinematic:
    BaseMesh->AddWorldRotation(FRotator(0.f, FRotator::NormalizeAxis(TargetAngle - CurrentAngle), 0.f));
    break;
  }
  
}