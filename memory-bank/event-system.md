# Event System for Component Communication

**Last Updated:** April 28, 2025

## Overview

This document describes the event system implementation for the Quantum Mechanics Simulation project, part of Sprint 2. The event system enables loose coupling between components through a publish-subscribe (observer) pattern, allowing different parts of the application to communicate without direct dependencies.

## Design Goals

1. **Loose Coupling**: Components communicate through events rather than direct method calls
2. **Flexibility**: Components can subscribe to multiple event types and react accordingly
3. **Extensibility**: New event types can be added without modifying existing components
4. **Debugging**: Events provide a traceable history of application behavior
5. **Testability**: Components can be tested in isolation with mock event publishers/subscribers

## Core Components

### Event Base Class

The `Event` class serves as the base for all events in the system:

```cpp
class Event {
public:
    Event(EventType type, const std::string& name);
    virtual ~Event() = default;
    EventType getType() const;
    const std::string& getName() const;
    const std::chrono::steady_clock::time_point& getTimestamp() const;
    virtual std::string toString() const;
    
private:
    EventType m_type;
    std::string m_name;
    std::chrono::steady_clock::time_point m_timestamp;
};

// Convenience typedef for shared pointers to events
using EventPtr = std::shared_ptr<Event>;
```

**Key Features**:
- **Type**: Enum value identifying the event category
- **Name**: Human-readable event identifier
- **Timestamp**: When the event was created
- **toString()**: Virtual method for string representation (useful for logging)

### Event Types

The `EventType` enum defines all possible event types in the application:

```cpp
enum class EventType {
    // Simulation events
    SimulationStarted,
    SimulationPaused,
    SimulationStepped,
    SimulationReset,
    
    // Wavefunction events
    WavefunctionUpdated,
    WavefunctionReset,
    WavefunctionNormalized,
    
    // Potential events
    PotentialChanged,
    
    // Configuration events
    ConfigurationUpdated,
    
    // UI events
    UIParameterChanged,
    
    // Visualization events
    RenderingStarted,
    RenderingCompleted,
    
    // System events
    ApplicationStarted,
    ApplicationExiting,
    ErrorOccurred
};
```

Events are organized into logical categories (simulation, wavefunction, UI, etc.) for clarity.

### EventBus

The `EventBus` class implements the publisher-subscriber pattern:

```cpp
class EventBus {
public:
    EventBus() = default;
    
    void subscribe(EventType eventType, std::shared_ptr<IEventHandler> handler);
    void unsubscribe(EventType eventType, std::shared_ptr<IEventHandler> handler);
    std::shared_ptr<IEventHandler> subscribeCallback(EventType eventType, 
                                                    std::function<bool(const EventPtr&)> callback);
    void publish(const EventPtr& event);
    
    std::vector<EventPtr> getEventHistory() const;
    void clearEventHistory();
    void setMaxHistorySize(size_t size);
    
private:
    // Internal callback handler implementation
    class CallbackEventHandler;
    
    // Maps event types to handlers
    std::unordered_map<EventType, std::vector<std::shared_ptr<IEventHandler>>> m_subscribers;
    
    // Event history for debugging and visualization
    std::deque<EventPtr> m_eventHistory;
    size_t m_maxHistorySize = 100;
    
    // Thread safety
    mutable std::mutex m_mutex;
};
```

**Key Features**:
- **subscribe/unsubscribe**: Register/unregister event handlers for specific event types
- **subscribeCallback**: Convenient method to subscribe using a lambda function
- **publish**: Send an event to all subscribers
- **Event History**: Maintains a history of recent events for debugging
- **Thread Safety**: Mutex protection for multi-threaded access

### IEventHandler Interface

The `IEventHandler` interface must be implemented by classes that want to handle events:

```cpp
class IEventHandler {
public:
    virtual ~IEventHandler() = default;
    virtual bool handleEvent(const EventPtr& event) = 0;
};
```

Components implement this interface to receive and process events.

### Concrete Event Classes

The system includes concrete event classes for each event type, organized by category:

#### Simulation Events

```cpp
class SimulationEvent : public Event {
    // Base class for simulation events
};

class SimulationStartedEvent : public SimulationEvent {
    // Fired when simulation starts
};

class SimulationSteppedEvent : public SimulationEvent {
    // Fired after each simulation step
    double getTime() const;
    double getDt() const; 
    double getTotalProbability() const;
};

// ...and others
```

#### Wavefunction Events

