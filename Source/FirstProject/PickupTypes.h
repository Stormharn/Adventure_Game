#pragma once
UENUM(BlueprintType)
enum class EPickupTypes : uint8
{
    Coins           UMETA(DisplayName = "Coins"),
    HealthPotion    UMETA(DisplayName = "HP Potion"),

    MAX             UMETA(DisplayName = "DefaultMAX")
};