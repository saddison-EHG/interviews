#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameActor.generated.h"

UCLASS()
class CODEREVIEW_API GameActor : public AActor
{
	std::function<void(GameActor*)> ActorDied;
	std::function<void(GameActor*)> ActorHealthChanged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FActorsDelegate, TArray<GameActor*>);
	static FActorsDelegate ActorAdded;

	DECLARE_MULTICAST_DELEGATE(FTest);
	FTest Test;
	
public:
	static TArray<GameActor*> AllActors;

	UPROPERTY(EditAnywhere)
    int StartingHealth;

private:
	UPROPERTY(EditAnywhere, Replicated)
	int _currentHealth;
	
 	bool _isDead;

public:	
	GameActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void TakeDamage(int damage);

	UFUNCTION(Server)
	void RecordHealthToServer(int damage);

	UFUNCTION(Client)
	void RecordHealthToClient(int damage);
	
	UFUNCTION(Server)
	void RecordDeathToServer();
};
