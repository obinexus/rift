@startuml
package "Backtracking Management" {
  class BacktrackingController {
    - configProvider: BacktrackLimitConfigProvider
    - timeTracker: ExecutionTimeTracker
    - depthTracker: DepthTracker
    - bailoutStrategy: BailoutStrategyManager
    + checkLimits(): boolean
    + registerBacktrackPoint(): boolean
  }
  
  interface BacktrackLimitConfigProvider {
    + getMaxDepth(): uint32_t
    + getMaxDuration(): uint64_t
    + getMaxTransitions(): uint64_t
    + getScope(): BacktrackLimitScope
  }
  
  class GlobalConfigProvider {
    - maxDepth: uint32_t
    - maxDuration: uint64_t
    - maxTransitions: uint64_t
  }
  
  class PatternConfigProvider {
    - patternId: uint32_t
    - overrideGlobal: boolean
    - maxDepth: uint32_t
    - maxDuration: uint64_t
    - maxTransitions: uint64_t
  }
  
  class MatchConfigProvider {
    - matchId: uint32_t
    - overridePattern: boolean
    - maxDepth: uint32_t
    - maxDuration: uint64_t
    - maxTransitions: uint64_t
  }
  
  class ExecutionTimeTracker {
    - startTime: uint64_t
    - maxDuration: uint64_t
    + start(): void
    + hasExceededLimit(): boolean
    + getElapsedTime(): uint64_t
  }
  
  class DepthTracker {
    - currentDepth: uint32_t
    - maxDepth: uint32_t
    + increment(): boolean
    + decrement(): void
    + getCurrentDepth(): uint32_t
  }
  
  class BailoutStrategyManager {
    - strategies: List<BailoutStrategy>
    - activeStrategy: BailoutStrategy
    + evaluateStrategies(): BailoutStrategy
    + executeStrategy(): boolean
  }
  
  interface BailoutStrategy {
    + shouldActivate(): boolean
    + execute(): boolean
    + getPriority(): int
  }
  
  class PatternComplexityStrategy {
    - stateCount: uint32_t
    - branchingFactor: float
    - repetitionCount: uint32_t
  }
  
  class ProgressTrackingStrategy {
    - progressThreshold: float
    - lastProgressPoint: uint32_t
    - stagnationCount: uint32_t
  }
  
  class TimeProgressionStrategy {
    - timeRatioThreshold: float
    - depthTimeRatio: float
  }
  
  enum BacktrackLimitScope {
    GLOBAL
    PATTERN_SPECIFIC
    MATCH_SPECIFIC
  }
}

package "Thread Safety" {
  class ThreadSafeBacktracker {
    - mutex: pthread_mutex_t
    - tlsKey: pthread_key_t
    - backtrackingController: BacktrackingController
    + getLocalBacktracker(): Backtracker
    + push(): boolean
    + pop(): boolean
  }
  
  class ThreadLocalBacktracker {
    - stack: BacktrackPointStack
    - controller: BacktrackingController
    + push(): boolean
    + pop(): boolean
  }
}

package "R'' Syntax Integration" {
  class SyntaxAwareBacktracker {
    - isSyntaxR: boolean
    - syntaxComplexityFactor: float
    + adjustLimitsForSyntax(): void
  }
}

BacktrackLimitConfigProvider <|.. GlobalConfigProvider
BacktrackLimitConfigProvider <|.. PatternConfigProvider
BacktrackLimitConfigProvider <|.. MatchConfigProvider

BailoutStrategy <|.. PatternComplexityStrategy
BailoutStrategy <|.. ProgressTrackingStrategy
BailoutStrategy <|.. TimeProgressionStrategy

BacktrackingController o-- BacktrackLimitConfigProvider
BacktrackingController o-- ExecutionTimeTracker
BacktrackingController o-- DepthTracker
BacktrackingController o-- BailoutStrategyManager

BailoutStrategyManager o-- BailoutStrategy

ThreadSafeBacktracker o-- BacktrackingController
ThreadLocalBacktracker o-- BacktrackingController

SyntaxAwareBacktracker --> BacktrackingController

note right of SyntaxAwareBacktracker
  Adjusts backtracking limits
  based on R'' syntax complexity
end note

note bottom of BailoutStrategyManager
  Selects and applies appropriate
  early termination strategies
end note

note left of ThreadSafeBacktracker
  Provides thread isolation via
  Thread-Local Storage (TLS)
end note
@enduml