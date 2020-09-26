#include "IAction.h"
#include "RenderLayerType.h"
#include "RenderContainer.h"
#include "RendererManager.h"

class ActionPushObject :
	public IAction
{
	class EObject* m_object;
	enum class RenderLayerType m_layer;

	ActionPushObject() = delete;

public:

	ActionPushObject(class EObject* const object, const enum class RenderLayerType layer);
	~ActionPushObject();

	void execute() override;
};

