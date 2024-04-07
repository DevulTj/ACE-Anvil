// An override of any health consumable.
// in ACE, we'll use this to spawn some loose items around the player (bloody bandages.. etc)
[BaseContainerProps()]
modded class SCR_ConsumableEffectHealthItems : SCR_ConsumableEffectBase
{	
	// Which resource should we spawn?
	[Attribute(params: "et")]
	protected ResourceName m_LooseItemResource;
	
	// How close to the player can the loose resource spawn?
	[Attribute(defvalue: "0.25", uiwidget: UIWidgets.Slider, desc: "Minimum spawn radius", "0.01 1")]
	protected float m_minSpawnRadius;
	
	// How far away from the player can the loose resource spawn?
	[Attribute(defvalue: "0.3", uiwidget: UIWidgets.Slider, desc: "Maximum spawn radius", "0.01 2")]
	protected float m_maxSpawnRadius;
	
	// When looking for a place to put our loose resource, how far down should we trace to find ground?
	[Attribute(defvalue: "-2", uiwidget: UIWidgets.Slider, desc: "Placement trace distance", "-10 -2")]
	protected float m_placementTraceDownDist;
	
	// Called when the consumable effect has finished.
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		super.ApplyEffect(target, user, item, animParams);
		
		SpawnPrefabFromResource(user, Resource.Load(m_LooseItemResource));
	}
	
	// Our trace callback to ignore characters from our trace.
	static bool PlacementFilterCallback(notnull IEntity target)
	{
		if (ChimeraCharacter.Cast(target))
			return false;
	
		return true;
	}
	
	// A helper method to find a good placement position for our resource.
	vector GetPlacementPosition(vector startPos, vector direction, out bool traceHit)
	{
		autoptr TraceParam trace = new TraceParam();
		trace.Start = startPos;
		trace.End = trace.Start + direction;
		trace.LayerMask = EPhysicsLayerDefs.Projectile;
		trace.Flags = TraceFlags.WORLD | TraceFlags.ENTS;

		float distance = GetGame().GetWorld().TraceMove(trace, PlacementFilterCallback);
		traceHit = distance != 1.0;

		return startPos + direction * distance;
	}
	
	// Spawns the prefab we want
	void SpawnPrefabFromResource(notnull IEntity target, notnull Resource resource)
	{		
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		target.GetTransform(params.Transform);
		
		RandomGenerator generator = new RandomGenerator;
		generator.SetSeed(Math.RandomInt(0, 100));
	
		// Get a random offset around a radius
		vector randOffset = generator.GenerateRandomPointInRadius(m_minSpawnRadius, m_maxSpawnRadius, vector.Zero);
		
		bool traceHit;
		params.Transform[3] = GetPlacementPosition(target.GetOrigin() + randOffset, vector.Up * m_placementTraceDownDist, traceHit);

		// If we didn't hit anything, don't bother spawning a resource, it'll be flying..
		if (!traceHit)
		{
			return;
		}
		
		GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
	}
}
