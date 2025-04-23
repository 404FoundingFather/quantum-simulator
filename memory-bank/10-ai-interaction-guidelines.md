# AI Interaction Guidelines

**Last Updated:** April 10, 2025

This document outlines guidelines for working with AI assistants on this project, including preferences for communication style, code formatting, and areas where AI assistance is most valuable.

## General Communication Preferences

### Response Style
- Level of detail: concise yet comprehensive; prioritize clarity over verbosity
- Technical depth: assume advanced C++ and numerical methods expertise
- Formality: professional and neutral tone
- Code explanations: include brief rationale and mention key design principles

### Context Sharing
- Reference Memory-Bank documents by filename (e.g., "05-uidesign.md") when relevant
- Provide file paths and code snippets for precise context
- Mention current sprint and active tasks from the Kanban board
- Specify operating system and build environment if OS-specific guidance is needed

## Code Style Preferences

### Clean Code Requirements

All code generated or suggested by AI assistants MUST adhere to Clean Code principles:

1. **Readability First**: Prioritize human readability over cleverness or conciseness
   - Use clear variable and function names
   - Maintain consistent formatting
   - Avoid excessive nesting

2. **Simplicity**: Write simple, straightforward code
   - Solve the problem directly
   - Avoid premature optimization
   - Prefer explicit over implicit approaches

3. **Self-Documenting Code**: Code should primarily explain itself
   - Choose descriptive names over comments
   - Use comments only for "why" not "what"
   - Structure code logically to tell a story

4. **DRY (Don't Repeat Yourself)**: Avoid duplication
   - Extract repeated code into reusable functions
   - Maintain single sources of truth
   - Create abstractions only when they reduce duplication

5. **SOLID Principles**:
   - Single Responsibility: Each component does one thing well
   - Open/Closed: Open for extension, closed for modification
   - Liskov Substitution: Subtypes must be substitutable for base types
   - Interface Segregation: Many specific interfaces are better than one general interface
   - Dependency Inversion: Depend on abstractions, not implementations

6. **Small Units**: Keep functions and classes small and focused
   - Functions should do one thing
   - Classes should have a single responsibility
   - Files should be organized around a cohesive concept

7. **Meaningful Names**: Use intention-revealing names
   - Variables tell what they contain
   - Functions tell what they do
   - Classes tell what they represent

8. **Minimal Dependencies**: Limit coupling between components
   - Create clear boundaries between modules
   - Minimize shared state
   - Use dependency injection where appropriate

9. **Error Handling**: Handle errors meaningfully
   - Be specific about error types
   - Provide useful error messages
   - Fail fast and visibly

10. **Testable Code**: Generate code with testing in mind
    - Avoid global state
    - Make side effects explicit
    - Design for easy mocking/stubbing

### General Formatting
- Indentation: 4 spaces, no tabs
- Line length: limit to 100 characters
- Naming conventions: camelCase for functions/variables, PascalCase for classes, snake_case for files
- File names: lowercase with hyphens (e.g., `simulation-engine.cpp`)

### Language-Specific Standards
- C++: follow Google C++ Style Guide with Doxygen comments
  - Use `nullptr`, `override`, `constexpr` where appropriate
  - Prefer `std::vector` and RAII for resource management
  - No raw pointers except for interop (e.g., OpenGL callbacks)

### Documentation Standards
- Use Doxygen for public APIs (`/** ... */`)
- Document parameters, return values, and preconditions
- Keep inline comments for "why" not "what"

## AI Assistance Guidelines

### Recommended Use Cases
- Generating class or function boilerplate (e.g., FFTW plan manager)
- Refactoring code to adhere to Clean Code and SOLID principles
- Writing unit tests and integration test scaffolding (Google Test)
- Crafting CMake configuration snippets
- Updating Memory-Bank markdown files based on new design doc sections

### Limited Use Cases
- Implementing core physics algorithms without verification
- Security-critical or licensing-sensitive logic
- Performance tuning without profiling data
- Final review of physics correctness—must be validated by domain expert

### Prompt Engineering Tips
- Provide the relevant Memory-Bank file path and section header
- Specify desired output type (e.g., "Generate a Google Test for class Wavefunction")
- Include examples of input parameters and expected behavior
- Ask for one responsibility at a time: "Write only the Doxygen comment for this function"

## Review Process

### AI-Generated Code Review
- Verify adherence to Clean Code principles and project style
- Ensure unit tests cover edge cases and performance constraints
- Cross-check algorithmic logic against design document equations

### Feedback Loop
- Annotate AI suggestions with inline comments in PRs
- Log AI-assisted changes in `CHANGELOG.md` under AI Contributions
- Periodically update guidelines based on AI success/failure metrics

## Project-Specific Guidelines

### Domain Knowledge
- Familiar with 2D Time‑Dependent Schrödinger Equation and Split‑Step Fourier Method
- Understand wavepacket initialization and static potentials (free, barrier, oscillator)

### Technical Context
- Code modules: `core`, `solver`, `visualization`, `ui`, `config`
- Build system: CMake with FFTW3, OpenGL3.3, GLFW, Dear ImGui
- Parallelism: OpenMP for CPU multi-threading

### Tools Integration
- Use `run_in_terminal` tool with CMake commands for build tasks
- Reference `insert_edit_into_file` when editing existing project files
- After edits, use `get_errors` to validate compilation

## Memory Bank Updates

### Recommended Update Frequency
- Update upon completion of each sprint or major design change
- Refresh "Last Updated" timestamp in each file

### Documentation of AI Contributions
- Add an "AI Contributions" section in `CHANGELOG.md` for significant AI-generated entries
- Tag commits made with AI assistance using `[ai]` in commit message

## Best Practices

- Always adhere to Clean Code principles in all generated code
- When suggesting code refactoring, explain how it improves adherence to Clean Code principles
- Prioritize readability and maintainability over clever or highly optimized solutions
- Balance Clean Code standards with project-specific requirements
- When in doubt about a code pattern, prefer the simpler, more readable approach
- Always align AI-generated code with Clean Code and project style
- Stop and review any AI suggestion that touches physics core algorithms
- When refactoring, ensure tests pass before merging
- Use AI to accelerate boilerplate and documentation, not to replace domain expertise
- Maintain an ongoing feedback loop to refine these guidelines over time