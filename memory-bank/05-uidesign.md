# UI Design

**Last Updated:** April 19, 2025

This document outlines the user interface design principles, components, and layouts for the Quantum Mechanics Simulation project.

## Design System

### Brand Identity
- Primary Color: #1e1e1e (dark gray background)
- Accent Color: #ff8c00 (bright orange for active controls)
- Secondary Color: #3c3c3c (panel backgrounds), #ffffff (text)
- Typography: 'Roboto' for UI labels (size 14px), 'Source Code Pro' for numeric readouts (size 12px)
- Logo: top-left corner, 32×32 px, monochrome white, links to project homepage

### Design Principles
1. Clarity: Every control and display must have clear labels and tooltips
2. Consistency: UI elements follow same layout, color, and interaction patterns
3. Efficiency: Minimize clicks and provide keyboard shortcuts for power users

## Layout and Components

### Main Window
- Full-screen OpenGL viewport displaying probability density texture on a screen-aligned quad
- Docked ImGui panel on the right side with tabs or collapsible sections

### Controls Section
- Buttons: [Start], [Stop], [Reset]
- Display: Current simulation time `t` and frame rate (FPS)
- Display: Total probability `P_total`

### Simulation Parameters
- Slider/Input for time step Δt (range: 1e-5 .. 1e-2)
- Read-only display of grid size (Nx × Ny)

### Potential Configuration
- Dropdown or radio buttons for potential type:
  - Free Space
  - Square Barrier/Well
  - Harmonic Oscillator
- Context-sensitive parameter inputs:
  - **Square Barrier/Well:** center (x_c, y_c), width, height/V0
  - **Harmonic Oscillator:** ω
- [Apply Changes] button to update potential without resetting simulation

### Initial Wavepacket Configuration
- Inputs/sliders for:
  - Center (x_0, y_0)
  - Widths (σ_x, σ_y)
  - Momentum (k_x, k_y)
- Changes trigger a simulation reset

### Monitoring and Diagnostics
- Real-time text display:
  - Simulation time t
  - FPS
  - Total probability ∑|Ψ|²·Δx·Δy
- Optional graphs (e.g., total probability vs. time) in a separate tab

## Interaction Flow
1. User adjusts parameters in ImGui panel
2. User clicks [Start] to begin simulation
3. SimulationEngine::step() runs each frame, UIManager retrieves data
4. VisualizationEngine renders updated texture
5. User may [Stop], tweak parameters, and [Reset] to restart

## Style Guidelines
- Consistent color scheme matching brand identity (e.g., dark background, accent colors for active controls)
- Use clear labels and tooltips for all inputs
- Group related controls in collapsible ImGui child windows
- Keyboard shortcuts: Space = Start/Stop, R = Reset, P = Pause

> See 09-environment.md for system setup and 03-systemPatterns.md for module responsibilities.

## Page Layouts

### Main Simulation Page
```
+-------------------------------------------------------------+
| LOGO | Quantum Simulator               [Time: 0.00s] [FPS: 60]|
+-------------------------------------------------------------+
|                     | Controls   |                           |
|                     |----------- | Diagnostics               |
|  OpenGL Viewport    | Start | ▶  | [Graph: P_total vs t]     |
|                     | Stop  | ■  | [Graph: Energy vs t]      |
|                     | Reset | ⟳  |                           |
|                     |-----------|---------------------------|
|                     | Δt: [0.001][slider]           [P_total]| 
|                     | Grid: 512×512 (read-only)     [Energy]| 
|                     | Potential: (dropdown) Free Space       |
|                     | [Configure]                               |
+-------------------------------------------------------------+
```

### Settings Page (Tabbed)
```
+---------------------------+
| [Simulation] [UI] [About] |
+---------------------------+
| Simulation Parameters     |
| - Δt, Grid Size, Threads  |
+---------------------------+
| UI Preferences            |
| - Theme: Dark/Light       |
| - Font Size               |
+---------------------------+
| About                     |
| - Version, Credits, License|
+---------------------------+
```

## User Flows

### Flow: Run and Observe Simulation
1. Launch application; Main Simulation Page appears
2. Adjust Δt slider or input precise value
3. Select potential type from dropdown
4. Click ▶ Start (or press Space)
5. Observe live OpenGL viewport and real-time metrics
6. Pause with Space, tweak parameters, then ▶ Resume
7. Click ■ Stop to end simulation
8. Click ⟳ Reset to clear state and return to t=0

### Flow: Configure Simulation Settings
1. Click ⚙ Settings icon or press 'S'
2. Switch to [Simulation] tab
3. Modify grid size or thread count
4. Click [Save] to apply changes or [Cancel]

### Flow: Export Data
1. Stop simulation
2. Open Diagnostics tab
3. Click [Export CSV] or [Export JSON]
4. Choose file location and confirm

## Responsive Design

### Breakpoints
- Desktop: >1024px width — full layout
- Tablet: 768–1024px width — stack controls below viewport
- Mobile: <768px width — single-column, collapsible panels

### Adaptations
- Tablet: collapse Diagnostics under a toggle button
- Mobile: hide graphs; show only key metrics, use dropdown for parameters
- All: ImGui windows scrollable to access off-screen widgets

## Accessibility Considerations

### Standards Compliance
- WCAG 2.1 Level AA for contrast and navigation

### Implementations
- Color contrast ratio ≥4.5:1 for text on backgrounds
- Keyboard navigation: Tab to focus, Enter/Space to activate buttons
- Tooltips appear on hover and focus
- Screen reader labels for all controls via ImGui callbacks

## UI Assets

### Icons
- Play: material-design 'play_arrow'
- Stop: 'stop'
- Reset: 'autorenew'
- Settings: 'settings'
- Export CSV/JSON: 'file_download'
- All icons in SVG format, stored in `/assets/icons/`

### Illustrations
- Default background grid texture: `/assets/textures/grid.png`
- Logo: `/assets/logo/quantum_logo.svg`

## Design Tools & Resources

- Design Files: Figma project at https://figma.com/file/quantum-sim-ui
- Style Guide: https://company.docs/quantum-sim/style-guide
- Prototypes: https://invisionapp.com/prototype/quantum-sim

> See 09-environment.md for setup and 03-systemPatterns.md for module responsibilities.