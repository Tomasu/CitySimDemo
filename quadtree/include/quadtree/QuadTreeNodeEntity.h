//
// Created by moose on 05/04/19.
//

#ifndef CITYSIM_QUADTREENODEENTITY_H
#define CITYSIM_QUADTREENODEENTITY_H

#include <Qt3DCore/QEntity>

namespace Qt3DCore {
	class QTransform;
}

namespace Qt3DInput {
	class QMouseDevice;
	class QMouseHandler;
	class QMouseEvent;
}

namespace Qt3DRender {
	class QObjectPicker;
	class QMaterial;
	class QPickEvent;
}

class MainWindow;
class QuadTreeNode;
class QuadTreeNodeEntity;
class QuadTreeNodeGeometryRenderer;

class QuadTreeNodeEntity : public Qt3DCore::QEntity
{
	Q_OBJECT
	public:
		explicit QuadTreeNodeEntity(QuadTreeNode *node, MainWindow *mw, Qt3DCore::QNode *parent = nullptr);
		~QuadTreeNodeEntity() override;

		QuadTreeNodeGeometryRenderer *renderer() { return mRenderer; }

		QuadTreeNode *node() const { return mNode; }
	protected:
		void
		sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

	private slots:
		void mousePositionChanged(Qt3DInput::QMouseEvent *ev);

		void objectPickerMoved(Qt3DRender::QPickEvent *ev);

	private:
		QuadTreeNode *mNode;
		Qt3DCore::QTransform *mTranslation;
		Qt3DCore::QTransform *mScale;
		Qt3DRender::QMaterial *mMaterial;
		QuadTreeNodeGeometryRenderer *mRenderer;
		Qt3DRender::QObjectPicker *mPicker;
		MainWindow *mMainWindow;

};


#endif //CITYSIM_QUADTREENODEENTITY_H
