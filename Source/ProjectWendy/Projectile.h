#pragma once

#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

/**
 * Projectile
 * Controls the behavior of a basic projectile
 */
UCLASS()
class AProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

	/* Object collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	TSubobjectPtr<USphereComponent> CollisionComp;

	/* Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement)
	TSubobjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	/* Called when the projectile hits another object */
	UFUNCTION()
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
