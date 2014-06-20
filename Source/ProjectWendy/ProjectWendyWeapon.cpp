

#include "ProjectWendy.h"
#include "ProjectWendyWeapon.h"


AProjectWendyWeapon::AProjectWendyWeapon(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	MeshPlayer = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMeshPlayer"));
	MeshPlayer->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	MeshPlayer->bChartDistanceFactor = false;
	MeshPlayer->bReceivesDecals = false;
	MeshPlayer->CastShadow = false;
	MeshPlayer->SetCollisionObjectType(ECC_WorldDynamic);
	MeshPlayer->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshPlayer->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = MeshPlayer;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(10.0f, 0.0f, 10.0f);

	bIsEquipped = false;
	bWantsToFire = false;
	CurrentState = EWeaponState::Idle;

	LastFireTime = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	// SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
}

void AProjectWendyWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	DetachMeshFromPawn();
}

void AProjectWendyWeapon::Destroyed()
{
	Super::Destroyed();

	//StopSimulatingWeaponFire();
}

//////////////////////////////////////////////////////////////////////////
// Inventory

void AProjectWendyWeapon::OnEquip()
{
	AttachMeshToPawn();

	DetermineWeaponState();

	EquipStartedTime = GetWorld()->GetTimeSeconds();
	EquipDuration = 3.0f;

	GetWorldTimerManager().SetTimer(this, &AProjectWendyWeapon::OnEquipFinished, EquipDuration, false);
}

void AProjectWendyWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;

	DetermineWeaponState();
}

void AProjectWendyWeapon::OnUnEquip()
{
	DetachMeshFromPawn();
	bIsEquipped = false;
	StopFire();

	//if (bPendingEquip)
	//{
	//	bPendingEquip = false;

	//	GetWorldTimerManager().ClearTimer(this, &AProjectWendyWeapon::OnEquipFinished);
	//}

	DetermineWeaponState();
}

void AProjectWendyWeapon::OnEnterInventory(AProjectWendyCharacter *NewOwner) {
	SetOwningPawn(NewOwner);

	// Additional logic
}

void AProjectWendyWeapon::AttachMeshToPawn()
{
	// TODO: Add attach point to mesh
	if (MyPawn)
	{
		// Remove and hide mesh
		DetachMeshFromPawn();

		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		//if (MyPawn->IsLocallyControlled() == true)
		{
			//USkeletalMeshComponent* PawnMeshPlayer = MyPawn->GetPawnMesh();
			//MeshPlayer->SetHiddenInGame(false);
			//MeshPlayer->AttachTo(PawnMeshPlayer, AttachPoint);
		}
		//else
		//{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachTo(UsePawnMesh, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		//}
	}
}

void AProjectWendyWeapon::DetachMeshFromPawn()
{
	// TODO: Add attach point to mesh
	//MeshPlayer->DetachFromParent();
	//MeshPlayer->SetHiddenInGame(true);
}


//////////////////////////////////////////////////////////////////////////
// Input

void AProjectWendyWeapon::StartFire()
{
	if (Role < ROLE_Authority)
	{
		// Fire the projectile
		// ServerStartFire();
		//FireWeapon();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		FireWeapon();
		//DetermineWeaponState();
	}
}

void AProjectWendyWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		// Stop firing weapon
		// ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

void AProjectWendyWeapon::FireWeapon() {

	UE_LOG(LogClass, Log, TEXT("AProjectWendyWeapon::FireWeapon()"));

	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = GetActorRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Control

bool AProjectWendyWeapon::CanFire() const
{
	//bool bCanFire = MyPawn && MyPawn->CanFire();
	bool bCanFire = true;
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanFire == true) && (bStateOKToFire == true));
}


//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AProjectWendyWeapon::HandleFiring()
{
	if (CanFire())
	{

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();
		}
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client will notify server
		if (Role < ROLE_Authority)
		{
			// ServerHandleFiring();
		}

		// setup refire timer
		bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(this, &AProjectWendyWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void AProjectWendyWeapon::SetWeaponState(EWeaponState::Type NewState)
{
	CurrentState = NewState;
}

void AProjectWendyWeapon::DetermineWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (CanFire() == true)
		{
			NewState = EWeaponState::Firing;
		}
	}

	SetWeaponState(NewState);
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage helpers

FVector AProjectWendyWeapon::GetAdjustedAim() const
{
	AProjectWendyPlayerController* const PlayerController = Instigator ? Cast<AProjectWendyPlayerController>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;
	// If we have a player controller use it for the aim
	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (Instigator)
	{
		//// Now see if we have an AI controller - we will want to get the aim from there if we do
		//AShooterAIController* AIController = MyPawn ? Cast<AShooterAIController>(MyPawn->Controller) : NULL;
		//if (AIController != NULL)
		//{
		//	FinalAim = AIController->GetControlRotation().Vector();
		//}
		//else
		//{
		//	FinalAim = Instigator->GetBaseAimRotation().Vector();
		//}
		FinalAim = Instigator->GetBaseAimRotation().Vector();
	}

	return FinalAim;
}

FHitResult AProjectWendyWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}

void AProjectWendyWeapon::SetOwningPawn(AProjectWendyCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}
}

USkeletalMeshComponent* AProjectWendyWeapon::GetWeaponMesh() const
{
	return MeshPlayer;
}

class AProjectWendyCharacter* AProjectWendyWeapon::GetPawnOwner() const
{
	return MyPawn;
}

bool AProjectWendyWeapon::IsEquipped() const
{
	return bIsEquipped;
}

bool AProjectWendyWeapon::IsAttachedToPawn() const
{
	return bIsEquipped;
}

EWeaponState::Type AProjectWendyWeapon::GetCurrentState() const
{
	return CurrentState;
}

float AProjectWendyWeapon::GetEquipStartedTime() const
{
	return EquipStartedTime;
}

float AProjectWendyWeapon::GetEquipDuration() const
{
	return EquipDuration;
}



