/*****************************************************************************/
/**
 * @file    geEvent.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/22
 * @brief   Templates and Classes for the creating on Event objects
 *
 * Thread safe Event object with callbacks for disconnection
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
#include "gePrerequisitesUtilities.h"

namespace geEngineSDK {
  using std::function;
  using std::forward;

  /**
   * @brief Data common to all event connections.
   */
  class BaseConnectionData
  {
   public:
    BaseConnectionData() = default;

    virtual ~BaseConnectionData() {
      GE_ASSERT(!m_handleLinks && !m_isActive);
    }

    virtual void
    deactivate() {
      m_isActive = false;
    }

   public:
    BaseConnectionData* m_prev = nullptr;
    BaseConnectionData* m_next = nullptr;
    bool m_isActive = true;
    uint32 m_handleLinks = 0;
  };

  /**
   * @brief Internal data for an Event, storing all connections.
   */
  struct EventInternalData
  {
    EventInternalData() = default;

    ~EventInternalData() {
      BaseConnectionData* conn = m_connections;
      while (nullptr != conn) {
        BaseConnectionData* next = conn->m_next;
        ge_free(conn);
        conn = next;
      }

      conn = m_freeConnections;
      while (nullptr != conn) {
        BaseConnectionData* next = conn->m_next;
        ge_free(conn);
        conn = next;
      }

      conn = m_newConnections;
      while (nullptr != conn) {
        BaseConnectionData* next = conn->m_next;
        ge_free(conn);
        conn = next;
      }
    }

    /**
     * @brief Appends a new connection to the active connection array.
     */
    void
    connect(BaseConnectionData* conn) {
      conn->m_prev = m_lastConnection;

      if (nullptr != m_lastConnection) {
        m_lastConnection->m_next = conn;
      }

      m_lastConnection = conn;

      //First connection
      if (nullptr == m_connections) {
        m_connections = conn;
      }
    }

    /**
     * @brief Disconnects the connection with the specified data, ensuring the
     *        event doesn't call its callback again.
     * @note  Only call this once.
     */
    void
    disconnect(BaseConnectionData* conn) {
      RecursiveLock lock(m_mutex);
      conn->deactivate();
      tryFree(conn);
    }

    /**
     * @brief Disconnects all connections in the event.
     */
    void
    clear() {
      RecursiveLock lock(m_mutex);

      BaseConnectionData* conn = m_connections;
      while (nullptr  != conn) {
        BaseConnectionData* next = conn->m_next;
        conn->deactivate();

        if (0 == conn->m_handleLinks) {
          free(conn);
        }

        conn = next;
      }

      m_connections = nullptr;
      m_lastConnection = nullptr;
    }

    /**
     * @brief Called when the event handle no longer keeps a reference to the
     *        connection data. This means we might be able to free (and reuse)
     *        its memory if the event is done with it too.
     */
    void
    freeHandle(BaseConnectionData* conn) {
      RecursiveLock lock(m_mutex);
      tryFree(conn);
    }

    /**
     * @brief Releases connection data and makes it available for re-use when
     *        next connection is formed.
     */
    void
    free(BaseConnectionData* conn) {
      if (nullptr != conn->m_prev) {
        conn->m_prev->m_next = conn->m_next;
      }
      else {
        m_connections = conn->m_next;
      }

      if (nullptr != conn->m_next) {
        conn->m_next->m_prev = conn->m_prev;
      }
      else {
        m_lastConnection = conn->m_prev;
      }

      conn->m_prev = nullptr;
      conn->m_next = nullptr;

      if (nullptr != m_freeConnections) {
        conn->m_next = m_freeConnections;
        m_freeConnections->m_prev = conn;
      }

      m_freeConnections = conn;
      m_freeConnections->~BaseConnectionData();
    }

   private:
    void
    tryFree(BaseConnectionData* conn) {
      --conn->m_handleLinks;
      if (0 == conn->m_handleLinks && !conn->m_isActive) {
        free(conn);
      }
    }

   public:
    BaseConnectionData* m_connections = nullptr;
    BaseConnectionData* m_freeConnections = nullptr;
    BaseConnectionData* m_lastConnection = nullptr;
    BaseConnectionData* m_newConnections = nullptr;

    RecursiveMutex m_mutex;
    bool m_isCurrentlyTriggering = false;
  };

  /**
   * @brief Event handle. Allows you to track to which events you subscribed to
   *        and disconnect from them when needed.
   */
  class HEvent
  {
   public:
    HEvent() = default;

    HEvent(const HEvent& e) {
      this->operator=(e);
    }

    explicit HEvent(SPtr<EventInternalData> eventData, BaseConnectionData* connection)
      : m_connection(connection),
        m_eventData(std::move(eventData)) {
      ++connection->m_handleLinks;
    }

    HEvent(HEvent&& other) _NOEXCEPT
      : m_connection(other.m_connection),
        m_eventData(std::move(other.m_eventData)) {
      other.m_connection = nullptr;
    }

    HEvent&
    operator=(HEvent&& other) _NOEXCEPT {
      if (this != &other) {
        if (m_connection) {
          m_eventData->freeHandle(m_connection);
        }

        m_connection = other.m_connection;
        m_eventData = std::move(other.m_eventData);
        other.m_connection = nullptr;
      }
      return *this;
    }

    ~HEvent() {
      if (nullptr != m_connection) {
        m_eventData->freeHandle(m_connection);
      }
    }

    /**
     * @brief Disconnect from the event you are subscribed to.
     */
    void
    disconnect() {
      if (nullptr != m_connection) {
        m_eventData->disconnect(m_connection);
        m_connection = nullptr;
        m_eventData = nullptr;
      }
    }

    struct Bool_struct
    {
      int _member;
    };

    /**
     * @brief Allows direct conversion of a handle to bool.
     * @note  Additional struct is needed because we can't directly convert to
     *        bool since then we can assign pointer to bool and that's wrong.
     */
    operator int
    Bool_struct::*() const {
      return (m_connection != nullptr ? &Bool_struct::_member : 0);
    }

    HEvent& operator=(const HEvent& rhs) {
      if (this == &rhs) {
        return *this;
      }

      m_connection = rhs.m_connection;
      m_eventData = rhs.m_eventData;

      if (nullptr != m_connection) {
        ++m_connection->m_handleLinks;
      }
      return *this;
    }

   private:
    BaseConnectionData* m_connection = nullptr;
    SPtr<EventInternalData> m_eventData;
  };

  /**
   * @brief Events allows you to register method callbacks that get notified
   *        when the event is triggered.
   * @note  Callback method return value is ignored.
   */
  //Note: I could create a policy template argument that allows creation of 
  //lockable and non-lockable events in the case mutex is causing too much overhead.
  template <class RetType, class... Args>
  class TEvent
  {
   private:
    struct ConnectionData : BaseConnectionData
    {
     public:
      void
      deactivate() override {
        m_func = nullptr;
        BaseConnectionData::deactivate();
      }
      
      function<RetType(Args...)> m_func;
    };

   public:
    TEvent() : m_internalData(ge_shared_ptr_new<EventInternalData>()) {}

    ~TEvent() {
      clear();
    }

    /**
     * @brief Register a new callback that will get notified once the event is triggered.
     * @param  func Callback method to be called when the event is triggered.
     * @return Handle to the event. This handle can be used to disconnect from the event.
     */
    HEvent
    connect(function<RetType(Args...)> func) {
      RecursiveLock lock(m_internalData->m_mutex);

      ConnectionData* connData = nullptr;
      if (nullptr != m_internalData->m_freeConnections) {
        connData = static_cast<ConnectionData*>(m_internalData->m_freeConnections);
        m_internalData->m_freeConnections = connData->m_next;

        new (connData)ConnectionData();
        if (nullptr != connData->m_next) {
          connData->m_next->m_prev = nullptr;
        }

        connData->m_isActive = true;
      }

      if (nullptr == connData) {
        connData = ge_new<ConnectionData>();
      }

      //If currently iterating over the connection list, delay modifying it until done
      if (m_internalData->m_isCurrentlyTriggering) {
        connData->m_prev = m_internalData->m_newConnections;

        if (nullptr != m_internalData->m_newConnections) {
          m_internalData->m_newConnections->m_next = connData;
        }

        m_internalData->m_newConnections = connData;
      }
      else
      {
        m_internalData->connect(connData);
      }
      
      connData->m_func = func;

      return HEvent(m_internalData, connData);
    }

    /**
     * @brief Trigger the event, notifying all register callback methods.
     */
    void
    operator()(Args... args) {
      //Increase ref count to ensure this event data isn't destroyed if one of
      //the callbacks deletes the event itself.
      SPtr<EventInternalData> internalData = m_internalData;

      RecursiveLock lock(internalData->m_mutex);
      internalData->m_isCurrentlyTriggering = true;

      ConnectionData* conn = static_cast<ConnectionData*>(internalData->m_connections);
      while (nullptr != conn) {
        //Save next here in case the callback itself disconnects this connection
        ConnectionData* next = static_cast<ConnectionData*>(conn->m_next);

        if (nullptr != conn->m_func) {
          conn->m_func(std::forward<Args>(args)...);
        }

        conn = next;
      }

      internalData->m_isCurrentlyTriggering = false;

      //If any new connections were added during the above calls,
      //add them to the connection list
      if (nullptr != internalData->m_newConnections) {
        BaseConnectionData* lastNewConnection = internalData->m_newConnections;
        while (nullptr != lastNewConnection) {
          lastNewConnection = lastNewConnection->m_next;
        }

        BaseConnectionData* currentConnection = lastNewConnection;
        while (nullptr != currentConnection) {
          BaseConnectionData* prevConnection = currentConnection->m_prev;
          currentConnection->m_next = nullptr;
          currentConnection->m_prev = nullptr;

          m_internalData->connect(currentConnection);
          currentConnection = prevConnection;
        }

        internalData->m_newConnections = nullptr;
      }
    }

    /**
     * @brief Clear all callbacks from the event.
     */
    void
    clear() {
      m_internalData->clear();
    }

    /**
     * @brief Check if event has any callbacks registered.
     * @note  It is safe to trigger an event even if no callbacks are registered.
     */
    bool
    empty() {
      RecursiveLock lock(m_internalData->m_mutex);
      return m_internalData->m_connections == nullptr;
    }

   private:
    SPtr<EventInternalData> m_internalData;
  };

  /***************************************************************************/
  /**                       SPECIALIZATIONS                                  */
  /**   SO YOU MAY USE FUNCTION LIKE SYNTAX FOR DECLARING EVENT SIGNATURE    */
  /***************************************************************************/

  /**
   * @copydoc TEvent
   */
  template<typename Signature>
  class Event;

  /**
   * @copydoc TEvent
   */
  template<class RetType, class... Args>
  class Event<RetType(Args...) > : public TEvent<RetType, Args...>
  { };
}
