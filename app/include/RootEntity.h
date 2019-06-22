//
// Created by moose on 17/04/19.
//

#ifndef CITYSIM_ROOTENTITY_H
#define CITYSIM_ROOTENTITY_H


#include <Qt3DCore/QEntity>

class RootEntity : public Qt3DCore::QEntity
{
	public:
		explicit RootEntity(Qt3DCore::QNode *parent = nullptr);
		~RootEntity() override ;

	protected:
		void
		sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;
};


#endif //CITYSIM_ROOTENTITY_H
