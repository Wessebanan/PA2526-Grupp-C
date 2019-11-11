#pragma once

#define DEFINE_WEAPON_RENDERER_HEADER(renderSystemName) \
	class renderSystemName : public ecs::ECSSystem<renderSystemName> \
	{ \
	public:  \
		renderSystemName(); \
		virtual ~renderSystemName() {} \
		void updateMultipleEntities(EntityIterator& _entities, float _delta) override; \
		void Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer); \
	private: \
		WeaponRenderer mRenderer; \
	};

#define SET_WEAPON_TYPE_FILTER(compName) \
	typeFilter.addRequirement(components::WeaponComponent::typeID); \
	typeFilter.addRequirement(components::TransformComponent::typeID); \
	typeFilter.addRequirement(components::ColorComponent::typeID); \
	typeFilter.addRequirement(components::compName::typeID);

#define INIT_WEAPON_RENDERER(pRenderMgr, pRenderBuffer, objectType) \
	ECSUser::InitializeSubECSUser(&mRenderer); \
	mRenderer.InitializeInternal(pRenderMgr, pRenderBuffer, objectType);

#define DEFINE_WEAPON_RENDERER(renderSystemName, weaponCompName, objectType) \
	renderSystemName::renderSystemName() { updateType = SystemUpdateType::MultiEntityUpdate; SET_WEAPON_TYPE_FILTER(weaponCompName) } \
	void renderSystemName::updateMultipleEntities(EntityIterator& _entities, float _delta) { mRenderer.RenderAllInternal(_entities); } \
	void renderSystemName::Initialize(graphics::RenderManager* pRenderMgr, graphics::RenderBuffer* pRenderBuffer) { INIT_WEAPON_RENDERER(pRenderMgr, pRenderBuffer, objectType) }