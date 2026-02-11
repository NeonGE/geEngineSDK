/*****************************************************************************/
/**
 * @file    geVertexDeclaration.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/06
 * @brief   Vertex Declaration objets to create Layouts.
 *
 * Vertex Declaration objets to create Layouts.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVertexDeclaration.h"
#include <geDebug.h>

namespace geEngineSDK {

  VertexElement::VertexElement(uint32 source,
    uint32 offset,
    VERTEX_ELEMENT_TYPE::E theType,
    VERTEX_ELEMENT_SEMANTIC::E semantic,
    uint32 index,
    uint32 instanceStepRate)
    : m_source(source),
    m_offset(offset),
    m_type(theType),
    m_semantic(semantic),
    m_index(index),
    m_instanceStepRate(instanceStepRate)
  {}

  uint32
  VertexElement::getSize(void) const {
    return getTypeSize(m_type);
  }

  uint32
  VertexElement::getTypeSize(VERTEX_ELEMENT_TYPE::E etype) {
    switch (etype)
    {
    case VERTEX_ELEMENT_TYPE::COLOR:
    case VERTEX_ELEMENT_TYPE::COLOR_ABGR:
    case VERTEX_ELEMENT_TYPE::COLOR_ARGB:
    case VERTEX_ELEMENT_TYPE::UBYTE4_NORM:
      return sizeof(uint32);
    case VERTEX_ELEMENT_TYPE::FLOAT1:
      return sizeof(float);
    case VERTEX_ELEMENT_TYPE::FLOAT2:
      return sizeof(float) * 2;
    case VERTEX_ELEMENT_TYPE::FLOAT3:
      return sizeof(float) * 3;
    case VERTEX_ELEMENT_TYPE::FLOAT4:
      return sizeof(float) * 4;
    case VERTEX_ELEMENT_TYPE::USHORT1:
      return sizeof(uint16);
    case VERTEX_ELEMENT_TYPE::USHORT2:
      return sizeof(uint16) * 2;
    case VERTEX_ELEMENT_TYPE::USHORT4:
      return sizeof(uint16) * 4;
    case VERTEX_ELEMENT_TYPE::SHORT1:
      return sizeof(int16);
    case VERTEX_ELEMENT_TYPE::SHORT2:
      return sizeof(int16) * 2;
    case VERTEX_ELEMENT_TYPE::SHORT4:
      return sizeof(int16) * 4;
    case VERTEX_ELEMENT_TYPE::UINT1:
      return sizeof(uint32);
    case VERTEX_ELEMENT_TYPE::UINT2:
      return sizeof(uint32) * 2;
    case VERTEX_ELEMENT_TYPE::UINT3:
      return sizeof(uint32) * 3;
    case VERTEX_ELEMENT_TYPE::UINT4:
      return sizeof(uint32) * 4;
    case VERTEX_ELEMENT_TYPE::INT4:
      return sizeof(int32) * 4;
    case VERTEX_ELEMENT_TYPE::INT1:
      return sizeof(int32);
    case VERTEX_ELEMENT_TYPE::INT2:
      return sizeof(int32) * 2;
    case VERTEX_ELEMENT_TYPE::INT3:
      return sizeof(int32) * 3;
    case VERTEX_ELEMENT_TYPE::UBYTE4:
      return sizeof(uint8) * 4;
    default:
      break;
    }

    return 0;
  }

  uint32
  VertexElement::getTypeCount(VERTEX_ELEMENT_TYPE::E etype) {
    switch (etype)
    {
    case VERTEX_ELEMENT_TYPE::COLOR:
    case VERTEX_ELEMENT_TYPE::COLOR_ABGR:
    case VERTEX_ELEMENT_TYPE::COLOR_ARGB:
      return 4;
    case VERTEX_ELEMENT_TYPE::FLOAT1:
    case VERTEX_ELEMENT_TYPE::SHORT1:
    case VERTEX_ELEMENT_TYPE::USHORT1:
    case VERTEX_ELEMENT_TYPE::INT1:
    case VERTEX_ELEMENT_TYPE::UINT1:
      return 1;
    case VERTEX_ELEMENT_TYPE::FLOAT2:
    case VERTEX_ELEMENT_TYPE::SHORT2:
    case VERTEX_ELEMENT_TYPE::USHORT2:
    case VERTEX_ELEMENT_TYPE::INT2:
    case VERTEX_ELEMENT_TYPE::UINT2:
      return 2;
    case VERTEX_ELEMENT_TYPE::FLOAT3:
    case VERTEX_ELEMENT_TYPE::INT3:
    case VERTEX_ELEMENT_TYPE::UINT3:
      return 3;
    case VERTEX_ELEMENT_TYPE::FLOAT4:
    case VERTEX_ELEMENT_TYPE::SHORT4:
    case VERTEX_ELEMENT_TYPE::USHORT4:
    case VERTEX_ELEMENT_TYPE::INT4:
    case VERTEX_ELEMENT_TYPE::UINT4:
    case VERTEX_ELEMENT_TYPE::UBYTE4:
    case VERTEX_ELEMENT_TYPE::UBYTE4_NORM:
      return 4;
    default:
      break;
    }

    return 0;
  }

  VERTEX_ELEMENT_TYPE::E
  VertexElement::getBestColorVertexElementType() {
#if USING(GE_PLATFORM_WINDOWS)
    return VERTEX_ELEMENT_TYPE::COLOR_ARGB; //Prefer D3D format on Windows
#else
    return VERTEX_ELEMENT_TYPE::COLOR_ABGR; //Prefer GL format
#endif
  }

  bool
  VertexElement::operator==(const VertexElement& rhs) const {
    return (m_type == rhs.m_type &&
            m_index == rhs.m_index &&
            m_offset == rhs.m_offset &&
            m_semantic == rhs.m_semantic &&
            m_source == rhs.m_source &&
            m_instanceStepRate == rhs.m_instanceStepRate);
  }

  bool
  VertexElement::operator!= (const VertexElement& rhs) const {
    return !(*this == rhs);
  }

  SIZE_T
  VertexElement::getHash(const VertexElement& element) {
    SIZE_T hash = 0;
    ge_hash_combine(hash, element.m_type);
    ge_hash_combine(hash, element.m_index);
    ge_hash_combine(hash, element.m_offset);
    ge_hash_combine(hash, element.m_semantic);
    ge_hash_combine(hash, element.m_source);
    ge_hash_combine(hash, element.m_instanceStepRate);
    return hash;
  }

  VertexDeclarationProperties::VertexDeclarationProperties(
    const Vector<VertexElement>& elements) {
    for (auto& elem : elements) {
      VERTEX_ELEMENT_TYPE::E type = elem.getType();
      m_elementList.emplace_back(elem.getStreamIndex(),
        elem.getOffset(),
        type,
        elem.getSemantic(),
        elem.getSemanticIndex(),
        elem.getInstanceStepRate());
    }
  }

  bool
  VertexDeclarationProperties::operator== (const VertexDeclarationProperties& rhs) const {
    if (m_elementList.size() != rhs.m_elementList.size()) {
      return false;
    }

    auto myIter = m_elementList.begin();
    auto theirIter = rhs.m_elementList.begin();

    for (; myIter != m_elementList.end() && theirIter != rhs.m_elementList.end();
      ++myIter, ++theirIter) {
      if (!(*myIter == *theirIter)) {
        return false;
      }
    }

    return true;
  }

  bool
  VertexDeclarationProperties::operator!= (const VertexDeclarationProperties& rhs) const {
    return !(*this == rhs);
  }

  const VertexElement*
  VertexDeclarationProperties::getElement(uint32 index) const {
    GE_ASSERT(index < m_elementList.size() && "Index out of bounds");

    auto iter = m_elementList.begin();
    for (uint32 i = 0; i < index; ++i) {
      ++iter;
    }

    return &(*iter);

  }

  const VertexElement*
  VertexDeclarationProperties::findElementBySemantic(VERTEX_ELEMENT_SEMANTIC::E sem,
      uint32 index) const {
    for (auto& elem : m_elementList) {
      if (elem.getSemantic() == sem && elem.getSemanticIndex() == index) {
        return &elem;
      }
    }

    return nullptr;
  }

  Vector<VertexElement>
  VertexDeclarationProperties::findElementsBySource(uint32 source) const {
    Vector<VertexElement> retList;
    for (auto& elem : m_elementList) {
      if (elem.getStreamIndex() == source) {
        retList.push_back(elem);
      }
    }

    return retList;
  }

  uint32
  VertexDeclarationProperties::getVertexSize(uint32 source) const {
    uint32 size = 0;

    for (auto& elem : m_elementList) {
      if (elem.getStreamIndex() == source) {
        size += elem.getSize();
      }
    }

    return size;
  }

  bool
  VertexDeclaration::isCompatible(const WeakSPtr<VertexDeclaration>& shaderDecl) {
    if (shaderDecl.expired()) {
      return false;
    }

    auto pShaderDecl = shaderDecl.lock();

    const auto& bufferProps = getProperties();
    const auto& shaderProps = pShaderDecl->getProperties();

    for (auto shaderIter = shaderProps.getElements().begin();
      shaderIter != shaderProps.getElements().end(); ++shaderIter) {
      const VertexElement* foundElem = nullptr;
      for (auto bufferIter = bufferProps.getElements().begin();
        bufferIter != bufferProps.getElements().end(); ++bufferIter) {
        if (shaderIter->getSemantic() == bufferIter->getSemantic() &&
          shaderIter->getSemanticIndex() == bufferIter->getSemanticIndex()) {
          foundElem = &(*bufferIter);
          break;
        }
      }
      if (!foundElem) {
        // If the shader element is not found in the buffer properties, they are not compatible
        return false;
      }
    }

    // If we reach here, all shader elements were found in the buffer properties
    return true;
  }

  Vector<VertexElement>
  VertexDeclaration::getMissingElements(const WeakSPtr<VertexDeclaration>& shaderDecl) {
    Vector<VertexElement> missingElements;
    if (shaderDecl.expired()) {
      return missingElements;
    }

    auto pShaderDecl = shaderDecl.lock();

    const auto& bufferProps = getProperties();
    const auto& shaderProps = pShaderDecl->getProperties();

    for (auto shaderIter = shaderProps.getElements().begin();
      shaderIter != shaderProps.getElements().end(); ++shaderIter) {
      const VertexElement* foundElem = nullptr;
      for (auto bufferIter = bufferProps.getElements().begin();
        bufferIter != bufferProps.getElements().end(); ++bufferIter) {
        if (shaderIter->getSemantic() == bufferIter->getSemantic() &&
          shaderIter->getSemanticIndex() == bufferIter->getSemanticIndex()) {
          foundElem = &(*bufferIter);
          break;
        }
      }
      if (!foundElem) {
        // If the shader element is not found in the buffer properties, they are not compatible
        missingElements.push_back(*shaderIter);
      }
    }

    // If we reach here, all shader elements were found in the buffer properties
    return missingElements;
  }

  String
  toString(const VERTEX_ELEMENT_SEMANTIC::E& val) {
    switch (val)
    {
    case VERTEX_ELEMENT_SEMANTIC::POSITION:
      return "POSITION";
    case VERTEX_ELEMENT_SEMANTIC::BLENDWEIGHT:
      return "BLEND_WEIGHTS";
    case VERTEX_ELEMENT_SEMANTIC::BLENDINDICES:
      return "BLEND_INDICES";
    case VERTEX_ELEMENT_SEMANTIC::NORMAL:
      return "NORMAL";
    case VERTEX_ELEMENT_SEMANTIC::COLOR:
      return "COLOR";
    case VERTEX_ELEMENT_SEMANTIC::TEXCOORD:
      return "TEXCOORD";
    case VERTEX_ELEMENT_SEMANTIC::BITANGENT:
      return "BITANGENT";
    case VERTEX_ELEMENT_SEMANTIC::TANGENT:
      return "TANGENT";
    case VERTEX_ELEMENT_SEMANTIC::POSITIONT:
      return "POSITIONT";
    case VERTEX_ELEMENT_SEMANTIC::PSIZE:
      return "PSIZE";
    case VERTEX_ELEMENT_SEMANTIC::CUSTOM:
      return "CUSTOM";
    }

    return "UNKNOWN_SEMANTIC";
  }

  bool
  StreamOutputElement::operator==(const StreamOutputElement& rhs) const {
    return (m_outputSlot == rhs.m_outputSlot &&
      m_semantic == rhs.m_semantic &&
      m_semanticIndex == rhs.m_semanticIndex &&
      m_componentStart == rhs.m_componentStart &&
      m_componentCount == rhs.m_componentCount &&
      m_streamIndex == rhs.m_streamIndex &&
      m_registerIndex == rhs.m_registerIndex);
  }

  bool
  StreamOutputElement::operator!=(const StreamOutputElement& rhs) const {
    return !(*this == rhs);
  }

  SIZE_T
  StreamOutputElement::getHash(const StreamOutputElement& element) {
    SIZE_T hash = 0;
    ge_hash_combine(hash, element.m_outputSlot);
    ge_hash_combine(hash, element.m_semantic);
    ge_hash_combine(hash, element.m_semanticIndex);
    ge_hash_combine(hash, element.m_componentStart);
    ge_hash_combine(hash, element.m_componentCount);
    ge_hash_combine(hash, element.m_streamIndex);
    ge_hash_combine(hash, element.m_registerIndex);
    return hash;
  }

  StreamOutputDeclarationProperties::StreamOutputDeclarationProperties(
    const Vector<StreamOutputElement>& elements)
    : m_elementList(elements) {
      {}

  } // namespace geEngineSDK

  bool
  StreamOutputDeclarationProperties::operator==(
                          const StreamOutputDeclarationProperties& rhs) const {
    return m_elementList == rhs.m_elementList;
  }

  bool
  StreamOutputDeclarationProperties::operator!=(
                          const StreamOutputDeclarationProperties& rhs) const {
    return !(*this == rhs);
  }

  uint32
  StreamOutputDeclarationProperties::getElementCount() const {
    return static_cast<uint32>(m_elementList.size());
  }

  const Vector<StreamOutputElement>&
  StreamOutputDeclarationProperties::getElements() const {
    return m_elementList;
  }

  const StreamOutputElement*
  StreamOutputDeclarationProperties::getElement(uint32 index) const {
    if (index >= m_elementList.size()) {
      return nullptr;
    }

    return &m_elementList[index];
  }

  Vector<StreamOutputElement>
  StreamOutputDeclarationProperties::findElementsByOutputSlot(uint32 outputSlot) const {
    Vector<StreamOutputElement> result;
    for (const auto& elem : m_elementList)
    {
      if (elem.getOutputSlot() == outputSlot) {
        result.push_back(elem);
      }
    }
    return result;
  }

  uint32
  StreamOutputDeclarationProperties::getComponentCountForOutputSlot(uint32 outputSlot) const {
    uint32 total = 0;
    for (const auto& elem : m_elementList)
    {
      if (elem.getOutputSlot() == outputSlot) {
        total += elem.getComponentCount();
      }
    }
    return total;
  }

  StreamOutputDeclaration::StreamOutputDeclaration(
                             const Vector<StreamOutputElement>& elements)
    : m_properties(elements)
  {}

  const StreamOutputDeclarationProperties&
  StreamOutputDeclaration::getProperties() const {
    return m_properties;
  }

  bool
  StreamOutputDeclaration::isCompatible(const WeakSPtr<StreamOutputDeclaration>& shaderDecl) {
    if (shaderDecl.expired()) {
      return false;
    }

    auto pShaderDecl = shaderDecl.lock();
    const auto& bufferProps = getProperties();
    const auto& shaderProps = pShaderDecl->getProperties();

    for (auto shaderIter = shaderProps.getElements().begin();
           shaderIter != shaderProps.getElements().end(); ++shaderIter) {
      const StreamOutputElement* foundElem = nullptr;
      for (auto bufferIter = bufferProps.getElements().begin();
             bufferIter != bufferProps.getElements().end(); ++bufferIter) {
        if (shaderIter->getSemantic() == bufferIter->getSemantic() &&
            shaderIter->getSemanticIndex() == bufferIter->getSemanticIndex()) {
          foundElem = &(*bufferIter);
          break;
        }
      }
      if (!foundElem) {
        // If the shader element is not found in the buffer properties, they are not compatible
        return false;
      }
    }
    // If we reach here, all shader elements were found in the buffer properties
    return true;
  }

  Vector<StreamOutputElement>
  StreamOutputDeclaration::getMissingElements(
                             const WeakSPtr<StreamOutputDeclaration>& shaderDecl) {
    Vector<StreamOutputElement> missingElements;
    if (shaderDecl.expired()) {
      return missingElements;
    }

    auto pShaderDecl = shaderDecl.lock();
    const auto& bufferProps = getProperties();
    const auto& shaderProps = pShaderDecl->getProperties();

    for (auto shaderIter = shaderProps.getElements().begin();
           shaderIter != shaderProps.getElements().end(); ++shaderIter) {
      const StreamOutputElement* foundElem = nullptr;
      for (auto bufferIter = bufferProps.getElements().begin();
             bufferIter != bufferProps.getElements().end(); ++bufferIter) {
        if (shaderIter->getSemantic() == bufferIter->getSemantic() &&
            shaderIter->getSemanticIndex() == bufferIter->getSemanticIndex()) {
          foundElem = &(*bufferIter);
          break;
        }
      }
      if (!foundElem) {
        // If the shader element is not found in the buffer properties,
        // they are not compatible
        missingElements.push_back(*shaderIter);
      }
    }
    // If we reach here, all shader elements were found in the buffer properties
    return missingElements;
  }

}
