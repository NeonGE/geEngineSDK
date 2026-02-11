/*****************************************************************************/
/**
 * @file    geVertexDeclaration.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/06
 * @brief   Vertex Declaration objets to create Layouts.
 *
 * Vertex Declaration objets to create Layouts.
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
#include "gePrerequisitesCore.h"
#include "geGraphicsTypes.h"

namespace geEngineSDK {

  class RenderAPI;

  class GE_CORE_EXPORT VertexElement
  {
   public:
    VertexElement() = default;
    VertexElement(uint32 source,
                  uint32 offset,
                  VERTEX_ELEMENT_TYPE::E theType,
                  VERTEX_ELEMENT_SEMANTIC::E semantic,
                  uint32 index = 0,
                  uint32 instanceStepRate = 0);

    bool
    operator==(const VertexElement& rhs) const;

    bool
    operator!=(const VertexElement& rhs) const;

    /**
     * @brief Returns index of the vertex buffer from which this element is stored.
     * @return The index of the stream as an unsigned 32-bit integer.
     */
    uint32
    getStreamIndex() const {
      return m_source;
    }

    /**
     * @brief Returns an offset into the buffer where this vertex is stored.
     *        This value might be in bytes but doesn't have to be, it's likely
     *        to be render API specific.
     * @return The offset of the vertex element as an unsigned 32-bit integer.
     */
    uint32
    getOffset() const {
      return m_offset;
    }

    /**
     * @brief Returns the type of the vertex element.
     * @return The type of the vertex element as a VERTEX_ELEMENT_TYPE::E.
     */
    VERTEX_ELEMENT_TYPE::E
    getType() const {
      return m_type;
    }

    /**
     * @brief Gets a semantic that describes what this element contains.
     * @return Semantic value of the element, as a VERTEX_ELEMENT_SEMANTIC::E.
     */
    VERTEX_ELEMENT_SEMANTIC::E
    getSemantic() const {
      return m_semantic;
    }

    /**
     * @brief Gets an index of this element. Only relevant when you have
     *        multiple elements with the same semantic, for example uv0, uv1.
     * @return The semantic index as an unsigned 32-bit integer.
     */
    uint32
    getSemanticIndex()
    const {
      return m_index;
    }

    /**
     * @brief Returns the size of this element in bytes.
     * @return The size of the vertex element as an unsigned 32-bit integer.
     */
    uint32
    getSize() const;

    /**
     * @brief Returns at what rate do the vertex elements advance during
     *        instanced rendering. Provide zero for default behaviour where
     *        each vertex receives the next value from the vertex buffer.
     *        Provide a value larger than zero to ensure vertex data is
     *        advanced with every instance, instead of every vertex
     *        (for example a value of 1 means each instance will retrieve a
     *        new value from the vertex buffer, a value of 2 means each second
     *        instance will, etc.).
     */
    uint32
    getInstanceStepRate() const {
      return m_instanceStepRate;
    }

    /**
     * @brief Returns the size of a base element type.
     * @param etype The vertex element type whose size is to be determined.
     * @return The size in bytes of the given vertex element type.
     */
    static uint32
    getTypeSize(VERTEX_ELEMENT_TYPE::E etype);

    /**
     * @brief Returns the number of values in the provided base element type.
     *        For example float4 has four values.
     * @param etype The vertex element type whose component count is to be
     *        retrieved.
     * @return The number of components associated with the specified vertex
     *         element type.
     */
    static uint32
    getTypeCount(VERTEX_ELEMENT_TYPE::E etype);

    /**
     * @brief Gets packed color vertex element type used by the render system.
     * @return The optimal VertexElementType to use for color vertex elements.
     */
    static VERTEX_ELEMENT_TYPE::E
    getBestColorVertexElementType();

    /**
     * @brief Calculates a hash value for the provided vertex element.
     * @param element The VertexElement object to hash.
     * @return A hash value representing the input VertexElement.
     */
    static SIZE_T
    getHash(const VertexElement& element);

   protected:
    uint32 m_source;
    uint32 m_offset;
    VERTEX_ELEMENT_TYPE::E m_type;
    VERTEX_ELEMENT_SEMANTIC::E m_semantic;
    uint32 m_index;
    uint32 m_instanceStepRate;
  };

  class GE_CORE_EXPORT StreamOutputElement
  {
   public:
    StreamOutputElement() = default;
    StreamOutputElement(uint32 outputSlot,
                        VERTEX_ELEMENT_SEMANTIC::E semantic,
                        uint32 semanticIndex = 0,
                        uint8 componentStart = 0,
                        uint8 componentCount = 4,
                        uint8 streamIndex = 0,
                        uint8 registerIndex = 0)
      : m_outputSlot(outputSlot),
        m_semantic(semantic),
        m_semanticIndex(semanticIndex),
        m_componentStart(componentStart),
        m_componentCount(componentCount),
        m_streamIndex(streamIndex),
        m_registerIndex(registerIndex)
    {}

    bool
    operator==(const StreamOutputElement& rhs) const;

    bool
    operator!=(const StreamOutputElement& rhs) const;

    uint32
    getOutputSlot() const {
      return m_outputSlot;
    }

    VERTEX_ELEMENT_SEMANTIC::E getSemantic() const {
      return m_semantic;
    }

    uint32
    getSemanticIndex() const {
      return m_semanticIndex;
    }

    uint8
    getComponentStart() const {
      return m_componentStart;
    }

    uint8
    getComponentCount() const {
      return m_componentCount;
    }

    uint8
    getStreamIndex() const {
      return m_streamIndex;
    }

    uint8
    getRegisterIndex() const {
      return m_registerIndex;
    }

    static SIZE_T
    getHash(const StreamOutputElement& element);

   private:
    uint32 m_outputSlot;          // Which buffer to write to (same as OutputSlot in D3D11_SO_DECLARATION_ENTRY)
    VERTEX_ELEMENT_SEMANTIC::E m_semantic;
    uint32 m_semanticIndex;
    uint8 m_componentStart;       // First component (0–3) to write
    uint8 m_componentCount;       // How many components (1–4)
    uint8 m_streamIndex;          // Which output stream (optional, default 0)
    uint8 m_registerIndex;        // Shader output register index
  };

  /**
   * @brief Represents the properties of a vertex declaration, providing access
   *        to its elements and related queries.
   */
  class GE_CORE_EXPORT VertexDeclarationProperties
  {
   public:
    VertexDeclarationProperties(const Vector<VertexElement>& elements);

    bool
    operator== (const VertexDeclarationProperties& rhs) const;

    bool
    operator!= (const VertexDeclarationProperties& rhs) const;

    /**
     * @brief Returns the number of elements in the declaration.
     * @return The number of elements in the element list as a uint32.
     */
    uint32
    getElementCount() const {
      return static_cast<uint32>(m_elementList.size());
    }

    /**
     * @brief Returns a list of vertex elements in the declaration.
     * @return A constant reference to the vector containing the vertex elements.
     */
    const Vector<VertexElement>&
    getElements() const {
      return m_elementList;
    }

    /**
     * @brief Returns a single vertex element at the specified index.
     * @param index The zero-based index of the vertex element to retrieve.
     * @return A pointer to the VertexElement at the given index, or nullptr
     *         if the index is out of range.
     */
    const VertexElement*
    getElement(uint32 index) const;

    /**
     * @brief Attempts to find an element by the given semantic and semantic
     *        index. If no element can be found null is returned.
     * @param sem The semantic to search for (e.g., position, normal, texCoord)
     * @param index The index of the semantic to find, used when multiple
     *        elements share the same semantic. Defaults to 0.
     * @return A pointer to the found VertexElement if it exists, or nullptr
     *         if no matching element is found.
     */
    const VertexElement*
    findElementBySemantic(VERTEX_ELEMENT_SEMANTIC::E sem,
                          uint32 index = 0) const;

    /**
     * @brief Returns a list of elements that use the provided source index.
     * @param source The source index to search for among the vertex elements.
     * @return A vector containing all VertexElement objects that have the
     *         specified source index.
     */
    Vector<VertexElement>
    findElementsBySource(uint32 source) const;

    /**
     * @brief Returns the total size of all vertex elements using the provided
     *        source index.
     * @param source The identifier of the vertex source whose size is to be
     *        retrieved.
     * @return The size of the vertex for the given source, in bytes.
     */
    uint32
    getVertexSize(uint32 source) const;

   protected:
    friend class VertexDeclaration;

    Vector<VertexElement> m_elementList;
  };

  class GE_CORE_EXPORT StreamOutputDeclarationProperties
  {
   public:
    StreamOutputDeclarationProperties() = default;
    StreamOutputDeclarationProperties(const Vector<StreamOutputElement>& elements);

    bool
    operator==(const StreamOutputDeclarationProperties& rhs) const;

    bool
    operator!=(const StreamOutputDeclarationProperties& rhs) const;

    /**
     * @brief Returns the number of elements in the stream output declaration.
     */
    uint32
    getElementCount() const;

    /**
     * @brief Returns a list of stream output elements in the declaration.
     */
    const Vector<StreamOutputElement>&
    getElements() const;

    /**
     * @brief Returns a single stream output element at the specified index.
     *        Returns nullptr if index is out of range.
     */
    const StreamOutputElement*
    getElement(uint32 index) const;

    /**
     * @brief Finds an element by semantic and index.
     */
    const StreamOutputElement*
    findElementBySemantic(VERTEX_ELEMENT_SEMANTIC::E sem, uint32 semanticIndex = 0) const;

    /**
     * @brief Returns a list of all elements writing to the specified output slot.
     */
    Vector<StreamOutputElement>
    findElementsByOutputSlot(uint32 outputSlot) const;

    /**
     * @brief Returns the total component count for the specified output slot.
     *        (e.g. used for estimating stride size in SO buffer)
     */
    uint32
    getComponentCountForOutputSlot(uint32 outputSlot) const;

   protected:
    Vector<StreamOutputElement> m_elementList;
  };


  /**
   * @brief Represents a vertex declaration, which defines the layout of vertex
   *        data in a graphics pipeline.
   */
  class GE_CORE_EXPORT VertexDeclaration
  {
   public:
    VertexDeclaration(const Vector<VertexElement>& elements)
      : m_properties(elements)
    {}

    virtual ~VertexDeclaration() = default;

    const VertexDeclarationProperties&
    getProperties() const {
      return m_properties;
    }

    /**
     * @brief Checks if a vertex buffer declared with this declaration can be
     *        bound to a shader defined with the provided declaration.
     * @param shaderDecl A shared pointer to the vertex declaration to check
     *        for compatibility.
     * @return true if the vertex declaration is compatible; otherwise, false.
     */
    bool
    isCompatible(const WeakSPtr<VertexDeclaration>& shaderDecl);

    /**
     * @brief Returns a list of vertex elements that the provided shader's
     *        vertex declaration expects but aren't present in this vertex
     *        declaration.
     * @param shaderDecl A shared pointer to the vertex declaration to compare
     *        against.
     * @return A vector containing the vertex elements that are missing from
     *         the current vertex declaration but present in shaderDecl.
     */
    Vector<VertexElement>
    getMissingElements(const WeakSPtr<VertexDeclaration>& shaderDecl);

   protected:
    VertexDeclarationProperties m_properties;
  };

  class GE_CORE_EXPORT StreamOutputDeclaration
  {
   public:
    StreamOutputDeclaration(const Vector<StreamOutputElement>& elements);
    virtual ~StreamOutputDeclaration() = default;

    /**
     * @brief Returns the declaration properties associated with this stream output declaration.
     */
    const StreamOutputDeclarationProperties&
    getProperties() const;

    /**
     * @brief Checks if this stream output declaration is compatible with another (e.g. from a shader).
     *        All elements in the shader declaration must exist in this declaration.
     * @param shaderDecl A weak pointer to the stream output declaration expected by the shader.
     * @return true if compatible; false otherwise.
     */
    bool
    isCompatible(const WeakSPtr<StreamOutputDeclaration>& shaderDecl);

    /**
     * @brief Returns a list of elements that are required by the shader declaration
     *        but are missing from this declaration.
     * @param shaderDecl A weak pointer to the expected shader declaration.
     * @return A vector of missing StreamOutputElements.
     */
    Vector<StreamOutputElement>
    getMissingElements(const WeakSPtr<StreamOutputDeclaration>& shaderDecl);

   protected:
    StreamOutputDeclarationProperties m_properties;
  };

  /**
   * @brief Converts a vertex semantic enum to a readable name.
   * @param val 
   * @return 
   */
  GE_CORE_EXPORT String
  toString(const VERTEX_ELEMENT_SEMANTIC::E& val);

} // namespace geEngineSDK