```cpp
class WavefunctionEvent : public Event {
    // Base class for wavefunction events
};

class WavefunctionUpdatedEvent : public WavefunctionEvent {
    // Fired when wavefunction changes
};

class WavefunctionResetEvent : public WavefunctionEvent {
    // Fired when wavefunction is reset
    // Contains wavepacket parameters
};

// ...and others
```

#### Visualization Events

```cpp
class VisualizationEvent : public Event {
    // Base class for visualization events
};

class RenderingStartedEvent : public VisualizationEvent {
    // Fired when rendering begins
};

class RenderingCompletedEvent : public VisualizationEvent {
    // Fired when rendering completes
    double getRenderTimeMs() const;
};
```

### Event Creation Helper

A template function simplifies event creation:

```cpp
template <typename T, typename... Args>
std::shared_ptr<T> makeEvent(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}
```

This allows for concise event creation with proper forwarding of constructor arguments.

## Integration with Components

### Event Publishers

Components that publish events typically:

1. Store a shared pointer to the EventBus
2. Create events using the makeEvent template
3. Publish events at appropriate times

Example from SimulationEngine:

```cpp
// In constructor
SimulationEngine::SimulationEngine(const PhysicsConfig& config, std::shared_ptr<EventBus> eventBus)
    : m_eventBus(eventBus)
{
    // ...initialization...
    
    // Publish event on construction
    if (m_eventBus) {
        m_eventBus->publish(makeEvent<SimulationStartedEvent>());
        DEBUG_LOG("SimulationEngine", "Published SimulationStarted event");
    }
}

// In step method
void SimulationEngine::step() {
    // ...simulation logic...
    
    // Publish events after step
    if (m_eventBus) {
        double totalProbability = getTotalProbability();
        m_eventBus->publish(makeEvent<SimulationSteppedEvent>(m_currentTime, m_dt, totalProbability));
        m_eventBus->publish(makeEvent<WavefunctionUpdatedEvent>());
    }
}
```

### Event Subscribers

Components that subscribe to events:

1. Implement the IEventHandler interface
2. Subscribe to relevant event types in their initialization
3. Implement handleEvent to process events
4. Unsubscribe in their destructor

Example from VisualizationEngine:

```cpp
// In header file
class VisualizationEngine : public IVisualizationEngine, 
                           public std::enable_shared_from_this<VisualizationEngine>,
                           public IEventHandler {
public:
    // ...other methods...
    
    // IEventHandler implementation
    bool handleEvent(const EventPtr& event) override;
    
private:
    std::shared_ptr<EventBus> m_eventBus;
};

// In constructor
VisualizationEngine::VisualizationEngine(int width, int height, std::shared_ptr<EventBus> eventBus)
    : m_width(width), m_height(height), m_eventBus(eventBus)
{
    // Subscribe to events
    if (m_eventBus) {
        m_eventBus->subscribe(EventType::WavefunctionUpdated, shared_from_this());
        m_eventBus->subscribe(EventType::PotentialChanged, shared_from_this());
    }
}

// In destructor
VisualizationEngine::~VisualizationEngine() {
    // Unsubscribe from events
    if (m_eventBus) {
        m_eventBus->unsubscribe(EventType::WavefunctionUpdated, shared_from_this());
        m_eventBus->unsubscribe(EventType::PotentialChanged, shared_from_this());
    }
}

// Event handler implementation
bool VisualizationEngine::handleEvent(const EventPtr& event) {
    // Process events based on type
    switch(event->getType()) {
        case EventType::WavefunctionUpdated:
            // Update visualization
            return true;
            
        case EventType::PotentialChanged:
            // Update potential visualization
            auto potentialEvent = std::dynamic_pointer_cast<PotentialChangedEvent>(event);
            if (potentialEvent) {
                // Use event data
                updatePotentialVisualization(potentialEvent->getPotentialType());
            }
            return true;
            
        default:
            return false;
    }
}
```

## UIManager Event Monitoring

The UIManager component includes a special event monitor panel that displays recent events:

```cpp
void UIManager::renderEventMonitor() {
    if (ImGui::Begin("Event Monitor")) {
        if (m_eventBus) {
            auto events = m_eventBus->getEventHistory();
            
            if (ImGui::Button("Clear")) {
                m_eventBus->clearEventHistory();
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Toggle Auto-scroll")) {
                m_autoScrollEvents = !m_autoScrollEvents;
            }
            
            ImGui::Separator();
            
            if (ImGui::BeginChild("EventList", ImVec2(0, 0), true)) {
                for (const auto& event : events) {
                    ImGui::TextWrapped("%s", event->toString().c_str());
                    ImGui::Separator();
                }
                
                if (m_autoScrollEvents && !events.empty()) {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
            ImGui::EndChild();
        } else {
            ImGui::Text("Event bus not available");
        }
    }
    ImGui::End();
}
```

