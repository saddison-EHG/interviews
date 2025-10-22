#include "GameActor.h"

GameActor::GameActor()
{
	StartingHealth = 100;
}

void GameActor::BeginPlay()
{
	AllActors.Add(this);
	ActorAdded(AllActors);
	
	_currentHealth = StartingHealth;
	Super::BeginPlay();
}

void GameActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void GameActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void GameActor::TakeDamage(int damage)
{
	_currentHealth -= damage;
	ActorHealthChanged(this);
	if (HasAuthority())
	{
		RecordHealthToServer(damage);
	}
	else
	{
		RecordHealthToClient(damage);
	}
	
	if (_currentHealth <= 0)
	{
		_isDead = true;
		ActorDied(this);
		RecordDeathToServer();
	}
}

void GameActor::RecordHealthToServer_Implementation(int damage)
{
	TakeDamage(damage);
}

void GameActor::RecordHealthToClient_Implementation(int damage)
{
	TakeDamage(damage);
}

void GameActor::RecordDeathToServer_Implementation()
{
	_isDead = true;
}


