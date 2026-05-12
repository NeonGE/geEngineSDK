/*****************************************************************************/
/**
 * @file    geModelComponent.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/04/27
 * @brief   
 *
 * 
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geComponent.h"
#include "geModel.h"

namespace geEngineSDK {
  class ModelComponent : public Component
  {
   public:
    static constexpr uint32 kTypeId = 0x00001001;

    uint32
    getTypeId() const override {
      return kTypeId;
    }

    void
    setModel(const SPtr<Model>& model) {
      m_model = model;
    }

    const SPtr<Model>&
    getModel() const {
      return m_model;
    }

    void
    setMaterialOverride(uint32 submeshIndex, const SPtr<Material>& material) {
      m_materialOverrides[submeshIndex] = material;
    }

    SPtr<Material>
    getMaterialOverride(uint32 submeshIndex) const {
      auto it = m_materialOverrides.find(submeshIndex);
      if (it != m_materialOverrides.end()) {
        return it->second;
      }

      return nullptr;
    }

   private:
    SPtr<Model> m_model;
    UnorderedMap<uint32, SPtr<Material>> m_materialOverrides;
  };

} // namespace geEngineSDK
