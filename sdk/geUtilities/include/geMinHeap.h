/*****************************************************************************/
/**
 * @file    geMinHeap.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/12/07
 * @brief   Binary tree where each nodes is less than or equal to the data in
 *          its node's children.
 *
 * Binary tree where each nodes is less than or equal to the data in its node's
 * children.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDynArray.h"

namespace geEngineSDK {

  /**
   * @brief Nodes for the heap.
   */
  template<class K, class V>
  struct HeapNode
  {
    K key;
    V value;
    uint32 index;
  };

  template<class K, class V>
  class MinHeap
  {
   public:
    MinHeap() = default;

    MinHeap(const MinHeap<K, V>& other) {
      *this = other;
    }

    MinHeap(uint32 elements) {
      resize(elements);
    }

    MinHeap<K, V>&
    operator=(const MinHeap<K, V>& other) {
      m_size = other.m_size;
      m_node = other.m_node;
      m_ptr.resize(other.m_ptr.size());

      for (auto& entry : m_node) {
        m_ptr[entry.index] = &entry;
      }

      return *this;
    }

    HeapNode<K, V>
    operator[](uint32 index) {
      return m_node[index];
    }

    const HeapNode<K, V>
    operator[](uint32 index) const {
      return m_node[index];
    }

    bool
    empty() const {
      return m_size == 0;
    }

    uint32
    size() const {
      return m_size;
    }

    void
    minimum(K& key, V& value) {
      GE_ASSERT(m_size > 0);

      key = m_ptr[0]->key;
      value = m_ptr[0]->value;
    }

    HeapNode<K, V>*
    insert(const K& key, const V& value) {
      if (m_node.size() == m_size) {
        return nullptr;
      }

      int32 child = m_size++;
      HeapNode<K, V>* node = m_ptr[child];

      node->key = key;
      node->value = value;

      while (child > 0) {
        const int32 parent = (child - 1) >> 1;

        if (m_ptr[parent]->value <= value) {
          break;
        }

        m_ptr[child] = m_ptr[parent];
        m_ptr[child]->index = child;

        m_ptr[parent] = node;
        m_ptr[parent]->index = parent;

        child = parent;
      }

      return m_ptr[child];
    }

    void
    erase(K& key, V& value) {
      GE_ASSERT(m_size > 0);

      HeapNode<K, V>* root = m_ptr[0];
      key = root->key;
      value = root->value;

      const int32 last = --m_size;
      HeapNode<K, V>* node = m_ptr[last];

      int32 parent = 0;
      int32 child = 1;

      while (child <= last) {
        if (child < last) {
          const int32 child2 = child + 1;

          if (m_ptr[child2]->value < m_ptr[child]->value) {
            child = child2;
          }
        }

        if (node->value <= m_ptr[child]->value) {
          break;
        }

        m_ptr[parent] = m_ptr[child];
        m_ptr[parent]->index = parent;

        parent = child;
        child = 2 * child + 1;
      }

      m_ptr[parent] = node;
      m_ptr[parent]->index = parent;

      m_ptr[last] = root;
      m_ptr[last]->index = last;
    }

    void
    update(HeapNode<K, V>* node, const V& value) {
      if (!node) {
        return;
      }

      int32 parent = 0;
      int32 child = 0;
      int32 child2 = 0;
      int32 maxChild = 0;

      if (node->value < value) {
        node->value = value;
        parent = node->index;
        child = 2 * parent + 1;

        while (child < m_size) {
          child2 = child + 1;
          if (child2 < m_size) {
            if (m_ptr[child]->value <= m_ptr[child2]->value) {
              maxChild = child;
            }
            else {
              maxChild = child2;
            }
          }
          else {
            maxChild = child;
          }

          if (value <= m_ptr[maxChild]->value) {
            break;
          }

          m_ptr[parent] = m_ptr[maxChild];
          m_ptr[parent]->index = parent;

          m_ptr[maxChild] = node;
          m_ptr[maxChild]->index = maxChild;

          parent = maxChild;
          child = 2 * parent + 1;
        }
      }
      else if (value < node->value) {
        node->value = value;
        child = node->index;

        while (child > 0) {
          parent = (child - 1) >> 1;

          if (m_ptr[parent]->value <= value) {
            break;
          }

          m_ptr[child] = m_ptr[parent];
          m_ptr[child]->index = child;

          m_ptr[parent] = node;
          m_ptr[parent]->index = parent;

          child = parent;
        }
      }
    }

    void
    resize(uint32 elements) {
      m_size = 0;
      if (elements > 0) {
        m_node.resize(elements);
        m_ptr.resize(elements);

        for (uint32 i = 0; i < elements; ++i) {
          m_ptr[i] = &m_node[i];
          m_ptr[i]->index = i;
        }
      }
      else {
        m_node.clear();
        m_ptr.clear();
      }
    }

    bool
    valid() const {
      for (int32 i = 0; i < static_cast<int32>(m_size); ++i) {
        int32 parent = (i - 1) >> 1;
        if (parent > 0) {
          if (m_ptr[i]->value < m_ptr[parent]->value ||
              static_cast<int>(m_ptr[parent]->index) != parent) {
            return false;
          }
        }
      }

      return true;
    }

   private:
    uint32 m_size;
    DynArray<HeapNode<K, V>> m_node;
    DynArray<HeapNode<K, V>*> m_ptr;
  };
}
