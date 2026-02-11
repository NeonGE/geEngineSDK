/*****************************************************************************/
/**
 * @file    geCoreFeatures.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2025/07/16
 * @brief   Configuration file to add/remove features to the Core Engine.
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
#include <gePrerequisitesUtilities.h>

#define VIRTUAL_FILE_SYSTEM IN_USE

/**
 * FILE_TRACKER Controls the file tracking system. If the system is enabled,
 * the engine will track changes to files and reload them if necessary.
 * This is useful for development purposes, but can be disabled in production
 */
#define FILE_TRACKER  IN_USE
