#include "Controller/ControllerType/BaseController/RMIRWheelAndTrolleyController.h"

DEFINE_LOG_CATEGORY_STATIC(LogRMIRWheelAndTrolleyController, Log, All)

URMIRWheelAndTrolleyController::URMIRWheelAndTrolleyController()
{
  TrolleyName = TEXT("carriage_link");
}

void URMIRWheelAndTrolleyController::SetControllerParameters(URControllerParameter *&ControllerParameters)
{
  URMIRWheelAndTrolleyControllerParameter *MIRWheelAndTrolleyControllerParameters = Cast<URMIRWheelAndTrolleyControllerParameter>(ControllerParameters);
  if (MIRWheelAndTrolleyControllerParameters)
  {
    Super::SetControllerParameters(ControllerParameters);
    TrolleyName = MIRWheelAndTrolleyControllerParameters->TrolleyName;
  }
}

void URMIRWheelAndTrolleyController::Init()
{
  Super::Init();
  FTimerHandle TimerHandle;
  GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, this, &URMIRWheelAndTrolleyController::FixateTrolley, 3.f, false);
}

void URMIRWheelAndTrolleyController::FixateTrolley()
{
  if (BaseControllerParameters.Mode == UBaseControllerMode::Dynamic)
  {
    if (ARModel *Owner = GetOwner())
    {
      if (URLink *TrolleyLink = Owner->GetLink(TrolleyName))
      {
        if (UStaticMeshComponent *TrolleyMesh = TrolleyLink->GetRootMesh())
        {
          UE_LOG(LogRMIRWheelAndTrolleyController, Log, TEXT("%s fixate %s"), *GetName(), *TrolleyMesh->GetName())
          TrolleyMesh->SetConstraintMode(EDOFMode::XYPlane);
        }
      }
    }
  }
}