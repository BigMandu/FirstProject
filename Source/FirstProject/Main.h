// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

//UENUM��ũ��, UFUNCTION, UPROPERTY��ũ�ο� ���� ������ ��.
UENUM(BlueprintType)
enum class EMovementStatus : uint8 //EmovementStatus��� �ҰŰ�, uint8�κ��� �Ļ���Ŵ.
{
	/** EMS�� EMovewmentStatus�� �����̴�. ��� UE������ �̷��� �� ����
		enum�� ����ϰ�, UENUM���� ��ŷ�ϴ� ��쿡�� enum��  UMETA�� ��ŷ�� ������Ѵ�.
	*/
	EMS_Normal UMETA(DisplayName = "Normal"), //UMETA�� DisplayName�̶�� �μ��� ����ϰ� �̸��� �ٿ��ش�.(Blueprint reflection system�� ���Ѱ���)
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),
	
	/** �Ʒ��� �ؾ��ϴ°� �ƴ����� ���� �ϴ°�����, enum����� �������� �ϳ��� �� �߰��ϴµ�
		�̰��� �۾�����, ������� enum constants�� ���� �ִ밪�� ��Ÿ����. ( enum constants ��밳���� ���ϴµ�.)
		������ ��������� �ʴ´�.
	*/
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

//Sprinting Status�� ���� enum
UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;
	
	//������ Pointer�� �����ϴµ� ���⼭�� FVector�� �����ߴ�.
	//FVector type�� TArray�� ���������, FVector�� �����ϴ� Array��.
	TArray<FVector> PickupLocations;

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminiaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	//Stamina�� ������ �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;
	//Stamina �ּ� ���� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	//ĳ������ ������ ������ ���ϰ� ����. (BP�� reflection���� ����, ���߿� �ʿ��ϸ� ��ũ�� �߰���)
	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Enemy) { CombatTarget = Enemy; }

	//Target�� ��ġ�� ������ Yaw�� ���ϴ� �Լ���.
	FRotator GetLookatRotationYaw(FVector Target);

	// Set Movement status and running speed
	//FORFCEINLINE�� ������� ���� ������ Movement Status�� �ٲٰ�, Running Speed�� �����Ұű� �����̴�. 
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;
	// Sprint Ȱ��ȭ
	void ShiftKeyDown();
	// Sprint ��Ȱ��ȭ
	void ShiftKeyUp();


	/* ī�޶� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/* ī�޶�	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// ī�޶� ȸ���� �����ϱ� ���� �⺻ ȸ�� �ӵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/***************************/
	// Player Stat
	/***************************/
	//�ۼ��������� ��Ÿ�������� MAX�� ���ε״�.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	//UnrealEngine������ �������� intŸ���� int32(32��Ʈ ����)�̴�., �����߿� � �÷����� ����ϵ��� ������°� ������(ũ�ν��÷���).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	// HP�� ���ҽ�Ű�� �Լ�, Explosive���� ���.
	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	//Coin�� ������Ű�� �Լ�, Pickup���� ���.
	UFUNCTION(BlueprintCallable)
	void IncrementCoin(int32 Amount);
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	//HP�� 0�̵Ǹ� �װ���.
	void Die();

	virtual void Jump() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ������, �ڷ�
	void MoveForward(float Value);
	//������, ����
	void MoveRight(float Value);

	//Yaw Rotation
	void Turn(float Value);
	//Pitch Rotation
	void LookUp(float Value);

	bool bMovingForward;
	bool bMovingRight;

	bool CanMove(float Value);

	/** ȸ�� �ӵ� 
	* @param Rate ����ȭ�� �����̴�. ��, 1.0 ���� ���ϴ� ȸ������ 100%�� �ǹ���.
	*/
	void TurnAtRate(float Rate);
	/** ��/�Ʒ� �ӵ� 
	* @param Rate ����ȭ�� �����̴�. ��, 1.0 ���� ���ϴ� ȸ������ 100%�� �ǹ���.
	*/
	void LookUpAtRate(float Rate);

	bool bLMBDown;
	void LMBDown();
	void LMBUp();
	
	bool bEKeyDown;
	void EkeyDown();
	void EkeyUp();
	
	bool bESCDown;
	void ESCDown();
	void ESCUp();
	

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	//EditDefaultsOnly = ĳ���� Instance���� ������ �� ������ �ϱ� �����̴�.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	//FORCEINLINE void SetEquippedWeapon(AWeapon* WeaponToset) { EquippedWeapon = WeaponToset; }
	void SetEquippedWeapon(AWeapon* WeaponToset); //Equipped�� ��� Destory�ϱ� ���� FORCEINLINE��� ���� ������.

	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* ItemToset) { ActiveOverlappingItem = ItemToset; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	//Montage�� ����ϱ� ���ؼ� UAnimMontage class�� �־���Ѵ�.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool bSetPosition);

	void LoadGameNoSwitch();
};
