// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/World.h"

APickup::APickup()
{
	//CoinCount = 1;
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	//UE_LOG(LogTemp, Warning, TEXT("APickup::OnOverlap_Begin"));

	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			//Main->IncrementCoin(CoinCount);
			OnPickupBP(Main);

			/** �̷��� �ϸ� �ӽ�Vector���� �Ѱ� �ְ����� TArray�� ����Add�Լ��� �ӽ÷� ���� �� �ִ� const FVector reference�� �ޱ⶧���� �������.
				��ư �̷��� pickups�� �������� ��ġ���� PickupLocation�̶�� TArray�� �־����. */
			Main->PickupLocations.Add(GetActorLocation());

			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}

			Destroy();
		}
	}
}
void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	//UE_LOG(LogTemp, Warning, TEXT("APickup::OnOverlap_End"));
} 
