// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

//UENUM매크로, UFUNCTION, UPROPERTY매크로와 같은 역할을 함.
UENUM(BlueprintType)
enum class EMovementStatus : uint8 //EmovementStatus라고 할거고, uint8로부터 파생시킴.
{
	/** EMS는 EMovewmentStatus의 약자이다. 대게 UE에서는 이렇게 약어를 지음
		enum을 사용하고, UENUM으로 마킹하는 경우에는 enum도  UMETA로 마킹을 해줘야한다.
	*/
	EMS_Normal UMETA(DisplayName = "Normal"), //UMETA는 DisplayName이라는 인수를 사용하고 이름을 붙여준다.(Blueprint reflection system을 위한거임)
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),
	
	/** 아래는 해야하는건 아니지만 많이 하는것으로, enum상수의 마지막에 하나를 더 추가하는데
		이것은 작업중인, 사용중인 enum constants에 대한 최대값을 나타낸다. ( enum constants 사용개수를 말하는듯.)
		실제로 사용하지는 않는다.
	*/
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

//Sprinting Status에 대한 enum
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
	
	//보통은 Pointer를 저장하는데 여기서는 FVector를 저장했다.
	//FVector type의 TArray가 만들어졌고, FVector를 저장하는 Array임.
	TArray<FVector> PickupLocations;

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminiaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	//Stamina가 빠지는 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;
	//Stamina 최소 기준 선
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	//캐릭터의 방향을 적으로 향하게 보간. (BP에 reflection하지 않음, 나중에 필요하면 매크로 추가함)
	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Enemy) { CombatTarget = Enemy; }

	//Target의 위치를 가지고 Yaw를 구하는 함수임.
	FRotator GetLookatRotationYaw(FVector Target);

	// Set Movement status and running speed
	//FORFCEINLINE을 사용하지 않은 이유는 Movement Status를 바꾸고, Running Speed를 변경할거기 때문이다. 
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;
	// Sprint 활성화
	void ShiftKeyDown();
	// Sprint 비활성화
	void ShiftKeyUp();


	/* 카메라 막대 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/* 카메라	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// 카메라 회전을 조절하기 위한 기본 회전 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/***************************/
	// Player Stat
	/***************************/
	//퍼센테이지를 나타내기위해 MAX를 따로뒀다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	//UnrealEngine에서의 전형적인 int타입은 int32(32비트 정수)이다., 개발중에 어떤 플랫폼을 사용하든지 상관없는게 장점임(크로스플랫폼).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	// HP를 감소시키는 함수, Explosive에서 사용.
	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	//Coin을 증가시키는 함수, Pickup에서 사용.
	UFUNCTION(BlueprintCallable)
	void IncrementCoin(int32 Amount);
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	//HP가 0이되면 죽게함.
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

	// 앞으로, 뒤로
	void MoveForward(float Value);
	//오른쪽, 왼쪽
	void MoveRight(float Value);

	//Yaw Rotation
	void Turn(float Value);
	//Pitch Rotation
	void LookUp(float Value);

	bool bMovingForward;
	bool bMovingRight;

	bool CanMove(float Value);

	/** 회전 속도 
	* @param Rate 정규화된 비율이다. 즉, 1.0 값은 원하는 회전율의 100%를 의미함.
	*/
	void TurnAtRate(float Rate);
	/** 위/아래 속도 
	* @param Rate 정규화된 비율이다. 즉, 1.0 값은 원하는 회전율의 100%를 의미함.
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


	//EditDefaultsOnly = 캐릭터 Instance마다 설정할 수 없도록 하기 위함이다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	//FORCEINLINE void SetEquippedWeapon(AWeapon* WeaponToset) { EquippedWeapon = WeaponToset; }
	void SetEquippedWeapon(AWeapon* WeaponToset); //Equipped된 장비를 Destory하기 위해 FORCEINLINE대신 직접 구현함.

	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* ItemToset) { ActiveOverlappingItem = ItemToset; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	//Montage를 사용하기 위해서 UAnimMontage class가 있어야한다.
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