This provides a real-time view of events flowing through the application, aiding in debugging and understanding component interactions.

## Application Startup Events

The application main function initializes the event system and publishes startup events:

```cpp
int main(int argc, char** argv) {
    // ...initialization...
    
    // Create and register event bus
    auto eventBus = std::make_shared<EventBus>();
    serviceContainer.registerInstance<EventBus, EventBus>(eventBus);
    
    // Create components with event bus
    auto simulationEngine = std::make_shared<SimulationEngine>(config, eventBus);
    auto visualizationEngine = std::make_shared<VisualizationEngine>(width, height, eventBus);
    auto uiManager = std::make_shared<UIManager>(eventBus);
    
    // Register components in service container
    serviceContainer.registerInstance<ISimulationEngine, SimulationEngine>(simulationEngine);
    serviceContainer.registerInstance<IVisualizationEngine, VisualizationEngine>(visualizationEngine);
    serviceContainer.registerInstance<IUIManager, UIManager>(uiManager);
    
    // ...component initialization...
    
    // Publish application started event
    eventBus->publish(makeEvent<ApplicationStartedEvent>());
    
    // ...main loop...
    
    // Publish application exiting event
    eventBus->publish(makeEvent<ApplicationExitingEvent>());
    
    // ...cleanup...
}
```

## Event System Best Practices

### 1. Event Creation and Publishing

- **Use makeEvent helper**: Always use the `makeEvent<T>()` helper for creating events
- **Null Checks**: Always check if the event bus is available before publishing
- **Appropriate Granularity**: Publish events at the right level of granularity (too many events can cause overhead)
- **Meaningful Data**: Include relevant data in events to avoid subscribers needing to fetch it elsewhere

Example:
```cpp
if (m_eventBus) {
    // Good: Include relevant data in the event
    m_eventBus->publish(makeEvent<SimulationSteppedEvent>(m_currentTime, m_dt, getTotalProbability()));
    
    // Avoid empty events when data is relevant
    // Not recommended: m_eventBus->publish(makeEvent<SimulationSteppedEvent>());
}
```

### 2. Event Handling

- **Use shared_from_this()**: Components that handle events should inherit from `std::enable_shared_from_this`
- **Type Safety**: Use dynamic_pointer_cast to safely access event-specific data
- **Event Filtering**: Only handle events that are relevant to the component
- **Error Handling**: Catch exceptions in event handlers to prevent crashes

Example:
```cpp
bool MyComponent::handleEvent(const EventPtr& event) {
    try {
        switch(event->getType()) {
            case EventType::SimulationStepped: {
                auto steppedEvent = std::dynamic_pointer_cast<SimulationSteppedEvent>(event);
                if (steppedEvent) {
                    // Use event data safely
                    updateWithNewTime(steppedEvent->getTime());
                }
                return true;
            }
            // Other cases...
            default:
                return false;
        }
    }
    catch (const std::exception& e) {
        DEBUG_LOG("MyComponent", "Error handling event: " + std::string(e.what()));
        return false;
    }
}
```

### 3. Subscription Management

- **Clean Unsubscription**: Always unsubscribe handlers in the destructor
- **Scoped Subscriptions**: Consider helper classes for automatic subscription/unsubscription
- **Avoid Circular References**: Be careful with shared_ptr capturing in lambda subscriptions

Example:
```cpp
class ScopedSubscription {
public:
    ScopedSubscription(std::shared_ptr<EventBus> eventBus, 
                      EventType eventType,
                      std::shared_ptr<IEventHandler> handler)
        : m_eventBus(eventBus), m_eventType(eventType), m_handler(handler)
    {
        if (m_eventBus && m_handler) {
            m_eventBus->subscribe(m_eventType, m_handler);
        }
    }
    
    ~ScopedSubscription() {
        if (m_eventBus && m_handler) {
            m_eventBus->unsubscribe(m_eventType, m_handler);
        }
    }
    
private:
    std::shared_ptr<EventBus> m_eventBus;
    EventType m_eventType;
    std::shared_ptr<IEventHandler> m_handler;
};
```

### 4. Debugging with Events

