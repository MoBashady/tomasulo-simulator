# Tomasulo Simulator

An interactive C++/Qt simulator for Tomasulo’s dynamic instruction scheduling algorithm.

The simulator models core concepts of out-of-order execution, including reservation stations, register dependency tracking, reorder buffering, operand broadcasting, functional-unit latencies, branch handling, and in-order commit.

## Features

- Cycle-level Tomasulo simulation
- Configurable reorder buffer size
- Configurable reservation station sizes
- Configurable functional-unit latencies
- Register dependency tracking
- Operand broadcasting
- In-order commit through a reorder buffer
- Branch misprediction recovery
- CALL / RET control-flow support
- Instruction timing analysis
- IPC and branch statistics
- Qt-based graphical interface
- 17 built-in validation scenarios

## Supported Instructions

- LOAD
- STORE
- ADD
- SUB
- MUL
- NAND
- BEQ
- CALL
- RET

## Architecture

The simulator models the main stages of Tomasulo-style dynamic scheduling:

1. Issue
2. Execute
3. Write Back
4. Commit

Each instruction is assigned to an appropriate functional unit and reservation station.

The simulator tracks:

- operand values
- unresolved dependencies
- reorder buffer entries
- execution latency
- instruction state
- branch outcomes
- register status
- instruction timing

## Reorder Buffer

The simulator uses a configurable reorder buffer to allow instructions to execute out of order while still committing results in program order.

Each ROB entry tracks information such as:

- instruction type
- destination
- computed value
- readiness
- program counter
- branch information

## Reservation Stations

Separate reservation stations are modeled for:

- Loads
- Stores
- ADD / SUB
- MUL
- NAND
- BEQ
- CALL / RET

The number of reservation stations for each functional unit can be changed through the Qt interface.

## Branch Handling

The simulator supports BEQ instructions and detects branch mispredictions.

When a misprediction occurs, the simulator:

- flushes speculative instructions
- clears affected reservation stations
- resets the reorder buffer tail
- redirects the program counter to the correct instruction

CALL and RET instructions also perform control-flow redirection.

## Qt Interface

The graphical interface allows the user to configure:

- Reorder buffer size
- Reservation station counts
- Functional-unit latencies
- Test scenario

The simulation output is displayed directly inside the application.

## Built-In Test Scenarios

The project includes 17 test scenarios covering:

- basic loads and stores
- store dependencies
- arithmetic operations
- chained dependencies
- multiplication latency
- NAND operations
- taken and non-taken branches
- branch misprediction recovery
- CALL
- CALL / RET
- mixed instruction workloads

## Performance Metrics

The simulator reports:

- Total execution cycles
- Number of committed instructions
- Instructions per cycle (IPC)
- Number of branches
- Branch mispredictions
- Branch misprediction rate
- Register values
- Modified memory locations
- Instruction timing

Instruction timing includes:

- Issue cycle
- Execution start
- Execution completion
- Write-back cycle
- Commit cycle

## Project Structure

```text
tomasulo-simulator/
│
├── QT_sim/
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   ├── mainwindow.ui
│   └── tomasulo_qt.pro
│
├── Tomasulo Algorithm Simulation.cpp
├── .gitignore
└── README.md
