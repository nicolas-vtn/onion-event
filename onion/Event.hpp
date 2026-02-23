#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <tuple>
#include <vector>

namespace onion
{
	/// @brief Represents a handle to an event subscription. Subscribed function won't be called anymore when the handle goes out of scope.
	class EventHandler
	{
	  public:
		template <typename EventArgs> friend class Event;

	  public:
		EventHandler(const EventHandler&) = default;
		EventHandler(EventHandler&&) noexcept = default;
		EventHandler& operator=(const EventHandler&) = default;
		EventHandler& operator=(EventHandler&&) noexcept = default;

	  protected:
		struct Token
		{
		};
		EventHandler() = delete;
		explicit EventHandler(std::shared_ptr<Token> handler) : m_handler(std::move(handler)) {}

	  private:
		std::shared_ptr<Token> m_handler;
	};

	/// @brief Generic event class that allows subscribing to, unsubscribing from, and triggering events with specific argument types.
	/// @tparam EventArgs The type of the event arguments that will be passed to handlers when the event is triggered.
	template <typename EventArgs> class Event
	{
	  public:
		/// @brief Subscribes a handler to the event. The handler will be invoked with the specified EventArgs when the event is triggered.
		/// The returned EventHandler is used as a token to manage the subscription's lifecycle.
		/// @param handler The handler function to be invoked when the event is triggered.
		/// @return An EventHandler that is used to manage the subscription's lifecycle.
		[[nodiscard]] EventHandler Subscribe(const std::function<void(const EventArgs&)>& handler)
		{
			// Store the handler with a weak pointer to the handler ID
			std::shared_ptr<EventHandler::Token> tokenPtr = std::make_shared<EventHandler::Token>();

			// Lock the mutex to safely modify the handlers vector
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_handlers.emplace_back(tokenPtr, handler);
			}

			// Clear expired handlers to keep the handlers vector clean
			ClearExpired();

			return EventHandler(tokenPtr);
		}

		/// @brief Unsubscribes a handler from the event using the provided EventHandler.
		/// @param eventHandler The EventHandler representing the subscription to be removed.
		void Unsubscribe(const EventHandler& eventHandler)
		{
			// Extract the handler ID from the EventHandler
			std::shared_ptr<EventHandler::Token> handlerId = eventHandler.m_handler;

			// Remove the handler with the matching ID
			std::lock_guard<std::mutex> lock(m_mutex);
			m_handlers.erase(std::remove_if(m_handlers.begin(),
											m_handlers.end(),
											[handlerId](const auto& tuple)
											{ return std::get<0>(tuple).lock() == handlerId; }),
							 m_handlers.end());
		}

		/// @brief Triggers the event, invoking all subscribed handlers with the provided EventArgs. Invokes handlers in the same thread that calls this method.
		/// @param args The event arguments to be passed to each handler when the event is triggered.
		void Trigger(const EventArgs& args) const
		{
			// Create a temporary copy of the handlers to avoid holding the lock while invoking them
			std::vector<std::tuple<std::weak_ptr<EventHandler::Token>, std::function<void(const EventArgs&)>>>
				tmpHandlers;
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				tmpHandlers = m_handlers;
			}

			// Invoke handlers outside the lock to prevent potential deadlocks
			for (const auto& [handlerId, handler] : tmpHandlers)
			{
				if (auto lockedHandlerId = handlerId.lock())
				{
					handler(args);
				}
			}
		}

		/// @brief Clears all handlers from the event, effectively unsubscribing all subscribers.
		void Clear()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_handlers.clear();
		}

		/// @brief Clears all expired handlers from the event, removing all handlers that have gone out of scope.
		void ClearExpired()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_handlers.erase(std::remove_if(m_handlers.begin(),
											m_handlers.end(),
											[](const auto& tuple) { return std::get<0>(tuple).expired(); }),
							 m_handlers.end());
		}

	  private:
		/// @brief Mutex to protect access to the handlers vector, ensuring thread safety when subscribing, unsubscribing, and triggering events.
		mutable std::mutex m_mutex;

		/// @brief Storage for event handlers, using a weak pointer to the handler ID to allow for automatic cleanup when handlers are unsubscribed or go out of scope.
		std::vector<std::tuple<std::weak_ptr<EventHandler::Token>, std::function<void(const EventArgs&)>>> m_handlers;
	};
} // namespace onion
