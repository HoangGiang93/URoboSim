#pragma once

#include "ROSCommunication/Action/Server/RActionStatusPublisher.h"
// clang-format off
#include "FJTAStatusPublisher.generated.h"
// clang-format on

UCLASS()
class UROBOSIM_API URFJTAStatusPublisher final : public URActionStatusPublisher
{
  GENERATED_BODY()

protected:
  void Init() override;
};