/*****************************************************************************/
/**
 * @file    geFwdDeclUtil.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/16
 * @brief   Utility header that makes forward declarations
 *
 * This header makes forward declarations for all the needed types
 * of the geUtilities library
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

namespace geEngineSDK {
  /**
   * @brief	Generic axis enum (mirrored for property use in Object.h)
   */
  namespace AXIS {
    enum E {
      kNone,
      kX,
      kY,
      kZ,
    };
  }

  /**
   * @brief Values that represent in which order are Euler angles applied when
   *        used in transformations.
   */
  namespace EULER_ANGLE_ORDER {
    enum E {
      kXYZ,
      kXZY,
      kYXZ,
      kYZX,
      kZXY,
      kZYX
    };
  }

  /**
   * @brief	Enum that defines possible window border styles.
   */
  namespace WINDOW_BORDER {
    enum E {
      kNormal,
      kNone,
      kFixed
    };
  }

  /**
   * @brief	Values that represent that a structure or class will be initialized
   *        with a "forced value" and what kind.
   */
	namespace FORCE_INIT {
    enum E {
      kForceInit,
      kForceInitToZero
    };
  }

  /***************************************************************************/
	/**
   * Forward declaration of utility classes
   */
  /***************************************************************************/
  class AABox;
  class Degree;
  class Math;
  class Matrix4;
  class RotationTranslationMatrix;
  class Transform;
  
  class Rotator;
  class Quaternion;
  class Radian;
  class Ray;
  class Sphere;
  class Vector2;
  class Vector2Half;
  class Vector3;
  class Vector4;
  class Vector2I;
  class Plane;
  class Box2DI;
  class Box2D;
  class Rect2;
  class Color;
  class LinearColor;

  class DynLib;
  class DynLibManager;

  class DataStream;
  class MemoryDataStream;
  class FileDataStream;

  class FileSystem;
  class Timer;
  class Task;

  class PixelData;

  class HString;
  class StringTable;
  struct LocalizedStringData;

  class Path;

  class HThread;
  class AsyncOpSyncData;

  class FrameAlloc;
  class LogEntry;

  /***************************************************************************/
  /**
   * Smart pointer types
   */
  /***************************************************************************/
  typedef SPtr<DataStream> DataStreamPtr;
  typedef SPtr<MemoryDataStream> MemoryDataStreamPtr;
  typedef SPtr<FileDataStream> FileDataStreamPtr;
  typedef SPtr<PixelData> PixelDataPtr;
  typedef SPtr<DataStream> DataStreamPtr;
  typedef SPtr<MemoryDataStream> MemoryDataStreamPtr;
  typedef SPtr<Task> TaskPtr;
  typedef SPtr<AsyncOpSyncData> AsyncOpSyncDataPtr;

  typedef List<DataStreamPtr> DataStreamList;
  typedef SPtr<DataStreamList> DataStreamListPtr;

  namespace TYPEID_UTILITY {
  	enum E {
      /**
       * kID_Abstract is a special type ID used for Abstract classes.
       * Only type ID that may be used by more than one class.
       */
      kID_Abstract              = 50,
      kID_Flags                 = 51,
      
      kID_StringID              = 60,
      kID_String                = 61,
      kID_WString               = 61,
      kID_Path                  = 62,

      kID_Vector                = 70,
      kID_Map                   = 71,
      kID_UnorderedMap          = 72,
      kID_Pair                  = 73,
      kID_Set                   = 74,
      kID_UnorderedSet          = 75,

      kID_SerializedInstance    = 80,
      kID_SerializedField       = 81,
      kID_SerializedObject      = 82,
      kID_SerializedArray       = 83,
      kID_SerializedEntry       = 84,
      kID_SerializedArrayEntry  = 85,
      kID_SerializedSubObject   = 86,
      kID_SerializedDataBlock   = 87,
      kID_SerializationContext  = 88,

      kID_IReflectable          = 90,

      kID_DataBlob              = 100,
      kID_Transform             = 101,
      kID_ColorGradient         = 102
    };
  }
}
