#pragma once

#include "gePrerequisitesRenderAPIDX11.h"

namespace geEngineSDK {

  namespace TranslateUtils {
    
    /**
     * @brief Converts a GRAPHICS_FORMAT::E value to the corresponding DXGI_FORMAT value.
     * @param inFormat The input texture format to convert.
     * @return The DXGI_FORMAT value corresponding to the specified texture format.
     */
    DXGI_FORMAT
    get(GRAPHICS_FORMAT::E inFormat);

    /**
     * @brief Returns a string representation of a given DXGI_FORMAT value.
     * @param format The DXGI_FORMAT value to convert to a string.
     * @return A string describing the specified DXGI_FORMAT.
     */
    String
    toString(const DXGI_FORMAT& format);

    /**
     * @brief Converts a string representation of a format to its corresponding
     *        DXGI_FORMAT enumeration value.
     * @param strFormat The string containing the format to convert.
     * @return The DXGI_FORMAT value corresponding to the provided string.
     *         If the string does not match a known format, a default or
     *         unknown value may be returned.
     */
    DXGI_FORMAT
    fromString(const String& strFormat);

    /**
     * @brief Converts a DXGI_FORMAT value to the corresponding GRAPHICS_FORMAT::E value.
     * @param inFormat The input DXGI_FORMAT to convert.
     * @return The GRAPHICS_FORMAT::E value corresponding to the specified DXGI_FORMAT.
     */
    TEXTURE2D_DESC
    get(const D3D11_TEXTURE2D_DESC& desc);

    /**
     * @brief Returns the number of bits per pixel for a given DXGI_FORMAT value.
     * @param format The DXGI_FORMAT value specifying the pixel format.
     * @return The number of bits per pixel for the specified format.
     */
    SIZE_T
    getBitsPerPixel(const DXGI_FORMAT& format);

    /**
     * @brief Converts a VERTEX_ELEMENT_SEMANTIC::E value to its string representation.
     * @param sem The vertex element semantic to convert.
     * @return A string representing the specified vertex element semantic.
     */
    const char*
    toString(VERTEX_ELEMENT_SEMANTIC::E sem);

    /**
     * @brief Returns the corresponding DXGI_FORMAT for a given vertex element type.
     * @param type The vertex element type for which to retrieve the DXGI_FORMAT.
     * @return The DXGI_FORMAT value that matches the specified vertex element type.
     */
    DXGI_FORMAT
    get(VERTEX_ELEMENT_TYPE::E type);

    /**
     * @brief Returns the corresponding VERTEX_ELEMENT_TYPE::E value for a
     *        given D3D register component type.
     * @param type The D3D register component type to convert.
     * @return The VERTEX_ELEMENT_TYPE::E value that matches the specified D3D
     *         register component type.
     */
    VERTEX_ELEMENT_TYPE::E
    getInputType(D3D_REGISTER_COMPONENT_TYPE type, uint8 mask);

    /**
     * @brief Retrieves the enumeration value corresponding to a given vertex
     *        element semantic name.
     * @param sem A pointer to a null-terminated string representing the
     *        semantic name.
     * @return The enumeration value of type VERTEX_ELEMENT_SEMANTIC::E that
     *         matches the provided semantic name.
     */
    VERTEX_ELEMENT_SEMANTIC::E
    get(const char* sem);

  }
}
