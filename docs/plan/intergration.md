@startuml
participant "Regex Matcher" as Matcher
participant "ThreadSafeBacktracker" as TSBacktracker
participant "ThreadLocalBacktracker" as TLBacktracker
participant "BacktrackingController" as Controller
participant "ExecutionTimeTracker" as TimeTracker
participant "DepthTracker" as DepthTracker
participant "BailoutStrategyManager" as BailoutMgr

Matcher -> TSBacktracker: push(state, position)
activate TSBacktracker

TSBacktracker -> TSBacktracker: getLocalBacktracker()
TSBacktracker -> TLBacktracker: push(state, position)
activate TLBacktracker

TLBacktracker -> Controller: registerBacktrackPoint()
activate Controller

Controller -> TimeTracker: hasExceededLimit()
activate TimeTracker
TimeTracker --> Controller: timeLimitResult
deactivate TimeTracker

Controller -> DepthTracker: increment()
activate DepthTracker
DepthTracker --> Controller: depthLimitResult
deactivate DepthTracker

alt limits exceeded
    Controller -> BailoutMgr: evaluateStrategies()
    activate BailoutMgr
    BailoutMgr --> Controller: selectedStrategy
    deactivate BailoutMgr
    
    Controller -> BailoutMgr: executeStrategy()
    activate BailoutMgr
    BailoutMgr --> Controller: bailoutResult
    deactivate BailoutMgr
    
    Controller --> TLBacktracker: false (limit exceeded)
    TLBacktracker --> TSBacktracker: false
    TSBacktracker --> Matcher: false
else limits not exceeded
    Controller --> TLBacktracker: true (point registered)
    TLBacktracker -> TLBacktracker: addToStack(point)
    TLBacktracker --> TSBacktracker: true
    TSBacktracker --> Matcher: true
end

deactivate Controller
deactivate TLBacktracker
deactivate TSBacktracker
@enduml