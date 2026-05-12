/*****************************************************************************/
/**
 * @file    geAnimationComponent.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/04/27
 * @brief   Simple animation component for a skinned ModelComponent.
 *
 * Simple animation component for a skinned ModelComponent.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

#include "geComponent.h"
#include "geActor.h"
#include "geModelComponent.h"
#include "geSkeleton.h"

namespace geEngineSDK {

  class GE_CORE_EXPORT AnimationComponent : public Component
  {
   public:
    static constexpr uint32 kTypeId = 0x00001002;

    uint32
    getTypeId() const override {
      return kTypeId;
    }

    void
    onAttach() override {
      rebuildFromModelComponent();
    }

    void
    update(float dt) override {
      if (nullptr == m_skeletonInstance.getSkeleton()) {
        rebuildFromModelComponent();
      }

      if (!m_player.isPlaying()) {
        return;
      }

      m_player.update(dt);
      m_skeletonInstance.applyPose(m_player.getCurrentPose());
    }

    bool
    rebuildFromModelComponent() {
      if (nullptr == getOwner()) {
        return false;
      }

      ModelComponent* modelComponent = getOwner()->getComponent<ModelComponent>();
      if (nullptr == modelComponent || nullptr == modelComponent->getModel()) {
        return false;
      }

      const SPtr<Model>& model = modelComponent->getModel();
      if (nullptr == model->m_skeleton) {
        return false;
      }

      m_skeletonInstance.setSkeleton(model->m_skeleton);
      return true;
    }

    bool
    play(const SPtr<AnimationClip>& clip, bool loop = true) {
      if (nullptr == clip) {
        return false;
      }

      if (nullptr == m_skeletonInstance.getSkeleton()) {
        if (!rebuildFromModelComponent()) {
          return false;
        }
      }

      const SPtr<Skeleton>& skeleton = m_skeletonInstance.getSkeleton();
      if (!clip->isCompatibleWith(*skeleton)) {
        return false;
      }

      m_player.play(clip, skeleton, loop);
      m_skeletonInstance.applyPose(m_player.getCurrentPose());
      return true;
    }

    void
    stop() {
      m_player.stop();
    }

    const SkeletonInstance&
    getSkeletonInstance() const {
      return m_skeletonInstance;
    }

    SkeletonInstance&
    getSkeletonInstance() {
      return m_skeletonInstance;
    }

    const AnimationPlayer&
    getPlayer() const {
      return m_player;
    }

    AnimationPlayer&
    getPlayer() {
      return m_player;
    }

   private:
    AnimationPlayer m_player;
    SkeletonInstance m_skeletonInstance;
  };

} // namespace geEngineSDK
