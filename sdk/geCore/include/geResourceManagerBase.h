/*****************************************************************************/
/**
 * @file    geResourceManagerBase.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2026/04/16
 * @brief   Resource manager CRTP base template.
 *
 * Shared cache and lifetime management for engine resource managers. Derived
 * managers only implement the resource-specific loading logic and the optional
 * hooks they need.
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
#include "geResource.h"

#include <geModule.h>
#include <geStringID.h>

namespace geEngineSDK {

  template<class TDerived, class TResource>
  class ResourceManagerBase : public Module<TDerived>
  {
   public:
    using ResourceType = TResource;
    using ResourcePtr = SPtr<TResource>;

    ResourcePtr
    load(const Path& filePath,
         bool useCacheIfAvailable = false,
         bool bReload = false) {
      const uint32 id = _resolveResourceID(filePath);

      if (!bReload) {
        if (ResourcePtr resource = _getLoadedResource(id)) {
          return resource;
        }
      }

      ResourcePtr resource = _loadManagedResource(filePath,
                                                  useCacheIfAvailable,
                                                  bReload);
      const bool bUsingFallback = !resource;

      if (!resource) {
        resource = _getFallbackResource(filePath);
      }

      if (!resource) {
        return nullptr;
      }

      if (!bUsingFallback || _shouldCacheFallbackResource(filePath, resource)) {
        resource = _storeLoadedResource(id, resource, bReload);
      }

      if (!bUsingFallback) {
        _notifyResourceLoaded(filePath,
                              resource,
                              useCacheIfAvailable,
                              bReload);
      }

      return resource;
    }

    void
    reload(const Path& filePath) {
      load(filePath, false, true);
    }

    bool
    isLoaded(const Path& filePath) const {
      return nullptr != _getLoadedResource(_resolveResourceID(filePath));
    }

    void
    garbageCollector() {
      Lock lock(m_mutex);
      for (auto it = m_loadedResources.begin(); it != m_loadedResources.end();) {
        const ResourcePtr& resource = it->second;

        if (!resource) {
          it = m_loadedResources.erase(it);
          continue;
        }

        if (_keepManagedResource(resource)) {
          ++it;
          continue;
        }

        if (1 == resource.use_count()) {
          it = m_loadedResources.erase(it);
          continue;
        }

        ++it;
      }
    }

    SIZE_T
    getMemoryUsage() const {
      Lock lock(m_mutex);

      SIZE_T totalSize = 0;
      for (const auto& resourceEntry : m_loadedResources) {
        if (resourceEntry.second) {
          totalSize += _getManagedResourceMemoryUsage(resourceEntry.second);
        }
      }

      return totalSize;
    }

   protected:
    ResourcePtr
    _getLoadedResource(const Path& filePath) const {
      return _getLoadedResource(_resolveResourceID(filePath));
    }

    ResourcePtr
    _getLoadedResource(uint32 resourceID) const {
      Lock lock(m_mutex);

      auto it = m_loadedResources.find(resourceID);
      if (it != m_loadedResources.end()) {
        return it->second;
      }

      return nullptr;
    }

    void
    _registerLoadedResource(const Path& filePath,
                            const ResourcePtr& resource) {
      _registerLoadedResource(_resolveResourceID(filePath), resource);
    }

    void
    _registerLoadedResource(uint32 resourceID,
                            const ResourcePtr& resource) {
      Lock lock(m_mutex);
      m_loadedResources[resourceID] = resource;
    }

    void
    _clearLoadedResources() {
      Lock lock(m_mutex);
      m_loadedResources.clear();
    }

   protected:
    UnorderedMap<uint32, ResourcePtr> m_loadedResources;
    mutable Mutex m_mutex;

   private:
    TDerived&
    derived() {
      return static_cast<TDerived&>(*this);
    }

    const TDerived&
    derived() const {
      return static_cast<const TDerived&>(*this);
    }

    uint32
    _resolveResourceID(const Path& filePath) const {
      if constexpr (requires(const TDerived& manager, const Path& path) {
        manager._getResourceID(path);
      }) {
        return derived()._getResourceID(filePath);
      }

      return StringID(filePath.toString()).id();
    }

    ResourcePtr
    _loadManagedResource(const Path& filePath,
                         bool useCacheIfAvailable,
                         bool bReload) {
      static_assert(requires(TDerived& manager, const Path& path) {
        manager._loadResource(path, false, false);
      },
                    "Resource managers must implement _loadResource().");

      return derived()._loadResource(filePath, useCacheIfAvailable, bReload);
    }

    ResourcePtr
    _getFallbackResource(const Path& filePath) const {
      if constexpr (requires(const TDerived& manager, const Path& path) {
        manager._getFallbackResource(path);
      }) {
        return derived()._getFallbackResource(filePath);
      }

      return nullptr;
    }

    bool
    _reuseManagedResource(const ResourcePtr& current,
                          const ResourcePtr& incoming,
                          bool bReload) const {
      if constexpr (requires(const TDerived& manager,
                             const ResourcePtr& currentResource,
                             const ResourcePtr& incomingResource,
                             bool reload) {
        manager._reuseLoadedResource(currentResource, incomingResource, reload);
      }) {
        return derived()._reuseLoadedResource(current, incoming, bReload);
      }

      return false;
    }

    bool
    _shouldCacheFallbackResource(const Path& filePath,
                                 const ResourcePtr& resource) const {
      if constexpr (requires(const TDerived& manager,
                             const Path& path,
                             const ResourcePtr& fallbackResource) {
        manager._cacheFallbackResource(path, fallbackResource);
      }) {
        return derived()._cacheFallbackResource(filePath, resource);
      }

      return false;
    }

    bool
    _keepManagedResource(const ResourcePtr& resource) const {
      if constexpr (requires(const TDerived& manager,
                             const ResourcePtr& managedResource) {
        manager._keepLoadedResource(managedResource);
      }) {
        return derived()._keepLoadedResource(resource);
      }

      return false;
    }

    SIZE_T
    _getManagedResourceMemoryUsage(const ResourcePtr& resource) const {
      if constexpr (requires(const TDerived& manager,
                             const ResourcePtr& managedResource) {
        manager._getResourceMemoryUsage(managedResource);
      }) {
        return derived()._getResourceMemoryUsage(resource);
      }

      return resource->getMemoryUsage();
    }

    void
    _notifyResourceLoaded(const Path& filePath,
                          const ResourcePtr& resource,
                          bool useCacheIfAvailable,
                          bool bReload) {
      if constexpr (requires(TDerived& manager,
                             const Path& path,
                             const ResourcePtr& managedResource,
                             bool useCache,
                             bool reload) {
        manager._onResourceLoaded(path,
                                  managedResource,
                                  useCache,
                                  reload);
      }) {
        derived()._onResourceLoaded(filePath,
                                    resource,
                                    useCacheIfAvailable,
                                    bReload);
      }
    }

    ResourcePtr
    _storeLoadedResource(uint32 resourceID,
                         const ResourcePtr& resource,
                         bool bReload) {
      if (!resource) {
        return nullptr;
      }

      Lock lock(m_mutex);
      auto it = m_loadedResources.find(resourceID);
      if (it == m_loadedResources.end() || !it->second) {
        m_loadedResources[resourceID] = resource;
        return resource;
      }

      if (it->second == resource) {
        return it->second;
      }

      if (_reuseManagedResource(it->second, resource, bReload)) {
        it->second->moveFrom(*resource);
        return it->second;
      }

      it->second = resource;
      return resource;
    }
  };

}