- **Use Event History**: The event history in EventBus is valuable for debugging
- **Detailed toString()**: Implement informative toString() methods for each event type
- **Consistent Logging**: Use DEBUG_LOG when publishing and handling events

Example for Debugging:
```cpp
// In event class
std::string MyEvent::toString() const override {
    return getName() + " - Parameter1: " + std::to_string(m_param1) +
           ", Parameter2: " + std::to_string(m_param2);
}

// When publishing
DEBUG_LOG("MyComponent", "Publishing MyEvent with params: " + 
          std::to_string(param1) + ", " + std::to_string(param2));

// When handling
DEBUG_LOG("MyComponent", "Handling event: " + event->toString());
```

## Testing the Event System

### Unit Testing

Example unit tests for the event system:

```cpp
TEST_CASE("EventBus basic functionality") {
    auto eventBus = std::make_shared<EventBus>();
    
    // Mock event handler
    class MockHandler : public IEventHandler {
    public:
        bool handleCalled = false;
        EventPtr lastEvent;
        
        bool handleEvent(const EventPtr& event) override {
            handleCalled = true;
            lastEvent = event;
            return true;
        }
    };
    
    auto handler = std::make_shared<MockHandler>();
    
    // Subscribe and publish
    eventBus->subscribe(EventType::SimulationStarted, handler);
    auto event = makeEvent<SimulationStartedEvent>();
    eventBus->publish(event);
    
    REQUIRE(handler->handleCalled);
    REQUIRE(handler->lastEvent == event);
    
    // Unsubscribe and publish again
    handler->handleCalled = false;
    eventBus->unsubscribe(EventType::SimulationStarted, handler);
    eventBus->publish(makeEvent<SimulationStartedEvent>());
    
    REQUIRE_FALSE(handler->handleCalled);
}
```

### Integration Testing

Integration tests should verify that components communicate correctly via events:

```cpp
TEST_CASE("SimulationEngine and VisualizationEngine event integration") {
    auto eventBus = std::make_shared<EventBus>();
    auto config = PhysicsConfig{}; // Initialize with test values
    
    auto simEngine = std::make_shared<SimulationEngine>(config, eventBus);
    auto visEngine = std::make_shared<VisualizationEngine>(256, 256, eventBus);
    
    // Mock window for initialization
    GLFWwindow* mockWindow = nullptr; // In real tests, use a test window
    
    // Initialize visualization engine
    REQUIRE(visEngine->initialize(mockWindow));
    
    // Track visualization updates
    bool visualizationUpdated = false;
    auto callback = eventBus->subscribeCallback(EventType::RenderingStarted, 
        [&visualizationUpdated](const EventPtr&) {
            visualizationUpdated = true;
            return true;
        });
    
    // Step simulation - should trigger WavefunctionUpdated event
    // which should cause VisualizationEngine to render
    simEngine->step();
    
    // Verify visualization was updated
    REQUIRE(visualizationUpdated);
}
```

## Benefits and Future Enhancements

### Benefits of the Event System

The event system provides several key benefits:

1. **Loose Coupling**: Components can communicate without direct dependencies
2. **Flexibility**: Components can be added or removed without affecting others
3. **Debugging**: Event history provides visibility into application behavior
4. **Testability**: Components can be tested in isolation
5. **Extensibility**: New event types can be added without modifying existing code

### Future Enhancements

Potential enhancements for the event system:

1. **Event Filtering**: Add filtering capabilities to subscribers
2. **Event Priorities**: Implement priority levels for event handling
3. **Event Batching**: Batch related events for improved performance
4. **Async Events**: Process events asynchronously in a separate thread
5. **Event Serialization**: Serialize events for logging or replay
6. **Event Visualization**: Enhance the event monitor with visualization tools

### Recommendations for Improvement

1. Enable Shared From This: Components that implement IEventHandler should also inherit from std::enable_shared_from_this<> to safely subscribe to events using shared_from_this(). This prevents potential memory issues with shared pointers.

2. Exception Handling: Add try-catch blocks in event handlers to prevent exceptions from crashing the application.

3. Event Filtering: Consider adding filtering capabilities to event subscribers to reduce unnecessary event processing.

4. Testing: Develop unit and integration tests specifically for the event system to ensure reliability.

## Conclusion

The event system implementation provides a robust foundation for component communication in the Quantum Simulator application. It enables loose coupling between components, improves testability, and enhances debugging capabilities. The system follows best practices for event-based architectures and is well-integrated with the application's other architectural patterns, particularly the interface-based design.