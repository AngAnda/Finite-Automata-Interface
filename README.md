# Finite Automata Interface

This tool is a graphical application designed for the creation, simulation, and analysis of finite automata, which are key concepts in the fields of computer science and discrete mathematics. It provides an interactive way to visualize and understand the workings of automata.

## Features
### State Management
- **Add State**: Introduce new states into the automaton.
- **Select Final State:** Mark states as final (acceptance) states, visually indicated by a double circle.
- **Select Starting State:** Designate the initial state of the automaton, indicated by an entering arrow with no originating state.
- **Delete State:** Remove states from the automaton.
![image](https://github.com/AngAnda/Interface-of-Finite-Automaton/assets/61116472/c0b9e39b-3218-4a69-aea1-963e9657d165)


### Transitions
- **Create Transitions**: Define transitions between states with specified input symbols.
- **Lambda Transitions**: Implement lambda (ε) transitions that allow the automaton to move between states without consuming any input symbols.

### Word Processing
- **Read One Word**: Test individual words to determine if they are accepted by the automaton. It will display an animation of the word passing throw the automata.
- **Read Words from File**: Bulk process words from a file to validate against the automaton.

![image](https://github.com/AngAnda/Interface-of-Finite-Automaton/assets/61116472/2193e845-610e-48e6-a4f3-bdb502315b1a)

### Automaton Management
- **Save Automaton:** Preserve the current automaton structure for future use.
- **Load Automaton:** Retrieve and load previously saved automata.
- **Select Automaton Type:** Choose among different types of automata like DFA, NFA, etc.

### Simulation and Validation
- **Input Validation:** Simulate the input processing and validate whether the automaton accepts or rejects it.
- **Invalid Automaton Detection:** The system notifies when the automaton is incorrectly configured or incomplete.

## Usage
The application is straightforward to use with a graphical interface that allows drag-and-drop manipulation of states and transitions. Users can input strings to be tested against the defined automaton and receive immediate feedback on whether the strings are accepted.

